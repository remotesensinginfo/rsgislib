/*
 *  RSGISMultivariantStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/08/2008.
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

#include "RSGISMultivariantStats.h"


namespace rsgis{namespace math{
	
	
	RSGISMultivariantStats::RSGISMultivariantStats()
	{
		
	}
	
	Matrix* RSGISMultivariantStats::findMeanVector(Matrix *inputData) throw(RSGISMatricesException)
	{
		RSGISMatrices matrixUtils;
		Matrix *meanVector = NULL;
		Matrix *sumVector = NULL;
		try
		{
			meanVector = matrixUtils.createMatrix(inputData->n, 1);
			sumVector = matrixUtils.createMatrix(inputData->n, 1);
			
			int index = 0;
			for(int i = 0; i < inputData->m; i++)
			{
				for(int j = 0; j < inputData->n; j++)
				{
					sumVector->matrix[j] += inputData->matrix[index++];
				}
			}
						
			for(int i = 0; i < meanVector->n; i++)
			{
				meanVector->matrix[i] = sumVector->matrix[i]/inputData->m;
			}
			
			matrixUtils.freeMatrix(sumVector);
			return meanVector;
		}
		catch(RSGISMatricesException e)
		{
			if(meanVector != NULL)
			{
				delete meanVector;
			}
			if(sumVector != NULL)
			{
				delete sumVector;
			}
			throw e;
		}
	}
	
	Matrix* RSGISMultivariantStats::standardiseMatrix(Matrix *inputData, Matrix *meanVector) throw(RSGISMatricesException,RSGISMultivariantStatsException)
	{
		RSGISMatrices matrixUtils;
		Matrix *standardiseMatrix = NULL;
		try
		{
			standardiseMatrix = matrixUtils.createMatrix(inputData->n, inputData->m);
			int index = 0;
			for(int i = 0; i < inputData->m; i++)
			{
				for(int j = 0; j < inputData->n; j++)
				{
					standardiseMatrix->matrix[index] = inputData->matrix[index] - meanVector->matrix[j];
					index++;
				}
			}
			return standardiseMatrix;
		}
		catch(RSGISMatricesException e)
		{
			if(standardiseMatrix != NULL)
			{
				delete standardiseMatrix;
			}
			throw e;
		}
		catch(RSGISMultivariantStatsException e)
		{
			if(standardiseMatrix != NULL)
			{
				delete standardiseMatrix;
			}
			throw e;
		}
	}
	
	float RSGISMultivariantStats::calcCovariance(Matrix *inputData, int var1, int var2, Matrix *meanVector) throw(RSGISMatricesException,RSGISMultivariantStatsException)
	{ 
		if(inputData->m < var1 | inputData->m < var2)
		{
			throw RSGISMatricesException("Variables are outside the matrix range..");
		}
		
		if(meanVector->n != inputData->n)
		{
			throw RSGISMultivariantStatsException("The mean vector is a different length to the input matrix");
		}
		float sum = 0;
		float value = 0;
		int var1Index = 0;
		int var2Index = 0;
		double part1 = 0;
		double part2 = 0;
		
		for(int i = 0; i < inputData->m; i++)
		{
			var1Index = (i*inputData->n) + var1;
			var2Index = (i*inputData->n) + var2;
			part1 = inputData->matrix[var1Index]-meanVector->matrix[var1];
			part2 = inputData->matrix[var2Index]-meanVector->matrix[var2];
			value = part1*part2;
 			sum += value;
		}
		return (sum/(inputData->m-1));
	}
	
	Matrix* RSGISMultivariantStats::calcCovarianceMatrix(Matrix *inputData, Matrix *meanVector) throw(RSGISMatricesException,RSGISMultivariantStatsException)
	{		
		if(meanVector->n != inputData->n)
		{
			throw RSGISMultivariantStatsException("The mean vector is a different length to the input matrix");
		}
				
		RSGISMatrices matrixUtils;
		Matrix *covariance = NULL;
		
		try
		{
			covariance = matrixUtils.createMatrix(inputData->n, inputData->n);
			int index = 0;
			float covar = 0;
			for(int i = 0; i < inputData->n; i++)
			{
				for(int j = 0; j < inputData->n; j++)
				{
					index = (i * inputData->n)+j;
					covar = this->calcCovariance(inputData, i, j, meanVector);
					covariance->matrix[index] = covar;
				}
			}
			return covariance;
		}
		catch(RSGISMatricesException e)
		{
			if(covariance != NULL)
			{
				delete covariance;
			}
			throw e;
		}
		catch(RSGISMultivariantStatsException e)
		{
			if(covariance != NULL)
			{
				delete covariance;
			}
			throw e;
		}
	}
	
	RSGISMultivariantStats::~RSGISMultivariantStats()
	{
		
	}
}}
