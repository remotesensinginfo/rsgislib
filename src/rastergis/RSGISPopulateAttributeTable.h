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

#include "rastergis/RSGISAttributeTable.h"
#include "rastergis/RSGISAttributeTableMem.h"
#include "rastergis/RSGISCalcClumpStats.h"

#include "math/RSGISMathsUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "gsl/gsl_statistics_double.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

namespace rsgis{namespace rastergis{
    
    struct RSGISBandAttStatsMeanLit
    {
        unsigned int band;
        std::string fieldName;
        unsigned int fieldIdx;
        RSGISAttributeDataType fieldDT;
        bool useUpperVals;
        bool calcMin;
        std::string minField;
        unsigned int minIdx;
        bool calcMax;
        std::string maxField;
        unsigned int maxIdx;
        bool calcMean;
        std::string meanField;
        unsigned int meanIdx;
        bool calcStdDev;
        std::string stdDevField;
        unsigned int stdDevIdx;
        bool calcMedian;
        std::string medianField;
        unsigned int medianIdx;
        bool calcSum;
        std::string sumField;
        unsigned int sumIdx;
    };
    
    
    struct RSGISBandAttName
    {
        unsigned int band;
        std::string attName;
        unsigned int fieldIdx;
        RSGISAttributeDataType fieldDT;
    };
    
    
    class RSGISPopulateAttributeTableBandMeans
    {
    public:
        RSGISPopulateAttributeTableBandMeans();
        RSGISAttributeTable* populateWithBandsMeans(GDALDataset **datasets, int numDatasets, std::string attrPrefix)throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        void populateWithBandStatistics(GDALDataset **datasets, int numDatasets, std::vector<RSGISBandAttStats*> *bandStats) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        unsigned long long calcMaxValue(GDALDataset *dataset)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISPopulateAttributeTableBandMeans();
    };
        
