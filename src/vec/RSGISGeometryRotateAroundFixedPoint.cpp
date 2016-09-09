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
	
	void RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRPolygon *polygon) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRMultiPolygon *multiPolygon) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRPoint *point) throw(RSGISVectorException)
	{
        rsgis::math::RSGISMathsUtils mathUtils;
		std::cout.precision(10);
		//std::cout << "point called\n";
		
		double normPtx = point->getX() - fixedPt->x;
		double normPty = point->getY() - fixedPt->y;
		//std::cout << "NormPTx: " << normPtx << std::endl;
		//std::cout << "NormPTy: " << normPty << std::endl;
				
		if(normPtx != 0 | normPty != 0)
		{
			double distanceX = normPtx * normPtx;
			double distanceY = normPty * normPty;
			
			//std::cout << "distanceX: " << distanceX << std::endl;
			//std::cout << "distanceY: " << distanceY << std::endl;
			
			double radius = sqrt((distanceX + distanceY));
			//std::cout << "radius = " << radius << std::endl;
			
			double inAngle = acos(normPtx/radius);
			//std::cout << "inAngle = " << mathUtils.radiansToDegrees(inAngle) << std::endl;
			
			if(normPty < 0) 
			{
				inAngle = (M_PI*2) - inAngle;
			}
			
			//std::cout << "inAngle = " << mathUtils.radiansToDegrees(inAngle) << std::endl;
			
			double outAngle = inAngle + angle;
			//std::cout << "Output Angle: " << outAngle << std::endl;
			
			double circleX = radius * cos(outAngle);
			double circleY = radius * sin(outAngle);
			
			//std::cout << mathUtils.radiansToDegrees(outAngle) << "," << circleX << "," << circleY << "\n";
			
			double outX = fixedPt->x + circleX;
			double outY = fixedPt->y + circleY;
			//std::cout << outX << "," << outY << "\n";

			point->setX(outX);
			point->setY(outY);
		}
		/*else
		{
			std::cout << "No change\n";
			std::cout << "output: [" << point->getX() << "," << point->getY() << "]\n";
		}*/
	}
	
	void RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRLineString *line) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	OGRPolygon* RSGISGeometryRotateAroundFixedPoint::processGeometry(OGRGeometry *geom) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	RSGISGeometryRotateAroundFixedPoint::~RSGISGeometryRotateAroundFixedPoint()
	{
		
	}
}}



