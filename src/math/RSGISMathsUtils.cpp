/*
 *  RSGISMathsUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 08/07/2008.
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

#include "RSGISMathsUtils.h"

namespace rsgis{namespace math{
	
	
	RSGISMathsUtils::RSGISMathsUtils()
	{
		
	}
	
	double RSGISMathsUtils::strtodouble(std::string inValue)throw(RSGISMathException)
	{
		double outValue = 0;
		try
        {
            outValue = boost::lexical_cast<double>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
			std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISMathException(message);
        }
		return outValue;
	}
	
	float RSGISMathsUtils::strtofloat(std::string inValue)throw(RSGISMathException)
	{
		float outValue = 0;
		try
        {
            outValue = boost::lexical_cast<float>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISMathException(message);
        }
		return outValue;
	}
	
	int RSGISMathsUtils::strtoint(std::string inValue)throw(RSGISMathException)
	{
		int outValue = 0;
		try
        {
            outValue = boost::lexical_cast<int>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISMathException(message);
        }
		return outValue;
	}
	
	unsigned int RSGISMathsUtils::strtounsignedint(std::string inValue)throw(RSGISMathException)
	{
		unsigned int outValue = 0;
		try
        {
            outValue = boost::lexical_cast<unsigned int>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISMathException(message);
        }
		return outValue;
	}
	
	long RSGISMathsUtils::strtolong(std::string inValue)throw(RSGISMathException)
	{
		long outValue = 0;
		try
        {
            outValue = boost::lexical_cast<long>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISMathException(message);
        }
		return outValue;
	}
    
    unsigned long RSGISMathsUtils::strtounsignedlong(std::string inValue)throw(RSGISMathException)
	{
		unsigned long outValue = 0;
		try
        {
            outValue = boost::lexical_cast<unsigned long>(inValue);
        }
        catch(boost::bad_lexical_cast &e)
        {
            std::string message = std::string("Trying to convert \"") + inValue + std::string("\" - ") + std::string(e.what());
            throw RSGISMathException(message);
        }
		return outValue;
	}
	
	std::string RSGISMathsUtils::floattostring(float number)throw(RSGISMathException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISMathException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISMathsUtils::doubletostring(double number)throw(RSGISMathException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISMathException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISMathsUtils::inttostring(int number)throw(RSGISMathException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISMathException(e.what());
        }
		return outValue;
	}
    
    std::string RSGISMathsUtils::uinttostring(unsigned int number)throw(RSGISMathException)
	{
        std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISMathException(e.what());
        }
		return outValue;
	}
	
	std::string RSGISMathsUtils::longtostring(long number)throw(RSGISMathException)
	{
		std::string outValue = "";
		try
        {
            outValue = boost::lexical_cast<std::string>(number);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw RSGISMathException(e.what());
        }
		return outValue;
	}
	
	float RSGISMathsUtils::degreesToRadians(float angle)
	{
		return angle * (M_PI/180);
	}
	
	float RSGISMathsUtils::radiansToDegrees(float angle)
	{
		return angle * (180 / M_PI);
	}
	
	void RSGISMathsUtils::getPowerSet(int numItems, int *numBands, int **indexes, int numSets) throw(RSGISMathException)
	{
		if(numItems == 0)
		{
			throw RSGISMathException("A power set cannot be created from zero items");
		}
		else if(numItems == 1)
		{
			if(numSets != 1)
			{
				throw RSGISMathException("numSets not correct.");
			}
			numBands[0] = 1;
			indexes[0] = new int[1];
			indexes[0][0] = 0;
		}
		else if(numItems == 2)
		{
			if(numSets != 3)
			{
				throw RSGISMathException("numSets not correct.");
			}
			numBands[0] = 1;
			numBands[1] = 1;
			numBands[2] = 2;
			
			indexes[0] = new int[1];
			indexes[1] = new int[1];
			indexes[2] = new int[2];
			indexes[0][0] = 0;
			indexes[1][0] = 1;
			indexes[2][0] = 0;
			indexes[2][1] = 1;
		}
		else if(numItems == 3)
		{
			if(numSets != 7)
			{
				throw RSGISMathException("numSets not correct.");
			}
			numBands[0] = 1;
			numBands[1] = 1;
			numBands[2] = 1;
			numBands[3] = 2;
			numBands[4] = 2;
			numBands[5] = 2;
			numBands[6] = 3;

			indexes[0] = new int[1];
			indexes[1] = new int[1];
			indexes[2] = new int[1];
			indexes[3] = new int[2];
			indexes[4] = new int[2];
			indexes[5] = new int[2];
			indexes[6] = new int[3];
			
			indexes[0][0] = 0;
			indexes[1][0] = 1;
			indexes[2][0] = 2;
			indexes[3][0] = 0;
			indexes[3][1] = 1;
			indexes[4][0] = 0;
			indexes[4][1] = 2;
			indexes[5][0] = 1;
			indexes[5][1] = 2;
			indexes[6][0] = 0;
			indexes[6][1] = 1;
			indexes[6][2] = 2;
		}
		else if(numItems == 4)
		{
			if(numSets != 15)
			{
				throw RSGISMathException("numSets not correct.");
			}

			numBands[0] = 1;
			numBands[1] = 1;
			numBands[2] = 1;
			numBands[3] = 1;
			numBands[4] = 2;
			numBands[5] = 2;
			numBands[6] = 2;
			numBands[7] = 2;
			numBands[8] = 2;
			numBands[9] = 2;
			numBands[10] = 3;
			numBands[11] = 3;
			numBands[12] = 3;
			numBands[13] = 3;
			numBands[14] = 4;

			for(int i = 0; i < 15; i++)
			{
				indexes[i] = new int[numBands[i]];
			}
			
			indexes[0][0] = 0;
			indexes[1][0] = 1;
			indexes[2][0] = 2;
			indexes[3][0] = 3;
			
			indexes[4][0] = 0;
			indexes[4][1] = 1;
			indexes[5][0] = 0;
			indexes[5][1] = 1;
			indexes[6][0] = 0;
			indexes[6][1] = 3;
			indexes[7][0] = 1;
			indexes[7][1] = 2;
			indexes[8][0] = 1;
			indexes[8][1] = 3;
			indexes[9][0] = 2;
			indexes[9][1] = 3;
			
			indexes[10][0] = 0;
			indexes[10][1] = 1;
			indexes[10][2] = 2;
			indexes[11][0] = 0;
			indexes[11][1] = 2;
			indexes[11][2] = 3;
			indexes[12][0] = 1;
			indexes[12][1] = 2;
			indexes[12][2] = 3;
			indexes[13][0] = 0;
			indexes[13][1] = 1;
			indexes[13][2] = 3;
			
			indexes[14][0] = 0;
			indexes[14][1] = 1;
			indexes[14][2] = 2;
			indexes[14][3] = 3;
		}
		else if(numItems == 5)
		{
			if(numSets != 31)
			{
				throw RSGISMathException("numSets not correct.");
			}
			
			numBands[0] = 1;
			numBands[1] = 1;
			numBands[2] = 1;
			numBands[3] = 1;
			numBands[4] = 1;
			
			numBands[5] = 2;
			numBands[6] = 2;
			numBands[7] = 2;
			numBands[8] = 2;
			numBands[9] = 2;
			numBands[10] = 2;
			numBands[11] = 2;
			numBands[12] = 2;
			numBands[13] = 2;
			numBands[14] = 2;
			
			numBands[15] = 3;
			numBands[16] = 3;
			numBands[17] = 3;
			numBands[18] = 3;
			numBands[19] = 3;
			numBands[20] = 3;
			numBands[21] = 3;
			numBands[22] = 3;
			numBands[23] = 3;
			numBands[24] = 3;
			
			numBands[25] = 4;
			numBands[26] = 4;
			numBands[27] = 4;
			numBands[28] = 4;
			numBands[29] = 4;
			
			numBands[30] = 5;

			for(int i = 0; i < 31; i++)
			{
				indexes[i] = new int[numBands[i]];
			}
			
			indexes[0][0] = 0;
			indexes[1][0] = 1;
			indexes[2][0] = 2;
			indexes[3][0] = 3;
			indexes[4][0] = 4;
			
			indexes[5][0] = 0;
			indexes[5][1] = 1;
			indexes[6][0] = 0;
			indexes[6][1] = 2;
			indexes[7][0] = 0;
			indexes[7][1] = 3;
			indexes[8][0] = 0;
			indexes[8][1] = 4;
			indexes[9][0] = 1;
			indexes[9][1] = 2;
			indexes[10][0] = 1;
			indexes[10][1] = 3;
			indexes[11][0] = 1;
			indexes[11][1] = 4;
			indexes[12][0] = 2;
			indexes[12][1] = 3;
			indexes[13][0] = 2;
			indexes[13][1] = 4;
			indexes[14][0] = 3;
			indexes[14][1] = 4;
			
			indexes[15][0] = 0;
			indexes[15][1] = 1;
			indexes[15][2] = 2;
			indexes[16][0] = 0;
			indexes[16][1] = 1;
			indexes[16][2] = 3;
			indexes[17][0] = 0;
			indexes[17][1] = 1;
			indexes[17][2] = 4;
			indexes[18][0] = 0;
			indexes[18][1] = 2;
			indexes[18][2] = 3;
			indexes[19][0] = 0;
			indexes[19][1] = 2;
			indexes[19][2] = 4;
			indexes[20][0] = 0;
			indexes[20][1] = 3;
			indexes[20][2] = 4;
			indexes[21][0] = 1;
			indexes[21][1] = 2;
			indexes[21][2] = 3;
			indexes[22][0] = 1;
			indexes[22][1] = 2;
			indexes[22][2] = 4;
			indexes[23][0] = 2;
			indexes[23][1] = 3;
			indexes[23][2] = 4;
			indexes[24][0] = 1;
			indexes[24][1] = 3;
			indexes[24][2] = 4;
			
			indexes[25][0] = 0;
			indexes[25][1] = 1;
			indexes[25][2] = 2;
			indexes[25][3] = 3;
			indexes[26][0] = 0;
			indexes[26][1] = 1;
			indexes[26][2] = 2;
			indexes[26][3] = 4;
			indexes[27][0] = 0;
			indexes[27][1] = 1;
			indexes[27][2] = 3;
			indexes[27][3] = 4;
			indexes[28][0] = 0;
			indexes[28][1] = 2;
			indexes[28][2] = 3;
			indexes[28][3] = 4;
			indexes[29][0] = 1;
			indexes[29][1] = 2;
			indexes[29][2] = 3;
			indexes[29][3] = 4;
			
			indexes[30][0] = 0;
			indexes[30][1] = 1;
			indexes[30][2] = 2;
			indexes[30][3] = 3;
			indexes[30][3] = 4;
		}
		else
		{
			throw RSGISMathException("Power sets are currently hardcoded and only support 1 - 5 items.");
		}
		
	}
	
	int RSGISMathsUtils::roundUp(double number)
	{
		return static_cast<int>(number+0.5);
	}
	
	Vector* RSGISMathsUtils::calculateCumulativeArea(Vector *inVec, Matrix *widths)
	{
		RSGISVectors vecUtils;
		
		Vector *newVec = vecUtils.createVector(inVec->n);
		
		bool first = true;
		for(int i = 0; i < inVec->n; ++i)
		{
			if(first)
			{
				newVec->vector[i] = widths->matrix[(i*2)+1] * inVec->vector[i];
				first = false;
			}
			else
			{
				newVec->vector[i] = newVec->vector[i-1] + (widths->matrix[(i*2)+1] * inVec->vector[i]);
			}
		}
		return newVec;
	}
	
	Matrix* RSGISMathsUtils::calculateCumulativeArea(Matrix *inData, Matrix *widths)
	{
		RSGISMatrices matrixUtils;
		Matrix *outData = matrixUtils.createMatrix(inData);
		
		double currentArea = 0;
		bool first = true;
		
		for(int i = 0; i < inData->m; ++i)
		{
			currentArea = 0;
			first = true;
			for(int j = 0; j < inData->n; ++j)
			{
				//cout << inData->matrix[(j*inData->m)+i] << ", ";
				if(first)
				{
					currentArea = widths->matrix[(i*2)+1] * inData->matrix[(j*inData->m)+i];
					outData->matrix[(j*inData->m)+i] = currentArea;
					first = false;
				}
				else
				{
					currentArea += widths->matrix[(i*2)+1] * inData->matrix[(j*inData->m)+i];
					outData->matrix[(j*inData->m)+i] = currentArea;
				}
                std::cout << currentArea << ", ";
			}
            std::cout << std::endl;
		}
		
		return outData;
	}
    
    void RSGISMathsUtils::fitPlane(double *x, double *y, double *z, unsigned int numValues, double *a, double *b, double *c) throw(RSGISMathException)
    {
        RSGISMatrices matrices;
		Matrix *matrixA = NULL;
		Matrix *matrixB = NULL;
		Matrix *matrixCoFactors = NULL;
		Matrix *matrixCoFactorsT = NULL;
		Matrix *outputs = NULL;
		try
		{
			double sXY = 0;
			double sX = 0;
			double sXSqu = 0;
			double sY = 0;
			double sYSqu = 0;
			double sXZ = 0;
			double sYZ = 0;
			double sZ = 0;
			
			for(unsigned int i = 0; i < numValues; i++)
			{
				sXY += (x[i] * y[i]);
				sX += x[i];
				sXSqu += (x[i] * x[i]);
				sY += y[i];
				sYSqu += (y[i] * y[i]);
				sXZ += (x[i] * z[i]);
				sYZ += (y[i] * z[i]);
				sZ += z[i];
			}
			
			matrixA = matrices.createMatrix(3, 3);
			matrixA->matrix[0] = sXSqu;
			matrixA->matrix[1] = sXY;
			matrixA->matrix[2] = sX;
			matrixA->matrix[3] = sXY;
			matrixA->matrix[4] = sYSqu;
			matrixA->matrix[5] = sY;
			matrixA->matrix[6] = sX;
			matrixA->matrix[7] = sY;
			matrixA->matrix[8] = numValues;
			matrixB = matrices.createMatrix(1, 3);
			matrixB->matrix[0] = sXZ;
			matrixB->matrix[1] = sYZ;
			matrixB->matrix[2] = sZ;
			
			double determinantA = matrices.determinant(matrixA);
			matrixCoFactors = matrices.cofactors(matrixA);
			matrixCoFactorsT = matrices.transpose(matrixCoFactors);
			double multiplier = 1/determinantA;
			matrices.multipleSingle(matrixCoFactorsT, multiplier);
			outputs = matrices.multiplication(matrixCoFactorsT, matrixB);
			*a = outputs->matrix[0];
			*b = outputs->matrix[1];
			*c = outputs->matrix[2];
		}
		catch(RSGISMatricesException e)
		{
			if(matrixA != NULL)
			{
				 matrices.freeMatrix(matrixA);
			}
			if(matrixB != NULL)
			{
				matrices.freeMatrix(matrixB);
			}
			if(matrixCoFactors != NULL)
			{
				matrices.freeMatrix(matrixCoFactors);
			}
			if(matrixCoFactorsT != NULL)
			{
				matrices.freeMatrix(matrixCoFactorsT);
			}
			if(outputs != NULL)
			{
				matrices.freeMatrix(outputs);
			}
			throw RSGISMathException(e.what());
		}
		
        matrices.freeMatrix(matrixA);
        matrices.freeMatrix(matrixB);
        matrices.freeMatrix(matrixCoFactors);
        matrices.freeMatrix(matrixCoFactorsT);
        matrices.freeMatrix(outputs);
    }
    
    void RSGISMathsUtils::generateStats(std::vector<double> *data, RSGISStatsSummary *stats) throw(RSGISMathException)
    {
        try
        {
            if(data->size() > 0)
            {
                if(stats->calcMin & stats->calcMax)
                {
                    gsl_stats_minmax (&stats->min, &stats->max, &(*data)[0], 1, data->size());
                }
                else if(stats->calcMin)
                {
                    stats->min = gsl_stats_min(&(*data)[0], 1, data->size());
                }
                else if(stats->calcMax)
                {
                    stats->max = gsl_stats_max(&(*data)[0], 1, data->size());
                }
                
                if(stats->calcMean & stats->calcStdDev)
                {
                    stats->mean = gsl_stats_mean (&(*data)[0], 1, data->size());
                    stats->stdDev = gsl_stats_sd_m (&(*data)[0], 1, data->size(), stats->mean);
                }
                else if(stats->calcMean & !stats->calcStdDev)
                {
                    stats->mean = gsl_stats_mean (&(*data)[0], 1, data->size());
                }
                else if(!stats->calcMean & stats->calcStdDev)
                {
                    stats->stdDev = gsl_stats_sd (&(*data)[0], 1, data->size());
                }
                
                if(stats->calcSum)
                {
                    stats->sum = 0;
                    for(std::vector<double>::iterator iterVals = data->begin(); iterVals != data->end(); ++iterVals)
                    {
                        stats->sum += *iterVals;
                    }
                }
                
                if(stats->calcMedian)
                {                        
                    std::sort(data->begin(), data->end());
                    stats->median = gsl_stats_median_from_sorted_data(&(*data)[0], 1, data->size());
                }
            }
            else
            {
                if(stats->calcMin)
                {
                    stats->min = 0;
                }
                if(stats->calcMax)
                {
                    stats->max = 0;
                }
                if(stats->calcMean)
                {
                    stats->mean = 0;
                }
                if(stats->calcStdDev)
                {
                    stats->stdDev = 0;
                }
                if(stats->calcSum)
                {
                    stats->sum = 0;
                }
                if(stats->calcMedian)
                {
                    stats->median = 0;
                }
            }
        }
        catch(RSGISMathException &e)
        {
            throw e;
        }
    }

}}

