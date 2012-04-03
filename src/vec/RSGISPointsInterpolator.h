/*
 *  RSGISPointsInterpolator.h
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

#ifndef RSGISPointsInterpolator_H
#define RSGISPointsInterpolator_H

#include <iostream>
#include <string>
#include "geom/RSGISGeometry.h"
#include "common/RSGISVectorException.h"

using namespace std;
using namespace rsgis::geom;
using namespace rsgis;

namespace rsgis{namespace vec{
	
	class RSGISPointsInterpolator
		{
		public:
			RSGISPointsInterpolator(){};
			virtual void calcValue(point3D *pts, int numPoints, point2D *pt, double *outVal) throw(RSGISVectorException) = 0;
			virtual ~RSGISPointsInterpolator(){};
		};
	
	class RSGISInverseWeightedDistanceInterpolator : public RSGISPointsInterpolator
		{
		public:
			RSGISInverseWeightedDistanceInterpolator(double power);
			virtual void calcValue(point3D *pts, int numPoints, point2D *pt, double *outVal) throw(RSGISVectorException);
			virtual ~RSGISInverseWeightedDistanceInterpolator();
		protected:
			double power;
		};
}}

#endif
