/*
 *  RSGISMorphologyErode.cpp
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 13/01/2012
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

#include "RSGISMorphologyErode.h"

namespace rsgis{namespace filter{

	RSGISImageMorphologyErode::RSGISImageMorphologyErode()
	{

	}

	void RSGISImageMorphologyErode::erodeImage(GDALDataset **datasets, string outputImage, Matrix *matrixOperator) throw(RSGISImageCalcException, RSGISImageBandException)
	{
        if(matrixOperator->n != matrixOperator->m)
        {
            throw RSGISImageCalcException("Morphological operator must be a square matrix.");
        }
        
		int numBands = datasets[0]->GetRasterCount();
		RSGISMorphologyErode *erodeImage = new RSGISMorphologyErode(numBands, matrixOperator);
		RSGISCalcImage calcImg = RSGISCalcImage(erodeImage, "", true);
        try 
        {
            calcImg.calcImageWindowData(datasets, 1, outputImage, matrixOperator->n);
        }
        catch(RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(RSGISImageBandException &e)
        {
            throw e;
        }
        delete erodeImage;
	}
    
    void RSGISImageMorphologyErode::erodeImageAll(GDALDataset **datasets, string outputImage, Matrix *matrixOperator) throw(RSGISImageCalcException, RSGISImageBandException)
	{
        if(matrixOperator->n != matrixOperator->m)
        {
            throw RSGISImageCalcException("Morphological operator must be a square matrix.");
        }
        
		RSGISMorphologyErodeAll *erodeImage = new RSGISMorphologyErodeAll(1, matrixOperator);
		RSGISCalcImage calcImg = RSGISCalcImage(erodeImage, "", true);
        try 
        {
            calcImg.calcImageWindowData(datasets, 1, outputImage, matrixOperator->n);
        }
        catch(RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(RSGISImageBandException &e)
        {
            throw e;
        }
        delete erodeImage;
	}

	RSGISMorphologyErode::RSGISMorphologyErode(int numberOutBands, Matrix *matrixOperator) : RSGISCalcImageValue(numberOutBands)
	{
        this->matrixOperator = matrixOperator;
	}

	void RSGISMorphologyErode::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		bool first = true;
		double *smallest = new double[numBands];
		
        unsigned int idx = 0;
        
		for(int i = 0; i < winSize; i++)
        {
			for(int j = 0; j < winSize; j++)
            {
                idx = (i*winSize)+j;
                
                if(matrixOperator->matrix[idx] > 0)
                {
                    if(first)
                    {
                        for(int b = 0; b < numBands; b++)
                        {
                            smallest[b] = dataBlock[b][i][j];
                        }
                        first = false;
                    }
                    else 
                    {
                        for(int b = 0; b < numBands; b++)
                        {
                            if(dataBlock[b][i][j] < smallest[b])
                            {
                                smallest[b] = dataBlock[b][i][j];
                            }
                        }
                    }
                }
			}
		}

		for(int b = 0; b < numBands; b++)
        {
			output[b] = smallest[b];
		}
		
		delete[] smallest;
	}

	RSGISMorphologyErode::~RSGISMorphologyErode()
	{
		
	}
    
    RSGISMorphologyErodeAll::RSGISMorphologyErodeAll(int numberOutBands, Matrix *matrixOperator) : RSGISCalcImageValue(numberOutBands)
	{
        this->matrixOperator = matrixOperator;
	}
    
	void RSGISMorphologyErodeAll::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		bool first = true;
		double smallest = 0;
		
        unsigned int idx = 0;
        
		for(int i = 0; i < winSize; i++)
        {
			for(int j = 0; j < winSize; j++)
            {
                idx = (i*winSize)+j;
                
                if(matrixOperator->matrix[idx] > 0)
                {
                    for(int b = 0; b < numBands; b++)
                    {
                        if(first)
                        {
                            smallest = dataBlock[b][i][j];
                        }
                        else if(dataBlock[b][i][j] < smallest)
                        {
                            smallest = dataBlock[b][i][j];
                        }
                        first = false;
                    }
                }
			}
		}
        
		output[0] = smallest;
    }
    
	RSGISMorphologyErodeAll::~RSGISMorphologyErodeAll()
	{
		
	}

}}

