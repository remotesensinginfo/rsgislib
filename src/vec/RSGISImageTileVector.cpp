/*
 *  RSGISImageTileVector.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/11/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#include "RSGISImageTileVector.h"

namespace rsgis{namespace vec{
	
	
	RSGISImageTileVector::RSGISImageTileVector(std::string filenameAttribute)  : RSGISPolygonData()
	{
		this->filenameAttribute = filenameAttribute;
	}
	
	void RSGISImageTileVector::readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		// Read the class attribute
		OGRFieldDefn *fieldDef = NULL;
		std::string columnName;
		int fieldCount = featDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			fieldDef = feature->GetFieldDefnRef(i);
			columnName = fieldDef->GetNameRef();
			if(columnName == this->filenameAttribute)
			{
				this->filename = std::string(feature->GetFieldAsString(i));
			}
		}
		
	}
	
	void RSGISImageTileVector::createLayerDefinition(OGRLayer *outputSHPLayer)throw(RSGISVectorOutputException)
	{
		// Create class attribute column
		OGRFieldDefn classField( "filename", OFTString );
		classField.SetWidth(100);
		if( outputSHPLayer->CreateField( &classField ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"filename\" has failed");
		}
		
	}
	
	void RSGISImageTileVector::populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		RSGISVectorUtils vecUtils;
		OGRPolygon *polygon = NULL;
		OGRMultiPolygon *mPolygon = NULL;
		
		if(polygonType)
		{
			polygon = vecUtils.convertGEOSPolygon2OGRPolygon(polygonGeom);
			feature->SetGeometry(polygon);
		}
		else
		{
			mPolygon = vecUtils.convertGEOSMultiPolygon2OGRMultiPolygon(multiPolygonGeom);
			feature->SetGeometry(mPolygon);
		}
		
		// Set class
		feature->SetField(featDefn->GetFieldIndex("filename"), filename.c_str());
	}
	
	std::string RSGISImageTileVector::getFileName()
	{
		return this->filename;
	}
	
	void RSGISImageTileVector::setFileName(std::string name)
	{
		this->filename = name;
	}
	
	RSGISImageTileVector::~RSGISImageTileVector()
	{
		
	}
}}

