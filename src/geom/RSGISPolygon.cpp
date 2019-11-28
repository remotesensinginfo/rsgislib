/*
 *  RSGISPolygon.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/07/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISPolygon.h"

namespace rsgis{namespace geom{
	
	RSGISPolygon::RSGISPolygon()
	{
		
	}
	
	RSGISPolygon::RSGISPolygon(geos::geom::Polygon *poly)
	{
		this->setPolygon(poly);
	}
	
	void RSGISPolygon::setPolygon(geos::geom::Polygon *poly)
	{
		this->poly = poly;
		this->point = new geos::geom::Coordinate();
		this->poly->getCentroid(*this->point);
				
		if(boost::math::isnan(this->point->x) | boost::math::isnan(this->point->y))
		{
			if(boost::math::isnan(this->point->x))
			{
				std::cout << "X has value NaN\n";
			}
			
			if(boost::math::isnan(this->point->y))
			{
				std::cout << "Y has value NaN\n";
			}
			
			const geos::geom::LineString *lineStringPoly = poly->getExteriorRing();
			const geos::geom::CoordinateSequence *coorSeq = lineStringPoly->getCoordinatesRO();
			int numPts = coorSeq->getSize();
			std::cout << "Polygon has " << poly->getNumPoints() << " points\n"; 
			std::cout << "CoordinateSequence has " << numPts << " points\n"; 
			
			geos::geom::Coordinate coord;
			for(int i = 0; i < numPts; i++)
			{
				coorSeq->getAt(i, coord);
				std::cout << i << ": [" << coord.x << "," << coord.y << "," << coord.z << "]\n";
			}
		}
	}
	
	geos::geom::Polygon* RSGISPolygon::getPolygon()
	{
		return this->poly;
	}
	
	void RSGISPolygon::getPoints(std::list<RSGIS2DPoint*> *pts)
	{
		if(poly != NULL)
		{
			geos::geom::CoordinateSequence *coords = poly->getCoordinates();
			geos::geom::Coordinate coord;
			RSGIS2DPoint *pt = NULL;
			for(unsigned int i = 0; i < coords->getSize(); i++)
			{
				coord = coords->getAt(i);
				pt = new RSGIS2DPoint(new geos::geom::Coordinate(coord.x, coord.y, coord.z));
				pts->push_back(pt);
			}
			delete coords;
		}
	}
	
	void RSGISPolygon::getPoints(std::list<RSGIS2DPoint*> *pts, unsigned int classID)
	{
		if(poly != NULL)
		{
			geos::geom::CoordinateSequence *coords = poly->getCoordinates();
			geos::geom::Coordinate coord;
			RSGIS2DPoint *pt = NULL;
			for(unsigned int i = 0; i < coords->getSize(); i++)
			{
				coord = coords->getAt(i);
				pt = new RSGIS2DPoint(new geos::geom::Coordinate(coord.x, coord.y, coord.z));
				pt->setClassID(classID);
				pts->push_back(pt);
			}
			delete coords;
		}
	}
    
    double RSGISPolygon::distance(RSGIS2DPoint *pt)
    {
        double dist = 0.0;
        if (typeid(*pt) == typeid(RSGISPolygon))
        {
            RSGISPolygon *poly = dynamic_cast<RSGISPolygon*>(pt);
            dist = poly->getPolygon()->distance(this->getPolygon());
        }
        else
        {
            geos::geom::Point *geosPt = pt->getAsGeosPoint();
            dist = this->getPolygon()->distance(geosPt);
            delete geosPt;
        }

        return dist;
    }
    
    double RSGISPolygon::distance(RSGISPolygon *poly)
    {
        double dist = poly->getPolygon()->distance(this->getPolygon());
        return dist;
    }
    
    double RSGISPolygon::distance(geos::geom::Coordinate *pt)
    {
        const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
        geos::geom::Point *geosPt = geomFactory->createPoint(*point);
        double dist = this->getPolygon()->distance(geosPt);
        delete geosPt;
        return dist;
    }
	
	RSGISPolygon::~RSGISPolygon()
	{
		delete poly;
	}
	
}}




