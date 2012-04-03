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

#include "geos/geom/Envelope.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/Coordinate.h"

using namespace std;
using namespace rsgis::img;
using namespace rsgis::utils;
using namespace rsgis::math;
using namespace geos::geom;

namespace rsgis 
{
	namespace vec
	{
		struct imagestats
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
		
		struct classzonalstats
		{
			string name;
			float threshold;
			rsgissummarytype summary;
			int numBands;
			int *bands;
		};
		
		struct ZonalAttributes
		{
			string name;
			string minName;
			string maxName;
			string meanName;
			string stdName;
			int numBands;
			bool outMin;
			bool outMax;
			bool outMean;
			bool outStDev;
			bool outCount;
            bool outMode;
			int *bands;
			float *minThreasholds;
			float *maxThreasholds;
		};
		
		class ZonalStats
			{
			public:
				ZonalStats();
				void zonalStatsVector(GDALDataset *image, OGRLayer *vector, bool **toCalc,  OGRLayer *outputSHPLayer) throw(RSGISVectorZonalException,RSGISVectorOutputException);
				void zonalStats(GDALDataset *image, OGRLayer *vector, bool **toCalc,  OGRLayer *outputSHPLayer)  throw(RSGISVectorZonalException,RSGISVectorOutputException);
				void zonalStatsRaster(GDALDataset *image, GDALDataset *rasterFeatures, OGRLayer *inputLayer, OGRLayer *outputLayer, bool **toCalc) throw(RSGISImageCalcException, RSGISImageBandException, RSGISVectorOutputException);
				void zonalStatsRaster2txt(GDALDataset *image, GDALDataset *rasterFeatures, OGRLayer *inputLayer, string outputTxt, bool **toCalc) throw(RSGISImageCalcException, RSGISImageBandException);
				~ZonalStats();
			protected:
				void createOutputSHPDefinition(OGRLayer *inputSHPLayer, OGRLayer *outputSHPLayer, bool **toCalc, int numBands) throw(RSGISVectorOutputException);
				void createOutputSHPDefinition(OGRLayer *outputSHPLayer, classzonalstats** attributes, int numAttributes, OGRFeatureDefn *inLayerDef) throw(RSGISVectorOutputException);
				void outputData2SHP(OGRLayer *inputLayer, OGRLayer *outputSHPLayer, int featureFieldCount, bool **toCalc, int numBands, imagestats **stats) throw(RSGISVectorOutputException);
				void calcImageStats(GDALDataset *image, OGRPolygon *polygon, imagestats *stats) throw(RSGISVectorZonalException);
				void calcImageStats(GDALDataset *image, RSGISZonalPolygons *polygon) throw(RSGISVectorZonalException);
				void outputData2Text(string outputTxt, bool **toCalc, imagestats **stats, int numFeatures, int numAttributes) throw(RSGISTextException);
			};
		
		class RSGISCalcZonalStatsFromRaster : public RSGISCalcImageValue
			{
			public: 
				RSGISCalcZonalStatsFromRaster(int numberOutBands, imagestats **stats, int numInBands, int numFeatures, bool stddev);
				void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException);
				void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
                void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
				void setStdDev(bool stddev);
				virtual ~RSGISCalcZonalStatsFromRaster();
			protected:
				imagestats **stats;
				int numInBands;
				int numFeatures;
				bool stddev;
			};
		
		class RSGISZonalStats : public RSGISProcessOGRFeature
		{
			/** Zonal stats, takes rasterised version of vector
			    and calculates statistics for each attribute
				The names for each attribute, statistics to calculate, bands to use and threasholds for each band are passed in
				in 'ZonalAttributes' structure
			 */
		public:
			RSGISZonalStats(GDALDataset *image, GDALDataset *rasterFeatures, ZonalAttributes** attributes, int numAttributes, bool outPxlCount, string outZonalFileName = "");
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
			virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			virtual ~RSGISZonalStats();
		protected:
			GDALDataset **datasets;
			ZonalAttributes** attributes;
			int numAttributes;
			int dataSize;
			double *data;
			bool outPxlCount;
			RSGISCalcImageSingle *calcImage;
			RSGISCalcImageSingleValue *calcValue;
			bool outputToTextFile;
			bool firstLine;
			ofstream outZonalFile;
		};
		
		class RSGISCalcZonalStatsFromRasterPolygon : public RSGISCalcImageSingleValue
		{
			/** Calculte statistics and save to data array
			 *  Array is in the form:
			 *	 totalPix
			 *   min attribute1
			 *   min attribute 2
			 *   max attribute 1
			 *   max attribute 2
			 *	 mean attribute 1
			 *   mean attribute 2
			 *	 stDev attribute 1
			 *	 stDev attribute 2
			 *
			 * This format is used to ensure compatibility with calc image
			 */
		public: 
			RSGISCalcZonalStatsFromRasterPolygon(int numOutputValues, ZonalAttributes **attributes, int numAttributes);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
			void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
			double* getOutputValues() throw(RSGISImageCalcException);
			void reset();
			~RSGISCalcZonalStatsFromRasterPolygon();
		protected:
			ZonalAttributes **attributes;
			vector<double> **values;
			int totalPxl;
			int numAttributes;
		};		
		
		class RSGISZonalStatsPoly : public RSGISProcessOGRFeature
		{
			/** Zonal stats, does not require rasterised version of vector
			 and calculates statistics for each attribute
			 The names for each attribute, statistics to calculate, bands to use and threasholds for each band are passed in
			 in 'ZonalAttributes' structure
			 */
		public:
			RSGISZonalStatsPoly(GDALDataset *image, ZonalAttributes** attributes, int numAttributes, bool outPxlCount, pixelInPolyOption method, string outZonalFileName = "");
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
			virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			virtual ~RSGISZonalStatsPoly();
		protected:
			GDALDataset **datasets;
			ZonalAttributes** attributes;
			int numAttributes;
			int dataSize;
			double *data;
			bool outPxlCount;
			RSGISCalcImageSingle *calcImage;
			RSGISCalcImageSingleValue *calcValue;
			pixelInPolyOption method;
			bool outputToTextFile;
			bool firstLine;
			ofstream outZonalFile;
		};
		
		class RSGISCalcZonalStatsFromPolygon : public RSGISCalcImageSingleValue
		{
			/** Calculte statistics and save to data array
			 *  Array is in the form:
			 *	 totalPix
			 *   min attribute1
			 *   min attribute 2
			 *   max attribute 1
			 *   max attribute 2
			 *	 mean attribute 1
			 *   mean attribute 2
			 *	 stDev attribute 1
			 *	 stDev attribute 2
			 *
			 * This format is used to ensure compatibility with calc image
			 */
		public: 
			RSGISCalcZonalStatsFromPolygon(int numOutputValues, ZonalAttributes **attributes, int numAttributes);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
			void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
			void calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException);
			double* getOutputValues() throw(RSGISImageCalcException);
			void reset();
			~RSGISCalcZonalStatsFromPolygon();
		protected:
			ZonalAttributes **attributes;
			vector<double> **values;
			int totalPxl;
			int numAttributes;
		};
		
	}}

#endif
