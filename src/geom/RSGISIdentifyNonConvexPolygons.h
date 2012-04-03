/*
 *  RSGISIdentifyNonConvexPolygons.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/07/2009.
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

#ifndef RSGISIdentifyNonConvexPolygons_H
#define RSGISIdentifyNonConvexPolygons_H

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include "geom/RSGISPolygon.h"
#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISGeometryException.h"

#include "geos/geom/Polygon.h"

#include "utils/RSGISGEOSFactoryGenerator.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis::utils;

namespace rsgis{namespace geom{
	
	class RSGISIdentifyNonConvexPolygons
		{
		public:
			virtual vector<Polygon*>* retrievePolygons(list<RSGIS2DPoint*> **clusters, int numClusters) throw(RSGISGeometryException) = 0;
			virtual vector<Polygon*>* retrievePolygons(list<RSGISPolygon*> **clusters, int numClusters) throw(RSGISGeometryException) = 0;
			virtual vector<Polygon*>* retrievePolygons(list<Polygon*> **clusters, int numClusters) throw(RSGISGeometryException) = 0;
			virtual Polygon* retrievePolygon(vector<Polygon*> *polygons) throw(RSGISGeometryException) = 0;
			virtual Polygon* retrievePolygon(list<Polygon*> *polygons) throw(RSGISGeometryException) = 0;
			virtual ~RSGISIdentifyNonConvexPolygons(){};
		};
}}

#endif


