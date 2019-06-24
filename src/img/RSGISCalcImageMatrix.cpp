/*
 *  RSGISCalcImageMatrix.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/07/2008.
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

#include "RSGISCalcImageMatrix.h"

namespace rsgis{namespace img{

	RSGISCalcImageMatrix::RSGISCalcImageMatrix(RSGISCalcImageSingle *calcImage)
	{
		this->calcImage = calcImage;
	}
	
	rsgis::math::Matrix* RSGISCalcImageMatrix::calcImageMatrix(GDALDataset **datasetsA, GDALDataset **datasetsB, int numDS)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		RSGISCalcImageSingleValue *calcImageSingleValue = this->calcImage->getRSGISCalcImageSingleValue();

		int numInBandsDSA = 0;
		int numInBandsDSB = 0;
		
		//double **matrix = NULL;
		double *outputValue = new double[calcImageSingleValue->getNumberOfOutValues()];
		
		rsgis::math::Matrix *outputMatrix = new rsgis::math::Matrix();

		try
		{
			// Number of bands..
			for(int i = 0; i < numDS; i++)
			{
				numInBandsDSA += datasetsA[i]->GetRasterCount();
			}
			
			for(int i = 0; i < numDS; i++)
			{
				numInBandsDSB += datasetsB[i]->GetRasterCount();
			}

			if(numInBandsDSA != numInBandsDSB)
			{
				throw RSGISImageBandException("The two image sets do not have the same number of bands.");
			}
            
			outputMatrix->m = numInBandsDSA;
			outputMatrix->n = numInBandsDSB;
			outputMatrix->matrix = new double[(outputMatrix->m * outputMatrix->n)];
			
			int counter = 0;
			//matrix = new double*[numInBandsDSA];
			for(int i = 0; i < numInBandsDSA; i++)
			{
				//calcImageSingleValue->setBandA(i);
				//matrix[i] = new double[numInBandsDSB];
				for(int j = 0; j < numInBandsDSB; j++)
				{
                    std::cout << "Matrix[" << i << "][" << j << "]: ";
					std::cout.flush();
					//calcImageSingleValue->setBandB(j);
					calcImageSingleValue->reset();
					this->calcImage->calcImage(datasetsA, datasetsB, numDS, outputValue, i, j);
					outputMatrix->matrix[counter++] = outputValue[0];
				}
			}
			
		}
		catch(RSGISImageCalcException &e)
		{
			throw e;
		}
		catch(RSGISImageBandException &e)
		{
			throw e;
		}
        catch(rsgis::RSGISException &e)
		{
			throw RSGISImageCalcException(e.what());
		}
        catch(std::exception &e)
		{
			throw RSGISImageCalcException(e.what());
		}
		
		return outputMatrix;
	}
	
	rsgis::math::Matrix* RSGISCalcImageMatrix::calcImageVector(GDALDataset **datasetsA, int numDS)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		RSGISCalcImageSingleValue *calcImageSingleValue = this->calcImage->getRSGISCalcImageSingleValue();
		
		int numInBandsDSA = 0;
		
		double *outputValue = new double[calcImageSingleValue->getNumberOfOutValues()];
		
		rsgis::math::Matrix *outputMatrix = new rsgis::math::Matrix();
		
		try
		{
			// Number of bands..
			for(int i = 0; i < numDS; i++)
			{
				numInBandsDSA += datasetsA[i]->GetRasterCount();
			}
			
			outputMatrix->m = numInBandsDSA;
			outputMatrix->n = 1;
			outputMatrix->matrix = new double[(outputMatrix->m * outputMatrix->n)];
			
			int counter = 0;
			for(int i = 0; i < numInBandsDSA; i++)
			{
				calcImageSingleValue->reset();
				this->calcImage->calcImage(datasetsA, numDS, outputValue, i);
				outputMatrix->matrix[counter++] = outputValue[0];
			}
			
			counter = 0;
			for(int i = 0; i < outputMatrix->m; i++)
			{
				for(int j = 0; j < outputMatrix->n; j++)
				{
					std::cout << outputMatrix->matrix[counter++] << ", ";
				}
				std::cout << std::endl;
			}
		}
		catch(RSGISImageCalcException &e)
		{
			std::cout << e.what() << std::endl;
			throw e;
		}
		catch(RSGISImageBandException &e)
		{
			std::cout << e.what() << std::endl;
			throw e;
		}
		return outputMatrix;
	}
	
	RSGISCalcImageMatrix::~RSGISCalcImageMatrix()
	{
		
	}
}}
