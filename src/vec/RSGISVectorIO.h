/*
 *  RSGISVectorIO.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2008.
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

#ifndef RSGISVectorIO_H
#define RSGISVectorIO_H

#include <iostream>
#include <string>
#include <list>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISPolygonData.h"
#include "vec/RSGISPointData.h"
#include "vec/RSGISEmptyPolygon.h"
#include "vec/RSGISVectorUtils.h"

#include "geom/RSGISPolygon.h"
#include "geom/RSGISGeometry.h"
#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISIdentifyNonConvexPolygons.h"

#include "utils/RSGISPlotPolygonsCSVParse.h"
#include "utils/RSGISImageFootprintPolygonsCSVParse.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
	class DllExport RSGISVectorIO
		{
		public:
			RSGISVectorIO();
			void readPoints(OGRLayer *vector, RSGISPointData **data, unsigned long numFeatures);
			void readPolygons(OGRLayer *vector, RSGISPolygonData **data, unsigned long numFeatures);
			void readOGRLineString2GEOSLinsSegment(OGRLineString *inLine, std::vector<geos::geom::LineSegment> *outLines); /// Read polyline as GEOS Line sement, where there are more than 2 vertices the first and last will be used.
            void exportPoints2Shp(OGRLayer *outputSHP, RSGISPointData **data, int numFeatures);
			void exportPolygons2Shp(OGRLayer *outputSHP, RSGISPolygonData **data, int numFeatures);
			void exportPolygons2Shp(OGRLayer *outputSHP, std::vector<RSGISPolygonData*> *data);
			void exportGEOSPolygons2SHP(std::string outputFile, geos::geom::Polygon **polys, int numFeatures);
			void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys);
			void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, std::vector<rsgis::utils::PlotPoly*> *polyDetails);
			void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, std::vector<rsgis::utils::ImageFootPrintPoly*> *polyDetails);
			void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef);
            void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef, std::vector<std::string> *outAtts, std::string attName);
			void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef);
			void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef, std::string attribute, std::string attributeVal);
			void exportGEOSCoordinates2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Coordinate*> *coords);
			void exportGEOSLineStrings2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::LineString*> *lines);
			void exportRSGISPolygonsClusters2SHP(std::list<rsgis::geom::RSGIS2DPoint*> **clusters, int numClusters, std::string outputFile, bool force, rsgis::geom::RSGISIdentifyNonConvexPolygons *createPolygon);
			void exportLinesAsShp(std::string outputFile, std::list<double> *x1, std::list<double> *y1, std::list<double> *x2, std::list<double> *y2, bool force);
			void exportGEOSPolygonClusters2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Polygon*> **polygons, int numClusters, OGRSpatialReference* spatialRef);
            void exportGEOSPointClusters2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Point*> **points, int numClusters, OGRSpatialReference* spatialRef);
            void exportGEOSMultiPolygonClusters2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Polygon*> **polygons, int numClusters, OGRSpatialReference* spatialRef);
			void exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef, std::vector<std::string> *numericColsName, std::vector<std::string> *textColsName, std::vector<float> **numericColsData, std::vector<std::string> **textColsData);
			void exportPolygons2Layer(OGRLayer *outLayer, std::list<OGRPolygon*> *polys);
            void exportOGRPoints2SHP(std::string outputFile, bool deleteIfPresent, std::vector<OGRPoint*> *pts, OGRSpatialReference* spatialRef);
            ~RSGISVectorIO();
		};
}}

#endif


