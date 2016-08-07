/*
 *  RSGISPopRATWithStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/05/2014.
 *  Copyright 2014 RSGISLib.
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

#ifndef RSGISPopRATWithStats_H
#define RSGISPopRATWithStats_H

#include <string>
#include <vector>
#include <math.h>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathsUtils.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

namespace rsgis{namespace rastergis{
	
    struct DllExport RSGISBandAttStats
    {
        unsigned int band;
        bool calcMin;
        std::string minField;
        unsigned int minFieldIdx;
        unsigned int minLocalIdx;
        bool calcMax;
        std::string maxField;
        unsigned int maxFieldIdx;
        unsigned int maxLocalIdx;
        bool calcMean;
        std::string meanField;
        unsigned int meanFieldIdx;
        unsigned int meanLocalIdx;
        bool calcStdDev;
        std::string stdDevField;
        unsigned int stdDevFieldIdx;
        unsigned int stdDevLocalIdx;
        bool calcSum;
        std::string sumField;
        unsigned int sumFieldIdx;
        unsigned int sumLocalIdx;
        
        void init()
        {
            band = 0;
            calcMin = false;
            minField = "";
            minFieldIdx = 0;
            minLocalIdx = 0;
            calcMax = false;
            maxField = "";
            maxFieldIdx = 0;
            maxLocalIdx = 0;
            calcMean = false;
            meanField = "";
            meanFieldIdx = 0;
            meanLocalIdx = 0;
            calcStdDev = false;
            stdDevField = "";
            stdDevFieldIdx = 0;
            stdDevLocalIdx = 0;
            calcSum = false;
            sumField = "";
            sumFieldIdx = 0;
            sumLocalIdx = 0;
        };
    };
    
    struct DllExport RSGISBandAttPercentiles
    {
        float percentile;
        std::string fieldName;
        unsigned int fieldIdx;
    };
    
    class DllExport RSGISPopRATWithStats
    {
    public:
        RSGISPopRATWithStats();
        void populateRATWithBasicStats(GDALDataset *inputClumps, GDALDataset *inputValsImage, std::vector<RSGISBandAttStats*> *bandStats, unsigned int ratBand)throw(RSGISAttributeTableException);
        void populateRATWithPercentileStats(GDALDataset *inputClumps, GDALDataset *inputValsImage, unsigned int band, std::vector<RSGISBandAttPercentiles*> *bandStats, unsigned int ratBand, unsigned int numHistBins)throw(RSGISAttributeTableException);
        void populateRATWithMeanLitStats(GDALDataset *inputClumps, GDALDataset *inputValsImage, GDALDataset *inputMeanLitImage, unsigned int meanLitBand, std::string meanLitCol, std::string pxlCountCol, std::vector<RSGISBandAttStats*> *bandStats, unsigned int ratBand)throw(RSGISAttributeTableException);
        void populateRATWithModeStats(GDALDataset *inputClumps, GDALDataset *inputValsImage, std::string outColsName, bool useNoDataVal, long noDataVal, bool outNoDataVal, unsigned int modeBand, unsigned int ratBand)throw(RSGISAttributeTableException);
        void populateRATWithPopValidPixels(GDALDataset *inputClumps, GDALDataset *inputValsImage, std::string outColsName, double noDataVal, unsigned int ratBand)throw(RSGISAttributeTableException);
        ~RSGISPopRATWithStats();
    };
    
    class DllExport RSGISCalcClusterPxlValueStats : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCalcClusterPxlValueStats(double **statsData, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal, unsigned int ratBand);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCalcClusterPxlValueStats();
    private:
        double **statsData;
        std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats;
        bool *firstVal;
        unsigned int ratBand;
    };
    
    class DllExport RSGISCalcClusterPxlValueStdDev : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCalcClusterPxlValueStdDev(double **stdDevData, double **statsData, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal, unsigned int ratBand);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCalcClusterPxlValueStdDev();
    private:
        double **stdDevData;
        double **statsData;
        std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats;
        bool *firstVal;
        unsigned int ratBand;
	};
    
    
    class DllExport RSGISCalcClusterPxlValueHistograms : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCalcClusterPxlValueHistograms(unsigned int **clumpHistData, double *binBounds, unsigned int numBins, unsigned int ratBand, unsigned int imgBand, double noDataVal, bool useNoDataVal);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCalcClusterPxlValueHistograms();
    private:
        unsigned int **clumpHistData;
        double *binBounds;
        unsigned int numBins;
        unsigned int ratBand;
        unsigned int imgBand;
        double noDataVal;
        bool useNoDataVal;
	};
    
    
    
    class DllExport RSGISCalcClusterPxlValueStatsMeanLit : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCalcClusterPxlValueStatsMeanLit(double **statsData, double *pxlCount, double *meanLitColVals, unsigned int meanLitBandArrIdx, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal, unsigned int ratBand);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCalcClusterPxlValueStatsMeanLit();
    private:
        double **statsData;
        double *pxlCount;
        double *meanLitColVals;
        unsigned int meanLitBandArrIdx;
        std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats;
        bool *firstVal;
        unsigned int ratBand;
    };
    
    class DllExport RSGISCalcClusterPxlValueStdDevMeanLit : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCalcClusterPxlValueStdDevMeanLit(double **stdDevData, double **statsData, double *pxlCount, double *meanLitColVals, unsigned int meanLitBandArrIdx, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal, unsigned int ratBand);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCalcClusterPxlValueStdDevMeanLit();
    private:
        double **stdDevData;
        double **statsData;
        double *pxlCount;
        double *meanLitColVals;
        unsigned int meanLitBandArrIdx;
        std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats;
        bool *firstVal;
        unsigned int ratBand;
	};
    
    class DllExport RSGISCalcImageMinMaxIntVals : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcImageMinMaxIntVals(long *minVal, long *maxVal, bool useNoDataVal, long noDataVal, int band);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        ~RSGISCalcImageMinMaxIntVals();
    private:
        long *minVal;
        long *maxVal;
        bool useNoDataVal;
        long noDataVal;
        bool first;
        int band;
    };
    
    
    class DllExport RSGISCalcClusterModeHistograms : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcClusterModeHistograms(unsigned long **clumpHists, long *modeBinVals, unsigned int numBins, bool useNoDataVal, long noDataVal, unsigned int ratBandIdx, unsigned int imgBandIdx);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        ~RSGISCalcClusterModeHistograms();
    private:
        unsigned long **clumpHists;
        long *modeBinVals;
        unsigned int numBins;
        unsigned int ratBandIdx;
        unsigned int imgBandIdx;
        bool useNoDataVal;
        long noDataVal;
    };
    
    
    class DllExport RSGISCalcCountValidPxlValues : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcCountValidPxlValues(unsigned long *numPxls, unsigned long *numValidPxls, unsigned long numFeats, double noDataVal, unsigned int ratBandIdx);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        ~RSGISCalcCountValidPxlValues();
    private:
        unsigned long *numPxls;
        unsigned long *numValidPxls;
        unsigned long numFeats;
        double noDataVal;
        unsigned int ratBandIdx;
    };
    
    
}}

#endif

