/*
 *  RSGISIntValuePolygonReader.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/09/2009.
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

#include "RSGISIntValuePolygonReader.h"



namespace rsgis{namespace vec{
	
	RSGISIntValuePolygonReader::RSGISIntValuePolygonReader(std::string valueattribute, std::list<rsgis::geom::RSGIS2DPoint*> *data)
	{
		this->valueattribute = valueattribute;
		vecUtils = new RSGISVectorUtils();
		this->data = data;
	}
	
	void RSGISIntValuePolygonReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISIntValuePolygonReader::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		// Set up the Species Polygon object and add to Delunay triangulation.
		
		
		OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
		
		OGRFeatureDefn *featureDefn = feature->GetDefnRef();
		
		int valueattributeIdx = featureDefn->GetFieldIndex(this->valueattribute.c_str());
		if(valueattributeIdx < 0)
		{
			std::string message = "This layer does not contain a field with the name \'" + this->valueattribute + "\'";
			throw RSGISVectorException(message.c_str());
		}
		int value = feature->GetFieldAsInteger(valueattributeIdx);
		
		if( geometryType == wkbPolygon )
		{
			RSGISIntValuePolygon *tmpValuePoly = new RSGISIntValuePolygon();
			tmpValuePoly->setValue(value);
			OGRPolygon *polygon = (OGRPolygon *) feature->GetGeometryRef();
			geos::geom::Polygon *geosPoly = vecUtils->convertOGRPolygon2GEOSPolygon(polygon);
			tmpValuePoly->setPolygon(geosPoly);
			data->push_back(tmpValuePoly);
		} 
		else if( geometryType == wkbMultiPolygon )
		{
			RSGISIntValuePolygon *tmpValuePoly = NULL;
			
            rsgis::geom::RSGISGeometry geomUtils;
			OGRMultiPolygon *mPolygon = (OGRMultiPolygon *) feature->GetGeometryRef();
			geos::geom::MultiPolygon *mGEOSPolygon = vecUtils->convertOGRMultiPolygonGEOSMultiPolygon(mPolygon);
            std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
			geomUtils.retrievePolygons(mGEOSPolygon, polys);
            std::vector<geos::geom::Polygon*>::iterator iterPolys = polys->begin();
			while(iterPolys != polys->end())
			{
				tmpValuePoly = new RSGISIntValuePolygon();
				tmpValuePoly->setValue(value);
				tmpValuePoly->setPolygon(*iterPolys);
				data->push_back(tmpValuePoly);
				iterPolys++;
			}
			delete polys;
		} 
		else
		{
			std::string message = std::string("Unsupport data type: ") + std::string(feature->GetGeometryRef()->getGeometryName());
			throw RSGISVectorException(message);
		}
		
		
	}
	
	void RSGISIntValuePolygonReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		// Nothing to do!
	}
	
	RSGISIntValuePolygonReader::~RSGISIntValuePolygonReader()
	{
		delete vecUtils;
	}
}}




