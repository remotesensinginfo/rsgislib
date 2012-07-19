/*
 *  RSGISSplitSmallLargePolygons.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/03/2010.
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

#include "RSGISSplitSmallLargePolygons.h"

namespace rsgis{namespace vec{
	
	
	RSGISSplitSmallLargePolygons::RSGISSplitSmallLargePolygons()
	{
		
	}
	
	void RSGISSplitSmallLargePolygons::splitPolygons(OGRLayer *input, OGRLayer *outputSmall, OGRLayer *outputLarge, float threshold)throw(RSGISVectorException)
	{		
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		OGRGeometry *geometry = NULL;
		OGRPolygon *polygon = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefnLarge = NULL;
		OGRFeatureDefn *outFeatureDefnSmall = NULL;
		
		long fid = 0;
		
		try
		{
			inFeatureDefn = input->GetLayerDefn();
			this->copyFeatureDefn(outputLarge, inFeatureDefn);
			outFeatureDefnLarge = outputLarge->GetLayerDefn();
			this->copyFeatureDefn(outputSmall, inFeatureDefn);
			outFeatureDefnSmall = outputSmall->GetLayerDefn();
						
			input->ResetReading();
			while( (inFeature = input->GetNextFeature()) != NULL )
			{		
				fid = inFeature->GetFID();
				
				// Get Geometry.
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
				{
					polygon = (OGRPolygon *) geometry;
					
					
					if(polygon->get_Area() > threshold)
					{
						outFeature = OGRFeature::CreateFeature(outFeatureDefnLarge);
						outFeature->SetGeometry(polygon);
						outFeature->SetFID(fid);
						this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefnLarge);
						
						if( outputLarge->CreateFeature(outFeature) != OGRERR_NONE )
						{
							throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
						}
						
						OGRFeature::DestroyFeature(outFeature);
						
					}
					else 
					{
						outFeature = OGRFeature::CreateFeature(outFeatureDefnSmall);
						outFeature->SetGeometry(polygon);
						outFeature->SetFID(fid);
						this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefnSmall);
						
						if( outputSmall->CreateFeature(outFeature) != OGRERR_NONE )
						{
							throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
						}
						
						OGRFeature::DestroyFeature(outFeature);
					}
				} 
				else 
				{
					std::cout << fid << ": Geometry was either the incorrect type or NULL." << std::endl;
				}
				OGRFeature::DestroyFeature(inFeature);
			}

		}
		catch(RSGISVectorException &e)
		{
			throw e;
		}
	}
	
	void RSGISSplitSmallLargePolygons::copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
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
	
	void RSGISSplitSmallLargePolygons::copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			outFeature->SetField(outFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef()), inFeature->GetRawFieldRef(inFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef())));
		}
		
	}
	
	RSGISSplitSmallLargePolygons::~RSGISSplitSmallLargePolygons()
	{
		
	}
	
}}




