/*
 *  RSGISClassPolygon.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/11/2008.
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

#include "RSGISClassPolygon.h"

namespace rsgis{namespace vec{
	
	
	RSGISClassPolygon::RSGISClassPolygon(std::string classAttribute)  : RSGISPolygonData()
	{
		this->classAttribute = classAttribute;
	}
	
	void RSGISClassPolygon::readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		// Read the class attribute
		OGRFieldDefn *fieldDef = NULL;
		std::string columnName;
		int fieldCount = featDefn->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			fieldDef = feature->GetFieldDefnRef(i);
			columnName = fieldDef->GetNameRef();
			if(columnName == this->classAttribute)
			{
				this->className = std::string(feature->GetFieldAsString(i));
			}
		}
		
	}
	
	void RSGISClassPolygon::createLayerDefinition(OGRLayer *outputSHPLayer)
	{
		// Create class attribute column
		OGRFieldDefn classField( "class", OFTString );
		classField.SetWidth(100);
		if( outputSHPLayer->CreateField( &classField ) != OGRERR_NONE )
		{
			throw RSGISVectorOutputException("Creating shapefile field \"class\" has failed");
		}
	}
	
	void RSGISClassPolygon::populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)
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
		feature->SetField(featDefn->GetFieldIndex("class"), className.c_str());
	}
	
	std::string RSGISClassPolygon::getClassName()
	{
		return this->className;
	}
	
	void RSGISClassPolygon::setClassName(std::string name)
	{
		this->className = name;
	}
	
	void RSGISClassPolygon::setNumPixels(int numPxls)
	{
		this->numPxls = numPxls;
	}
	
	int RSGISClassPolygon::getNumPixels()
	{
		return numPxls;
	}
	
	void RSGISClassPolygon::setPixelValues(rsgis::math::Matrix *pxlValues)
	{
        rsgis::math::RSGISMatrices matrixUtils;
		this->pxlValues = matrixUtils.createMatrix(pxlValues->n, pxlValues->m);
		int numElements = (pxlValues->n * pxlValues->m);
		for(int i = 0; i < numElements; i++)
		{
			this->pxlValues->matrix[i] = pxlValues->matrix[i];
		}
	}
	
	rsgis::math::Matrix* RSGISClassPolygon::getPixelValues()
	{
		return pxlValues;
	}
	
	RSGISClassPolygon::~RSGISClassPolygon()
	{
		
	}
}}

