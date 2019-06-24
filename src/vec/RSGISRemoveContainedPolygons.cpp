/*
 *  RSGISRemoveContainedPolygons.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/03/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISRemoveContainedPolygons.h"

namespace rsgis{namespace vec{
	
	
	RSGISRemoveContainedPolygons::RSGISRemoveContainedPolygons()
	{
		
	}
	
	long unsigned RSGISRemoveContainedPolygons::removeContainedPolygons(OGRLayer *input, OGRLayer *output)
	{		
		OGRFeature *inFeature = NULL;
		OGRFeature *tmpFeature = NULL;
		OGRFeature *outFeature = NULL;
		OGRGeometry *geometry = NULL;
		OGRGeometry *tmpGeometry = NULL;

		bool geomContained = false;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefn = NULL;
		
		unsigned long currentFID = 0;
		unsigned long tmpFID = 0;
		unsigned long numOutputted = 0;
		
		try
		{
			// Copy feature defenitions for output shapefile
			inFeatureDefn = input->GetLayerDefn();
			this->copyFeatureDefn(output, inFeatureDefn);
			outFeatureDefn = output->GetLayerDefn();
			
			unsigned long numFeatures = input->GetFeatureCount(true);
			
			for(unsigned long i = 0; i < numFeatures; ++i)
			{
				geomContained = false;
				inFeature = input->GetFeature(i);
				
				currentFID = inFeature->GetFID();
				
				geometry = inFeature->GetGeometryRef(); // Get geometry 
				
				if( geometry != NULL )
				{					
					for(unsigned long j = 0; j < numFeatures; ++j)
					{
						tmpFeature = input->GetFeature(j);
						tmpFID = tmpFeature->GetFID();
						
						if(currentFID != tmpFID)
						{
							tmpGeometry = tmpFeature->GetGeometryRef();
							if( tmpGeometry != NULL )
							{
								try 
								{
									if(tmpGeometry->Contains(geometry))
									{
										geomContained = true;
										break;
									}
								}
								catch (geos::util::TopologyException &e) 
								{
									std::cout << "WARNING: " << e.what() << std::endl;
								}
								
							}
							else 
							{
								std::cout << "Tmp Geometry is NULL - IGNORING...\n";
							}
						}
						OGRFeature::DestroyFeature(tmpFeature);
					}
				
				
					if(!geomContained)
					{
						numOutputted++;
						
						outFeature = OGRFeature::CreateFeature(outFeatureDefn);
						outFeature->SetGeometry(geometry);
						outFeature->SetFID(currentFID);
						this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefn);
						
						if( output->CreateFeature(outFeature) != OGRERR_NONE )
						{
							throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
						}
						
						OGRFeature::DestroyFeature(outFeature);
					}
					OGRFeature::DestroyFeature(inFeature);
				}
				else 
				{
					std::cout << "Current Geometry is NULL - IGNORING...\n";
				}
			}
		}
		catch(RSGISVectorException &e)
		{
			throw e;
		}
		return numOutputted;
	}
	
	long unsigned RSGISRemoveContainedPolygons::removeContainedPolygons(OGRLayer *input, OGRLayer *output, std::vector<OGRPolygon*> *inputPolys)
	{
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		OGRGeometry *geometry = NULL;
		
		bool geomContained = false;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefn = NULL;
		
		unsigned long currentFID = 0;
		unsigned long numOutputted = 0;
		
		std::vector<OGRPolygon*>::iterator iterPolys;
		
		try
		{
			// Copy feature defenitions for output shapefile
			inFeatureDefn = input->GetLayerDefn();
			this->copyFeatureDefn(output, inFeatureDefn);
			outFeatureDefn = output->GetLayerDefn();
			
			unsigned long numFeatures = input->GetFeatureCount(true);
			
			for(unsigned long i = 0; i < numFeatures; ++i)
			{
				geomContained = false;
				inFeature = input->GetFeature(i);
				
				currentFID = inFeature->GetFID();
				
				geometry = inFeature->GetGeometryRef(); // Get geometry 
				
				if( geometry != NULL )
				{					
					for(iterPolys = inputPolys->begin(); iterPolys != inputPolys->end(); ++iterPolys)
					{
						try 
						{
							if((*iterPolys)->Contains(geometry))
							{
								geomContained = true;
								break;
							}
						}
						catch (geos::util::TopologyException &e) 
						{
							std::cout << "WARNING: " << e.what() << std::endl;
						}
					}
					
					
					if(!geomContained)
					{
						numOutputted++;
						
						outFeature = OGRFeature::CreateFeature(outFeatureDefn);
						outFeature->SetGeometry(geometry);
						outFeature->SetFID(currentFID);
						this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefn);
						
						if( output->CreateFeature(outFeature) != OGRERR_NONE )
						{
							throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
						}
						
						OGRFeature::DestroyFeature(outFeature);
					}
					OGRFeature::DestroyFeature(inFeature);
				}
				else 
				{
					std::cout << "Current Geometry is NULL - IGNORING...\n";
				}
			}
		}
		catch(RSGISVectorException &e)
		{
			throw e;
		}
		return numOutputted;
	}
	
	void RSGISRemoveContainedPolygons::copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn)
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
	
	void RSGISRemoveContainedPolygons::copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			outFeature->SetField(outFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef()), inFeature->GetRawFieldRef(inFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef())));
		}
		
	}
	
	RSGISRemoveContainedPolygons::~RSGISRemoveContainedPolygons()
	{
		
	}
	
}}

