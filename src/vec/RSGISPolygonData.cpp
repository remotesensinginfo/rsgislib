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
		multiPolygonGeom = RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createMultiPolygon();
		polygonGeom = RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createPolygon();
		envelope = NULL;
	}
	
	void RSGISPolygonData::readPolygon(OGRPolygon *polygon)
	{
		RSGISVectorUtils vecUtils;
		polygonType = true;
		
		if(polygonGeom != NULL)
		{
			RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(polygonGeom);
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
			RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(multiPolygonGeom);
		}
		
		multiPolygonGeom = vecUtils.convertOGRMultiPolygonGEOSMultiPolygon(multipolygon);
		envelope = vecUtils.getEnvelope(multiPolygonGeom);
	}
	
	double RSGISPolygonData::distance(RSGISPolygonData *data)
	{
		Geometry *thisGeom = NULL;
		Geometry *dataGeom = NULL;
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
			cout << polygonGeom->toString() << endl;
		}
		else
		{
			cout << multiPolygonGeom->toString() << endl;
		}
	}
	
	bool RSGISPolygonData::contains(Coordinate *coord)
	{
		RSGISVectorUtils vecUtils;
		Geometry *geom = (Geometry *) vecUtils.createPoint(coord);
		
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

	bool RSGISPolygonData::contains(Geometry *geom)
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
	
	Envelope* RSGISPolygonData::getBBox()
	{
		return envelope;
	}
	
	Polygon* RSGISPolygonData::getPolygon()
	{
		return polygonGeom;
	}
	
	void RSGISPolygonData::setPolygon(Polygon *polygon)
	{
		polygonType = true;
		
		if(polygonGeom != NULL)
		{
			RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(polygonGeom);
		}
		
		this->polygonGeom = dynamic_cast<Polygon*>(RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createGeometry(polygon));
		
		//this->polygonGeom = (Polygon*) polygon->clone();
		RSGISVectorUtils vecUtils;
		delete envelope;
		envelope = vecUtils.getEnvelope(polygonGeom);
	}
	
	Geometry* RSGISPolygonData::getGeometry()
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
			RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(polygonGeom);
		}
		if(multiPolygonGeom != NULL)
		{
			RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(multiPolygonGeom);
		}
	}

}}

