/*
 *  RSGISOptimisationFunction.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/08/2009.
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

#ifndef RSGISOptimisationFunction_H
#define RSGISOptimisationFunction_H

#include <iostream>
#include <string>
#include <math.h>

#include "math/RSGISOptimisationException.h"

#include "geos/geom/Coordinate.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace math{
    
	class DllExport RSGISOptimisationFunction
		{
		public:
			virtual double calcValue(geos::geom::Coordinate *coord) throw(RSGISOptimisationException) = 0;
			virtual ~RSGISOptimisationFunction(){};
		};
}}

#endif




