/*
 *  RSGISEmptyPolygon.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/09/2008.
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

#include "RSGISEmptyPolygon.h"

namespace rsgis{namespace vec{
	
	
	RSGISEmptyPolygon::RSGISEmptyPolygon()  : RSGISPolygonData()
	{
	}
	
	void RSGISEmptyPolygon::readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)
	{
		// There are no attributes to read.
	}
	
	void RSGISEmptyPolygon::createLayerDefinition(OGRLayer *outputSHPLayer)
	{
		// There are no attributes to output
	}
	
	void RSGISEmptyPolygon::populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)
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
		
		// There are no attributes.
	}

	RSGISEmptyPolygon::~RSGISEmptyPolygon()
	{
		
	}
}}


