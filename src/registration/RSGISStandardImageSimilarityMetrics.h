/*
 *  RSGISStandardImageSimilarityMetrics.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/09/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 *
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef RSGISStandardImageSimilarityMetrics_H
#define RSGISStandardImageSimilarityMetrics_H

#include <math.h>

#include "math/RSGISMathException.h"

#include "registration/RSGISImageSimilarityMetric.h"

#include "boost/math/special_functions/fpclassify.hpp"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_registration_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace reg{
    
	class DllExport RSGISEuclideanSimilarityMetric : public RSGISImageSimilarityMetric
	{
	public:
		RSGISEuclideanSimilarityMetric(){};
		float calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims) throw(rsgis::math::RSGISMathException);
		bool findMin(){return true;};
		~RSGISEuclideanSimilarityMetric(){};
	};

	class DllExport RSGISManhattanSimilarityMetric : public RSGISImageSimilarityMetric // Also referred to as the taxicab distance
	{
	public:
		RSGISManhattanSimilarityMetric(){};
		float calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims) throw(rsgis::math::RSGISMathException);
		bool findMin(){return true;};
		~RSGISManhattanSimilarityMetric(){};
	};

	class DllExport RSGISSquaredDifferenceSimilarityMetric : public RSGISImageSimilarityMetric
	{
	public:
		RSGISSquaredDifferenceSimilarityMetric(){};
		float calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims) throw(rsgis::math::RSGISMathException);
		bool findMin(){return true;};
		~RSGISSquaredDifferenceSimilarityMetric(){};
	};

	class DllExport RSGISCorrelationSimilarityMetric : public RSGISImageSimilarityMetric
	{
	public:
		RSGISCorrelationSimilarityMetric(){};
		float calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims) throw(rsgis::math::RSGISMathException);
		bool findMin(){return false;};
		~RSGISCorrelationSimilarityMetric(){};
	};
}}

#endif


