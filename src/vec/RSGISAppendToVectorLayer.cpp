/*
 *  RSGISAppendToVectorLayer.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/03/2010.
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

#include "RSGISAppendToVectorLayer.h"

namespace rsgis{namespace vec{
	
	
	RSGISAppendToVectorLayer::RSGISAppendToVectorLayer()
	{
		
	}
	
	void RSGISAppendToVectorLayer::appendLayer(OGRLayer *input, OGRLayer *outputLayer, bool ignoreAttr)
	{		
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		OGRGeometry *geometry = NULL;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefn = NULL;
		
		long fid = 0;
		
		try
		{
			inFeatureDefn = input->GetLayerDefn();
			outFeatureDefn = outputLayer->GetLayerDefn();
			
			input->ResetReading();
			while( (inFeature = input->GetNextFeature()) != NULL )
			{		
				fid = inFeature->GetFID();
				
				// Get Geometry.
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL )
				{
					outFeature = OGRFeature::CreateFeature(outFeatureDefn);
					outFeature->SetGeometry(geometry);
					if(!ignoreAttr)
					{
						this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefn);
					}
					
					if( outputLayer->CreateFeature(outFeature) != OGRERR_NONE )
					{
						throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
					}
					
					OGRFeature::DestroyFeature(outFeature);
				} 
				else 
				{
					std::cout << inFeatureDefn->GetName() << ": Feature " << fid << ": Geometry was NULL so feature ignored." << std::endl;
				}
				OGRFeature::DestroyFeature(inFeature);
			}
		}
		catch(RSGISVectorException &e)
		{
			throw e;
		}
	}
	
	void RSGISAppendToVectorLayer::copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn)
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
	
	void RSGISAppendToVectorLayer::copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			outFeature->SetField(outFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef()), inFeature->GetRawFieldRef(inFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef())));
		}
		
	}
	
	RSGISAppendToVectorLayer::~RSGISAppendToVectorLayer()
	{
		
	}
	
}}


