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
	
	class DllExport RSGISIdentifyNonConvexPolygonsSnakes : public RSGISIdentifyNonConvexPolygons
		{
		public:
			RSGISIdentifyNonConvexPolygonsSnakes(double resolution, OGRSpatialReference* spatialRef, double alpha, double beta, double gamma, double delta, int maxNumIterations);
			virtual std::vector<geos::geom::Polygon*>* retrievePolygons(std::list<RSGIS2DPoint*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual std::vector<geos::geom::Polygon*>* retrievePolygons(std::list<RSGISPolygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual std::vector<geos::geom::Polygon*>* retrievePolygons(std::list<geos::geom::Polygon*> **clusters, int numClusters) throw(RSGISGeometryException);
			virtual geos::geom::Polygon* retrievePolygon(std::vector<geos::geom::Polygon*> *polygons) throw(RSGISGeometryException);
			virtual geos::geom::Polygon* retrievePolygon(std::list<geos::geom::Polygon*> *polygons) throw(RSGISGeometryException);
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
			GDALDataset* createDataset(GDALDriver *gdalDriver, geos::geom::Geometry *geom, std::string filename, float resolution, float constVal) throw(RSGISImageException);
			void rasterizeLayer(geos::geom::Geometry *geom, GDALDataset *image, float constVal) throw(rsgis::RSGISImageException);
			void createDistanceImage(GDALDataset *inputImage, geos::geom::Geometry *geom) throw(RSGISImageException, RSGISGeometryException);
			geos::geom::GeometryCollection* createGeomCollection(std::vector<geos::geom::Polygon*> *polys) throw(RSGISGeometryException); 
			geos::geom::GeometryCollection* createGeomCollection(std::list<geos::geom::Polygon*> *polys) throw(RSGISGeometryException); 
			void populatePixelPolygons(GDALDataset *image, float threshold, std::vector<geos::geom::Polygon*> *polys) throw(RSGISGeometryException); 
			geos::geom::Polygon* createPolygonFromEnv(geos::geom::Envelope env);
		};
	
	
	class DllExport RSGISSnakeNonConvexGlobalOptimisationFunction : public rsgis::math::RSGISGlobalOptimisationFunction
		{
		public:
			RSGISSnakeNonConvexGlobalOptimisationFunction(GDALDataset *image, double alpha, double beta, double gamma);
			virtual double calcValue(std::vector<geos::geom::Coordinate*> *coords) throw(rsgis::math::RSGISOptimisationException);
			virtual ~RSGISSnakeNonConvexGlobalOptimisationFunction();
		protected:
			GDALDataset *image;
			GDALRasterBand *imageBand;
			int imgWidth;
			int imgHeight;
			double resolution;
			geos::geom::Envelope *env;
			float *data;
			double alpha;
			double beta;
			double gamma;
		};
	
	class DllExport RSGISSnakeNonConvexLineProjGlobalOptimisationFunction : public rsgis::math::RSGISGlobalOptimisationFunction
		{
		public:
			RSGISSnakeNonConvexLineProjGlobalOptimisationFunction(GDALDataset *image, double alpha, double beta, double gamma, double delta, std::vector<geos::geom::LineSegment*> *lines);
			virtual double calcValue(std::vector<geos::geom::Coordinate*> *coords) throw(rsgis::math::RSGISOptimisationException);
			virtual ~RSGISSnakeNonConvexLineProjGlobalOptimisationFunction();
		protected:
			GDALDataset *image;
			GDALRasterBand *imageBand;
            std::vector<geos::geom::LineSegment*> *lines;
			int imgWidth;
			int imgHeight;
			double resolution;
			geos::geom::Envelope *env;
			float *data;
			double alpha;
			double beta;
			double gamma;
			double delta;
		};
	
}}

#endif





