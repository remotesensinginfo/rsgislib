/*
 *  RSGISIdentifyNonConvexPolygonsSnakes.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/08/2009.
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

#ifndef RSGISIdentifyNonConvexPolygonsSnakes_H
#define RSGISIdentifyNonConvexPolygonsSnakes_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#include "common/RSGISImageException.h"

#include "geom/RSGISPolygon.h"
#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISGeometryException.h"
#include "geom/RSGISGeometry.h"
#include "geom/RSGISIdentifyNonConvexPolygons.h"
#include "geom/RSGISGeomTestExport.h"

#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/PrecisionModel.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISGlobalOptimisationFunction.h"
#include "math/RSGISGlobalHillClimbingOptimiser2D.h"
#include "math/RSGISOptimisationException.h"

#include "gdal_priv.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis;
using namespace rsgis::math;

namespace rsgis{namespace geom{
	
	class RSGISIdentifyNonConvexPolygonsSnakes : public RSGISIdentifyNonConvexPolygons
		{
		public:
			RSGISIdentifyNonConvexPolygonsSnakes(double resolution, OGRSpatialReference* spatialRef, double alpha, double beta, double gamma, double delta, int maxNumIterations);
			virtual vector<Polygon*>* retrievePolygons(list<RSGIS2DPoint*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual vector<Polygon*>* retrievePolygons(list<RSGISPolygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual vector<Polygon*>* retrievePolygons(list<Polygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual Polygon* retrievePolygon(vector<Polygon*> *polygons) throw(RSGISGeometryException);
			virtual Polygon* retrievePolygon(list<Polygon*> *polygons) throw(RSGISGeometryException);
			virtual ~RSGISIdentifyNonConvexPolygonsSnakes();
		private:
			double resolution;
			double alpha;
			double beta;
			double gamma;
			double delta;
			int maxNumIterations;
			OGRSpatialReference* spatialRef;
			GDALDriver *gdalDriver;
			GDALDataset* createDataset(GDALDriver *gdalDriver, Geometry *geom, string filename, float resolution, float constVal) throw(RSGISImageException);
			void rasterizeLayer(Geometry *geom, GDALDataset *image, float constVal) throw(RSGISImageException);
			void createDistanceImage(GDALDataset *inputImage, Geometry *geom) throw(RSGISImageException, RSGISGeometryException);
			GeometryCollection* createGeomCollection(vector<Polygon*> *polys) throw(RSGISGeometryException); 
			GeometryCollection* createGeomCollection(list<Polygon*> *polys) throw(RSGISGeometryException); 
			void populatePixelPolygons(GDALDataset *image, float threshold, vector<Polygon*> *polys) throw(RSGISGeometryException); 
			Polygon* createPolygonFromEnv(Envelope env);
		};
	
	
	class RSGISSnakeNonConvexGlobalOptimisationFunction : public RSGISGlobalOptimisationFunction
		{
		public:
			RSGISSnakeNonConvexGlobalOptimisationFunction(GDALDataset *image, double alpha, double beta, double gamma);
			virtual double calcValue(vector<Coordinate*> *coords) throw(RSGISOptimisationException);
			virtual ~RSGISSnakeNonConvexGlobalOptimisationFunction();
		protected:
			GDALDataset *image;
			GDALRasterBand *imageBand;
			int imgWidth;
			int imgHeight;
			double resolution;
			Envelope *env;
			float *data;
			double alpha;
			double beta;
			double gamma;
		};
	
	class RSGISSnakeNonConvexLineProjGlobalOptimisationFunction : public RSGISGlobalOptimisationFunction
		{
		public:
			RSGISSnakeNonConvexLineProjGlobalOptimisationFunction(GDALDataset *image, double alpha, double beta, double gamma, double delta, vector<LineSegment*> *lines);
			virtual double calcValue(vector<Coordinate*> *coords) throw(RSGISOptimisationException);
			virtual ~RSGISSnakeNonConvexLineProjGlobalOptimisationFunction();
		protected:
			GDALDataset *image;
			GDALRasterBand *imageBand;
			vector<LineSegment*> *lines;
			int imgWidth;
			int imgHeight;
			double resolution;
			Envelope *env;
			float *data;
			double alpha;
			double beta;
			double gamma;
			double delta;
		};
	
}}

#endif





