/*
 *  RSGISOptimiser2D.h
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

#ifndef RSGISOptimiser2D_H
#define RSGISOptimiser2D_H

#include <iostream>
#include <string>
#include <math.h>

#include "math/RSGISOptimisationFunction.h"
#include "math/RSGISOptimisationException.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/Envelope.h"

namespace rsgis{namespace math{
    
	class DllExport RSGISOptimiser2D
		{
		public:
			RSGISOptimiser2D(RSGISOptimisationFunction *func, bool maximise)
			{
				this->func = func;
				this->maximise = maximise;
			}
			virtual Coordinate* optimise(geos::geom::Coordinate *coord, double step, geos::geom::Envelope *boundary) throw(RSGISOptimisationException) = 0;
			virtual ~RSGISOptimiser2D(){};
		protected:
			RSGISOptimisationFunction *func;
			bool maximise;
		};
}}

#endif






