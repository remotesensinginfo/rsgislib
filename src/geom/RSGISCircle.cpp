/*
 *  RSGISCircle.cpp
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 17/06/2009.
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

#include "RSGISCircle.h"


namespace rsgis{namespace geom{
	
	RSGISCircle::RSGISCircle(RSGIS2DPoint *centre, double radius)
	{
		this->centre = centre;
		this->radius = radius;
		
		double minX = centre->getPoint()->x - radius;
		double maxX = centre->getPoint()->x + radius;
		double minY = centre->getPoint()->y - radius;
		double maxY = centre->getPoint()->y + radius;
		bbox = new geos::geom::Envelope(minX, maxX, minY, maxY);
	}
	
	RSGISCircle::RSGISCircle(RSGIS2DPoint *a, RSGIS2DPoint *b, RSGIS2DPoint *c)
	{
		this->updateCircle(a, b, c);
		
		double minX = centre->getPoint()->x - radius;
		double maxX = centre->getPoint()->x + radius;
		double minY = centre->getPoint()->y - radius;
		double maxY = centre->getPoint()->y + radius;
		bbox = new geos::geom::Envelope(minX, maxX, minY, maxY);
	}
	
	void RSGISCircle::updateCircle(RSGIS2DPoint *a, RSGIS2DPoint *b, RSGIS2DPoint *c)
	{
        geos::geom::Coordinate *centreCoord = new geos::geom::Coordinate();
        // Centre X
        // Upper Part
        double tmpU1 = (c->getPoint()->y - b->getPoint()->y)*((a->getPoint()->x*a->getPoint()->x)+(a->getPoint()->y*a->getPoint()->y));
        double tmpU2 = (a->getPoint()->y - c->getPoint()->y)*((b->getPoint()->x*b->getPoint()->x)+(b->getPoint()->y*b->getPoint()->y));
        double tmpU3 = (b->getPoint()->y - a->getPoint()->y)*((c->getPoint()->x*c->getPoint()->x)+(c->getPoint()->y*c->getPoint()->y));
        // Lower Part
        double tmpL1 = (c->getPoint()->x - b->getPoint()->x)*(b->getPoint()->y - a->getPoint()->y);
        double tmpL2 = (b->getPoint()->x - a->getPoint()->x)*(c->getPoint()->y - b->getPoint()->y);
        double tmpL = (tmpL1-tmpL2);
        
        if(tmpL == 0)
        {
            rsgis::math::RSGISMathsUtils mathsUtils;
            std::string message  = std::string("No circle cannot be identified. a : [") 
							+ mathsUtils.floattostring(a->getX()) + std::string(",") 
							+ mathsUtils.floattostring(a->getY()) + std::string("] b: [") 
							+ mathsUtils.floattostring(b->getX()) + std::string(",") 
							+ mathsUtils.floattostring(b->getY()) + std::string("] c: [") 
							+ mathsUtils.floattostring(c->getX()) + std::string(",") 
							+ mathsUtils.floattostring(c->getY()) + std::string("]\n");
            throw RSGISGeometryException(message);
        }
        
        centreCoord->x = (tmpU1 + tmpU2 + tmpU3) / (2 * tmpL);
        
        // Centre Y
        // Upper Part
        tmpU1 = (c->getPoint()->x - b->getPoint()->x)*((a->getPoint()->x*a->getPoint()->x)+(a->getPoint()->y*a->getPoint()->y));
        tmpU2 = (a->getPoint()->x - c->getPoint()->x)*((b->getPoint()->x*b->getPoint()->x)+(b->getPoint()->y*b->getPoint()->y));
        tmpU3 = (b->getPoint()->x - a->getPoint()->x)*((c->getPoint()->x*c->getPoint()->x)+(c->getPoint()->y*c->getPoint()->y));
        
        centreCoord->y = ((tmpU1 + tmpU2 + tmpU3) / (2 * tmpL)) * (-1);
        
        radius = sqrt(((centreCoord->x - a->getPoint()->x) * (centreCoord->x - a->getPoint()->x)) + 
					  ((centreCoord->y - a->getPoint()->y) * (centreCoord->y - a->getPoint()->y)));
		
		centre = new RSGIS2DPoint(centreCoord);
	}
	
	bool RSGISCircle::contained(RSGIS2DPoint *pt)
	{
		double distance = centre->distance(pt);
		if(distance < radius)
		{
			return true;
		}
		return false;
	}
	
	bool RSGISCircle::containedBBOX(RSGIS2DPoint *pt)
	{
		return bbox->contains(*pt->getPoint());
	}
	
	double RSGISCircle::getRadius()
	{
		return radius;
	}
	
	RSGIS2DPoint* RSGISCircle::getCentre()
	{
		return centre;
	}
	
    std::ostream& operator<<(std::ostream& ostr, const RSGISCircle& circle)
	{
		ostr << "CIRCLE[" << circle.centre->getPoint()->x << "," << circle.centre->getPoint()->y << "]: " << circle.radius;
        return ostr;
	}
	
    std::ostream& RSGISCircle::operator<<(std::ostream& ostr)
	{
		ostr << "CIRCLE[" << centre->getPoint()->x << "," << centre->getPoint()->y << "]: " << radius;
		return ostr;
	}
	
	RSGISCircle::~RSGISCircle()
	{
		delete centre;
		delete bbox;
	}
}}


