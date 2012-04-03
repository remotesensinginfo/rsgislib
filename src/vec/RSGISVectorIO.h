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

using namespace std;
using namespace rsgis;
using namespace rsgis::geom;
using namespace rsgis::utils;

namespace rsgis{namespace vec{
	class RSGISVectorIO
		{
		public:
			RSGISVectorIO();
			void readPoints(OGRLayer *vector, RSGISPointData **data, unsigned long numFeatures) throw(RSGISVectorException);
			void readPolygons(OGRLayer *vector, RSGISPolygonData **data, unsigned long numFeatures) throw(RSGISVectorException);
			void readOGRLineString2GEOSLinsSegment(OGRLineString *inLine, vector <LineSegment> *outLines); /// Read polyline as GEOS Line sement, where there are more than 2 vertices the first and last will be used.
			void exportPoints2Shp(OGRLayer *outputSHP, RSGISPointData **data, int numFeatures) throw(RSGISVectorOutputException);
			void exportPolygons2Shp(OGRLayer *outputSHP, RSGISPolygonData **data, int numFeatures) throw(RSGISVectorOutputException);
			void exportPolygons2Shp(OGRLayer *outputSHP, vector<RSGISPolygonData*> *data) throw(RSGISVectorOutputException);
			void exportGEOSPolygons2SHP(string outputFile, Polygon **polys, int numFeatures) throw(RSGISVectorOutputException);
			void exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys) throw(RSGISVectorOutputException);
			void exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, vector<PlotPoly*> *polyDetails) throw(RSGISVectorOutputException);
			void exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, vector<ImageFootPrintPoly*> *polyDetails) throw(RSGISVectorOutputException);
			void exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, OGRSpatialReference* spatialRef) throw(RSGISVectorOutputException);
			void exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, list<Polygon*> *polys, OGRSpatialReference* spatialRef) throw(RSGISVectorOutputException);
			void exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, OGRSpatialReference* spatialRef, string attribute, string attributeVal) throw(RSGISVectorOutputException);
			void exportGEOSCoordinates2SHP(string outputFile, bool deleteIfPresent, vector<Coordinate*> *coords) throw(RSGISVectorOutputException);
			void exportGEOSLineStrings2SHP(string outputFile, bool deleteIfPresent, vector<LineString*> *lines) throw(RSGISVectorOutputException);
			void exportRSGISPolygonsClusters2SHP(list<RSGIS2DPoint*> **clusters, int numClusters, string outputFile, bool force, RSGISIdentifyNonConvexPolygons *createPolygon);
			void exportLinesAsShp(string outputFile, list<double> *x1, list<double> *y1, list<double> *x2, list<double> *y2, bool force) throw(RSGISVectorOutputException);
			void exportGEOSPolygonClusters2SHP(string outputFile, bool deleteIfPresent, list<Polygon*> **polygons, int numClusters, OGRSpatialReference* spatialRef) throw(RSGISVectorOutputException);
			void exportGEOSMultiPolygonClusters2SHP(string outputFile, bool deleteIfPresent, list<Polygon*> **polygons, int numClusters, OGRSpatialReference* spatialRef) throw(RSGISVectorOutputException);
			void exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, OGRSpatialReference* spatialRef, vector<string> *numericColsName, vector<string> *textColsName, vector<float> **numericColsData, vector<string> **textColsData) throw(RSGISVectorOutputException);
			void exportPolygons2Layer(OGRLayer *outLayer, list<OGRPolygon*> *polys) throw(RSGISVectorOutputException);
			~RSGISVectorIO();
		};
}}

#endif


