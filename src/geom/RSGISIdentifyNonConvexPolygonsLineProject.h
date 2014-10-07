/*
 *  RSGISIdentifyNonConvexPolygonsLineProject.h
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

#ifndef RSGISIdentifyNonConvexPolygonsLineProject_H
#define RSGISIdentifyNonConvexPolygonsLineProject_H

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include "geom/RSGISPolygon.h"
#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISGeometryException.h"
#include "geom/RSGISGeometry.h"
#include "geom/RSGISIdentifyNonConvexPolygons.h"

#include "geos/geom/Polygon.h"
#include "geos/geom/LineSegment.h"

namespace rsgis{namespace geom{
	
	class DllExport RSGISIdentifyNonConvexPolygonsLineProject : public RSGISIdentifyNonConvexPolygons
		{
		public:
			RSGISIdentifyNonConvexPolygonsLineProject(float resolution);
			virtual std::vector<geos::geom::Polygon*>* retrievePolygons(std::list<RSGIS2DPoint*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual std::vector<geos::geom::Polygon*>* retrievePolygons(std::list<RSGISPolygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual std::vector<geos::geom::Polygon*>* retrievePolygons(std::list<geos::geom::Polygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual geos::geom::Polygon* retrievePolygon(std::vector<geos::geom::Polygon*> *polygons) throw(RSGISGeometryException);
			virtual geos::geom::Polygon* retrievePolygon(std::list<geos::geom::Polygon*> *polygons) throw(RSGISGeometryException);
			virtual ~RSGISIdentifyNonConvexPolygonsLineProject();
		protected:
			float resolution;
			virtual geos::geom::Polygon* identifyIrregularBoundaryLineProj(std::vector<geos::geom::LineSegment> *lines) throw(RSGISGeometryException);
			virtual std::vector<geos::geom::Coordinate*>* identifyIrregularBoundaryCoordinatesLineProj(std::vector<geos::geom::LineSegment> *lines, geos::geom::Polygon *convexhull) throw(RSGISGeometryException);
		};
}}

#endif


