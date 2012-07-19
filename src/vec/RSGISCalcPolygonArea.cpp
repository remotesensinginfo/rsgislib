/*
 *  RSGISCalcPolygonArea.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/07/2009.
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

#include "RSGISCalcPolygonArea.h"

namespace rsgis{namespace vec{
	
	RSGISCalcPolygonArea::RSGISCalcPolygonArea()
	{

	}
	
	void RSGISCalcPolygonArea::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
		
		
		OGRwkbGeometryType geometryType = inFeature->GetGeometryRef()->getGeometryType();
		
		if( geometryType == wkbPolygon )
		{
			OGRPolygon *polygon = (OGRPolygon *) inFeature->GetGeometryRef();
			outFeature->SetField(outFeatureDefn->GetFieldIndex("Area"), polygon->get_Area());
		} 
		else
		{
			throw RSGISVectorException("Unsupport data type.");
		}
	}
	
	void RSGISCalcPolygonArea::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
		
	}
	
	void RSGISCalcPolygonArea::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		OGRFieldDefn shpField("Area", OFTReal);
		shpField.SetPrecision(10);
		if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
		{
			std::string message = std::string("Creating shapefile field \'Area\' has failed");
			throw RSGISVectorOutputException(message.c_str());
		}
	}
	
	RSGISCalcPolygonArea::~RSGISCalcPolygonArea()
	{

	}
}}







