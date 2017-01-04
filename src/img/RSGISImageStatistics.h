/*
 *  RSGISImageStatistics.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/05/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISImageStatistics_H
#define RSGISImageStatistics_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "geos/geom/Envelope.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImage.h"

#include "math/RSGISMathFunction.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISMathsUtils.h"

#include "gsl/gsl_statistics_double.h"

#include <boost/math/special_functions/fpclassify.hpp>

namespace rsgis{namespace img{
	
	struct DllExport ImageStats
	{
		double mean;
		double max;
		double min;
		double stddev;
        double sum;
	};
    
	class DllExport RSGISImageStatistics
    {
    public: 
        RSGISImageStatistics();
        void calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats **stats, int numInputBands, bool stddev, bool ignoreZeros=false, bool onePassSD=false)throw(RSGISImageCalcException,RSGISImageBandException);
        void calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats **stats, int numInputBands, bool stddev, rsgis::math::RSGISMathFunction *func, bool ignoreZeros=false, bool onePassSD=false)throw(RSGISImageCalcException,RSGISImageBandException);
        void calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats *stats, bool stddev, bool ignoreZeros=false, bool onePassSD=false)throw(RSGISImageCalcException,RSGISImageBandException);
        void calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats **stats, int numInputBands, bool stddev, bool noDataSpecified, float noDataVal, bool onePassSD, double xMin, double xMax, double yMin, double yMax)throw(RSGISImageCalcException,RSGISImageBandException);
        void calcImageHistogram(GDALDataset **datasets, int numDS, unsigned int imgBand, unsigned int numBins, float *binRanges, unsigned int *binCounts, bool noDataSpecified, float noDataVal, double xMin, double xMax, double yMin, double yMax)throw(RSGISImageCalcException,RSGISImageBandException);
        void calcImageStatisticsMask(GDALDataset *dataset, GDALDataset *imgMask, long maskVal, ImageStats **stats, double *noDataVals, bool useNoData, int numInputBands, bool stddev, bool onePassSD=false)throw(RSGISImageCalcException,RSGISImageBandException);
    };
	
	
	class DllExport RSGISCalcImageStatistics : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcImageStatistics(int numberOutBands, int numInputBands, bool calcSD, rsgis::math::RSGISMathFunction *func, bool ignoreZeros, bool onePassSD = false);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void getImageStats(ImageStats** inStats, int numInputBands) throw(RSGISImageCalcException);
        void calcStdDev();
        ~RSGISCalcImageStatistics();
    protected:
        bool ignoreZeros;
        bool onePassSD;
        bool calcSD;
        int numInputBands;
        bool *firstMean;
        bool *firstSD;
        bool calcMean;
        unsigned long *n;
        double *mean;
        double *meanSum;
        double *sumSq;
        double *min;
        double *max;
        double *sumDiffZ;
        double diffZ;
        rsgis::math::RSGISMathFunction *func;
    };
    
    class DllExport RSGISCalcImageStatisticsNoData : public RSGISCalcImageValue
    {
    public:
        RSGISCalcImageStatisticsNoData(int numInputBands, bool calcSD, rsgis::math::RSGISMathFunction *func, bool noDataSpecified, float noDataVal, bool onePassSD);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void getImageStats(ImageStats** inStats, int numInputBands) throw(RSGISImageCalcException);
        void calcStdDev();
        ~RSGISCalcImageStatisticsNoData();
    protected:
        bool noDataSpecified;
        float noDataVal;
        bool onePassSD;
        bool calcSD;
        int numInputBands;
        bool *firstMean;
        bool *firstSD;
        bool calcMean;
        unsigned long *n;
        double *mean;
        double *meanSum;
        double *sumSq;
        double *min;
        double *max;
        double *sumDiffZ;
        double diffZ;
        rsgis::math::RSGISMathFunction *func;
    };
    
    class DllExport RSGISCalcImageStatisticsAllBands : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcImageStatisticsAllBands(int numberOutBands, bool calcSD, rsgis::math::RSGISMathFunction *func, bool ignoreZeros);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void getImageStats(ImageStats *inStats) throw(RSGISImageCalcException);
        void calcStdDev();
        ~RSGISCalcImageStatisticsAllBands();
    protected:
        bool ignoreZeros;
        bool calcSD;
        bool firstMean;
        bool firstSD;
        bool calcMean;
        unsigned long n;
        double mean;
        double meanSum;
        double min;
        double max;
        double sumDiffZ;
        double diffZ;
        rsgis::math::RSGISMathFunction *func;
        
    };
    
    class DllExport RSGISImagePercentiles
    {
    public:
        RSGISImagePercentiles();
        rsgis::math::Matrix* getPercentilesForAllBands(GDALDataset* dataset, float percentile, float noDataVal, bool noDataDefined)throw(rsgis::RSGISImageException);
        double getPercentile(GDALDataset *dataset, unsigned int band, float percentile, float noDataVal, bool noDataDefined)throw(rsgis::RSGISImageException);
        double getPercentile(GDALDataset *dataset, unsigned int band, GDALDataset *maskDS, int maskVal, float percentile, float noDataVal, bool noDataDefined)throw(rsgis::RSGISImageException);
        double getPercentile(GDALDataset *dataset, unsigned int band, GDALDataset *maskDS, int maskVal, float percentile, float noDataVal, bool noDataDefined, geos::geom::Envelope *env, bool quiet=false)throw(rsgis::RSGISImageException);
        ~RSGISImagePercentiles();
    };
    
    
    
    
    class DllExport RSGISGetPixelBandValues : public RSGISCalcImageValue
    {
    public:
        RSGISGetPixelBandValues(std::vector<double> *dataVals, unsigned int band, int maskVal, float noDataVal, bool noDataDefined):RSGISCalcImageValue(0)
        {
            this->dataVals = dataVals;
            this->band = band;
            this->maskVal = maskVal;
            this->noDataVal = noDataVal;
            this->noDataDefined = noDataDefined;
        };
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException) {throw RSGISImageCalcException("No implemented");};
        ~RSGISGetPixelBandValues(){};
    protected:
        std::vector<double> *dataVals;
        unsigned int band;
        int maskVal;
        float noDataVal;
        bool noDataDefined;
    };
    
    
    class DllExport RSGISImagePixelSummaries: public RSGISCalcImageValue
    {
    public:
        RSGISImagePixelSummaries(unsigned int numOutBands, rsgis::math::RSGISStatsSummary *statsSummary, float noDataValue=0, bool useNoDataValue=false);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISImagePixelSummaries();
    protected:
        rsgis::math::RSGISStatsSummary *statsSummary;
        float noDataValue;
        bool useNoDataValue;
    };
    
    class DllExport RSGISCalcImageHistogramNoData : public RSGISCalcImageValue
    {
    public:
        RSGISCalcImageHistogramNoData(unsigned int imgBand, bool noDataSpecified, float noDataVal, unsigned int numBins, float *binRanges, unsigned int *binCounts);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcImageHistogramNoData();
    protected:
        unsigned int imgBand;
        bool noDataSpecified;
        float noDataVal;
        unsigned int numBins;
        float *binRanges;
        unsigned int *binCounts;
    };
    
    class DllExport RSGISCalcImageStatisticsMaskStatsNoData : public RSGISCalcImageValue
    {
    public:
        RSGISCalcImageStatisticsMaskStatsNoData(int numberOutBands, int numInputBands, long maskVal, double *noDataVals, bool useNoData, bool calcSD, bool onePassSD = false);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands)throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals)throw(RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void getImageStats(ImageStats** inStats, int numInputBands) throw(RSGISImageCalcException);
        void calcStdDev();
        ~RSGISCalcImageStatisticsMaskStatsNoData();
    protected:
        bool onePassSD;
        bool calcSD;
        int numInputBands;
        bool *firstMean;
        bool *firstSD;
        bool calcMean;
        unsigned long *n;
        double *mean;
        double *meanSum;
        double *sumSq;
        double *min;
        double *max;
        double *sumDiffZ;
        double diffZ;
        double *noDataVals;
        long maskVal;
        bool useNoData;
    };
    
    
    
    class DllExport RSGISCalcMultiImageStatSummaries: public RSGISCalcImageValue
    {
    public:
        RSGISCalcMultiImageStatSummaries(unsigned int numOutBands, rsgis::math::rsgissummarytype sumType, unsigned int numInImgs, unsigned int numInImgBands, float noDataValue=0, bool useNoDataValue=false);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcMultiImageStatSummaries();
    protected:
        rsgis::math::rsgissummarytype sumType;
        unsigned int numInImgBands;
        unsigned int numInImgs;
        unsigned int totNumInBands;
        float noDataValue;
        bool useNoDataValue;
        rsgis::math::RSGISMathsUtils *mathUtils;
        rsgis::math::RSGISStatsSummary *statsSumObj;
        std::vector<double> *data;
    };
    
    
    class DllExport RSGISCalcImageDifference: public RSGISCalcImageValue
    {
    public:
        RSGISCalcImageDifference(unsigned int numOutBands);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcImageDifference();
    };
    
    
    
    
	
}}
#endif


