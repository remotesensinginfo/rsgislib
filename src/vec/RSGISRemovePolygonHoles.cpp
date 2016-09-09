/*
 *  RSGISRemovePolygonHoles.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/05/2010.
 *  Copyright 2009 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
 *
 *  RSGISLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RSGISLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISRemovePolygonHoles.h"

namespace rsgis{namespace vec{
	
	
	RSGISRemovePolygonHoles::RSGISRemovePolygonHoles(float areaThreshold, bool areaThresholdUsed)
	{
		this->areaThreshold = areaThreshold;
        this->areaThresholdUsed = areaThresholdUsed;
	}
	
	void RSGISRemovePolygonHoles::removeholes(OGRLayer *input, OGRLayer *output)throw(RSGISVectorException)
	{
		RSGISVectorUtils vecUtils;
		
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		OGRGeometry *geometry = NULL;
		OGRPolygon *polygon = NULL;
		OGRPolygon *nPolygon = NULL;
		
		bool nullGeometry = false;
		bool polyOK = false;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefn = NULL;
		
		long fid = 0;
		long unsigned numOutputted = 0;
		
		try
		{
			inFeatureDefn = input->GetLayerDefn();
			this->copyFeatureDefn(output, inFeatureDefn);
			outFeatureDefn = output->GetLayerDefn();
			
			unsigned long numFeatures = input->GetFeatureCount(true);
			
			std::cout << "There are " << numFeatures << " to process\n";
			
			unsigned long feedback = numFeatures/10;
			unsigned long feedbackCounter = 0;
			unsigned long i = 0;
			std::cout << "Started" << std::flush;
			
			input->ResetReading();
			while( (inFeature = input->GetNextFeature()) != NULL )
			{
				if((numFeatures >= 10) && ((i % feedback) == 0))
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				++i;
				
				fid = inFeature->GetFID();
				
				// Get Geometry.
				polyOK = false;
				nullGeometry = false;
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
				{
					polygon = (OGRPolygon *) geometry;
					
					try 
					{
                        if(areaThresholdUsed)
                        {
                            nPolygon = vecUtils.removeHolesOGRPolygon(polygon, areaThreshold);
                        }
                        else
                        {
                            nPolygon = vecUtils.removeHolesOGRPolygon(polygon);
                        }
						polyOK = true;
					}
					catch (RSGISVectorException &e) 
					{
						std::cout << fid << ": " << e.what() << std::endl;
						polyOK = false;
					}
					
				} 
				else 
				{
					nullGeometry = true;
					std::cout << fid << ": Geometry was either the incorrect type or NULL." << std::endl;
				}
				
				if(polyOK && !nullGeometry)
				{
					outFeature = OGRFeature::CreateFeature(outFeatureDefn);
					outFeature->SetGeometry(nPolygon);
					outFeature->SetFID(fid);
					this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefn);
					
					if( output->CreateFeature(outFeature) != OGRERR_NONE )
					{
						throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
					}
					
					OGRFeature::DestroyFeature(outFeature);
					numOutputted++;
				}
				else 
				{
					std::cout << fid << ": Geometry is either NULL or not a polygon\n";
				}
				
				OGRFeature::DestroyFeature(inFeature);
			}
			std::cout << " Complete.\n";
			
			std::cout << numOutputted << " Polygons have been outputted from the " << numFeatures << " in the input file.\n";
			std::cout << (numFeatures-numOutputted) << " features have been dropped during this process\n";
			if(numOutputted != numFeatures)
			{
				std::cout << "** It is recommend that you check the output file before using for further processing ** \n";
			}
		}
		catch(RSGISVectorException &e)
		{
			throw e;
		}
	}
	
	void RSGISRemovePolygonHoles::copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			if( outputSHPLayer->CreateField( inFeatureDefn->GetFieldDefn(i) ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating ") + std::string(inFeatureDefn->GetFieldDefn(i)->GetNameRef()) + std::string(" field has failed.");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
	}
	
	void RSGISRemovePolygonHoles::copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			outFeature->SetField(outFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef()), inFeature->GetRawFieldRef(inFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef())));
		}
		
	}
	
	RSGISRemovePolygonHoles::~RSGISRemovePolygonHoles()
	{
		
	}
	
}}

