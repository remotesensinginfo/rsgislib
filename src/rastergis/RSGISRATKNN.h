/*
 *  RSGISRATKNN.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 28/02/2015.
 *  Copyright 2015 RSGISLib.
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

#ifndef RSGISRATKNN_H
#define RSGISRATKNN_H

#include <iostream>
#include <string>
#include <vector>
#include <list>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalcValue.h"
#include "rastergis/RSGISRATCalc.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISDistMetrics.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_rastergis_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace rastergis{
    
    
    class DllExport RSGISApplyRATKNN
    {
    public:
        RSGISApplyRATKNN();
        void applyKNNExtrapolation(GDALDataset *clumpsDS, std::string inExtrapField, std::string outExtrapField, std::string trainRegionsField, std::string applyRegionsField, bool useApplyField, std::vector<std::string> fields, unsigned int kFeatures=12, rsgis::math::rsgisdistmetrics distKNN=rsgis::math::rsgis_mahalanobis, float distThreshold=100000, rsgis::math::rsgissummarytype summeriseKNN=rsgis::math::sumtype_median, unsigned int ratBand=1) throw(RSGISAttributeTableException);
        ~RSGISApplyRATKNN();
    };
    
    class DllExport RSGISCountTrainingValues : public RSGISRATCalcValue
    {
    public:
        RSGISCountTrainingValues(size_t *numTrainPts);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISCountTrainingValues();
    private:
        size_t *numTrainPts;
    };
    
    class DllExport RSGISExtractTrainingValues : public RSGISRATCalcValue
    {
    public:
        RSGISExtractTrainingValues(double **trainData, size_t n, size_t m);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        void resetCounter();
        ~RSGISExtractTrainingValues();
    private:
        double **trainData;
        size_t n;
        size_t m;
        size_t counter;
    };
    
    class DllExport RSGISPerformKNNCalcValues : public RSGISRATCalcValue
    {
    public:
        RSGISPerformKNNCalcValues(double **trainData, size_t n, size_t m, unsigned int kFeatures, rsgis::math::RSGISCalcDistMetric *calcDist, float distThreshold, rsgis::math::RSGISStatsSummary *mathSumStats);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        void findKVals(std::list<std::pair<double, double*> > *kVals, double *featVals)throw(RSGISAttributeTableException);
        ~RSGISPerformKNNCalcValues();
    private:
        double **trainData;
        size_t n;
        size_t m;
        unsigned int kFeatures;
        rsgis::math::RSGISCalcDistMetric *calcDist;
        float distThreshold;
        rsgis::math::RSGISStatsSummary *mathSumStats;
    };
    
    
    
}}

#endif
