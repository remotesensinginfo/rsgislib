/*
 *  RSGISCalcEditImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/08/2011.
 *  Copyright 2011 RSGISLib.
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

#include "img/RSGISCalcEditImage.h"

namespace rsgis 
{
	namespace img
	{
        RSGISCalcEditImage::RSGISCalcEditImage(RSGISCalcImageValue *valueCalc)
        {
            calc = valueCalc;
        }
            
        void RSGISCalcEditImage::calcImage(GDALDataset *dataset)
        {
            RSGISImageUtils imgUtils;
            double *gdalTranslation = new double[6];

            int height = 0;
            int width = 0;
            int numInBands = 0;
            
            float **inputData = NULL;
            float *inDataColumn = NULL;
            
            GDALRasterBand **inputRasterBands = NULL;
            
            try
            {
                dataset->GetGeoTransform(gdalTranslation);
                numInBands = dataset->GetRasterCount();
                width = dataset->GetRasterXSize();
                height = dataset->GetRasterYSize();
                
                // Get Image Input Bands
                inputRasterBands = new GDALRasterBand*[numInBands];
                for(int i = 0; i < numInBands; i++)
				{
					inputRasterBands[i] = dataset->GetRasterBand(i+1);
                }
                
                // Allocate memory
                inputData = new float*[numInBands];
                for(int i = 0; i < numInBands; i++)
                {
                    inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
                }
                inDataColumn = new float[numInBands];
                
                geos::geom::Envelope extent;
                double pxlTLX = 0;
                double pxlTLY = 0;
                double pxlWidth = 0;
                double pxlHeight = 0;
                
                pxlTLX = gdalTranslation[0];
                pxlTLY = gdalTranslation[3];
                pxlWidth = gdalTranslation[1];
                pxlHeight = gdalTranslation[5];
                
                if(pxlHeight < 0)
                {
                    pxlHeight *= (-1);
                }
                
                int feedback = height/10;
                int feedbackCounter = 0;
                std::cout << "Started " << std::flush;
                // Loop images to process data
                for(int i = 0; i < height; i++)
                {
                    if((i % feedback) == 0)
                    {
                        std::cout << ".." << feedbackCounter << ".." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int n = 0; n < numInBands; n++)
                    {
                        inputRasterBands[n]->RasterIO(GF_Read, 0, i, width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][j];
                        }
                        
                        extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
                        this->calc->calcImageValue(inDataColumn, numInBands, extent);
                        
                        for(int n = 0; n < numInBands; n++)
                        {
                            inputData[n][j] = inDataColumn[n];
                        }
                        
                        pxlTLX += pxlWidth;
                    }
                    
                    for(int n = 0; n < numInBands; n++)
                    {
                        inputRasterBands[n]->RasterIO(GF_Write, 0, i, width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
                    }
                    
                    pxlTLY -= pxlHeight;
                    pxlTLX = gdalTranslation[0];
                }
                std::cout << " Complete.\n";
            }
            catch(RSGISImageCalcException& e)
            {
                if(gdalTranslation != NULL)
                {
                    delete[] gdalTranslation;
                }
                
                if(inputData != NULL)
                {
                    for(int i = 0; i < numInBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }		
                if(inDataColumn != NULL)
                {
                    delete[] inDataColumn;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }
                throw e;
            }
            catch(RSGISImageBandException& e)
            {
                if(gdalTranslation != NULL)
                {
                    delete[] gdalTranslation;
                }			
                
                if(inputData != NULL)
                {
                    for(int i = 0; i < numInBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }		
                if(inDataColumn != NULL)
                {
                    delete[] inDataColumn;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }
                throw e;
            }
            
            if(gdalTranslation != NULL)
            {
                delete[] gdalTranslation;
            }
            
            if(inputData != NULL)
            {
                for(int i = 0; i < numInBands; i++)
                {
                    if(inputData[i] != NULL)
                    {
                        delete[] inputData[i];
                    }
                }
                delete[] inputData;
            }		
            if(inDataColumn != NULL)
            {
                delete[] inDataColumn;
            }
            if(inputRasterBands != NULL)
            {
                delete[] inputRasterBands;
            }
        }
        
        void RSGISCalcEditImage::calcImageUseOut(GDALDataset *dataset)
        {
            RSGISImageUtils imgUtils;
            
            int height = 0;
            int width = 0;
            int numBands = 0;
            int xBlockSize = 0;
            int yBlockSize = 0;
            
            float **inputData = NULL;
            double **outputData = NULL;
            float *inDataColumn = NULL;
            double *outDataColumn = NULL;
            
            GDALRasterBand **rasterBands = NULL;
            
            try
            {
                width = dataset->GetRasterXSize();
                height = dataset->GetRasterYSize();
                numBands = dataset->GetRasterCount();
                                
                //Get Image Bands
                rasterBands = new GDALRasterBand*[numBands];
                for(int i = 0; i < numBands; i++)
                {
                    rasterBands[i] = dataset->GetRasterBand(i+1);
                }

                rasterBands[0]->GetBlockSize (&xBlockSize, &yBlockSize);
                                
                // Allocate memory
                inputData = new float*[numBands];
                for(int i = 0; i < numBands; i++)
                {
                    inputData[i] = (float *) CPLMalloc(sizeof(float)*(width*yBlockSize));
                }
                inDataColumn = new float[numBands];
                
                outputData = new double*[numBands];
                for(int i = 0; i < numBands; i++)
                {
                    outputData[i] = (double *) CPLMalloc(sizeof(double)*(width*yBlockSize));
                }
                outDataColumn = new double[numBands];
                
                int nYBlocks = floor(((double)height) / ((double)yBlockSize));
                int remainRows = height - (nYBlocks * yBlockSize);
                int rowOffset = 0;
                
                int feedback = height/10.0;
                int feedbackCounter = 0;
                std::cout << "Started " << std::flush;
                // Loop images to process data
                for(int i = 0; i < nYBlocks; i++)
                {
                    for(int n = 0; n < numBands; n++)
                    {
                        rowOffset = (yBlockSize * i);
                        rasterBands[n]->RasterIO(GF_Read, 0, rowOffset, width, yBlockSize, inputData[n], width, yBlockSize, GDT_Float32, 0, 0);
                    }
                    
                    for(int m = 0; m < yBlockSize; ++m)
                    {
                        if((feedback != 0) && (((i*yBlockSize)+m) % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        
                        for(int j = 0; j < width; j++)
                        {
                            for(int n = 0; n < numBands; n++)
                            {
                                inDataColumn[n] = inputData[n][(m*width)+j];
                            }
                            
                            this->calc->calcImageValue(inDataColumn, numBands, outDataColumn);
                            
                            for(int n = 0; n < numBands; n++)
                            {
                                outputData[n][(m*width)+j] = outDataColumn[n];
                            }
                            
                        }
                    }
                    
                    for(int n = 0; n < numBands; n++)
                    {
                        rowOffset = yBlockSize * i;
                        rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, yBlockSize, outputData[n], width, yBlockSize, GDT_Float32, 0, 0);
                    }
                }
                
                if(remainRows > 0)
                {
                    for(int n = 0; n < numBands; n++)
                    {
                        rowOffset = (yBlockSize * nYBlocks);
                        rasterBands[n]->RasterIO(GF_Read, 0, rowOffset, width, remainRows, inputData[n], width, remainRows, GDT_Float32, 0, 0);
                    }
                    
                    for(int m = 0; m < remainRows; ++m)
                    {
                        if((feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        for(int j = 0; j < width; j++)
                        {
                            for(int n = 0; n < numBands; n++)
                            {
                                inDataColumn[n] = inputData[n][(m*width)+j];
                            }
                            
                            this->calc->calcImageValue(inDataColumn, numBands, outDataColumn);
                            
                            for(int n = 0; n < numBands; n++)
                            {
                                outputData[n][(m*width)+j] = outDataColumn[n];
                            }
                            
                        }
                    }
                    
                    for(int n = 0; n < numBands; n++)
                    {
                        rowOffset = (yBlockSize * nYBlocks);
                        rasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, remainRows, outputData[n], width, remainRows, GDT_Float32, 0, 0);
                    }
                }
                std::cout << " Complete.\n";
            }
            catch(RSGISImageCalcException& e)
            {                
                if(inputData != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }
                
                if(outputData != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        if(outputData[i] != NULL)
                        {
                            delete[] outputData[i];
                        }
                    }
                    delete[] outputData;
                }
                
                if(inDataColumn != NULL)
                {
                    delete[] inDataColumn;
                }
                
                if(outDataColumn != NULL)
                {
                    delete[] outDataColumn;
                }
                
                if(rasterBands != NULL)
                {
                    delete[] rasterBands;
                }
                throw e;
            }
            catch(RSGISImageBandException& e)
            {
                if(inputData != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }
                
                if(outputData != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        if(outputData[i] != NULL)
                        {
                            delete[] outputData[i];
                        }
                    }
                    delete[] outputData;
                }
                
                if(inDataColumn != NULL)
                {
                    delete[] inDataColumn;
                }
                
                if(outDataColumn != NULL)
                {
                    delete[] outDataColumn;
                }
                
                if(rasterBands != NULL)
                {
                    delete[] rasterBands;
                }
                
                throw e;
            }
            
            if(inputData != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    if(inputData[i] != NULL)
                    {
                        CPLFree(inputData[i]);
                    }
                }
                delete[] inputData;
            }
            
            if(outputData != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    if(outputData[i] != NULL)
                    {
                        CPLFree(outputData[i]);
                    }
                }
                delete[] outputData;
            }
            
            if(inDataColumn != NULL)
            {
                delete[] inDataColumn;
            }
            
            if(outDataColumn != NULL)
            {
                delete[] outDataColumn;
            }
            
            if(rasterBands != NULL)
            {
                delete[] rasterBands;
            }
        }
        
        void RSGISCalcEditImage::calcImageWindowData(GDALDataset *dataset, int windowSize, float fillval)
        {
            if(dataset == NULL)
            {
                throw RSGISImageBandException("Dataset is not valid.");
            }
            
            RSGISImageUtils imgUtils;
            int height = 0;
            int width = 0;
            int numBands = 0;
            int xBlockSize = 0;
            int yBlockSize = 0;
            size_t numPxlsInBlock = 0;
            
            float **inputDataUpper = NULL;
            float **inputDataMain = NULL;
            float **inputDataLower = NULL;
            double **outputData = NULL;
            float ***inDataBlock = NULL;
            double *outDataColumn = NULL;
            
            GDALRasterBand **rasterBands = NULL;
            
            try
            {
                if(windowSize % 2 == 0)
                {
                    throw RSGISImageCalcException("Window size needs to be an odd number (min = 3).");
                }
                else if(windowSize < 3)
                {
                    throw RSGISImageCalcException("Window size needs to be 3 or greater and an odd number.");
                }
                int windowMid = floor(((float)windowSize)/2.0); // Starting at 0!! NOT 1 otherwise would be ceil.
                
                numBands = dataset->GetRasterCount();
                width = dataset->GetRasterXSize();
                height = dataset->GetRasterYSize();
                
                //Get Image Output Bands
                rasterBands = new GDALRasterBand*[numBands];
                for(int i = 0; i < numBands; i++)
                {
                    rasterBands[i] = dataset->GetRasterBand(i+1);
                }
                
                rasterBands[0]->GetBlockSize (&xBlockSize, &yBlockSize);
                
                int numOfLines = yBlockSize;
                if(yBlockSize < windowSize)
                {
                    numOfLines = ceil(((float)windowSize)/((float)yBlockSize))*yBlockSize;
                }
                
                // Allocate memory
                numPxlsInBlock = width*numOfLines;
                inputDataUpper = new float*[numBands];
                for(int i = 0; i < numBands; i++)
                {
                    inputDataUpper[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                    for(int k = 0; k < numPxlsInBlock; k++)
                    {
                        inputDataUpper[i][k] = 0;
                    }
                }
                
                inputDataMain = new float*[numBands];
                for(int i = 0; i < numBands; i++)
                {
                    inputDataMain[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                    for(int k = 0; k < numPxlsInBlock; k++)
                    {
                        inputDataMain[i][k] = 0;
                    }
                }
                
                inputDataLower = new float*[numBands];
                for(int i = 0; i < numBands; i++)
                {
                    inputDataLower[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                    for(int k = 0; k < numPxlsInBlock; k++)
                    {
                        inputDataLower[i][k] = 0;
                    }
                }
                
                inDataBlock = new float**[numBands];
                for(int i = 0; i < numBands; i++)
                {
                    inDataBlock[i] = new float*[windowSize];
                    for(int j = 0; j < windowSize; j++)
                    {
                        inDataBlock[i][j] = new float[windowSize];
                    }
                }
                
                outputData = new double*[numBands];
                for(int i = 0; i < numBands; i++)
                {
                    outputData[i] = (double *) CPLMalloc(sizeof(double)*numPxlsInBlock);
                }
                outDataColumn = new double[numBands];
                
                
                int nYBlocks = floor(((double)height) / ((double)numOfLines));
                int remainRows = height - (nYBlocks * numOfLines);
                int rowOffset = 0;
                unsigned int line = 0;
                long cLinePxl = 0;
                long cPxl = 0;
                long dLinePxls = 0;
                int dWinX = 0;
                int dWinY = 0;
                
                int feedback = height/10.0;
                int feedbackCounter = 0;
                std::cout << "Started " << std::flush;
                
                if(nYBlocks > 0)
                {
                    for(int i = 0; i < nYBlocks; i++)
                    {
                        if(i == 0)
                        {
                            // Set Upper Block with Zeros.
                            for(int n = 0; n < numBands; n++)
                            {
                                for(int k = 0; k < numPxlsInBlock; k++)
                                {
                                    inputDataUpper[n][k] = fillval;
                                }
                            }
                            
                            // Read Main Block
                            for(int n = 0; n < numBands; n++)
                            {
                                rowOffset = (numOfLines * i);
                                rasterBands[n]->RasterIO(GF_Read, 0, rowOffset, width, numOfLines, inputDataMain[n], width, numOfLines, GDT_Float32, 0, 0);
                            }
                            // Read Lower Block
                            for(int n = 0; n < numBands; n++)
                            {
                                if(nYBlocks == 1)
                                {
                                    if(remainRows > 0)
                                    {
                                        rowOffset = (numOfLines * (i+1));
                                        rasterBands[n]->RasterIO(GF_Read, 0, rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
                                        for(int k = (remainRows*width); k < numPxlsInBlock; k++)
                                        {
                                            inputDataLower[n][k] = 0;
                                        }
                                    }
                                    else
                                    {
                                        for(int k = 0; k < numPxlsInBlock; k++)
                                        {
                                            inputDataLower[n][k] = 0;
                                        }
                                    }
                                }
                                else
                                {
                                    rowOffset = (numOfLines * (i+1));
                                    rasterBands[n]->RasterIO(GF_Read, 0, rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                                }
                            }
                            
                        }
                        else if(i == (nYBlocks-1))
                        {
                            // Shift Lower Block to Main Block
                            for(int n = 0; n < numBands; n++)
                            {
                                for(int k = 0; k < numPxlsInBlock; k++)
                                {
                                    inputDataUpper[n][k] = inputDataMain[n][k];
                                }
                            }
                            
                            // Shift Lower Block to Main Block
                            for(int n = 0; n < numBands; n++)
                            {
                                for(int k = 0; k < numPxlsInBlock; k++)
                                {
                                    inputDataMain[n][k] = inputDataLower[n][k];
                                }
                            }
                            
                            // Set Lower Block with Zeros.
                            for(int n = 0; n < numBands; n++)
                            {
                                if(remainRows > 0)
                                {
                                    rowOffset = (numOfLines * (i+1));
                                    rasterBands[n]->RasterIO(GF_Read, 0, rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
                                    for(int k = (remainRows*width); k < numPxlsInBlock; k++)
                                    {
                                        inputDataLower[n][k] = fillval;
                                    }
                                }
                                else
                                {
                                    for(int k = 0; k < numPxlsInBlock; k++)
                                    {
                                        inputDataLower[n][k] = fillval;
                                    }
                                }
                            }
                        }
                        else
                        {
                            // Shift Lower Block to Main Block
                            for(int n = 0; n < numBands; n++)
                            {
                                for(int k = 0; k < numPxlsInBlock; k++)
                                {
                                    inputDataUpper[n][k] = inputDataMain[n][k];
                                }
                            }
                            
                            // Shift Lower Block to Main Block
                            for(int n = 0; n < numBands; n++)
                            {
                                for(int k = 0; k < numPxlsInBlock; k++)
                                {
                                    inputDataMain[n][k] = inputDataLower[n][k];
                                }
                            }
                            
                            // Read Lower Block
                            for(int n = 0; n < numBands; n++)
                            {
                                rowOffset = (numOfLines * (i+1));
                                rasterBands[n]->RasterIO(GF_Read, 0, rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                            }
                        }
                        
                        for(int m = 0; m < numOfLines; ++m)
                        {
                            line = (i*numOfLines)+m;
                            if((feedback != 0) && (line % feedback) == 0)
                            {
                                std::cout << "." << feedbackCounter << "." << std::flush;
                                feedbackCounter = feedbackCounter + 10;
                            }
                            
                            cLinePxl = m*width;
                            
                            for(int j = 0; j < width; j++)
                            {
                                cPxl = cLinePxl+j;
                                if(m < windowMid)
                                {
                                    for(int y = 0; y < windowSize; y++)
                                    {
                                        dWinY = y-windowMid;
                                        dLinePxls = dWinY * width;
                                        
                                        if((cPxl + dLinePxls) < 0)
                                        {
                                            for(int x = 0; x < windowSize; x++)
                                            {
                                                dWinX = x-windowMid;
                                                
                                                if((j+dWinX) < 0)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else if((j+dWinX) >= width)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = inputDataUpper[n][(numPxlsInBlock+(cPxl+dLinePxls))+dWinX];
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            for(int x = 0; x < windowSize; x++)
                                            {
                                                dWinX = x-windowMid;
                                                
                                                if((j+dWinX) < 0)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else if((j+dWinX) >= width)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if(m > ((numOfLines-1)-windowMid))
                                {
                                    for(int y = 0; y < windowSize; y++)
                                    {
                                        dWinY = y-windowMid;
                                        dLinePxls = dWinY * width;
                                        
                                        if((cPxl + dLinePxls) >= numPxlsInBlock)
                                        {
                                            for(int x = 0; x < windowSize; x++)
                                            {
                                                dWinX = x-windowMid;
                                                
                                                if((j+dWinX) < 0)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else if((j+dWinX) >= width)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = inputDataLower[n][((cPxl+dLinePxls)-numPxlsInBlock)+dWinX];
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            for(int x = 0; x < windowSize; x++)
                                            {
                                                dWinX = x-windowMid;
                                                
                                                if((j+dWinX) < 0)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else if((j+dWinX) >= width)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    for(int y = 0; y < windowSize; y++)
                                    {
                                        dWinY = y-windowMid;
                                        dLinePxls = dWinY * width;
                                        
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = fillval;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = fillval;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                this->calc->calcImageValue(inDataBlock, numBands, windowSize, outDataColumn);
                                
                                for(int n = 0; n < numBands; n++)
                                {
                                    outputData[n][cPxl] = outDataColumn[n];
                                }
                            }
                            
                        }
                        
                        for(int n = 0; n < numBands; n++)
                        {
                            rasterBands[n]->RasterIO(GF_Write, 0, (numOfLines * i), width, numOfLines, outputData[n], width, numOfLines, GDT_Float64, 0, 0);
                        }
                    }
                    
                    if(remainRows > 0)
                    {
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = inputDataMain[n][k];
                            }
                        }
                        
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataMain[n][k] = inputDataLower[n][k];
                            }
                        }
                        
                        // Read Lower Block
                        for(int n = 0; n < numBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataLower[n][k] = fillval;
                            }
                        }
                        
                        for(int m = 0; m < remainRows; ++m)
                        {
                            line = (nYBlocks*numOfLines)+m;

                            if((feedback != 0) && (line % feedback) == 0)
                            {
                                std::cout << "." << feedbackCounter << "." << std::flush;
                                feedbackCounter = feedbackCounter + 10;
                            }
                            
                            cLinePxl = m*width;
                            
                            for(int j = 0; j < width; j++)
                            {
                                cPxl = cLinePxl+j;
                                if(m < windowMid)
                                {
                                    for(int y = 0; y < windowSize; y++)
                                    {
                                        dWinY = y-windowMid;
                                        dLinePxls = dWinY * width;
                                        
                                        if((cPxl + dLinePxls) < 0)
                                        {
                                            for(int x = 0; x < windowSize; x++)
                                            {
                                                dWinX = x-windowMid;
                                                
                                                if((j+dWinX) < 0)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else if((j+dWinX) >= width)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = inputDataUpper[n][(numPxlsInBlock+(cPxl+dLinePxls))+dWinX];
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            for(int x = 0; x < windowSize; x++)
                                            {
                                                dWinX = x-windowMid;
                                                
                                                if((j+dWinX) < 0)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else if((j+dWinX) >= width)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if(m > ((numOfLines-1)-windowMid))
                                {
                                    for(int y = 0; y < windowSize; y++)
                                    {
                                        dWinY = y-windowMid;
                                        dLinePxls = dWinY * width;
                                        
                                        if((cPxl + dLinePxls) >= numPxlsInBlock)
                                        {
                                            for(int x = 0; x < windowSize; x++)
                                            {
                                                dWinX = x-windowMid;
                                                
                                                if((j+dWinX) < 0)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else if((j+dWinX) >= width)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = inputDataLower[n][((cPxl+dLinePxls)-numPxlsInBlock)+dWinX];
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            for(int x = 0; x < windowSize; x++)
                                            {
                                                dWinX = x-windowMid;
                                                
                                                if((j+dWinX) < 0)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else if((j+dWinX) >= width)
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = fillval;
                                                    }
                                                }
                                                else
                                                {
                                                    for(int n = 0; n < numBands; n++)
                                                    {
                                                        inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    for(int y = 0; y < windowSize; y++)
                                    {
                                        dWinY = y-windowMid;
                                        dLinePxls = dWinY * width;
                                        
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = fillval;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = fillval;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                this->calc->calcImageValue(inDataBlock, numBands, windowSize, outDataColumn);
                                
                                for(int n = 0; n < numBands; n++)
                                {
                                    outputData[n][cPxl] = outDataColumn[n];
                                }
                            }
                        }
                        
                        for(int n = 0; n < numBands; n++)
                        {
                            rasterBands[n]->RasterIO(GF_Write, 0, (nYBlocks*numOfLines), width, remainRows, outputData[n], width, remainRows, GDT_Float32, 0, 0);
                        }
                    }
                    
                }
                else
                {
                    
                }
                
                
                std::cout << " Complete.\n";
            }
            catch(RSGISImageCalcException& e)
            {
                if(inputDataUpper != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        delete[] inputDataUpper[i];
                    }
                    delete[] inputDataUpper;
                }
                
                if(inputDataMain != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        delete[] inputDataMain[i];
                    }
                    delete[] inputDataMain;
                }
                
                if(inputDataLower != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        delete[] inputDataLower[i];
                    }
                    delete[] inputDataLower;
                }
                
                if(inDataBlock != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        for(int j = 0; j < windowSize; j++)
                        {
                            delete[] inDataBlock[i][j];
                        }
                        delete[] inDataBlock[i];
                    }
                    delete[] inDataBlock;
                }
                
                if(outputData != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        delete outputData[i];
                    }
                    delete outputData;
                }
                
                if(outDataColumn != NULL)
                {
                    delete[] outDataColumn;
                }
                
                throw e;
            }
            catch(RSGISImageBandException& e)
            {                
                if(inputDataUpper != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        delete[] inputDataUpper[i];
                    }
                    delete[] inputDataUpper;
                }
                
                if(inputDataMain != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        delete[] inputDataMain[i];
                    }
                    delete[] inputDataMain;
                }
                
                if(inputDataLower != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        delete[] inputDataLower[i];
                    }
                    delete[] inputDataLower;
                }
                
                if(inDataBlock != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        for(int j = 0; j < windowSize; j++)
                        {
                            delete[] inDataBlock[i][j];
                        }
                        delete[] inDataBlock[i];
                    }
                    delete[] inDataBlock;
                }
                
                if(outputData != NULL)
                {
                    for(int i = 0; i < numBands; i++)
                    {
                        delete outputData[i];
                    }
                    delete outputData;
                }
                
                if(outDataColumn != NULL)
                {
                    delete[] outDataColumn;
                }
                
                throw e;
            }
            
            if(inputDataUpper != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    delete[] inputDataUpper[i];
                }
                delete[] inputDataUpper;
            }
            
            if(inputDataMain != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    delete[] inputDataMain[i];
                }
                delete[] inputDataMain;
            }
            
            if(inputDataLower != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    delete[] inputDataLower[i];
                }
                delete[] inputDataLower;
            }
            
            if(inDataBlock != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    for(int j = 0; j < windowSize; j++)
                    {
                        delete[] inDataBlock[i][j];
                    }
                    delete[] inDataBlock[i];
                }
                delete[] inDataBlock;
            }
            
            if(outputData != NULL)
            {
                for(int i = 0; i < numBands; i++)
                {
                    delete outputData[i];
                }
                delete outputData;
            }
            
            if(outDataColumn != NULL)
            {
                delete[] outDataColumn;
            }
        }
        
        RSGISCalcEditImage::~RSGISCalcEditImage()
        {
            
        }
	}
}


