/*
 *  RSGISMathsUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/10/2008.
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

#ifndef RSGISMathsUtils_H
#define RSGISMathsUtils_H

#include <string>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <list>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

#include "gsl/gsl_statistics_double.h"

#include "math/RSGISMathException.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMatrices.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace math{
    
	enum rsgissummarytype
	{
		sumtype_mean,
		sumtype_max,
		sumtype_min,
		sumtype_stddev,
		sumtype_count,
		sumtype_aggregate,
		sumtype_value,
		sumtype_histogram,
        sumtype_median,
        sumtype_mode,
        sumtype_sum,
        sumtype_range
	};
    
    enum rsgiscomparetype
    {
        compare_gt,
        compare_lt,
        compare_gteq,
        compare_lteq,
        compare_eq,
        compare_neq
    };
    
    enum rsgisdistmetrics
    {
        rsgis_euclidean = 1,
        rsgis_manhatten = 2,
        rsgis_mahalanobis = 3,
        rsgis_minkowski = 4,
        rsgis_chebyshev = 5,
        rsgis_mutualinfo = 6
    };
    
    enum rsgissort
    {
        rsgis_ascending = 1,
        rsgis_descending = 2
    };
    
    struct DllExport RSGISStatsSummary
    {
        bool calcMin;
        bool calcMax;
        bool calcMean;
        bool calcSum;
        bool calcStdDev;
        bool calcMedian;
        bool calcMode;
        bool calcVariance;
        double min;
        double max;
        double mean;
        double sum;
        double stdDev;
        double median;
        double mode;
        double variance;
    };
    
    struct DllExport RSGIS2DHistBin
    {
        RSGIS2DHistBin(double binCentre1, double binCentre2, double freq)
        {
            this->binCentre1 = binCentre1;
            this->binCentre2 = binCentre2;
            this->freq = freq;
        };
        double binCentre1;
        double binCentre2;
        double freq;
    };
    
    struct DllExport RSGISLinearFitVals
    {
        double  slope;
        double  intercept;
        double  coeff;
        double  pvar;
    };
    
    inline bool comparePairsData(std::pair<size_t, double> firstVal, std::pair<size_t, double> secondVal)
    {
        return firstVal.second > secondVal.second;
    };
	
	class DllExport RSGISMathsUtils
		{
		public:
			RSGISMathsUtils();
			double strtodouble(std::string inValue);
			float strtofloat(std::string inValue);
			int strtoint(std::string inValue);
			unsigned int strtounsignedint(std::string inValue);
			long strtolong(std::string inValue);
            unsigned long strtounsignedlong(std::string inValue);
			std::string doubletostring(double number);
			std::string floattostring(float number);
			std::string inttostring(int number);
            std::string uinttostring(unsigned int number);
			std::string longtostring(long number);
			void getPowerSet(int numItems, int *numBands, int **indexes, int numSets);
			float degreesToRadians(float angle);
			float radiansToDegrees(float angle);
			int roundUp(double number);
			Vector* calculateCumulativeArea(Vector *inVec, Matrix *widths);
			Matrix* calculateCumulativeArea(Matrix *inData, Matrix *widths);
            void fitPlane(double *x, double *y, double *z, unsigned int numValues, double *a, double *b, double *c);
            void generateStats(std::vector<double> *data, RSGISStatsSummary *stats);
            void initStatsSummary(RSGISStatsSummary *stats);
            void initStatsSummaryValues(RSGISStatsSummary *stats);
            bool angleWithinRange(float angle, float lower, float upper);
            double calcPercentile(float percentile, double *binBounds, double binWidth, unsigned int numBins, unsigned int *hist);
            double calcPercentile(float percentile, double histMinVal, double binWidth, unsigned int numBins, unsigned int *hist);
            double* calcMeanVector(double **data, size_t n, size_t m, size_t sMIdx, size_t eMIdx);
            double** calcCovarianceMatrix(double **data, double *meanVec, size_t n, size_t m, size_t sMIdx, size_t eMIdx);
            std::vector<std::pair<size_t, double> >* sampleUseHistogramMethod(std::vector<std::pair<size_t, double> > *inData, double minVal, double maxVal, double binWidth, float propOfPop);
            std::vector<std::pair<size_t, double> >** calcHistogram(std::vector<std::pair<size_t, double> > *inData, double minVal, double maxVal, double binWidth, size_t *numBins);
            std::vector<std::pair<double, double> >* calcHistogram(std::vector<double> *data, double minVal, double maxVal, double binWidth, bool norm);
            std::vector<std::vector<RSGIS2DHistBin>* >* calc2DHistogram(std::vector<double> *data1, double minVal1, double maxVal1, double binWidth1, std::vector<double> *data2, double minVal2, double maxVal2, double binWidth2, bool norm);
            unsigned int* calcHistogram(double *data, size_t numVals, double binWidth, double *minVal, double *maxVal, unsigned int *numBins, bool ignoreFirstVal=false);
            RSGISLinearFitVals* performLinearFit(double *xData, double *yData, size_t nVals, double noDataVal);
            void performLinearFit(double *xData, double *yData, size_t nVals, double noDataVal, RSGISLinearFitVals *fitVals);
            double predFromLinearFit(double val, RSGISLinearFitVals *fitVals, double minAccVal, double maxAccVal);
		};
	
}}

#endif
