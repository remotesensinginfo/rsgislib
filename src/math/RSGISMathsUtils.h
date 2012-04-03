/*
 *  RSGISMathUtils.h
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


#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

#include "gsl/gsl_statistics_double.h"

#include "math/RSGISMathException.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMatrices.h"

using boost::lexical_cast;
using boost::bad_lexical_cast;
using namespace std;

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
		sumtype_histogram
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
    
    struct RSGISStatsSummary
    {
        bool calcMin;
        bool calcMax;
        bool calcMean;
        bool calcSum;
        bool calcStdDev;
        bool calcMedian;
        double min;
        double max;
        double mean;
        double sum;
        double stdDev;
        double median;
    };
	
	class RSGISMathsUtils
		{
		public:
			RSGISMathsUtils();
			double strtodouble(string inValue)throw(RSGISMathException);
			float strtofloat(string inValue)throw(RSGISMathException);
			int strtoint(string inValue)throw(RSGISMathException);
			unsigned int strtounsignedint(string inValue)throw(RSGISMathException);
			long strtolong(string inValue)throw(RSGISMathException);
            unsigned long strtounsignedlong(string inValue)throw(RSGISMathException);
			string doubletostring(double number)throw(RSGISMathException);
			string floattostring(float number)throw(RSGISMathException);
			string inttostring(int number)throw(RSGISMathException);
            string uinttostring(unsigned int number)throw(RSGISMathException);
			string longtostring(long number)throw(RSGISMathException);
			void getPowerSet(int numItems, int *numBands, int **indexes, int numSets) throw(RSGISMathException);
			float degreesToRadians(float angle);
			float radiansToDegrees(float angle);
			int roundUp(double number);
			Vector* calculateCumulativeArea(Vector *inVec, Matrix *widths);
			Matrix* calculateCumulativeArea(Matrix *inData, Matrix *widths);
            void fitPlane(double *x, double *y, double *z, unsigned int numValues, double *a, double *b, double *c) throw(RSGISMathException);
            void generateStats(vector<double> *data, RSGISStatsSummary *stats) throw(RSGISMathException);
		};
	
}}

#endif
