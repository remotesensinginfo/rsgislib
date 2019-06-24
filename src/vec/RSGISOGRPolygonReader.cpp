/*
 *  RSGISOGRPolygonReader.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/03/2010.
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

#include "RSGISOGRPolygonReader.h"

namespace rsgis{namespace vec{
	
	RSGISOGRPolygonReader::RSGISOGRPolygonReader(std::vector<OGRPolygon*> *polygons)
	{
		this->polygons = polygons;
	}
	
	void RSGISOGRPolygonReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISOGRPolygonReader::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
		
		if( geometryType == wkbPolygon )
		{
			OGRPolygon *polygon = (OGRPolygon *) feature->GetGeometryRef()->clone();
			polygons->push_back(polygon);
		} 
		else
		{
			std::string message = std::string("Unsupport data type: ") + std::string(feature->GetGeometryRef()->getGeometryName());
			throw RSGISVectorException(message);
		}
	}
	
	void RSGISOGRPolygonReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{
		// Nothing to do!
	}
	
	RSGISOGRPolygonReader::~RSGISOGRPolygonReader()
	{

	}
    
    
    RSGISOGRPolygonReader2Index::RSGISOGRPolygonReader2Index(geos::index::SpatialIndex *polysIdx)
    {
        this->polysIdx = polysIdx;
    }
    
    void RSGISOGRPolygonReader2Index::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
    {
        throw RSGISVectorException("Not implemented..");
    }
    
    void RSGISOGRPolygonReader2Index::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
    {
        OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
        
        if( geometryType == wkbPolygon )
        {
            RSGISGEOSGeomFID *geomObj = new RSGISGEOSGeomFID();
            geomObj->geom = feature->GetGeometryRef()->clone();
            geomObj->env = env;
            geomObj->fid = fid;
            this->polysIdx->insert(env, geomObj);
        }
        else
        {
            std::string message = std::string("Unsupport data type: ") + std::string(feature->GetGeometryRef()->getGeometryName());
            throw RSGISVectorException(message);
        }
    }
    
    void RSGISOGRPolygonReader2Index::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
    {
        // Nothing to do!
    }
    
    RSGISOGRPolygonReader2Index::~RSGISOGRPolygonReader2Index()
    {
        
    }
    
}}







