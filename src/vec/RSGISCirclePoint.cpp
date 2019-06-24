/*
 *  RSGISCirclePoint.cpp
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

#include "RSGISCirclePoint.h"

namespace rsgis{namespace vec{
	
	RSGISCirclePoint::RSGISCirclePoint(std::string radiusAttribute, std::string areaAttribute, std::string heightAttribute)
	{
		this->radiusAttribute = radiusAttribute;
		this->areaAttribute = areaAttribute;
		this->heightAttribute = heightAttribute;
	}
	
	void RSGISCirclePoint::readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)
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
	
	void RSGISCirclePoint::createLayerDefinition(OGRLayer *outputSHPLayer)
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
	
	void RSGISCirclePoint::populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		RSGISVectorUtils vecUtils;
		OGRPoint *point= NULL;
		
		point = vecUtils.convertGEOSPoint2OGRPoint(pointGeom);
		feature->SetGeometry(point);
		
		// Set Attrbutes
		feature->SetField(featDefn->GetFieldIndex("Area"), this->area);
		feature->SetField(featDefn->GetFieldIndex("Height"), this->height);
		feature->SetField(featDefn->GetFieldIndex("Radius"), this->radius);
	}
	
	void RSGISCirclePoint::setArea(float area)
	{
		this->area = area;
	}
	
	void RSGISCirclePoint::setHeight(float height)
	{
		this->height = height;
	}
	
	void RSGISCirclePoint::setRadius(float radius)
	{
		this->radius = radius;
	}
	
	float RSGISCirclePoint::getArea()
	{
		return this->area;
	}
	
	float RSGISCirclePoint::getHeight()
	{
		return this->height;
	}
	
	float RSGISCirclePoint::getRadius()
	{
		return this->radius;
	}
	
	RSGISCirclePoint::~RSGISCirclePoint()
	{
		
	}
}}



