/*
 *  RSGISClassificationPolygon.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2009.
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

#ifndef RSGISClassificationPolygon_H
#define RSGISClassificationPolygon_H

#include <iostream>
#include <string>

#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISPolygon.h"

#include "math/RSGISMathsUtils.h"

#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LineString.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis::geom;

namespace rsgis{namespace vec{
	
	class RSGISClassificationPolygon : public RSGISPolygon
		{
		public:
			RSGISClassificationPolygon();
			virtual void setClassification(string classification);
			virtual string getClassification();
			virtual ~RSGISClassificationPolygon();
		protected:
			string classification;
		};
}}

#endif


