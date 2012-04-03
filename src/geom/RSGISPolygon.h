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

#include "geom/RSGIS2DPoint.h"

#include "math/RSGISMathsUtils.h"

#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"

#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LineString.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis::math;

namespace rsgis{namespace geom{
	
	class RSGISPolygon : public RSGIS2DPoint
		{
		public:
			RSGISPolygon();
			RSGISPolygon(Polygon *poly);
			virtual void setPolygon(Polygon *poly);
			virtual Polygon* getPolygon();
			virtual void getPoints(list<RSGIS2DPoint*> *pts);
			virtual void getPoints(list<RSGIS2DPoint*> *pts, unsigned int classID);
			virtual ~RSGISPolygon();
		protected:
			Polygon *poly;
		};
}}

#endif


