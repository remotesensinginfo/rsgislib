/*
 *  RSGISCopyPolygonsInPolygon.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 26/02/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISCopyPolygonsInPolygon.h"

namespace rsgis{namespace vec{
	
	
	RSGISCopyPolygonsInPolygon::RSGISCopyPolygonsInPolygon()
	{
		
	}
	
	long unsigned RSGISCopyPolygonsInPolygon::copyPolygonsInPoly(OGRLayer *input, OGRLayer *output, OGRGeometry *coverPolygon)throw(RSGISVectorException)
	{
		
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		OGRGeometry *geometry = NULL;
		
		bool polyOK = false;
		
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRFeatureDefn *outFeatureDefn = NULL;
		
		long fid = 0;
		long unsigned numOutputted = 0;
		
		try
		{
			// Copy feature defenitions for output shapefile
			inFeatureDefn = input->GetLayerDefn();
			this->copyFeatureDefn(output, inFeatureDefn);
			outFeatureDefn = output->GetLayerDefn();
			
			unsigned long numFeatures = input->GetFeatureCount(true);
			
			std::cout << "There are " << numFeatures << " to process\n";
			
			input->ResetReading();
			while( (inFeature = input->GetNextFeature()) != NULL )
			{
				fid = inFeature->GetFID();
				
				// Get Geometry.
				polyOK = false;
				geometry = inFeature->GetGeometryRef(); // Get geometry 
				
				// If coverPolygon contains polygons add to new shapefile
				if(coverPolygon->Contains(geometry))
				{
					numOutputted++;
					
					outFeature = OGRFeature::CreateFeature(outFeatureDefn);
					outFeature->SetGeometry(geometry);
					outFeature->SetFID(fid);
					this->copyFeatureData(inFeature, outFeature, inFeatureDefn, outFeatureDefn);
					
					if( output->CreateFeature(outFeature) != OGRERR_NONE )
					{
						throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
					}
					
					OGRFeature::DestroyFeature(outFeature);
				}
				OGRFeature::DestroyFeature(inFeature);
			}
		}
		catch(RSGISVectorException &e)
		{
			throw e;
		}
		return numOutputted;
	}
	
	void RSGISCopyPolygonsInPolygon::copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
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
	
	void RSGISCopyPolygonsInPolygon::copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn)
	{
		int fieldCount = inFeatureDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			outFeature->SetField(outFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef()), inFeature->GetRawFieldRef(inFeatureDefn->GetFieldIndex(inFeatureDefn->GetFieldDefn(i)->GetNameRef())));
		}
		
	}
	
	RSGISCopyPolygonsInPolygon::~RSGISCopyPolygonsInPolygon()
	{
		
	}
	
}}

