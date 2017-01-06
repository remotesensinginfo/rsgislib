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
	class DllExport RSGISGeomTestExport
		{
		public:
			RSGISGeomTestExport();
			
            std::string getLayerName(std::string filepath);
			OGRLineString* convertGEOSLineSegment2OGRLineString(geos::geom::LineSegment *line);
			OGRLineString* convertGEOSLineString2OGRLineString(geos::geom::LineString *line);
			OGRLinearRing* convertGEOSLineString2OGRLinearRing(geos::geom::LineString *line);
			OGRPolygon* convertGEOSPolygon2OGRPolygon(geos::geom::Polygon *poly);
			OGRPoint* convertGEOSPoint2OGRPoint(geos::geom::Point *point);
			OGRPoint* convertGEOSCoordinate2OGRPoint(geos::geom::Coordinate *coord);
			bool checkDIR4SHP(std::string dir, std::string shp) throw(RSGISGeometryException);
			void deleteSHP(std::string dir, std::string shp) throw(RSGISGeometryException);
			
			void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Polygon*> *polys) throw(RSGISGeometryException);
			void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys) throw(RSGISGeometryException);
			void exportGEOSCoordinates2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Coordinate*> *coords) throw(RSGISGeometryException);
			void exportGEOSLineStrings2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::LineString*> *lines) throw(RSGISGeometryException);
			void exportGEOSLineSegments2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::LineSegment*> *lines) throw(RSGISGeometryException);
            void exportGEOSLineSegments2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::LineSegment*> *lines, std::vector<double> *vals) throw(RSGISGeometryException);
			
			~RSGISGeomTestExport();
		};
}}

#endif



