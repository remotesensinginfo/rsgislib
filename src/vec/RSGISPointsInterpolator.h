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

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_vec_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace vec{
	
	class DllExport RSGISPointsInterpolator
		{
		public:
			RSGISPointsInterpolator(){};
			virtual void calcValue(rsgis::geom::point3D *pts, int numPoints, rsgis::geom::point2D *pt, double *outVal) throw(RSGISVectorException) = 0;
			virtual ~RSGISPointsInterpolator(){};
		};
	
	class DllExport RSGISInverseWeightedDistanceInterpolator : public RSGISPointsInterpolator
		{
		public:
			RSGISInverseWeightedDistanceInterpolator(double power);
			virtual void calcValue(rsgis::geom::point3D *pts, int numPoints, rsgis::geom::point2D *pt, double *outVal) throw(RSGISVectorException);
			virtual ~RSGISInverseWeightedDistanceInterpolator();
		protected:
			double power;
		};
}}

#endif
