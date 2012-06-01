/*
 *  RSGISPopulateAttributeTable.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/02/2012.
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

#ifndef RSGISPopulateAttributeTable_H
#define RSGISPopulateAttributeTable_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "common/RSGISAttributeTableException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageSingleValue.h"

#include "rastergis/RSGISAttributeTable.h"
#include "rastergis/RSGISAttributeTableMem.h"

#include "math/RSGISMathsUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "gsl/gsl_statistics_double.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

using namespace std;
using namespace rsgis;
using namespace rsgis::utils;
using namespace rsgis::math;
using namespace rsgis::img;
using boost::lexical_cast;
using boost::bad_lexical_cast;

namespace rsgis{namespace rastergis{
    
    struct RSGISBandAttStats
    {
        unsigned int band;
        float threshold;
        bool calcCount;
        string countField;
        unsigned int countIdx;
        bool calcMin;
        string minField;
        unsigned int minIdx;
        bool calcMax;
        string maxField;
        unsigned int maxIdx;
        bool calcMean;
        string meanField;
        unsigned int meanIdx;
        bool calcStdDev;
        string stdDevField;
        unsigned int stdDevIdx;
        bool calcMedian;
        string medianField;
        unsigned int medianIdx;
        bool calcSum;
        string sumField;
        unsigned int sumIdx;
    };
    
    struct RSGISBandAttStatsMeanLit
    {
        unsigned int band;
        string fieldName;
        unsigned int fieldIdx;
        RSGISAttributeDataType fieldDT;
        bool useUpperVals;
        bool calcMin;
        string minField;
        unsigned int minIdx;
        bool calcMax;
        string maxField;
        unsigned int maxIdx;
        bool calcMean;
        string meanField;
        unsigned int meanIdx;
        bool calcStdDev;
        string stdDevField;
        unsigned int stdDevIdx;
        bool calcMedian;
        string medianField;
        unsigned int medianIdx;
        bool calcSum;
        string sumField;
        unsigned int sumIdx;
    };
    
    
    struct RSGISBandAttName
    {
        unsigned int band;
        string attName;
        unsigned int fieldIdx;
        RSGISAttributeDataType fieldDT;
    };
    
    
    class RSGISPopulateAttributeTableBandMeans
    {
    public:
        RSGISPopulateAttributeTableBandMeans();
        RSGISAttributeTable* populateWithBandsMeans(GDALDataset **datasets, int numDatasets, string attrPrefix)throw(RSGISImageCalcException, RSGISAttributeTableException);
        void populateWithBandStatistics(GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException);
        unsigned long long calcMaxValue(GDALDataset *dataset)throw(RSGISImageCalcException);
        ~RSGISPopulateAttributeTableBandMeans();
    };
        
    class RSGISPopulateAttributeTableBandMeansCalcImg : public RSGISCalcImageValue
    {
    public: 
        RSGISPopulateAttributeTableBandMeansCalcImg(RSGISAttributeTable *attTable, string attrPrefix, unsigned int pxlCountIdx, unsigned int *bandMeanIdxs, unsigned int numBandMeanIdxs);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISPopulateAttributeTableBandMeansCalcImg();
    protected:
        RSGISAttributeTable *attTable;
        string attrPrefix;
        unsigned int pxlCountIdx;
        unsigned int *bandMeanIdxs;
        unsigned int numBandMeanIdxs;
    };
    
    
    class RSGISPopulateAttributeTableBandWithSumAndMeans
    {
    public:
        RSGISPopulateAttributeTableBandWithSumAndMeans();
        void populateWithBandStatistics(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandWithSumAndMeans();
    };
    
    
    class RSGISPopulateAttributeTableBandStats
    {
    public:
        RSGISPopulateAttributeTableBandStats();
        void populateWithBandStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException);
        void populateWithBandStatisticsWithinAtt(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandStats();
    };
    
    class RSGISPopulateAttributeTableBandThresholdedStats
    {
    public:
        RSGISPopulateAttributeTableBandThresholdedStats();
        void populateWithBandStatisticsWithinAtt(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandThresholdedStats();
    };
    
    class RSGISPopulateAttributeTableBandStatsMeanLit
    {
    public:
        RSGISPopulateAttributeTableBandStatsMeanLit();
        void populateWithBandStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats, unsigned int meanLitBand, string meanLitField, bool useMeanLitValAbove) throw(RSGISImageCalcException, RSGISAttributeTableException);
        void populateWithBandStatisticsWithinAtt(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStats*> *bandStats, unsigned int meanLitBand, string meanLitField, bool useMeanLitValAbove) throw(RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandStatsMeanLit();
    };
    
    class RSGISPopulateAttributeTableBandStatsMeanLitBands
    {
    public:
        RSGISPopulateAttributeTableBandStatsMeanLitBands();
        void populateWithBandStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttStatsMeanLit*> *bandStats) throw(RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandStatsMeanLitBands();
    };
    
    class RSGISGetPixelValuesForClumps : public RSGISCalcImageValue
    {
    public: 
        RSGISGetPixelValuesForClumps(vector<vector<double> > **clumpData, unsigned int numDataBands, unsigned int *dataBandIdxs);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISGetPixelValuesForClumps();
    protected:
        vector<vector<double> > **clumpData;
        unsigned int numBands;
        unsigned int numDataBands;
        unsigned int *dataBandIdxs;
    };
    
    class RSGISCalcClumpStatsWithinAtt : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcClumpStatsWithinAtt(RSGISAttributeTable *attTable, vector<RSGISBandAttStats*> *bandStats, bool calcStdDev, unsigned int pxlCountIdx, unsigned int firstFieldIdx);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcClumpStatsWithinAtt();
    protected:
        RSGISAttributeTable *attTable;
        vector<RSGISBandAttStats*> *bandStats;
        bool calcStdDev;
        unsigned int pxlCountIdx;
        unsigned int firstFieldIdx;
    };
    
    class RSGISCalcClumpThresholdedStatsWithinAtt : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcClumpThresholdedStatsWithinAtt(RSGISAttributeTable *attTable, vector<RSGISBandAttStats*> *bandStats, bool calcStdDev, unsigned int firstFieldIdx);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcClumpThresholdedStatsWithinAtt();
    protected:
        RSGISAttributeTable *attTable;
        vector<RSGISBandAttStats*> *bandStats;
        bool calcStdDev;
        unsigned int firstFieldIdx;
    };
    
    class RSGISGetPixelValuesForClumpsMeanLit : public RSGISCalcImageValue
    {
    public: 
        RSGISGetPixelValuesForClumpsMeanLit(RSGISAttributeTable *attTable, vector<vector<double> > **clumpData, unsigned int numDataBands, unsigned int *dataBandIdxs, unsigned int meanLitBand, unsigned int meanLitFieldIdx, RSGISAttributeDataType meanLitFieldDT, bool useMeanLitValAbove);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISGetPixelValuesForClumpsMeanLit();
    protected:
        RSGISAttributeTable *attTable;
        vector<vector<double> > **clumpData;
        unsigned int numBands;
        unsigned int numDataBands;
        unsigned int *dataBandIdxs;
        unsigned int meanLitBand;
        unsigned int meanLitFieldIdx;
        RSGISAttributeDataType meanLitFieldDT;
        bool useMeanLitValAbove;
    };
    
    class RSGISClumpsMeanLitStatsWithinAtt : public RSGISCalcImageValue
    {
    public: 
        RSGISClumpsMeanLitStatsWithinAtt(RSGISAttributeTable *attTable, vector<RSGISBandAttStats*> *bandStats, bool calcStdDev, unsigned int pxlCountIdx, unsigned int firstFieldIdx, unsigned int meanLitBand, unsigned int meanLitFieldIdx, RSGISAttributeDataType meanLitFieldDT, bool useMeanLitValAbove);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISClumpsMeanLitStatsWithinAtt();
    protected:
        RSGISAttributeTable *attTable;
        vector<RSGISBandAttStats*> *bandStats;
        bool calcStdDev;
        unsigned int pxlCountIdx;
        unsigned int firstFieldIdx;
        unsigned int meanLitBand;
        unsigned int meanLitFieldIdx;
        RSGISAttributeDataType meanLitFieldDT;
        bool useMeanLitValAbove;
    };
    
    class RSGISPopulateAttributeTableImageStats
    {
    public:
        RSGISPopulateAttributeTableImageStats();
        void populateWithImageStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, RSGISBandAttStats *imageStats) throw(RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableImageStats();
    };
    
    class RSGISGetAllBandPixelValuesForClumps : public RSGISCalcImageValue
    {
    public: 
        RSGISGetAllBandPixelValuesForClumps(vector<double> **clumpData);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISGetAllBandPixelValuesForClumps();
    protected:
        vector<double> **clumpData;
        unsigned int numBands;
    };
    
    
    class RSGISCalcAttTableWithinSegmentPixelDistStats
    {
    public:
        RSGISCalcAttTableWithinSegmentPixelDistStats();
        void populateWithImageStatistics(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, vector<RSGISBandAttName*> *bands, RSGISBandAttStats *imageStats) throw(RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISCalcAttTableWithinSegmentPixelDistStats();
    };
    
    class RSGISCalcEucDistWithSegments : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcEucDistWithSegments(RSGISAttributeTable *attTable, vector<double> **clumpData, vector<RSGISBandAttName*> *bands);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcEucDistWithSegments();
    protected:
        RSGISAttributeTable *attTable;
        vector<RSGISBandAttName*> *bands;
        vector<double> **clumpData;
        unsigned int numBands;
    };
    
    class RSGISCalcClumpSumAndCount : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcClumpSumAndCount(size_t *pxlCount, double **sumVals, size_t *bandIdxs, size_t numSpecBands, size_t numFeats);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcClumpSumAndCount();
    protected:
        size_t *pxlCount;
        double **sumVals;
        size_t *bandIdxs;
        size_t numSpecBands;
        size_t numFeats;
    };
    
    
    
    
    
}}

#endif
