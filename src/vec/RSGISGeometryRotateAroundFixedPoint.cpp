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
	
	RSGISGeometryRotateAroundFixedPoint::RSGISGeometryRotateAroundFixedPoint(Coordinate *fixedPt, float angle)
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
		RSGISMathsUtils mathUtils;
		cout.precision(10);
		//cout << "point called\n";
		
		double normPtx = point->getX() - fixedPt->x;
		double normPty = point->getY() - fixedPt->y;
		//cout << "NormPTx: " << normPtx << endl;
		//cout << "NormPTy: " << normPty << endl;
				
		if(normPtx != 0 | normPty != 0)
		{
			double distanceX = normPtx * normPtx;
			double distanceY = normPty * normPty;
			
			//cout << "distanceX: " << distanceX << endl;
			//cout << "distanceY: " << distanceY << endl;
			
			double radius = sqrt((distanceX + distanceY));
			//cout << "radius = " << radius << endl;
			
			double inAngle = acos(normPtx/radius);
			//cout << "inAngle = " << mathUtils.radiansToDegrees(inAngle) << endl;
			
			if(normPty < 0) 
			{
				inAngle = (M_PI*2) - inAngle;
			}
			
			//cout << "inAngle = " << mathUtils.radiansToDegrees(inAngle) << endl;
			
			double outAngle = inAngle + angle;
			//cout << "Output Angle: " << outAngle << endl;
			
			double circleX = radius * cos(outAngle);
			double circleY = radius * sin(outAngle);
			
			//cout << mathUtils.radiansToDegrees(outAngle) << "," << circleX << "," << circleY << "\n";
			
			double outX = fixedPt->x + circleX;
			double outY = fixedPt->y + circleY;
			//cout << outX << "," << outY << "\n";

			point->setX(outX);
			point->setY(outY);
		}
		/*else
		{
			cout << "No change\n";
			cout << "output: [" << point->getX() << "," << point->getY() << "]\n";
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



