/*
 *  RSGISGeomTestExport.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/06/2009.
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

#ifndef RSGISGeomTestExport_H
#define RSGISGeomTestExport_H

#include <iostream>
#include <string>
#include "ogrsf_frmts.h"

#include "geom/RSGISGeometryException.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/LineString.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LineSegment.h"

#include "geom/RSGISGeometryException.h"

#include "utils/RSGISGEOSFactoryGenerator.h"
#include "utils/RSGISFileUtils.h"

using namespace rsgis::utils;
using namespace std;
using namespace geos::geom;

namespace rsgis{namespace geom{
	class RSGISGeomTestExport
		{
		public:
			RSGISGeomTestExport();
			
			string getLayerName(string filepath);
			OGRLineString* convertGEOSLineSegment2OGRLineString(LineSegment *line);
			OGRLineString* convertGEOSLineString2OGRLineString(LineString *line);
			OGRLinearRing* convertGEOSLineString2OGRLinearRing(LineString *line);
			OGRPolygon* convertGEOSPolygon2OGRPolygon(Polygon *poly);
			OGRPoint* convertGEOSPoint2OGRPoint(Point *point);
			OGRPoint* convertGEOSCoordinate2OGRPoint(Coordinate *coord);
			bool checkDIR4SHP(string dir, string shp) throw(RSGISGeometryException);
			void deleteSHP(string dir, string shp) throw(RSGISGeometryException);
			
			void exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, list<Polygon*> *polys) throw(RSGISGeometryException);
			void exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys) throw(RSGISGeometryException);
			void exportGEOSCoordinates2SHP(string outputFile, bool deleteIfPresent, vector<Coordinate*> *coords) throw(RSGISGeometryException);
			void exportGEOSLineStrings2SHP(string outputFile, bool deleteIfPresent, vector<LineString*> *lines) throw(RSGISGeometryException);
			void exportGEOSLineSegments2SHP(string outputFile, bool deleteIfPresent, vector<LineSegment*> *lines) throw(RSGISGeometryException);
			
			~RSGISGeomTestExport();
		};
}}

#endif



