/*
 *  RSGISPolygonData.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2008.
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

#include "RSGISPolygonData.h"

namespace rsgis{namespace vec{

	RSGISPolygonData::RSGISPolygonData()
	{
		polygonType = true;
		multiPolygonGeom = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createMultiPolygon();
		polygonGeom = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createPolygon();
		envelope = NULL;
	}
	
	void RSGISPolygonData::readPolygon(OGRPolygon *polygon)
	{
		RSGISVectorUtils vecUtils;
		polygonType = true;
		
		if(polygonGeom != NULL)
		{
			rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(polygonGeom);
		}
		
		polygonGeom = vecUtils.convertOGRPolygon2GEOSPolygon(polygon);
		envelope = vecUtils.getEnvelope(polygonGeom);
	}
	
	void RSGISPolygonData::readMultiPolygon(OGRMultiPolygon *multipolygon)
	{
		RSGISVectorUtils vecUtils;
		polygonType = false;
		
		if(multiPolygonGeom != NULL)
		{
			rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(multiPolygonGeom);
		}
		
		multiPolygonGeom = vecUtils.convertOGRMultiPolygonGEOSMultiPolygon(multipolygon);
		envelope = vecUtils.getEnvelope(multiPolygonGeom);
	}
	
	double RSGISPolygonData::distance(RSGISPolygonData *data)
	{
        geos::geom::Geometry *thisGeom = NULL;
		geos::geom::Geometry *dataGeom = NULL;
		if(this->polygonType)
		{
			thisGeom = this->polygonGeom;
		}
		else
		{
			thisGeom = this->multiPolygonGeom;
		}
		
		if(data->polygonType)
		{
			dataGeom = data->polygonGeom;
		}
		else
		{
			dataGeom = data->multiPolygonGeom;
		}
		
		return thisGeom->distance(dataGeom);
	}
	
	void RSGISPolygonData::printGeometry()
	{
		if(polygonType)
		{
			std::cout << polygonGeom->toString() << std::endl;
		}
		else
		{
			std::cout << multiPolygonGeom->toString() << std::endl;
		}
	}
	
	bool RSGISPolygonData::contains(geos::geom::Coordinate *coord)
	{
		RSGISVectorUtils vecUtils;
		geos::geom::Geometry *geom = (geos::geom::Geometry *) vecUtils.createPoint(coord);
		
		bool contains = false;
		if(polygonType)
		{
			contains = polygonGeom->contains(geom);
		}
		else
		{
			contains = multiPolygonGeom->contains(geom);
		}
		delete geom;
		return contains;
	}

	bool RSGISPolygonData::contains(geos::geom::Geometry *geom)
	{
		bool contains = false;
		if(polygonType)
		{
			contains = polygonGeom->contains(geom);
		}
		else
		{
			contains = multiPolygonGeom->contains(geom);
		}
		return contains;
	}
	
	geos::geom::Envelope* RSGISPolygonData::getBBox()
	{
		return envelope;
	}
	
	geos::geom::Polygon* RSGISPolygonData::getPolygon()
	{
		return polygonGeom;
	}
	
	void RSGISPolygonData::setPolygon(geos::geom::Polygon *polygon)
	{
		polygonType = true;
		
		if(polygonGeom != NULL)
		{
			rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(polygonGeom);
		}
		
		this->polygonGeom = dynamic_cast<geos::geom::Polygon*>(rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createGeometry(polygon));
		
		//this->polygonGeom = (Polygon*) polygon->clone();
		RSGISVectorUtils vecUtils;
		delete envelope;
		envelope = vecUtils.getEnvelope(polygonGeom);
	}
	
	geos::geom::Geometry* RSGISPolygonData::getGeometry()
	{
		if(polygonType)
		{
			return polygonGeom;
		}
		return multiPolygonGeom;
	}
	
	RSGISPolygonData::~RSGISPolygonData()
	{
		delete envelope;
		if(polygonGeom != NULL)
		{
			rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(polygonGeom);
		}
		if(multiPolygonGeom != NULL)
		{
			rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(multiPolygonGeom);
		}
	}

}}

