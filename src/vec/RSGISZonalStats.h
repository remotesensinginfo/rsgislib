/*
 *  RSGISZonalStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/04/2008.
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

#ifndef RSGISZonalStats_H
#define RSGISZonalStats_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageSingle.h"
#include "img/RSGISPixelInPoly.h"

#include "utils/RSGISTextException.h"

#include "math/RSGISMathsUtils.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorZonalException.h"
#include "vec/RSGISPolygonData.h"
#include "vec/RSGISZonalPolygons.h"
#include "vec/RSGISVectorIO.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISProcessVector.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/Coordinate.h"

#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/algorithm/string.hpp>

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

namespace rsgis 
{
	namespace vec
	{
		struct DllExport imagestats
		{
			double mean;
			double max;
			double min;
			double stddev;
			double meanSum;
			double n;
			double sumDiff;
			bool first;
		};
		
		struct DllExport classzonalstats
		{
			std::string name;
			float threshold;
            rsgis::math::rsgissummarytype summary;
			int numBands;
			int *bands;
		};

        struct DllExport ZonalBandAttrs
        {
            int band;
            std::string baseName;
            std::string minName;
            std::string maxName;
            std::string meanName;
            std::string stdName;
            std::string countName;
            std::string modeName;
            std::string medianName;
            std::string sumName;
            bool outMin;
            bool outMax;
            bool outMean;
            bool outStDev;
            bool outCount;
            bool outMode;
            bool outMedian;
            bool outSum;
            float minThres;
            float maxThres;
        };

		struct DllExport ZonalAttributes
		{
			std::string name;
			std::string minName;
			std::string maxName;
			std::string meanName;
			std::string stdName;
			int numBands;
			bool outMin;
			bool outMax;
			bool outMean;
			bool outStDev;
			bool outCount;
            bool outMode;
            bool outSum;
			int *bands;
			float *minThresholds;
			float *maxThresholds;
		};
		
		class ZonalStats
			{
			public:
				ZonalStats();
				void zonalStatsVector(GDALDataset *image, OGRLayer *vector, bool **toCalc,  OGRLayer *outputSHPLayer);
				void zonalStats(GDALDataset *image, OGRLayer *vector, bool **toCalc,  OGRLayer *outputSHPLayer);
				void zonalStatsRaster(GDALDataset *image, GDALDataset *rasterFeatures, OGRLayer *inputLayer, OGRLayer *outputLayer, bool **toCalc);
				void zonalStatsRaster2txt(GDALDataset *image, GDALDataset *rasterFeatures, OGRLayer *inputLayer, std::string outputTxt, bool **toCalc);
                void zonalStatsFeatsVectorLyr(GDALDataset *image, OGRLayer *vecLyr, std::vector<ZonalBandAttrs> *zonalBandAtts, rsgis::img::pixelInPolyOption pixelInPolyMethod);
                ~ZonalStats();
			protected:
                void addVecLyrDefn(OGRLayer *vecLyr, std::vector<ZonalBandAttrs> *zonalBandAtts);
				void createOutputSHPDefinition(OGRLayer *inputSHPLayer, OGRLayer *outputSHPLayer, bool **toCalc, int numBands);
				void createOutputSHPDefinition(OGRLayer *outputSHPLayer, classzonalstats** attributes, int numAttributes, OGRFeatureDefn *inLayerDef);
				void outputData2SHP(OGRLayer *inputLayer, OGRLayer *outputSHPLayer, int featureFieldCount, bool **toCalc, int numBands, imagestats **stats);
				void calcImageStats(GDALDataset *image, OGRPolygon *polygon, imagestats *stats);
				void calcImageStats(GDALDataset *image, RSGISZonalPolygons *polygon);
				void outputData2Text(std::string outputTxt, bool **toCalc, imagestats **stats, int numFeatures, int numAttributes);
			};
		
		class DllExport RSGISCalcZonalStatsFromRaster : public rsgis::img::RSGISCalcImageValue
			{
			public: 
				RSGISCalcZonalStatsFromRaster(int numberOutBands, imagestats **stats, int numInBands, int numFeatures, bool stddev);
				void calcImageValue(float *bandValues, int numBands, double *output);
				void calcImageValue(float *bandValues, int numBands);
                void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent);
				void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent);
				void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
				void setStdDev(bool stddev);
				virtual ~RSGISCalcZonalStatsFromRaster();
			protected:
				imagestats **stats;
				int numInBands;
				int numFeatures;
				bool stddev;
			};
		
		class DllExport RSGISZonalStats : public RSGISProcessOGRFeature
		{
			/** Zonal stats, takes rasterised version of vector
			    and calculates statistics for each attribute
				The names for each attribute, statistics to calculate, bands to use and threasholds for each band are passed in
				in 'ZonalAttributes' structure
			 */
		public:
			RSGISZonalStats(GDALDataset *image, GDALDataset *rasterFeatures, ZonalAttributes** attributes, int numAttributes, bool outPxlCount, std::string outZonalFileName = "");
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
			virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
			virtual ~RSGISZonalStats();
		protected:
			GDALDataset **datasets;
			ZonalAttributes** attributes;
			int numAttributes;
			int dataSize;
			double *data;
			bool outPxlCount;
			rsgis::img::RSGISCalcImageSingle *calcImage;
			rsgis::img::RSGISCalcImageSingleValue *calcValue;
			bool outputToTextFile;
			bool firstLine;
			std::ofstream outZonalFile;
		};
		
		class DllExport RSGISCalcZonalStatsFromRasterPolygon : public rsgis::img::RSGISCalcImageSingleValue
		{
			/** Calculte statistics and save to data array
			 *  Array is in the form:
			 *	 totalPix
             *   min attribute1
             *   max attribute 1
             *	 mean attribute 1
             *	 stDev attribute 1
             *   mode attribute 1
             *   sum attribute 1
             *   count attribute 1
             *   min attribute2
             *   max attribute 2
             *	 mean attribute 2
             *	 stDev attribute 2
             *   mode attribute 2
             *   sum attribute 2
             *   count attribute 2
			 *
			 * This format is used to ensure compatibility with calc image
			 */
		public: 
			RSGISCalcZonalStatsFromRasterPolygon(int numOutputValues, ZonalAttributes **attributes, int numAttributes);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) {throw rsgis::img::RSGISImageCalcException("RSGISCalcZonalStatsFromRasterPolygon: Not Implemented");};
			void calcImageValue(float *bandValuesImage, int numBands, int band);
			void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) {throw rsgis::img::RSGISImageCalcException("RSGISCalcZonalStatsFromRasterPolygon: Not Implemented");};
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) {throw rsgis::img::RSGISImageCalcException("RSGISCalcZonalStatsFromRasterPolygon: Not Implemented");};
			double* getOutputValues();
			void reset();
			~RSGISCalcZonalStatsFromRasterPolygon();
		protected:
			ZonalAttributes **attributes;
			std::vector<double> **values;
			int totalPxl;
			int numAttributes;
		};		
		
		class DllExport RSGISZonalStatsPoly : public RSGISProcessOGRFeature
		{
			/** Zonal stats, does not require rasterised version of vector
			 and calculates statistics for each attribute
			 The names for each attribute, statistics to calculate, bands to use and threasholds for each band are passed in
			 in 'ZonalAttributes' structure
			 */
		public:
			RSGISZonalStatsPoly(GDALDataset *image, ZonalAttributes** attributes, int numAttributes, bool outPxlCount, rsgis::img::pixelInPolyOption method, std::string outZonalFileName = "");
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
			virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
			virtual ~RSGISZonalStatsPoly();
		protected:
			GDALDataset **datasets;
			ZonalAttributes** attributes;
			int numAttributes;
			int dataSize;
			double *data;
			bool outPxlCount;
			rsgis::img::RSGISCalcImageSingle *calcImage;
			rsgis::img::RSGISCalcImageSingleValue *calcValue;
			rsgis::img::pixelInPolyOption method;
			bool outputToTextFile;
			bool firstLine;
			std::ofstream outZonalFile;
		};
		
		class DllExport RSGISCalcZonalStatsFromPolygon : public rsgis::img::RSGISCalcImageSingleValue
		{
			/** Calculte statistics and save to data array
			 *  Array is in the form:
			 *	 totalPix
             *   min attribute1
             *   max attribute 1
             *	 mean attribute 1
             *	 stDev attribute 1
             *   mode attribute 1
             *   sum attribute 1
             *   count attribute 1
             *   min attribute2
             *   max attribute 2
             *	 mean attribute 2
             *	 stDev attribute 2
             *   mode attribute 2
             *   sum attribute 2
             *   count attribute 2
			 *
			 * This format is used to ensure compatibility with calc image
			 */
		public: 
			RSGISCalcZonalStatsFromPolygon(int numOutputValues, ZonalAttributes **attributes, int numAttributes);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) {throw rsgis::img::RSGISImageCalcException("RSGISCalcZonalStatsFromPolygon, Not Implemented");};
			void calcImageValue(float *bandValuesImage, int numBands, int band) {throw rsgis::img::RSGISImageCalcException("RSGISCalcZonalStatsFromPolygon, Not Implemented");};
			void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) {throw rsgis::img::RSGISImageCalcException("RSGISCalcZonalStatsFromPolygon, Not Implemented");};
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt);
			double* getOutputValues();
			void reset();
			~RSGISCalcZonalStatsFromPolygon();
		protected:
			ZonalAttributes **attributes;
			std::vector<double> **values;
			int totalPxl;
			int numAttributes;
		};

        class DllExport RSGISZonalStatsPolyUpdateLyr : public RSGISProcessOGRFeature
        {
        public:
            RSGISZonalStatsPolyUpdateLyr(GDALDataset *image, std::vector<ZonalBandAttrs> *zonalBandAtts, rsgis::img::pixelInPolyOption method);
            virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid){throw RSGISVectorOutputException("Not Implemented RSGISZonalStatsPolyUpdateLyr::processFeature; outFeature");};
            virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid);
            virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn){throw RSGISVectorOutputException("Not Implemented RSGISZonalStatsPolyUpdateLyr::createOutputLayerDefinition");};
            virtual ~RSGISZonalStatsPolyUpdateLyr();
        protected:
            GDALDataset **datasets;
            std::vector<ZonalBandAttrs> *zonalBandAtts;
            std::vector<float> **pxlVals;
            unsigned int nBands;
            rsgis::img::RSGISCalcImage *calcImage;
            rsgis::img::RSGISCalcImageValue *calcValue;
            rsgis::img::pixelInPolyOption method;
            RSGISVectorUtils *vecUtils;
            OGRPolygon *inOGRPoly;
            geos::geom::Polygon *poly;
            std::vector<double> *dataVal;
            rsgis::math::RSGISStatsSummary *statsSummary;
            rsgis::math::RSGISMathsUtils *mathUtils;
        };

	}}

#endif
