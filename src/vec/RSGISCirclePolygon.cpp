/*
 *  RSGISCirclePolygon.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/01/2009.
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

#include "RSGISCirclePolygon.h"


namespace rsgis{namespace vec{
	
	RSGISCirclePolygon::RSGISCirclePolygon(std::string radiusAttribute, std::string areaAttribute, std::string heightAttribute)
	{
		this->radiusAttribute = radiusAttribute;
		this->areaAttribute = areaAttribute;
		this->heightAttribute = heightAttribute;
	}
	
	void RSGISCirclePolygon::readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
        rsgis::math::RSGISMathsUtils mathsUtils;
		// Read the class attribute
		OGRFieldDefn *fieldDef = NULL;
		std::string columnName;
		int fieldCount = featDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			fieldDef = feature->GetFieldDefnRef(i);
			columnName = fieldDef->GetNameRef();
			if(columnName == this->areaAttribute)
			{
				this->area = mathsUtils.strtofloat(std::string(feature->GetFieldAsString(i)));
			}
			else if(columnName == this->heightAttribute)
			{
				this->height = mathsUtils.strtofloat(std::string(feature->GetFieldAsString(i)));
			}
			else if(columnName == this->radiusAttribute)
			{
				this->radius = mathsUtils.strtofloat(std::string(feature->GetFieldAsString(i)));
			}
		}
		
	}
	
	void RSGISCirclePolygon::createLayerDefinition(OGRLayer *outputSHPLayer)
	{
		// Create class attribute column
		OGRFieldDefn areaField( "Area", OFTReal );
		if( outputSHPLayer->CreateField( &areaField ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"Area\" has failed");
		}
		
		OGRFieldDefn heightField( "Height", OFTReal );
		if( outputSHPLayer->CreateField( &heightField ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"Height\" has failed");
		}
		
		OGRFieldDefn radiusField( "Radius", OFTReal );
		if( outputSHPLayer->CreateField( &radiusField ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"Radius\" has failed");
		}
	}
	
	void RSGISCirclePolygon::populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)
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
		
		// Set Attrbutes
		feature->SetField(featDefn->GetFieldIndex("Area"), this->area);
		feature->SetField(featDefn->GetFieldIndex("Height"), this->height);
		feature->SetField(featDefn->GetFieldIndex("Radius"), this->radius);
	}
	
	void RSGISCirclePolygon::setArea(float area)
	{
		this->area = area;
	}
	
	void RSGISCirclePolygon::setHeight(float height)
	{
		this->height = height;
	}
	
	void RSGISCirclePolygon::setRadius(float radius)
	{
		this->radius = radius;
	}
	
	float RSGISCirclePolygon::getArea()
	{
		return this->area;
	}
	
	float RSGISCirclePolygon::getHeight()
	{
		return this->height;
	}
	
	float RSGISCirclePolygon::getRadius()
	{
		return this->radius;
	}
	
	RSGISCirclePolygon::~RSGISCirclePolygon()
	{
		
	}
}}


