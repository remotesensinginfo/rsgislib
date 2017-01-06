/*
 *  RSGISPolygon.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/07/2009.
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

#ifndef RSGISPolygon_H
#define RSGISPolygon_H

#include <iostream>
#include <string>
#include <list>
#include <math.h>

#include "geom/RSGIS2DPoint.h"

#include "math/RSGISMathsUtils.h"

#include "utils/RSGISGEOSFactoryGenerator.h"

#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"

#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LineString.h"

#include <boost/math/special_functions/fpclassify.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
	
	class DllExport RSGISPolygon : public RSGIS2DPoint
		{
		public:
			RSGISPolygon();
			RSGISPolygon(geos::geom::Polygon *poly);
			virtual void setPolygon(geos::geom::Polygon *poly);
			virtual geos::geom::Polygon* getPolygon();
			virtual void getPoints(std::list<RSGIS2DPoint*> *pts);
			virtual void getPoints(std::list<RSGIS2DPoint*> *pts, unsigned int classID);
            virtual double distance(RSGIS2DPoint *pt);
            virtual double distance(RSGISPolygon *poly);
            virtual double distance(geos::geom::Coordinate *pt);
			virtual ~RSGISPolygon();
		protected:
			geos::geom::Polygon *poly;
		};
}}

#endif


