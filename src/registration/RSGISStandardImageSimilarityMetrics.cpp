/*
 *  RSGISStandardImageSimilarityMetrics.cpp
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

#include "RSGISStandardImageSimilarityMetrics.h"


namespace rsgis{namespace reg{
	
	float RSGISEuclideanSimilarityMetric::calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims) throw(rsgis::math::RSGISMathException)
	{
		unsigned int totalNumVals = numDims * numVals;
		
		double sqDiff = 0;
		
		for(unsigned int i = 0; i < numDims; ++i)
		{
			for(unsigned int j = 0; j < numVals; ++j)
			{
				if((!((boost::math::isnan)(reference[i][j]))) & (!((boost::math::isnan)(floating[i][j]))))
				{
					sqDiff += ((reference[i][j] - floating[i][j]) * (reference[i][j] - floating[i][j]));
				}
			}
		}
		
		return sqrt(sqDiff/totalNumVals);
	}
	
	float RSGISSquaredDifferenceSimilarityMetric::calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims) throw(rsgis::math::RSGISMathException)
	{
		unsigned int totalNumVals = numDims * numVals;
		
		double sqDiff = 0;
		
		for(unsigned int i = 0; i < numDims; ++i)
		{
			for(unsigned int j = 0; j < numVals; ++j)
			{
				if((!((boost::math::isnan)(reference[i][j]))) & (!((boost::math::isnan)(floating[i][j]))))
				{
					sqDiff += ((reference[i][j] - floating[i][j]) * (reference[i][j] - floating[i][j]));
				}
			}
		}
		
		return sqrt(sqDiff/totalNumVals);
	}
	
	float RSGISManhattanSimilarityMetric::calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims) throw(rsgis::math::RSGISMathException)
	{
		unsigned int totalNumVals = numDims * numVals;
		
		double absDiff = 0;
		
		for(unsigned int i = 0; i < numDims; ++i)
		{
			for(unsigned int j = 0; j < numVals; ++j)
			{
				if((!((boost::math::isnan)(reference[i][j]))) & (!((boost::math::isnan)(floating[i][j]))))
				{
					absDiff += fabs(reference[i][j] - floating[i][j]);
				}
			}
		}
		
		return absDiff/totalNumVals;
	}
	
	float RSGISCorrelationSimilarityMetric::calcValue(float **reference, float **floating, unsigned int numVals, unsigned int numDims) throw(rsgis::math::RSGISMathException)
	{
		unsigned int n = numDims * numVals;
		
		double sumRF = 0;
		double sumR = 0;
		double sumF = 0;
		double sumRSq = 0;
		double sumFSq = 0;
				
		for(unsigned int i = 0; i < numDims; ++i)
		{
			for(unsigned int j = 0; j < numVals; ++j)
			{
				if((!((boost::math::isnan)(reference[i][j]))) & (!((boost::math::isnan)(floating[i][j]))))
				{
					sumRF += (reference[i][j] * floating[i][j]);
					sumR += reference[i][j];
					sumF += floating[i][j];
					sumRSq += (reference[i][j] * reference[i][j]);
					sumFSq += (floating[i][j] * floating[i][j]);
				}
			}
		}
		
		float val = (((n * sumRF) - (sumR * sumF))/sqrt(((n*sumRSq)-(sumR*sumR))*((n*sumFSq)-(sumF*sumF))));
        
        if(val < 0)
        {
            val *= -1;
        }
		
		return val;
	}

	
}}

