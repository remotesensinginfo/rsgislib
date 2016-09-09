/*
 *  RSGISGEOSPolygonReader.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/07/2009.
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

#include "RSGISGEOSPolygonReader.h"

namespace rsgis{namespace vec{
	
	RSGISGEOSPolygonReader::RSGISGEOSPolygonReader(std::vector<geos::geom::Polygon*> *polygons)
	{
		vecUtils = new RSGISVectorUtils();
		this->polygons = polygons;
	}
	
	void RSGISGEOSPolygonReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISGEOSPolygonReader::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
		
		// Calculate the centre coordinates
		if( geometryType == wkbPolygon )
		{
			OGRPolygon *polygon = (OGRPolygon *) feature->GetGeometryRef();
			polygons->push_back(vecUtils->convertOGRPolygon2GEOSPolygon(polygon));
		} 
		else if( geometryType == wkbMultiPolygon )
		{
            rsgis::geom::RSGISGeometry geomUtils;
			OGRMultiPolygon *mPolygon = (OGRMultiPolygon *) feature->GetGeometryRef();
			//cout << polygon->exportToGML() << endl;
			geos::geom::MultiPolygon *mGEOSPolygon = vecUtils->convertOGRMultiPolygonGEOSMultiPolygon(mPolygon);
			geomUtils.retrievePolygons(mGEOSPolygon, polygons);
		} 
		else
		{
            std::string message = std::string("Unsupport data type: ") + std::string(feature->GetGeometryRef()->getGeometryName());
			throw RSGISVectorException(message);
		}
	}
	
	void RSGISGEOSPolygonReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		// Nothing to do!
	}
	
	RSGISGEOSPolygonReader::~RSGISGEOSPolygonReader()
	{
		delete vecUtils;
	}
}}







