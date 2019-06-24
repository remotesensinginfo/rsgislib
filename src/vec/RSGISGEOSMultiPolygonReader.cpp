/*
 *  RSGISGEOSMultiPolygonReader.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/09/2009.
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

#include "RSGISGEOSMultiPolygonReader.h"

namespace rsgis{namespace vec{
	
	RSGISGEOSMultiPolygonReader::RSGISGEOSMultiPolygonReader(std::vector<geos::geom::MultiPolygon*> *mPolygons)
	{
		vecUtils = new RSGISVectorUtils();
		this->mPolygons = mPolygons;
	}
	
	void RSGISGEOSMultiPolygonReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISGEOSMultiPolygonReader::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
		
		// Calculate the centre coordinates
		if( geometryType == wkbPolygon )
		{
			std::vector<geos::geom::Geometry*> *polys = new std::vector<geos::geom::Geometry*>();
			OGRPolygon *polygon = (OGRPolygon *) feature->GetGeometryRef();
			polys->push_back(vecUtils->convertOGRPolygon2GEOSPolygon(polygon));
			geos::geom::MultiPolygon *mGEOSPolygon = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createMultiPolygon(polys);
			mPolygons->push_back(mGEOSPolygon);
		} 
		else if( geometryType == wkbMultiPolygon )
		{
            rsgis::geom::RSGISGeometry geomUtils;
			OGRMultiPolygon *mPolygon = (OGRMultiPolygon *) feature->GetGeometryRef();
			geos::geom::MultiPolygon *mGEOSPolygon = vecUtils->convertOGRMultiPolygonGEOSMultiPolygon(mPolygon);
			mPolygons->push_back(mGEOSPolygon);
		} 
		else
		{
			std::string message = std::string("Unsupport data type: ") + std::string(feature->GetGeometryRef()->getGeometryName());
			throw RSGISVectorException(message);
		}
	}
	
	void RSGISGEOSMultiPolygonReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{
		// Nothing to do!
	}
	
	RSGISGEOSMultiPolygonReader::~RSGISGEOSMultiPolygonReader()
	{
		delete vecUtils;
	}
}}






