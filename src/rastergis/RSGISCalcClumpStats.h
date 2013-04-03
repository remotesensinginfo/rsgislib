/*
 *  RSGISCalcClumpStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISCalcClumpStats_H
#define RSGISCalcClumpStats_H

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathException.h"

#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

#include "gsl/gsl_statistics_double.h"

namespace rsgis{namespace rastergis{
	
    struct RSGISBandAttStats
    {
        unsigned int band;
        float threshold;
        bool calcCount;
        std::string countField;
        unsigned int countIdx;
        bool countIdxDef;
        bool calcMin;
        std::string minField;
        unsigned int minIdx;
        bool minIdxDef;
        bool calcMax;
        std::string maxField;
        unsigned int maxIdx;
        bool maxIdxDef;
        bool calcMean;
        std::string meanField;
        unsigned int meanIdx;
        bool meanIdxDef;
        bool calcStdDev;
        std::string stdDevField;
        unsigned int stdDevIdx;
        bool stdDevIdxDef;
        bool calcMedian;
        std::string medianField;
        unsigned int medianIdx;
        bool medianIdxDef;
        bool calcSum;
        std::string sumField;
        unsigned int sumIdx;
        bool sumIdxDef;
    };
    
    struct RSGISBandAttPercentiles
    {
        unsigned int band;
        unsigned int percentile;
        std::string fieldName;
        unsigned int fieldIdx;
        bool fieldIdxDef;
    };
    
    
    class RSGISCalcClumpStats
    {
    public:
        RSGISCalcClumpStats();
        void calcImageClumpStatistic(GDALDataset *clumpDS, GDALDataset *imageDS, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats) throw(rsgis::RSGISAttributeTableException);
        void calcImageClumpPercentiles(GDALDataset *clumpDS, GDALDataset *imageDS, std::vector<rsgis::rastergis::RSGISBandAttPercentiles*> *bandPercentiles) throw(rsgis::RSGISAttributeTableException);
        void populateColourTable(GDALDataset *clumpDS, GDALDataset *imageDS, unsigned int red, unsigned int green, unsigned int blue) throw(rsgis::RSGISAttributeTableException);
        void calcValidPixelCount(GDALDataset *clumpDS, GDALDataset *imageDS, float noData, std::string validPixelCount) throw(rsgis::RSGISAttributeTableException);
        ~RSGISCalcClumpStats();
    };
    
    
    class RSGISCalcClusterPxlValueStats : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISCalcClusterPxlValueStats(GDALRasterAttributeTable *attTable, int countColIdx, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCalcClusterPxlValueStats();
    private:
        GDALRasterAttributeTable *attTable;
        int countColIdx;
        std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats;
        bool *firstVal;
	};
    
    class RSGISCalcClusterPxlValueStdDev : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISCalcClusterPxlValueStdDev(GDALRasterAttributeTable *attTable, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCalcClusterPxlValueStdDev();
    private:
        GDALRasterAttributeTable *attTable;
        std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats;
        bool *firstVal;
	};
    
    class RSGISPopDataWithClusterPxlValue : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISPopDataWithClusterPxlValue(std::vector<double> ***data);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISPopDataWithClusterPxlValue();
    private:
        std::vector<double> ***data;
	};
    
    
    class RSGISGetColourTableValues : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISGetColourTableValues(size_t *redSum, size_t *greenSum, size_t *blueSum, size_t *countVals, unsigned int redIdx, unsigned int greenIdx, unsigned int blueIdx);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISGetColourTableValues();
    private:
        size_t *redSum;
        size_t *blueSum;
        size_t *greenSum;
        size_t *countVals;
        unsigned int redIdx;
        unsigned int greenIdx;
        unsigned int blueIdx;
	};
    
    class RSGISGetValidPxlCounts : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISGetValidPxlCounts(size_t *validPxlCount, float noDataValue);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISGetValidPxlCounts();
    private:
        size_t *validPxlCount;
        float noDataValue;
	};
    
    
    
    
    inline int StatsTextProgress( double dfComplete, const char *pszMessage, void *pData)
    {
        if( pszMessage != NULL )
            printf( "%d%% complete: %s\r", (int) (dfComplete*100), pszMessage );
        else
            printf( "%d%% complete.\r", (int) (dfComplete*100) );
        
        return true;
    };
    
    
    
    class RSGISPopulateWithImageStats
    {
    public:
        RSGISPopulateWithImageStats();
        void populateImageWithRasterGISStats(GDALDataset *clumpsDataset, bool addColourTable, bool calcImagePyramids) throw(rsgis::RSGISImageException);
        ~RSGISPopulateWithImageStats();
    };
    
    
    class RSGISGetClumpsHistogram : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISGetClumpsHistogram(unsigned int *histogram, unsigned int maxVal);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISGetClumpsHistogram();
    private:
        unsigned int *histogram;
        unsigned int maxVal;
	};
    
	
}}

#endif



