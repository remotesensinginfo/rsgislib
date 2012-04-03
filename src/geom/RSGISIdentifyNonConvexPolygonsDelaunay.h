/*
 *  RSGISIdentifyNonConvexPolygonsDelaunay.h
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

#ifndef RSGISIdentifyNonConvexPolygonsDelaunay_H
#define RSGISIdentifyNonConvexPolygonsDelaunay_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#include "geom/RSGISPolygon.h"
#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISGeometryException.h"
#include "geom/RSGISGeometry.h"
#include "geom/RSGISIdentifyNonConvexPolygons.h"
#include "geom/RSGISDelaunayTriangulation.h"
#include "geom/RSGISGeomTestExport.h"

#include "geos/geom/Polygon.h"
#include "geos/geom/LineSegment.h"

#include "math/RSGISMathsUtils.h"

#include "utils/RSGISExportForPlotting.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis::utils;
using namespace rsgis::math;

namespace rsgis{namespace geom{
	
	class RSGISIdentifyNonConvexPolygonsDelaunay : public RSGISIdentifyNonConvexPolygons
		{
		public:
			RSGISIdentifyNonConvexPolygonsDelaunay();
			virtual vector<Polygon*>* retrievePolygons(list<RSGIS2DPoint*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual vector<Polygon*>* retrievePolygons(list<RSGISPolygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual vector<Polygon*>* retrievePolygons(list<Polygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual Polygon* retrievePolygon(vector<Polygon*> *polygons) throw(RSGISGeometryException);
			virtual Polygon* retrievePolygon(list<Polygon*> *polygons) throw(RSGISGeometryException);
			virtual ~RSGISIdentifyNonConvexPolygonsDelaunay();
		private:
			vector<Polygon*>* findPolygonsFromCoordinates(list<RSGIS2DPoint*> *pts, int numClusters) throw(RSGISGeometryException);
			Polygon* findPolygonFromTriangles(list<RSGISTriangle*> *cluster, int id, bool tryAgain) throw(RSGISGeometryException);
			void plotTriangulationAsEdges(string filename, list<RSGISTriangle*> *triangleList);
			void getPoints(Polygon *poly, list<RSGIS2DPoint*> *pts, unsigned int classID);
		};
}}

#endif




