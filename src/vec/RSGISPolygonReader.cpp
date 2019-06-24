/*
 *  RSGISPolygonReader.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/07/2009.
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

#include "RSGISPolygonReader.h"


namespace rsgis{namespace vec{
	
	RSGISPolygonReader::RSGISPolygonReader(std::list<rsgis::geom::RSGIS2DPoint*> *data)
	{
		vecUtils = new RSGISVectorUtils();
		this->dataList = data;
		this->listtype = true;
	}
	
	RSGISPolygonReader::RSGISPolygonReader(std::vector<rsgis::geom::RSGIS2DPoint*> *data)
	{
		vecUtils = new RSGISVectorUtils();
		this->dataVector= data;
		this->listtype = false;
	}
	
	void RSGISPolygonReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISPolygonReader::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		
		OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
		if( geometryType == wkbPolygon )
		{
			rsgis::geom::RSGISPolygon *tmpPoly = new rsgis::geom::RSGISPolygon();
			OGRPolygon *polygon = (OGRPolygon *) feature->GetGeometryRef();
			tmpPoly->setPolygon(vecUtils->convertOGRPolygon2GEOSPolygon(polygon));
			if(listtype)
			{
				dataList->push_back(tmpPoly);
			}
			else
			{
				dataVector->push_back(tmpPoly);
			}
		} 
		else if( geometryType == wkbMultiPolygon )
		{
			rsgis::geom::RSGISPolygon *tmpPoly = NULL;
			
			rsgis::geom::RSGISGeometry geomUtils;
			OGRMultiPolygon *mPolygon = (OGRMultiPolygon *) feature->GetGeometryRef();

            geos::geom::MultiPolygon *mGEOSPolygon = vecUtils->convertOGRMultiPolygonGEOSMultiPolygon(mPolygon);
            std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
			geomUtils.retrievePolygons(mGEOSPolygon, polys);
            std::vector<geos::geom::Polygon*>::iterator iterPolys = polys->begin();
			while(iterPolys != polys->end())
			{
				tmpPoly = new rsgis::geom::RSGISPolygon();
				tmpPoly->setPolygon(*iterPolys);
				if(listtype)
				{
					dataList->push_back(tmpPoly);
				}
				else
				{
					dataVector->push_back(tmpPoly);
				}
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
	
	void RSGISPolygonReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{
		// Nothing to do!
	}
	
	RSGISPolygonReader::~RSGISPolygonReader()
	{
		delete vecUtils;
	}
    

    
    
    
    
    
    
    
    RSGISPointReader::RSGISPointReader(std::vector<rsgis::geom::RSGIS2DPoint*> *data)
    {
        this->dataVector= data;
    }
    
    void RSGISPointReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
    {
        throw RSGISVectorException("Not implemented..");
    }
    
    void RSGISPointReader::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
    {
        
        OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
        if( geometryType == wkbPoint )
        {
            OGRPoint *pt = (OGRPoint *) feature->GetGeometryRef();
            geos::geom::Coordinate *coord = new geos::geom::Coordinate(pt->getX(), pt->getY());
            rsgis::geom::RSGIS2DPoint *tmpPt = new rsgis::geom::RSGIS2DPoint(coord);
            dataVector->push_back(tmpPt);
        }
        else
        {
            std::string message = std::string("Unsupport data type: ") + std::string(feature->GetGeometryRef()->getGeometryName());
            throw RSGISVectorException(message);
        }
        
    }
    
    void RSGISPointReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
    {
        // Nothing to do!
    }
    
    RSGISPointReader::~RSGISPointReader()
    {

    }
}}





