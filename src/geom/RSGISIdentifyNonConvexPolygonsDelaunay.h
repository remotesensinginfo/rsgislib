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
	
	class DllExport RSGISIdentifyNonConvexPolygonsDelaunay : public RSGISIdentifyNonConvexPolygons
		{
		public:
			RSGISIdentifyNonConvexPolygonsDelaunay();
			virtual std::vector<geos::geom::Polygon*>* retrievePolygons(std::list<RSGIS2DPoint*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual std::vector<geos::geom::Polygon*>* retrievePolygons(std::list<RSGISPolygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual std::vector<geos::geom::Polygon*>* retrievePolygons(std::list<geos::geom::Polygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual geos::geom::Polygon* retrievePolygon(std::vector<geos::geom::Polygon*> *polygons) throw(RSGISGeometryException);
			virtual geos::geom::Polygon* retrievePolygon(std::list<geos::geom::Polygon*> *polygons) throw(RSGISGeometryException);
			virtual ~RSGISIdentifyNonConvexPolygonsDelaunay();
		private:
			std::vector<geos::geom::Polygon*>* findPolygonsFromCoordinates(std::list<RSGIS2DPoint*> *pts, int numClusters) throw(RSGISGeometryException);
			geos::geom::Polygon* findPolygonFromTriangles(std::list<RSGISTriangle*> *cluster, int id, bool tryAgain) throw(RSGISGeometryException);
			void plotTriangulationAsEdges(std::string filename, std::list<RSGISTriangle*> *triangleList);
			void getPoints(geos::geom::Polygon *poly, std::list<RSGIS2DPoint*> *pts, unsigned int classID);
		};
}}

#endif




