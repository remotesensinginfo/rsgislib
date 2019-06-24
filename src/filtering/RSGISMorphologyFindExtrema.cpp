/*
 *  RSGISMorphologyFindExtrema.cpp
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

#include "RSGISMorphologyFindExtrema.h"

namespace rsgis{namespace filter{
    
	RSGISImageMorphologyFindExtrema::RSGISImageMorphologyFindExtrema()
	{
        
	}
    
    void RSGISImageMorphologyFindExtrema::findMinima(GDALDataset **datasets, std::string outputImage, rsgis::math::Matrix *matrixOperator, RSGISMinimaOutputs outputType, bool allowEquals, std::string format, GDALDataType outDataType)
	{
        if(matrixOperator->n != matrixOperator->m)
        {
            throw rsgis::img::RSGISImageCalcException("Morphological operator must be a square matrix.");
        }
        
		int numBands = datasets[0]->GetRasterCount();
		RSGISMorphologyFindLocalMinima *findMinima = new RSGISMorphologyFindLocalMinima(numBands, matrixOperator, outputType, allowEquals);
		rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(findMinima, "", true);
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
        delete findMinima;
	}
    
    void RSGISImageMorphologyFindExtrema::findMinimaAll(GDALDataset **datasets, std::string outputImage, rsgis::math::Matrix *matrixOperator, RSGISMinimaOutputs outputType, bool allowEquals, std::string format, GDALDataType outDataType)
	{
        if(matrixOperator->n != matrixOperator->m)
        {
            throw rsgis::img::RSGISImageCalcException("Morphological operator must be a square matrix.");
        }
        
		RSGISMorphologyFindLocalMinimaAll *findMinima = new RSGISMorphologyFindLocalMinimaAll(1, matrixOperator, outputType, allowEquals);
		rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(findMinima, "", true);
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
        delete findMinima;
	}
    
    
	RSGISMorphologyFindLocalMinima::RSGISMorphologyFindLocalMinima(int numberOutBands, rsgis::math::Matrix *matrixOperator, RSGISImageMorphologyFindExtrema::RSGISMinimaOutputs outputType, bool allowEquals) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
        this->matrixOperator = matrixOperator;
        this->outputType = outputType;
        this->allowEquals = allowEquals;
        this->outVal = new unsigned long[numberOutBands];
        for(int b = 0; b < numberOutBands; b++)
        {
            this->outVal[b] = 1;
        }
	}
    
	void RSGISMorphologyFindLocalMinima::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
        int cPxlIdx = ((winSize-1)/2);
		float *values = new float[numBands];
        bool *isMinima = new bool[numBands];
        for(int b = 0; b < numBands; b++)
        {
            values[b] = dataBlock[b][cPxlIdx][cPxlIdx];
            isMinima[b] = true;
        }
        
        unsigned int idx = 0;
        
        if(allowEquals)
        {
            for(int i = 0; i < winSize; i++)
            {
                for(int j = 0; j < winSize; j++)
                {
                    if((i != cPxlIdx) & (j != cPxlIdx))
                    {
                        idx = (i*winSize)+j;
                        if(matrixOperator->matrix[idx] > 0)
                        {
                            for(int b = 0; b < numBands; b++)
                            {
                                if(dataBlock[b][i][j] < values[b])
                                {
                                    isMinima[b] = false;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            for(int i = 0; i < winSize; i++)
            {
                for(int j = 0; j < winSize; j++)
                {
                    if((i != cPxlIdx) & (j != cPxlIdx))
                    {
                        idx = (i*winSize)+j;
                        if(matrixOperator->matrix[idx] > 0)
                        {
                            for(int b = 0; b < numBands; b++)
                            {
                                if(dataBlock[b][i][j] <= values[b])
                                {
                                    isMinima[b] = false;
                                }
                            }
                        }
                    }
                }
            }
        }
        
		for(int b = 0; b < numBands; b++)
        {
            if(isMinima[b])
            {
                output[b] = outVal[b];
                if(outputType == RSGISImageMorphologyFindExtrema::sequential)
                {
                    ++outVal[b];
                }
            }
            else
            {
                output[b] = 0;
            }
		}
		
		delete[] values;
        delete[] isMinima;
	}
    
	RSGISMorphologyFindLocalMinima::~RSGISMorphologyFindLocalMinima()
	{
		delete[] outVal;
	}
    
    RSGISMorphologyFindLocalMinimaAll::RSGISMorphologyFindLocalMinimaAll(int numberOutBands, rsgis::math::Matrix *matrixOperator, RSGISImageMorphologyFindExtrema::RSGISMinimaOutputs outputType, bool allowEquals) : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
        this->matrixOperator = matrixOperator;
        this->outputType = outputType;
        this->allowEquals = allowEquals;
        this->outVal = 1;
	}
    
	void RSGISMorphologyFindLocalMinimaAll::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		int cPxlIdx = ((winSize-1)/2);
		float value = 0;
        bool isMinima = true;
        for(int b = 0; b < numBands; b++)
        {
            if(b == 0)
            {
                value = dataBlock[b][cPxlIdx][cPxlIdx];
            }
            else if(dataBlock[b][cPxlIdx][cPxlIdx] < value)
            {
                value = dataBlock[b][cPxlIdx][cPxlIdx];
            }
        }
        
        unsigned int idx = 0;
        
        if(allowEquals)
        {
            for(int i = 0; i < winSize; i++)
            {
                for(int j = 0; j < winSize; j++)
                {
                    if((i != cPxlIdx) & (j != cPxlIdx))
                    {
                        idx = (i*winSize)+j;
                        if(matrixOperator->matrix[idx] > 0)
                        {
                            for(int b = 0; b < numBands; b++)
                            {
                                if(dataBlock[b][i][j] < value)
                                {
                                    isMinima = false;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            for(int i = 0; i < winSize; i++)
            {
                for(int j = 0; j < winSize; j++)
                {
                    if((i != cPxlIdx) & (j != cPxlIdx))
                    {
                        idx = (i*winSize)+j;
                        if(matrixOperator->matrix[idx] > 0)
                        {
                            for(int b = 0; b < numBands; b++)
                            {
                                if(dataBlock[b][i][j] <= value)
                                {
                                    isMinima = false;
                                }
                            }
                        }
                    }
                }
            }
        }
        
		if(isMinima)
        {
            output[0] = outVal;
            if(outputType == RSGISImageMorphologyFindExtrema::sequential)
            {
                ++outVal;
            }
        }
        else
        {
            output[0] = 0;
        }
    }
    
	RSGISMorphologyFindLocalMinimaAll::~RSGISMorphologyFindLocalMinimaAll()
	{
		
	}
    
}}

