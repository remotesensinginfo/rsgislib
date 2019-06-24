/*
 *  RSGISMorphologyGradient.cpp
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 01/03/2012
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

#include "RSGISMorphologyGradient.h"

namespace rsgis{namespace filter{
    
	RSGISImageMorphologyGradient::RSGISImageMorphologyGradient()
	{
        
	}
    
	void RSGISImageMorphologyGradient::calcGradientImage(GDALDataset **datasets, std::string outputImage, rsgis::math::Matrix *matrixOperator, std::string format, GDALDataType outDataType)
	{
        if(matrixOperator->n != matrixOperator->m)
        {
            throw rsgis::img::RSGISImageCalcException("Morphological operator must be a square matrix.");
        }
        
		int numBands = datasets[0]->GetRasterCount();
		RSGISMorphologyGradient *gradImage = new RSGISMorphologyGradient(numBands, matrixOperator);
		rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(gradImage, "", true); // creates the calc that we are going to do
        try
        {
            calcImg.calcImageWindowData(datasets, 1, outputImage, matrixOperator->n, format, outDataType);
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw e;
        }
        delete gradImage;
	}
    
    void RSGISImageMorphologyGradient::calcGradientImageAll(GDALDataset **datasets, std::string outputImage, rsgis::math::Matrix *matrixOperator, std::string format, GDALDataType outDataType)
	{
        if(matrixOperator->n != matrixOperator->m)
        {
            throw rsgis::img::RSGISImageCalcException("Morphological operator must be a square matrix.");
        }
        
		RSGISMorphologyGradientAll *gradImage = new RSGISMorphologyGradientAll(1, matrixOperator);
		rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(gradImage, "", true);
        try
        {
            calcImg.calcImageWindowData(datasets, 1, outputImage, matrixOperator->n, format, outDataType);
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw e;
        }
        delete gradImage;
	}
    
	RSGISMorphologyGradient::RSGISMorphologyGradient(int numberOutBands, rsgis::math::Matrix *matrixOperator) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
        this->matrixOperator = matrixOperator;
	}
	
	void RSGISMorphologyGradient::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		bool first = true;
		double *largest = new double[numBands];
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
                            largest[b] = dataBlock[b][i][j];
                            smallest[b] = dataBlock[b][i][j];
                        }
                        first = false;
                    }
                    else 
                    {
                        for(int b = 0; b < numBands; b++)
                        {
                            if(dataBlock[b][i][j] > largest[b])
                            {
                                largest[b] = dataBlock[b][i][j];
                            }
                            else if(dataBlock[b][i][j] < smallest[b])
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
			output[b] = largest[b]-smallest[b];
		}
		
		delete[] largest;
        delete[] smallest;
	}
    
	RSGISMorphologyGradient::~RSGISMorphologyGradient()
	{
		
	}
    
    RSGISMorphologyGradientAll::RSGISMorphologyGradientAll(int numberOutBands, rsgis::math::Matrix *matrixOperator) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
        this->matrixOperator = matrixOperator;
	}
	
	void RSGISMorphologyGradientAll::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		bool first = true;
		double largest = 0;
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
                            largest = dataBlock[b][i][j];
                            smallest = dataBlock[b][i][j];
                            first = false;
                        }
                        else
                        {
                            if(dataBlock[b][i][j] > largest)
                            {
                                largest = dataBlock[b][i][j];
                            }
                            else if(dataBlock[b][i][j] < smallest)
                            {
                                smallest = dataBlock[b][i][j];
                            }
                        }
                    }
                }
			}
		}
        
		output[0] = largest-smallest;
	}
    
	RSGISMorphologyGradientAll::~RSGISMorphologyGradientAll()
	{
		
	}
    
}}

