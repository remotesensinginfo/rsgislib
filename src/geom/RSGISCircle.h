/*
 *  RSGISCircle.h
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 17/06/2009.
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

#ifndef RSGISCircle_H
#define RSGISCircle_H

#include <string>
#include <iostream>
#include <math.h>

#include "geom/RSGISGeometryException.h"

#include "geom/RSGIS2DPoint.h"

#include "math/RSGISMathsUtils.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/Envelope.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_geom_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace geom{
	
	class DllExport RSGISCircle
		{
		public:
			RSGISCircle(RSGIS2DPoint *centre, double radius);
			RSGISCircle(RSGIS2DPoint *a, RSGIS2DPoint *b, RSGIS2DPoint *c) throw(RSGISGeometryException);
			void updateCircle(RSGIS2DPoint *a, RSGIS2DPoint *b, RSGIS2DPoint *c) throw(RSGISGeometryException);
			bool contained(RSGIS2DPoint *pt);
			bool containedBBOX(RSGIS2DPoint *pt);
			double getRadius();
			RSGIS2DPoint* getCentre();
			friend std::ostream& operator<<(std::ostream& ostr, const RSGISCircle& tri);
            std::ostream& operator<<(std::ostream& ostr);
			virtual ~RSGISCircle();
		protected:
			RSGIS2DPoint *centre;
			double radius;
			geos::geom::Envelope *bbox;
		};
}}

#endif


