/*
 *  RSGISVectorUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/04/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#ifndef RSGISVectorUtils_H
#define RSGISVectorUtils_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <list>

#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "geos/geom/GeometryFactory.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/LineString.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LineString.h"

#include "vec/RSGISPolygonData.h"

#include "common/RSGISVectorException.h"

#include "utils/RSGISGEOSFactoryGenerator.h"
#include "utils/RSGISFileUtils.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_vec_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace vec{
    
	class DllExport RSGISVectorUtils
		{
		public:
			std::string getLayerName(std::string filepath);
            geos::geom::LineString* convertOGRLineString2GEOSLineString(OGRLineString *line);
			OGRLineString* convertGEOSLineString2OGRLineString(geos::geom::LineString *line);
			geos::geom::LinearRing* convertOGRLinearRing2GEOSLinearRing(OGRLinearRing *ring);
			OGRLinearRing* convertGEOSLineString2OGRLinearRing(geos::geom::LineString *line);
			geos::geom::Polygon* convertOGRPolygon2GEOSPolygon(OGRPolygon *poly);
			geos::geom::MultiPolygon* convertOGRMultiPolygonGEOSMultiPolygon(OGRMultiPolygon *mPoly);
			geos::geom::Point* convertOGRPoint2GEOSPoint(OGRPoint *point);
			OGRPolygon* convertGEOSPolygon2OGRPolygon(geos::geom::Polygon *poly);
			OGRMultiPolygon* convertGEOSMultiPolygon2OGRMultiPolygon(geos::geom::MultiPolygon *mPoly);
			OGRMultiPolygon* convertGEOSPolygons2OGRMultiPolygon(std::list<geos::geom::Polygon*> *polys);
			geos::geom::MultiPolygon* convertGEOSPolygons2GEOSMultiPolygon(std::vector<geos::geom::Polygon*> *polys);
			OGRPoint* convertGEOSPoint2OGRPoint(geos::geom::Point *point);
			OGRPoint* convertGEOSCoordinate2OGRPoint(geos::geom::Coordinate *coord);
			geos::geom::Envelope* getEnvelope(geos::geom::Geometry *geom);
			geos::geom::Envelope* getEnvelope(OGRGeometry *geom);
			geos::geom::Envelope* getEnvelopePixelBuffer(OGRGeometry *geom, double imageRes);
			geos::geom::Point* createPoint(geos::geom::Coordinate *coord);
			bool checkDIR4SHP(std::string dir, std::string shp) throw(RSGISVectorException);
			void deleteSHP(std::string dir, std::string shp) throw(RSGISVectorException);
			geos::geom::GeometryCollection* createGeomCollection(std::vector<geos::geom::Polygon*> *polys) throw(RSGISVectorException);
			geos::geom::Polygon* createPolygon(double tlX, double tlY, double brX, double brY) throw(RSGISVectorException);
			OGRPolygon* createOGRPolygon(double tlX, double tlY, double brX, double brY) throw(RSGISVectorException);
            OGRPolygon* createOGRPolygon(geos::geom::Envelope *env) throw(RSGISVectorException);
			OGRPolygon* checkCloseOGRPolygon(OGRPolygon *poly) throw(RSGISVectorException);
			OGRPolygon* removeHolesOGRPolygon(OGRPolygon *poly) throw(RSGISVectorException);
            OGRPolygon* removeHolesOGRPolygon(OGRPolygon *poly, float areaThreshold) throw(RSGISVectorException);
			OGRPolygon* moveOGRPolygon(OGRPolygon *poly, double shiftX, double shiftY, double shiftZ) throw(RSGISVectorException);
			std::vector<std::string>* findUniqueVals(OGRLayer *layer, std::string attribute) throw(RSGISVectorException);
            std::vector<std::string>* getColumnNames(OGRLayer *layer) throw(RSGISVectorException);
            std::vector<OGRPoint*>* getRegularStepPoints(std::vector<OGRLineString*> *lines, double step) throw(RSGISVectorException);
		};

    
}}

#endif


