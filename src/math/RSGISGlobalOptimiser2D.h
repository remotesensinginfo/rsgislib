/*
 *  RSGISGlobalOptimiser2D.h
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

#ifndef RSGISGlobalOptimiser2D_H
#define RSGISGlobalOptimiser2D_H

#include <iostream>
#include <string>
#include <math.h>

#include "math/RSGISGlobalOptimisationFunction.h"
#include "math/RSGISOptimisationException.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/Envelope.h"

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
	
	class DllExport RSGISGlobalOptimiser2D
		{
		public:
			RSGISGlobalOptimiser2D(RSGISGlobalOptimisationFunction *func, bool maximise)
			{
				this->func = func;
				this->maximise = maximise;
			}
			virtual std::vector<geos::geom::Coordinate*>* optimise4Neighbor(std::vector<geos::geom::Coordinate*> *coords, double step, geos::geom::Envelope *boundary) = 0;
			virtual std::vector<geos::geom::Coordinate*>* optimise8Neighbor(std::vector<geos::geom::Coordinate*> *coords, double step, geos::geom::Envelope *boundary) = 0;
			virtual ~RSGISGlobalOptimiser2D(){};
		protected:
			RSGISGlobalOptimisationFunction *func;
			bool maximise;
		};
}}

#endif




