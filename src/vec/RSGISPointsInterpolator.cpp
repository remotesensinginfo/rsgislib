/*
 *  RSGISPointsInterpolator.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/06/2008.
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

#include "RSGISPointsInterpolator.h"

namespace rsgis{namespace vec{
	
	RSGISInverseWeightedDistanceInterpolator::RSGISInverseWeightedDistanceInterpolator(double power) : RSGISPointsInterpolator()
	{
		this->power = power;
	}
	
	void RSGISInverseWeightedDistanceInterpolator::calcValue(rsgis::geom::point3D *pts, int numPoints, rsgis::geom::point2D *pt, double *outVal)
	{
		if(numPoints == 0)
		{
			*outVal = 0;
		}
		else
		{
            rsgis::geom::RSGISGeometry *geomUtils = new rsgis::geom::RSGISGeometry();
			double sumWeightedDistance = 0;
			double weightedDistance = 0;
			double sumWeightedValue = 0;
			for(int i = 0; i < numPoints; i++)
			{
				weightedDistance = (1/(pow(geomUtils->distance2D(&pts[i], pt), power)));
				sumWeightedDistance += weightedDistance;
				sumWeightedValue += (weightedDistance * pts[i].z);
			}
			delete geomUtils;
			
			*outVal = sumWeightedValue/sumWeightedDistance;
		}
	}
	
	RSGISInverseWeightedDistanceInterpolator::~RSGISInverseWeightedDistanceInterpolator()
	{
		
	}
}}

