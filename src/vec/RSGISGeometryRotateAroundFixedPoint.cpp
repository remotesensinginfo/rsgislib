/*
 *  RSGISGeometryRotateAroundFixedPoint.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/04/2009.
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

#include "RSGISGeometryRotateAroundFixedPoint.h"

namespace rsgis{namespace vec{
	
	RSGISGeometryRotateAroundFixedPoint::RSGISGeometryRotateAroundFixedPoint(geos::geom::Coordinate *fixedPt, float angle)
	{
		this->fixedPt = fixedPt;
		this->angle = angle;
	}
	
	void RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRPolygon *polygon)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRMultiPolygon *multiPolygon)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRPoint *point)
	{
        rsgis::math::RSGISMathsUtils mathUtils;
		
		double normPtx = point->getX() - fixedPt->x;
		double normPty = point->getY() - fixedPt->y;
				
		if(normPtx != 0 | normPty != 0)
		{
			double distanceX = normPtx * normPtx;
			double distanceY = normPty * normPty;
			double radius = sqrt((distanceX + distanceY));
			double inAngle = acos(normPtx/radius);
			
			if(normPty < 0) 
			{
				inAngle = (M_PI*2) - inAngle;
			}
			double outAngle = inAngle + angle;
			
			double circleX = radius * cos(outAngle);
			double circleY = radius * sin(outAngle);
						
			double outX = fixedPt->x + circleX;
			double outY = fixedPt->y + circleY;

			point->setX(outX);
			point->setY(outY);
		}
	}
	
	void RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRLineString *line)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	OGRPolygon* RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRGeometry *geom)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	RSGISGeometryRotateAroundFixedPoint::~RSGISGeometryRotateAroundFixedPoint()
	{
		
	}
}}



