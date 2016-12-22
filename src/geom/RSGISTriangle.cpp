/*
 *  RSGISTriangle.cpp
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 16/06/2009.
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

#include "RSGISTriangle.h"

namespace rsgis{namespace geom{
	
	RSGISTriangle::RSGISTriangle(RSGIS2DPoint *p1, RSGIS2DPoint *p2, RSGIS2DPoint *p3)
	{
		this->setTriangleVertex(p1, p2, p3);
		
		double minX = this->a->getPoint()->x;
		double maxX = this->a->getPoint()->x;
		double minY = this->a->getPoint()->y;
		double maxY = this->a->getPoint()->y;
		
		// X
		if(this->b->getPoint()->x < minX)
		{
			minX = this->b->getPoint()->x;
		}
		else if(this->b->getPoint()->x > maxX)
		{
			maxX = this->b->getPoint()->x;
		}
		
		if(this->c->getPoint()->x < minX)
		{
			minX = this->c->getPoint()->x;
		}
		else if(this->c->getPoint()->x > maxX)
		{
			maxX = this->c->getPoint()->x;
		}
		
		// Y
		if(this->b->getPoint()->y < minY)
		{
			minY = this->b->getPoint()->y;
		}
		else if(this->b->getPoint()->y > maxY)
		{
			maxY = this->b->getPoint()->y;
		}
		
		if(this->c->getPoint()->y < minY)
		{
			minY = this->c->getPoint()->y;
		}
		else if(this->c->getPoint()->y > maxY)
		{
			maxY = this->c->getPoint()->y;
		}
		
		bbox = new geos::geom::Envelope(minX, maxX, minY, maxY);
		circle = new RSGISCircle(a, b, c);
	}
	
	void RSGISTriangle::updateTriangle(RSGIS2DPoint *p1, RSGIS2DPoint *p2, RSGIS2DPoint *p3)
	{
		delete a;
		delete b;
		delete c;
		this->setTriangleVertex(p1, p2, p3);
		
		double minX = this->a->getPoint()->x;
		double maxX = this->a->getPoint()->x;
		double minY = this->a->getPoint()->y;
		double maxY = this->a->getPoint()->y;
		
		// X
		if(this->b->getPoint()->x < minX)
		{
			minX = this->b->getPoint()->x;
		}
		else if(this->b->getPoint()->x > maxX)
		{
			maxX = this->b->getPoint()->x;
		}
		
		if(this->c->getPoint()->x < minX)
		{
			minX = this->c->getPoint()->x;
		}
		else if(this->c->getPoint()->x > maxX)
		{
			maxX = this->c->getPoint()->x;
		}
		
		// Y
		if(this->b->getPoint()->y < minY)
		{
			minY = this->b->getPoint()->y;
		}
		else if(this->b->getPoint()->y > maxY)
		{
			maxY = this->b->getPoint()->y;
		}
		
		if(this->c->getPoint()->y < minY)
		{
			minY = this->c->getPoint()->y;
		}
		else if(this->c->getPoint()->y > maxY)
		{
			maxY = this->c->getPoint()->y;
		}
		delete bbox;
		delete circle;
		bbox = new geos::geom::Envelope(minX, maxX, minY, maxY);
		circle = new RSGISCircle(a, b, c);
	}
	
	bool RSGISTriangle::contained(RSGIS2DPoint *pt)
	{
		/// Calculates whether a point is contained within a triangle
		/**
		 * For an explantion of Barycentric Coordinates see <a href="http://mathworld.wolfram.com/BarycentricCoordinates.html">
		 * while notes from <a href="http://www.blackpawn.com/texts/pointinpoly/default.html"> where used for reference.
		 */
		bool returnValue = false;
		
        rsgis::math::RSGISVectors vecUtils;
		
		// C - A
		gsl_vector *v0 = gsl_vector_alloc(2);
		gsl_vector_set(v0, 0, (c->getPoint()->x - a->getPoint()->x));
		gsl_vector_set(v0, 1, (c->getPoint()->y - a->getPoint()->y));
		// B - A
		gsl_vector *v1 = gsl_vector_alloc(2);
		gsl_vector_set(v1, 0, (b->getPoint()->x - a->getPoint()->x));
		gsl_vector_set(v1, 1, (b->getPoint()->y - a->getPoint()->y));
		// P - A
		gsl_vector *v2 = gsl_vector_alloc(2);
		gsl_vector_set(v2, 0, (pt->getPoint()->x - a->getPoint()->x));
		gsl_vector_set(v2, 1, (pt->getPoint()->y - a->getPoint()->y));
		
		double dot00 = vecUtils.dotProductVectorVectorGSL(v0, v0);
		double dot01 = vecUtils.dotProductVectorVectorGSL(v0, v1);
		double dot02 = vecUtils.dotProductVectorVectorGSL(v0, v2);
		double dot11 = vecUtils.dotProductVectorVectorGSL(v1, v1);
		double dot12 = vecUtils.dotProductVectorVectorGSL(v1, v2);
		
		double invDenom = 1 / ((dot00 * dot11) - (dot01 * dot01));
		double u = ((dot11 * dot02) - (dot01 * dot12)) * invDenom;
		double v = ((dot00 * dot12) - (dot01 * dot02)) * invDenom;
		
		if( (u > 0) && (v > 0) && ((u+v) < 1))
		{
			returnValue = true;
		}
		
		gsl_vector_free(v0);
		gsl_vector_free(v1);
		gsl_vector_free(v2);
		
		return returnValue;
	}
	
	bool RSGISTriangle::containedBBOX(RSGIS2DPoint *pt)
	{
		return bbox->contains(*pt->getPoint());
	}
	
	bool RSGISTriangle::containedCircle(RSGIS2DPoint *pt)
	{
		return circle->contained(pt);
	}
	
	geos::geom::Envelope* RSGISTriangle::getBBox()
	{
		return bbox;
	}
	
	RSGISCircle* RSGISTriangle::getCircle()
	{
		return circle;
	}
	
	RSGIS2DPoint* RSGISTriangle::getPointA()
	{
		return a;
	}
	
	RSGIS2DPoint* RSGISTriangle::getPointB()
	{
		return b;
	}
	
	RSGIS2DPoint* RSGISTriangle::getPointC()
	{
		return c;
	}
	
	double RSGISTriangle::getArea()
	{
		double xMin = 0;
		double xMax = 0;
		double yMin = 0;
		double yMax = 0;
		
		// Get Min and Max X values
		
		if(a->getX() < b->getX() & a->getX() < c->getX())
		{
			xMin = a->getX();
		}
		else if(b->getX() < c->getX())
		{
			xMin = b->getX();
		}
		else
		{
			xMin = c->getX();
		}
		
		if(a->getX() > b->getX() & a->getX() > c->getX())
		{
			xMax = a->getX();
		}
		else if(b->getX() > c->getX())
		{
			xMax = b->getX();
		}
		else
		{
			xMax = c->getX();
		}
		
		// Get min and max Y values
		
		if(a->getY() < b->getY() & a->getY() < c->getY())
		{
			yMin = a->getY();
		}
		else if(b->getY() < c->getY())
		{
			yMin = b->getY();
		}
		else
		{
			yMin = c->getY();
		}
		
		if(a->getY() > b->getY() & a->getY() > c->getY())
		{
			yMax = a->getY();
		}
		else if(b->getY() > c->getY())
		{
			yMax = b->getY();
		}
		else
		{
			yMax = c->getY();
		}
		
		double area = ((xMax - xMin) * (yMax - yMin))/2;
		return area;
			
	}
	
	geos::geom::Polygon* RSGISTriangle::createPolygon()
	{
		geos::geom::CoordinateSequence *coords = new geos::geom::CoordinateArraySequence();
		coords->add(geos::geom::Coordinate(a->getX(), a->getY(), a->getZ()));
		coords->add(geos::geom::Coordinate(b->getX(), b->getY(), b->getZ()));
		coords->add(geos::geom::Coordinate(c->getX(), c->getY(), c->getZ()));
		coords->add(geos::geom::Coordinate(a->getX(), a->getY(), a->getZ()));
		
		geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel();
		geos::geom::GeometryFactory *geomFac = new geos::geom::GeometryFactory(pm);
		geos::geom::LinearRing *lr = geomFac->createLinearRing(coords);
		
		geos::geom::Polygon *polyTri = geomFac->createPolygon(lr, NULL);
		
		return polyTri;
	}
	
    std::ostream& operator<<(std::ostream& ostr, const RSGISTriangle& tri)
	{
		ostr << "TRIANGLE[[" << tri.a->getPoint()->x << "," << tri.a->getPoint()->y << "][" << tri.b->getPoint()->x << "," << tri.b->getPoint()->y << "][" << tri.c->getPoint()->x << "," << tri.c->getPoint()->y << "]]";
		return ostr;
	}
	
    std::ostream& RSGISTriangle::operator<<(std::ostream& ostr)
	{
		ostr << "TRIANGLE[[" << a->getPoint()->x << "," << a->getPoint()->y << "][" << b->getPoint()->x << "," << b->getPoint()->y << "][" << c->getPoint()->x << "," << c->getPoint()->y << "]]";
		return ostr;
	}
	
	
	void RSGISTriangle::setTriangleVertex(RSGIS2DPoint *p1, RSGIS2DPoint *p2, RSGIS2DPoint *p3)
	{
		RSGIS2DPoint *tmp = p1;
		double maxY = p1->getY();
		
		if(p2->getY() > maxY)
		{
			tmp = p2;
		}
		if(p3->getY() > maxY)
		{
			tmp = p3;
		}
		
		if(p1 == tmp)
		{
			this->a = p1;
			if(p2->getX() > p3->getX())
			{
				this->b = p2;
				this->c = p3;
			}
			else
			{
				this->b = p3;
				this->c = p2;
			}
		}
		else if(p2 == tmp)
		{
			this->a = p2;
			if(p1->getX() > p3->getX())
			{
				this->b = p1;
				this->c = p3;
			}
			else
			{
				this->b = p3;
				this->c = p1;
			}
			
		}
		else
		{
			this->a = p3;
			if(p1->getX() > p2->getX())
			{
				this->b = p1;
				this->c = p2;
			}
			else
			{
				this->b = p2;
				this->c = p1;
			}
		}
		
		
	}
	
	RSGISTriangle::~RSGISTriangle()
	{
		delete bbox;
		delete circle;
	}
}}