    class RSGISPopulateAttributeTableBandMeansCalcImg : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISPopulateAttributeTableBandMeansCalcImg(RSGISAttributeTable *attTable, std::string attrPrefix, unsigned int pxlCountIdx, unsigned int *bandMeanIdxs, unsigned int numBandMeanIdxs);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISPopulateAttributeTableBandMeansCalcImg();
    protected:
        RSGISAttributeTable *attTable;
        std::string attrPrefix;
        unsigned int pxlCountIdx;
        unsigned int *bandMeanIdxs;
        unsigned int numBandMeanIdxs;
    };
    
    
    class RSGISPopulateAttributeTableBandWithSumAndMeans
    {
    public:
        RSGISPopulateAttributeTableBandWithSumAndMeans();
        void populateWithBandStatistics(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, std::vector<RSGISBandAttStats*> *bandStats) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandWithSumAndMeans();
    };
    
    
    class RSGISPopulateAttributeTableBandStats
    {
    public:
        RSGISPopulateAttributeTableBandStats();
        void populateWithBandStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, std::vector<RSGISBandAttStats*> *bandStats) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        void populateWithBandStatisticsWithinAtt(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, std::vector<RSGISBandAttStats*> *bandStats) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandStats();
    };
    
    class RSGISPopulateAttributeTableBandThresholdedStats
    {
    public:
        RSGISPopulateAttributeTableBandThresholdedStats();
        void populateWithBandStatisticsWithinAtt(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, std::vector<RSGISBandAttStats*> *bandStats) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandThresholdedStats();
    };
    
    class RSGISPopulateAttributeTableBandStatsMeanLit
    {
    public:
        RSGISPopulateAttributeTableBandStatsMeanLit();
        void populateWithBandStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, std::vector<RSGISBandAttStats*> *bandStats, unsigned int meanLitBand, std::string meanLitField, bool useMeanLitValAbove) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        void populateWithBandStatisticsWithinAtt(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, std::vector<RSGISBandAttStats*> *bandStats, unsigned int meanLitBand, std::string meanLitField, bool useMeanLitValAbove) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandStatsMeanLit();
    };
    
    class RSGISPopulateAttributeTableBandStatsMeanLitBands
    {
    public:
        RSGISPopulateAttributeTableBandStatsMeanLitBands();
        void populateWithBandStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, std::vector<RSGISBandAttStatsMeanLit*> *bandStats) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableBandStatsMeanLitBands();
    };
    
    class RSGISGetPixelValuesForClumps : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISGetPixelValuesForClumps(std::vector<std::vector<double> > **clumpData, unsigned int numDataBands, unsigned int *dataBandIdxs);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISGetPixelValuesForClumps();
    protected:
        std::vector<std::vector<double> > **clumpData;
        unsigned int numBands;
        unsigned int numDataBands;
        unsigned int *dataBandIdxs;
    };
    
    class RSGISCalcClumpStatsWithinAtt : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISCalcClumpStatsWithinAtt(RSGISAttributeTable *attTable, std::vector<RSGISBandAttStats*> *bandStats, bool calcStdDev, unsigned int pxlCountIdx, unsigned int firstFieldIdx);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISCalcClumpStatsWithinAtt();
    protected:
        RSGISAttributeTable *attTable;
        std::vector<RSGISBandAttStats*> *bandStats;
        bool calcStdDev;
        unsigned int pxlCountIdx;
        unsigned int firstFieldIdx;
    };
    
    class RSGISCalcClumpThresholdedStatsWithinAtt : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISCalcClumpThresholdedStatsWithinAtt(RSGISAttributeTable *attTable, std::vector<RSGISBandAttStats*> *bandStats, bool calcStdDev, unsigned int firstFieldIdx);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISCalcClumpThresholdedStatsWithinAtt();
    protected:
        RSGISAttributeTable *attTable;
        std::vector<RSGISBandAttStats*> *bandStats;
        bool calcStdDev;
        unsigned int firstFieldIdx;
    };
    
    class RSGISGetPixelValuesForClumpsMeanLit : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISGetPixelValuesForClumpsMeanLit(RSGISAttributeTable *attTable, std::vector<std::vector<double> > **clumpData, unsigned int numDataBands, unsigned int *dataBandIdxs, unsigned int meanLitBand, unsigned int meanLitFieldIdx, RSGISAttributeDataType meanLitFieldDT, bool useMeanLitValAbove);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISGetPixelValuesForClumpsMeanLit();
    protected:
        RSGISAttributeTable *attTable;
        std::vector<std::vector<double> > **clumpData;
        unsigned int numBands;
        unsigned int numDataBands;
        unsigned int *dataBandIdxs;
        unsigned int meanLitBand;
        unsigned int meanLitFieldIdx;
        RSGISAttributeDataType meanLitFieldDT;
        bool useMeanLitValAbove;
    };
    
    class RSGISClumpsMeanLitStatsWithinAtt : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISClumpsMeanLitStatsWithinAtt(RSGISAttributeTable *attTable, std::vector<RSGISBandAttStats*> *bandStats, bool calcStdDev, unsigned int pxlCountIdx, unsigned int firstFieldIdx, unsigned int meanLitBand, unsigned int meanLitFieldIdx, RSGISAttributeDataType meanLitFieldDT, bool useMeanLitValAbove);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISClumpsMeanLitStatsWithinAtt();
    protected:
        RSGISAttributeTable *attTable;
        std::vector<RSGISBandAttStats*> *bandStats;
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
        void populateWithImageStatisticsInMem(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, RSGISBandAttStats *imageStats, float noDataVal, bool noDataValDefined) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISPopulateAttributeTableImageStats();
    };
    
    class RSGISGetAllBandPixelValuesForClumps : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISGetAllBandPixelValuesForClumps(std::vector<double> **clumpData, float noDataVal, bool noDataValDefined);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISGetAllBandPixelValuesForClumps();
    protected:
        std::vector<double> **clumpData;
        unsigned int numBands;
        float noDataVal;
        bool noDataValDefined;
    };
    
    
    class RSGISCalcAttTableWithinSegmentPixelDistStats
    {
    public:
        RSGISCalcAttTableWithinSegmentPixelDistStats();
        void populateWithImageStatistics(RSGISAttributeTable *attTable, GDALDataset **datasets, int numDatasets, std::vector<RSGISBandAttName*> *bands, RSGISBandAttStats *imageStats) throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISCalcAttTableWithinSegmentPixelDistStats();
    };
    
    class RSGISCalcEucDistWithSegments : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISCalcEucDistWithSegments(RSGISAttributeTable *attTable, std::vector<double> **clumpData, std::vector<RSGISBandAttName*> *bands);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISCalcEucDistWithSegments();
    protected:
        RSGISAttributeTable *attTable;
        std::vector<RSGISBandAttName*> *bands;
        std::vector<double> **clumpData;
        unsigned int numBands;
    };
    
    class RSGISCalcClumpSumAndCount : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISCalcClumpSumAndCount(size_t *pxlCount, double **sumVals, size_t *bandIdxs, size_t numSpecBands, size_t numFeats);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
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
