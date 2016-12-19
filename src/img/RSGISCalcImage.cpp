/*
 *  RSGISCalcImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/04/2008.
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

#include "RSGISCalcImage.h"

namespace rsgis{namespace img{
	
	RSGISCalcImage::RSGISCalcImage(RSGISCalcImageValue *valueCalc, std::string proj, bool useImageProj)
	{
		this->calc = valueCalc;
		this->numOutBands = valueCalc->getNumOutBands();
		this->proj = proj;
		this->useImageProj = useImageProj;
	}
    
    
    void RSGISCalcImage::calcImage(GDALDataset **datasets, int numDS, std::string outputImage, bool setOutNames, std::string *bandNames, std::string gdalFormat, GDALDataType gdalDataType) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		float **inputData = NULL;
		double **outputData = NULL;
		float *inDataColumn = NULL;
		double *outDataColumn = NULL;
		
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
        
        try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
                        
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
            
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw RSGISImageBandException("Requested GDAL driver does not exists..");
			}
			std::cout << "New image width = " << width << " height = " << height << " bands = " << this->numOutBands << std::endl;
			
			outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, this->numOutBands, gdalDataType, NULL);
			
			if(outputImageDS == NULL)
			{
				throw RSGISImageBandException("Output image could not be created. Check filepath.");
			}
			outputImageDS->SetGeoTransform(gdalTranslation);
			if(useImageProj)
			{
				outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
			}
			else
			{
				outputImageDS->SetProjection(proj.c_str());
			}
            
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
				if (setOutNames) // Set output band names
				{
					outputRasterBands[i]->SetDescription(bandNames[i].c_str());
				}
			}
            int outXBlockSize = 0;
            int outYBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&outXBlockSize, &outYBlockSize);
            
            if(outYBlockSize > yBlockSize)
            {
                yBlockSize = outYBlockSize;
            }
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*(width*yBlockSize));
			}
			inDataColumn = new float[numInBands];
            
			outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*(width*yBlockSize));
			}
			outDataColumn = new double[this->numOutBands];
                      
            int nYBlocks = floor(((double)height) / ((double)yBlockSize));
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10.0;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * i);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, yBlockSize, inputData[n], width, yBlockSize, GDT_Float32, 0, 0);
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
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                        
                    }
                }
				
				for(int n = 0; n < this->numOutBands; n++)
				{
                    rowOffset = yBlockSize * i;
					outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, yBlockSize, outputData[n], width, yBlockSize, GDT_Float64, 0, 0);
				}
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputData[n], width, remainRows, GDT_Float32, 0, 0);
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
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                        
                    }
                }
				
				for(int n = 0; n < this->numOutBands; n++)
				{
                    rowOffset = (yBlockSize * nYBlocks);
					outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, remainRows, outputData[n], width, remainRows, GDT_Float64, 0, 0);
				}
            }
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		
		GDALClose(outputImageDS);
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
		}
		
		if(inputData != NULL)
		{
			for(int i = 0; i < numInBands; i++)
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
			for(int i = 0; i < this->numOutBands; i++)
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
		
		if(inputRasterBands != NULL)
		{
			delete[] inputRasterBands;
		}
		
		if(outputRasterBands != NULL)
		{
			delete[] outputRasterBands;
		}
    }
    
    void RSGISCalcImage::calcImage(GDALDataset **datasets, int numDS, GDALDataset *outputImageDS) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputData = NULL;
		double **outputData = NULL;
		float *inDataColumn = NULL;
		double *outDataColumn = NULL;
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
                        
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
            
			
			if(outputImageDS->GetRasterXSize() != width)
            {
                throw RSGISImageCalcException("The output dataset does not have the correct width\n");
            }
            
            if(outputImageDS->GetRasterYSize() != height)
            {
                throw RSGISImageCalcException("The output dataset does not have the correct height\n");
            }
            
            if(outputImageDS->GetRasterCount() != this->numOutBands)
            {
                throw RSGISImageCalcException("The output dataset does not have the correct number of image bands\n");
            }
            
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
			}
            int outXBlockSize = 0;
            int outYBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&outXBlockSize, &outYBlockSize);
            
            if(outYBlockSize > yBlockSize)
            {
                yBlockSize = outYBlockSize;
            }
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width*yBlockSize);
			}
			inDataColumn = new float[numInBands];
            
			outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*width*yBlockSize);
			}
			outDataColumn = new double[this->numOutBands];
            
			int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * i);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, yBlockSize, inputData[n], width, yBlockSize, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                        
                    }
                }
				
				for(int n = 0; n < this->numOutBands; n++)
				{
                    rowOffset = yBlockSize * i;
					outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, yBlockSize, outputData[n], width, yBlockSize, GDT_Float64, 0, 0);
				}
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputData[n], width, remainRows, GDT_Float32, 0, 0);
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
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                        
                    }
                }
				
				for(int n = 0; n < this->numOutBands; n++)
				{
                    rowOffset = (yBlockSize * nYBlocks);
					outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, remainRows, outputData[n], width, remainRows, GDT_Float64, 0, 0);
				}
            }
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
				
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
		}
		
		if(inputData != NULL)
		{
			for(int i = 0; i < numInBands; i++)
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
			for(int i = 0; i < this->numOutBands; i++)
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
		
		if(inputRasterBands != NULL)
		{
			delete[] inputRasterBands;
		}
		
		if(outputRasterBands != NULL)
		{
			delete[] outputRasterBands;
		}
	}
    
    void RSGISCalcImage::calcImage(GDALDataset **datasets, int numIntDS, int numFloatDS, std::string outputImage, bool setOutNames, std::string *bandNames , std::string gdalFormat, GDALDataType gdalDataType) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
		RSGISImageUtils imgUtils;
        int numDS = numIntDS + numFloatDS;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandIntOffsets = NULL;
        int **bandFloatOffsets = NULL;
		int height = 0;
		int width = 0;
		int numIntBands = 0;
        int numFloatBands = 0;
		
		float **inputFloatData = NULL;
		float *inDataFloatColumn = NULL;
        unsigned int **inputIntData = NULL;
		long *inDataIntColumn = NULL;
        
        double **outputData = NULL;
		double *outDataColumn = NULL;
        
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		GDALRasterBand **inputRasterFloatBands = NULL;
        GDALRasterBand **inputRasterIntBands = NULL;
        
        GDALDataset *outputImageDS = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            //std::cout << "height = " << height << std::endl;
            //std::cout << "Width = " << width << std::endl;
            
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
			// Count number of image bands
			for(int i = 0; i < numIntDS; i++)
			{
				numIntBands += datasets[i]->GetRasterCount();
			}
            for(int i = numIntDS; i < numDS; i++)
			{
				numFloatBands += datasets[i]->GetRasterCount();
			}
            
            //std::cout << "Number of Int Bands: " << numIntBands << std::endl;
            //std::cout << "Number of Float Bands: " << numFloatBands << std::endl;
			
            // Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw RSGISImageBandException("Requested GDAL driver does not exists..");
			}
			std::cout << "New image width = " << width << " height = " << height << " bands = " << this->numOutBands << std::endl;
			
			outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, this->numOutBands, gdalDataType, NULL);
			
			if(outputImageDS == NULL)
			{
				throw RSGISImageBandException("Output image could not be created. Check filepath.");
			}
			outputImageDS->SetGeoTransform(gdalTranslation);
			if(useImageProj)
			{
				outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
			}
			else
			{
				outputImageDS->SetProjection(proj.c_str());
			}
            
			// Get Image Input Bands
			bandIntOffsets = new int*[numIntBands];
			inputRasterIntBands = new GDALRasterBand*[numIntBands];
            
            bandFloatOffsets = new int*[numFloatBands];
			inputRasterFloatBands = new GDALRasterBand*[numFloatBands];
            
			int counter = 0;
			for(int i = 0; i < numIntDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterIntBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandIntOffsets[counter] = new int[2];
					bandIntOffsets[counter][0] = dsOffsets[i][0];
					bandIntOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " (int) band " << j << " offset [" << bandIntOffsets[counter][0] << "," << bandIntOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
            counter = 0;
            for(int i = numIntDS; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterFloatBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandFloatOffsets[counter] = new int[2];
					bandFloatOffsets[counter][0] = dsOffsets[i][0];
					bandFloatOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " (float) band " << j << " offset [" << bandFloatOffsets[counter][0] << "," << bandFloatOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
            //Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
				if (setOutNames) // Set output band names
				{
					outputRasterBands[i]->SetDescription(bandNames[i].c_str());
				}
			}
            int outXBlockSize = 0;
            int outYBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&outXBlockSize, &outYBlockSize);
            
            if(outYBlockSize > yBlockSize)
            {
                yBlockSize = outYBlockSize;
            }
			
			// Allocate memory
			inputIntData = new unsigned int*[numIntBands];
			for(int i = 0; i < numIntBands; i++)
			{
				inputIntData[i] = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
			}
			inDataIntColumn = new long[numIntBands];
            
            inputFloatData = new float*[numFloatBands];
			for(int i = 0; i < numFloatBands; i++)
			{
				inputFloatData[i] = (float *) CPLMalloc(sizeof(float)*width*yBlockSize);
			}
			inDataFloatColumn = new float[numFloatBands];
            
            outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*(width*yBlockSize));
			}
			outDataColumn = new double[this->numOutBands];
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				for(int n = 0; n < numIntBands; n++)
				{
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * i);
					inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, yBlockSize, inputIntData[n], width, yBlockSize, GDT_UInt32, 0, 0);
				}
                
                for(int n = 0; n < numFloatBands; n++)
				{
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * i);
					inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, yBlockSize, inputFloatData[n], width, yBlockSize, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                    }
                }
                
                for(int n = 0; n < this->numOutBands; n++)
				{
                    rowOffset = yBlockSize * i;
					outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, yBlockSize, outputData[n], width, yBlockSize, GDT_Float64, 0, 0);
				}
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numIntBands; n++)
				{
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, remainRows, inputIntData[n], width, remainRows, GDT_UInt32, 0, 0);
				}
                
                for(int n = 0; n < numFloatBands; n++)
				{
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, remainRows, inputFloatData[n], width, remainRows, GDT_Float32, 0, 0);
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
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                    }
                    
                    for(int n = 0; n < this->numOutBands; n++)
                    {
                        rowOffset = (yBlockSize * nYBlocks);
                        outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, remainRows, outputData[n], width, remainRows, GDT_Float64, 0, 0);
                    }
                }
            }
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandIntOffsets != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(bandIntOffsets[i] != NULL)
					{
						delete[] bandIntOffsets[i];
					}
				}
				delete[] bandIntOffsets;
			}
            
            if(bandFloatOffsets != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(bandFloatOffsets[i] != NULL)
					{
						delete[] bandFloatOffsets[i];
					}
				}
				delete[] bandFloatOffsets;
			}
			
			if(inputIntData != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(inputIntData[i] != NULL)
					{
						CPLFree(inputIntData[i]);
					}
				}
				delete[] inputIntData;
			}
            
            if(inputFloatData != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(inputFloatData[i] != NULL)
					{
						delete[] inputFloatData[i];
					}
				}
				delete[] inputFloatData;
			}
            
			if(inDataIntColumn != NULL)
			{
				delete[] inDataIntColumn;
			}
            
            if(inDataFloatColumn != NULL)
			{
				delete[] inDataFloatColumn;
			}
            
			if(inputRasterIntBands != NULL)
			{
				delete[] inputRasterIntBands;
			}
            
            if(inputRasterFloatBands != NULL)
			{
				delete[] inputRasterFloatBands;
			}
            
            if(outputData != NULL)
            {
                for(int i = 0; i < this->numOutBands; i++)
                {
                    if(outputData[i] != NULL)
                    {
                        delete[] outputData[i];
                    }
                }
                delete[] outputData;
            }
            
            if(outDataColumn != NULL)
            {
                delete[] outDataColumn;
            }
            
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
            
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandIntOffsets != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(bandIntOffsets[i] != NULL)
					{
						delete[] bandIntOffsets[i];
					}
				}
				delete[] bandIntOffsets;
			}
            
            if(bandFloatOffsets != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(bandFloatOffsets[i] != NULL)
					{
						delete[] bandFloatOffsets[i];
					}
				}
				delete[] bandFloatOffsets;
			}
			
			if(inputIntData != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(inputIntData[i] != NULL)
					{
						CPLFree(inputIntData[i]);
					}
				}
				delete[] inputIntData;
			}
            
            if(inputFloatData != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(inputFloatData[i] != NULL)
					{
						delete[] inputFloatData[i];
					}
				}
				delete[] inputFloatData;
			}
            
			if(inDataIntColumn != NULL)
			{
				delete[] inDataIntColumn;
			}
            
            if(inDataFloatColumn != NULL)
			{
				delete[] inDataFloatColumn;
			}
            
			if(inputRasterIntBands != NULL)
			{
				delete[] inputRasterIntBands;
			}
            
            if(inputRasterFloatBands != NULL)
			{
				delete[] inputRasterFloatBands;
			}
            
            if(outputData != NULL)
            {
                for(int i = 0; i < this->numOutBands; i++)
                {
                    if(outputData[i] != NULL)
                    {
                        delete[] outputData[i];
                    }
                }
                delete[] outputData;
            }
            
            if(outDataColumn != NULL)
            {
                delete[] outDataColumn;
            }
            
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
            
			throw e;
		}
        
        GDALClose(outputImageDS);
        
		if(gdalTranslation != NULL)
        {
            delete[] gdalTranslation;
        }
        
        if(dsOffsets != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                if(dsOffsets[i] != NULL)
                {
                    delete[] dsOffsets[i];
                }
            }
            delete[] dsOffsets;
        }
        
        if(bandIntOffsets != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(bandIntOffsets[i] != NULL)
                {
                    delete[] bandIntOffsets[i];
                }
            }
            delete[] bandIntOffsets;
        }
        
        if(bandFloatOffsets != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(bandFloatOffsets[i] != NULL)
                {
                    delete[] bandFloatOffsets[i];
                }
            }
            delete[] bandFloatOffsets;
        }
        
        if(inputIntData != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(inputIntData[i] != NULL)
                {
                    CPLFree(inputIntData[i]);
                }
            }
            delete[] inputIntData;
        }
        
        if(inputFloatData != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(inputFloatData[i] != NULL)
                {
                    delete[] inputFloatData[i];
                }
            }
            delete[] inputFloatData;
        }
        
        if(inDataIntColumn != NULL)
        {
            delete[] inDataIntColumn;
        }
        
        if(inDataFloatColumn != NULL)
        {
            delete[] inDataFloatColumn;
        }
        
        if(inputRasterIntBands != NULL)
        {
            delete[] inputRasterIntBands;
        }
        
        if(inputRasterFloatBands != NULL)
        {
            delete[] inputRasterFloatBands;
        }
        
        if(outputData != NULL)
        {
            for(int i = 0; i < this->numOutBands; i++)
            {
                if(outputData[i] != NULL)
                {
                    delete[] outputData[i];
                }
            }
            delete[] outputData;
        }
        
        if(outDataColumn != NULL)
        {
            delete[] outDataColumn;
        }
        
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }
    }
    
    void RSGISCalcImage::calcImage(GDALDataset **datasets, int numIntDS, int numFloatDS, geos::geom::Envelope *env, bool quiet) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
		RSGISImageUtils imgUtils;
        int numDS = numIntDS + numFloatDS;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandIntOffsets = NULL;
        int **bandFloatOffsets = NULL;
		int height = 0;
		int width = 0;
		int numIntBands = 0;
        int numFloatBands = 0;
		
		float **inputFloatData = NULL;
		float *inDataFloatColumn = NULL;
        unsigned int **inputIntData = NULL;
		long *inDataIntColumn = NULL;
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		GDALRasterBand **inputRasterFloatBands = NULL;
        GDALRasterBand **inputRasterIntBands = NULL;
		
		try
		{
			// Find image overlap
            if(env == NULL)
            {
                imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            }
			else
            {
                imgUtils.getImageOverlapCut2Env(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env, &xBlockSize, &yBlockSize);
            }
            
            //std::cout << "height = " << height << std::endl;
            //std::cout << "Width = " << width << std::endl;
            
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
			// Count number of image bands
			for(int i = 0; i < numIntDS; i++)
			{
				numIntBands += datasets[i]->GetRasterCount();
			}
            for(int i = numIntDS; i < numDS; i++)
			{
				numFloatBands += datasets[i]->GetRasterCount();
			}
            
            //std::cout << "Number of Int Bands: " << numIntBands << std::endl;
            //std::cout << "Number of Float Bands: " << numFloatBands << std::endl;
			
			// Get Image Input Bands
			bandIntOffsets = new int*[numIntBands];
			inputRasterIntBands = new GDALRasterBand*[numIntBands];
            
            bandFloatOffsets = new int*[numFloatBands];
			inputRasterFloatBands = new GDALRasterBand*[numFloatBands];
            
			int counter = 0;
			for(int i = 0; i < numIntDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterIntBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandIntOffsets[counter] = new int[2];
					bandIntOffsets[counter][0] = dsOffsets[i][0];
					bandIntOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " (int) band " << j << " offset [" << bandIntOffsets[counter][0] << "," << bandIntOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
            counter = 0;
            for(int i = numIntDS; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterFloatBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandFloatOffsets[counter] = new int[2];
					bandFloatOffsets[counter][0] = dsOffsets[i][0];
					bandFloatOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " (float) band " << j << " offset [" << bandFloatOffsets[counter][0] << "," << bandFloatOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			// Allocate memory
			inputIntData = new unsigned int*[numIntBands];
			for(int i = 0; i < numIntBands; i++)
			{
				inputIntData[i] = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
			}
			inDataIntColumn = new long[numIntBands];
            
            inputFloatData = new float*[numFloatBands];
			for(int i = 0; i < numFloatBands; i++)
			{
				inputFloatData[i] = (float *) CPLMalloc(sizeof(float)*width*yBlockSize);
			}
			inDataFloatColumn = new float[numFloatBands];
            
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10;
			int feedbackCounter = 0;
            if(!quiet)
            {
                std::cout << "Started " << std::flush;
            }
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				for(int n = 0; n < numIntBands; n++)
				{
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * i);
					inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, yBlockSize, inputIntData[n], width, yBlockSize, GDT_UInt32, 0, 0);
				}
                
                for(int n = 0; n < numFloatBands; n++)
				{
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * i);
					inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, yBlockSize, inputFloatData[n], width, yBlockSize, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((!quiet) && (feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands);
                    }
                }
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numIntBands; n++)
				{
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, remainRows, inputIntData[n], width, remainRows, GDT_UInt32, 0, 0);
				}
                
                
                for(int n = 0; n < numFloatBands; n++)
				{
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, remainRows, inputFloatData[n], width, remainRows, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < remainRows; ++m)
                {
                    if((!quiet) && (feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands);
                    }
                }
            }
            if(!quiet)
            {
                std::cout << " Complete.\n";
            }
		}
		catch(RSGISImageCalcException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandIntOffsets != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(bandIntOffsets[i] != NULL)
					{
						delete[] bandIntOffsets[i];
					}
				}
				delete[] bandIntOffsets;
			}
            
            if(bandFloatOffsets != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(bandFloatOffsets[i] != NULL)
					{
						delete[] bandFloatOffsets[i];
					}
				}
				delete[] bandFloatOffsets;
			}
			
			if(inputIntData != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(inputIntData[i] != NULL)
					{
						CPLFree(inputIntData[i]);
					}
				}
				delete[] inputIntData;
			}
            
            if(inputFloatData != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(inputFloatData[i] != NULL)
					{
						delete[] inputFloatData[i];
					}
				}
				delete[] inputFloatData;
			}
            
			if(inDataIntColumn != NULL)
			{
				delete[] inDataIntColumn;
			}
            
            if(inDataFloatColumn != NULL)
			{
				delete[] inDataFloatColumn;
			}
            
			if(inputRasterIntBands != NULL)
			{
				delete[] inputRasterIntBands;
			}
            
            if(inputRasterFloatBands != NULL)
			{
				delete[] inputRasterFloatBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandIntOffsets != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(bandIntOffsets[i] != NULL)
					{
						delete[] bandIntOffsets[i];
					}
				}
				delete[] bandIntOffsets;
			}
            
            if(bandFloatOffsets != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(bandFloatOffsets[i] != NULL)
					{
						delete[] bandFloatOffsets[i];
					}
				}
				delete[] bandFloatOffsets;
			}
			
			if(inputIntData != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(inputIntData[i] != NULL)
					{
						CPLFree(inputIntData[i]);
					}
				}
				delete[] inputIntData;
			}
            
            if(inputFloatData != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(inputFloatData[i] != NULL)
					{
						delete[] inputFloatData[i];
					}
				}
				delete[] inputFloatData;
			}
            
			if(inDataIntColumn != NULL)
			{
				delete[] inDataIntColumn;
			}
            
            if(inDataFloatColumn != NULL)
			{
				delete[] inDataFloatColumn;
			}
            
			if(inputRasterIntBands != NULL)
			{
				delete[] inputRasterIntBands;
			}
            
            if(inputRasterFloatBands != NULL)
			{
				delete[] inputRasterFloatBands;
			}
			throw e;
		}
        
		if(gdalTranslation != NULL)
        {
            delete[] gdalTranslation;
        }
        
        if(dsOffsets != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                if(dsOffsets[i] != NULL)
                {
                    delete[] dsOffsets[i];
                }
            }
            delete[] dsOffsets;
        }
        
        if(bandIntOffsets != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(bandIntOffsets[i] != NULL)
                {
                    delete[] bandIntOffsets[i];
                }
            }
            delete[] bandIntOffsets;
        }
        
        if(bandFloatOffsets != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(bandFloatOffsets[i] != NULL)
                {
                    delete[] bandFloatOffsets[i];
                }
            }
            delete[] bandFloatOffsets;
        }
        
        if(inputIntData != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(inputIntData[i] != NULL)
                {
                    CPLFree(inputIntData[i]);
                }
            }
            delete[] inputIntData;
        }
        
        if(inputFloatData != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(inputFloatData[i] != NULL)
                {
                    delete[] inputFloatData[i];
                }
            }
            delete[] inputFloatData;
        }
        
        if(inDataIntColumn != NULL)
        {
            delete[] inDataIntColumn;
        }
        
        if(inDataFloatColumn != NULL)
        {
            delete[] inDataFloatColumn;
        }
        
        if(inputRasterIntBands != NULL)
        {
            delete[] inputRasterIntBands;
        }
        
        if(inputRasterFloatBands != NULL)
        {
            delete[] inputRasterFloatBands;
        }
    }
	
    void RSGISCalcImage::calcImage(GDALDataset **datasets, int numIntDS, int numFloatDS, GDALDataset *outputImageDS) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
		RSGISImageUtils imgUtils;
        int numDS = numIntDS + numFloatDS;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandIntOffsets = NULL;
        int **bandFloatOffsets = NULL;
		int height = 0;
		int width = 0;
		int numIntBands = 0;
        int numFloatBands = 0;
		
		float **inputFloatData = NULL;
		float *inDataFloatColumn = NULL;
        unsigned int **inputIntData = NULL;
		long *inDataIntColumn = NULL;
        
        double **outputData = NULL;
		double *outDataColumn = NULL;
        
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		GDALRasterBand **inputRasterFloatBands = NULL;
        GDALRasterBand **inputRasterIntBands = NULL;
        
		GDALRasterBand **outputRasterBands = NULL;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            if(outputImageDS->GetRasterXSize() != width)
            {
                throw RSGISImageCalcException("The output dataset does not have the correct width\n");
            }
            
            if(outputImageDS->GetRasterYSize() != height)
            {
                throw RSGISImageCalcException("The output dataset does not have the correct height\n");
            }
            
            if(outputImageDS->GetRasterCount() != this->numOutBands)
            {
                throw RSGISImageCalcException("The output dataset does not have the correct number of image bands\n");
            }
            
			// Count number of image bands
			for(int i = 0; i < numIntDS; i++)
			{
				numIntBands += datasets[i]->GetRasterCount();
			}
            for(int i = numIntDS; i < numDS; i++)
			{
				numFloatBands += datasets[i]->GetRasterCount();
			}
            
            //std::cout << "Number of Int Bands: " << numIntBands << std::endl;
            //std::cout << "Number of Float Bands: " << numFloatBands << std::endl;
			
            // Get Image Input Bands
			bandIntOffsets = new int*[numIntBands];
			inputRasterIntBands = new GDALRasterBand*[numIntBands];
            
            bandFloatOffsets = new int*[numFloatBands];
			inputRasterFloatBands = new GDALRasterBand*[numFloatBands];
            
			int counter = 0;
			for(int i = 0; i < numIntDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterIntBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandIntOffsets[counter] = new int[2];
					bandIntOffsets[counter][0] = dsOffsets[i][0];
					bandIntOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " (int) band " << j << " offset [" << bandIntOffsets[counter][0] << "," << bandIntOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
            counter = 0;
            for(int i = numIntDS; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterFloatBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandFloatOffsets[counter] = new int[2];
					bandFloatOffsets[counter][0] = dsOffsets[i][0];
					bandFloatOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " (float) band " << j << " offset [" << bandFloatOffsets[counter][0] << "," << bandFloatOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
            //Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
			}
            int outXBlockSize = 0;
            int outYBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&outXBlockSize, &outYBlockSize);
            
            if(outYBlockSize > yBlockSize)
            {
                yBlockSize = outYBlockSize;
            }
			
			// Allocate memory
			inputIntData = new unsigned int*[numIntBands];
			for(int i = 0; i < numIntBands; i++)
			{
				inputIntData[i] = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
			}
			inDataIntColumn = new long[numIntBands];
            
            inputFloatData = new float*[numFloatBands];
			for(int i = 0; i < numFloatBands; i++)
			{
				inputFloatData[i] = (float *) CPLMalloc(sizeof(float)*width*yBlockSize);
			}
			inDataFloatColumn = new float[numFloatBands];
            
            outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*(width*yBlockSize));
			}
			outDataColumn = new double[this->numOutBands];
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
            std::cout.precision(20);
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				for(int n = 0; n < numIntBands; n++)
				{
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * i);
					inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, yBlockSize, inputIntData[n], width, yBlockSize, GDT_UInt32, 0, 0);
				}
                
                for(int n = 0; n < numFloatBands; n++)
				{
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * i);
					inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, yBlockSize, inputFloatData[n], width, yBlockSize, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                    }
                }
                
                for(int n = 0; n < this->numOutBands; n++)
				{
                    rowOffset = yBlockSize * i;
					outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, yBlockSize, outputData[n], width, yBlockSize, GDT_Float64, 0, 0);
				}
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numIntBands; n++)
				{
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, remainRows, inputIntData[n], width, remainRows, GDT_UInt32, 0, 0);
				}
                
                for(int n = 0; n < numFloatBands; n++)
				{
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, remainRows, inputFloatData[n], width, remainRows, GDT_Float32, 0, 0);
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
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                    }
                    
                    for(int n = 0; n < this->numOutBands; n++)
                    {
                        rowOffset = (yBlockSize * nYBlocks);
                        outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, remainRows, outputData[n], width, remainRows, GDT_Float64, 0, 0);
                    }
                }
            }
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandIntOffsets != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(bandIntOffsets[i] != NULL)
					{
						delete[] bandIntOffsets[i];
					}
				}
				delete[] bandIntOffsets;
			}
            
            if(bandFloatOffsets != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(bandFloatOffsets[i] != NULL)
					{
						delete[] bandFloatOffsets[i];
					}
				}
				delete[] bandFloatOffsets;
			}
			
			if(inputIntData != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(inputIntData[i] != NULL)
					{
						CPLFree(inputIntData[i]);
					}
				}
				delete[] inputIntData;
			}
            
            if(inputFloatData != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(inputFloatData[i] != NULL)
					{
						delete[] inputFloatData[i];
					}
				}
				delete[] inputFloatData;
			}
            
			if(inDataIntColumn != NULL)
			{
				delete[] inDataIntColumn;
			}
            
            if(inDataFloatColumn != NULL)
			{
				delete[] inDataFloatColumn;
			}
            
			if(inputRasterIntBands != NULL)
			{
				delete[] inputRasterIntBands;
			}
            
            if(inputRasterFloatBands != NULL)
			{
				delete[] inputRasterFloatBands;
			}
            
            if(outputData != NULL)
            {
                for(int i = 0; i < this->numOutBands; i++)
                {
                    if(outputData[i] != NULL)
                    {
                        delete[] outputData[i];
                    }
                }
                delete[] outputData;
            }
            
            if(outDataColumn != NULL)
            {
                delete[] outDataColumn;
            }
            
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
            
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandIntOffsets != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(bandIntOffsets[i] != NULL)
					{
						delete[] bandIntOffsets[i];
					}
				}
				delete[] bandIntOffsets;
			}
            
            if(bandFloatOffsets != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(bandFloatOffsets[i] != NULL)
					{
						delete[] bandFloatOffsets[i];
					}
				}
				delete[] bandFloatOffsets;
			}
			
			if(inputIntData != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(inputIntData[i] != NULL)
					{
						CPLFree(inputIntData[i]);
					}
				}
				delete[] inputIntData;
			}
            
            if(inputFloatData != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(inputFloatData[i] != NULL)
					{
						delete[] inputFloatData[i];
					}
				}
				delete[] inputFloatData;
			}
            
			if(inDataIntColumn != NULL)
			{
				delete[] inDataIntColumn;
			}
            
            if(inDataFloatColumn != NULL)
			{
				delete[] inDataFloatColumn;
			}
            
			if(inputRasterIntBands != NULL)
			{
				delete[] inputRasterIntBands;
			}
            
            if(inputRasterFloatBands != NULL)
			{
				delete[] inputRasterFloatBands;
			}
            
            if(outputData != NULL)
            {
                for(int i = 0; i < this->numOutBands; i++)
                {
                    if(outputData[i] != NULL)
                    {
                        delete[] outputData[i];
                    }
                }
                delete[] outputData;
            }
            
            if(outDataColumn != NULL)
            {
                delete[] outDataColumn;
            }
            
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
            
			throw e;
		}
        
		if(gdalTranslation != NULL)
        {
            delete[] gdalTranslation;
        }
        
        if(dsOffsets != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                if(dsOffsets[i] != NULL)
                {
                    delete[] dsOffsets[i];
                }
            }
            delete[] dsOffsets;
        }
        
        if(bandIntOffsets != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(bandIntOffsets[i] != NULL)
                {
                    delete[] bandIntOffsets[i];
                }
            }
            delete[] bandIntOffsets;
        }
        
        if(bandFloatOffsets != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(bandFloatOffsets[i] != NULL)
                {
                    delete[] bandFloatOffsets[i];
                }
            }
            delete[] bandFloatOffsets;
        }
        
        if(inputIntData != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(inputIntData[i] != NULL)
                {
                    CPLFree(inputIntData[i]);
                }
            }
            delete[] inputIntData;
        }
        
        if(inputFloatData != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(inputFloatData[i] != NULL)
                {
                    delete[] inputFloatData[i];
                }
            }
            delete[] inputFloatData;
        }
        
        if(inDataIntColumn != NULL)
        {
            delete[] inDataIntColumn;
        }
        
        if(inDataFloatColumn != NULL)
        {
            delete[] inDataFloatColumn;
        }
        
        if(inputRasterIntBands != NULL)
        {
            delete[] inputRasterIntBands;
        }
        
        if(inputRasterFloatBands != NULL)
        {
            delete[] inputRasterFloatBands;
        }
        
        if(outputData != NULL)
        {
            for(int i = 0; i < this->numOutBands; i++)
            {
                if(outputData[i] != NULL)
                {
                    delete[] outputData[i];
                }
            }
            delete[] outputData;
        }
        
        if(outDataColumn != NULL)
        {
            delete[] outDataColumn;
        }
        
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }
    }
    
	void RSGISCalcImage::calcImage(GDALDataset **datasets, int numDS) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputData = NULL;
		float *inDataColumn = NULL;
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		GDALRasterBand **inputRasterBands = NULL;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            //std::cout << "height = " << height << std::endl;
            //std::cout << "Width = " << width << std::endl;
            
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width*yBlockSize);
			}
			inDataColumn = new float[numInBands];
            
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * i);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, yBlockSize, inputData[n], width, yBlockSize, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands);
                        
                    }
                }
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputData[n], width, remainRows, GDT_Float32, 0, 0);
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
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands);
                        
                    }
                }
            }
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
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
    
    void RSGISCalcImage::calcImageBand(GDALDataset **datasets, int numDS, std::string outputImageBase, std::string gdalFormat) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
        rsgis::math::RSGISMathsUtils mathUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float *inputData = NULL;
		double **outputData = NULL;
		float *inDataColumn = NULL;
		double *outDataColumn = NULL;
		
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
        
        std::string outputImageFileName = "";
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation);
            
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
            
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw RSGISImageBandException("ENVI driver does not exists..");
			}
            
            for(int cImgBand = 0; cImgBand < numInBands; ++cImgBand)
            {
                outputImageFileName = outputImageBase + mathUtils.inttostring(cImgBand) + std::string(".env");
                
                std::cout << "New image width = " << width << " height = " << height << " bands = " << this->numOutBands << std::endl;
				
				outputImageDS = gdalDriver->Create(outputImageFileName.c_str(), width, height, this->numOutBands, GDT_Float32, NULL);
				
                if(outputImageDS == NULL)
                {
                    throw RSGISImageBandException("Output image could not be created. Check filepath.");
                }
                outputImageDS->SetGeoTransform(gdalTranslation);
                if(useImageProj)
                {
                    outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
                }
                else
                {
                    outputImageDS->SetProjection(proj.c_str());
                }
            
                // Get Image Input Bands
                bandOffsets = new int*[numInBands];
                inputRasterBands = new GDALRasterBand*[numInBands];
                int counter = 0;
                for(int i = 0; i < numDS; i++)
                {
                    for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
                    {
                        inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
                        bandOffsets[counter] = new int[2];
                        bandOffsets[counter][0] = dsOffsets[i][0];
                        bandOffsets[counter][1] = dsOffsets[i][1];
                        //std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
                        counter++;
                    }
                }
                
                //Get Image Output Bands
                outputRasterBands = new GDALRasterBand*[this->numOutBands];
                for(int i = 0; i < this->numOutBands; i++)
                {
                    outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
                }
            
                // Allocate memory
                inputData = (float *) CPLMalloc(sizeof(float)*width);
                inDataColumn = new float[1];
                
                outputData = new double*[this->numOutBands];
                for(int i = 0; i < this->numOutBands; i++)
                {
                    outputData[i] = (double *) CPLMalloc(sizeof(double)*width);
                }
                outDataColumn = new double[this->numOutBands];
            
                int feedback = height/10;
                int feedbackCounter = 0;
                std::cout << "Started (Band " << cImgBand+1 << "):\t" << std::flush;
                // Loop images to process data
                for(int i = 0; i < height; i++)
                {
                    //std::cout << i << " of " << height << std::endl;
                    
                    if((feedback != 0) && ((i % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }

                    inputRasterBands[cImgBand]->RasterIO(GF_Read, bandOffsets[cImgBand][0], (bandOffsets[cImgBand][1]+i), width, 1, inputData, width, 1, GDT_Float32, 0, 0);
                    
                    for(int j = 0; j < width; j++)
                    {
                        inDataColumn[0] = inputData[j];
                        
                        this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][j] = outDataColumn[n];
                        }
                        
                    }
                    
                    for(int n = 0; n < this->numOutBands; n++)
                    {
                        outputRasterBands[n]->RasterIO(GF_Write, 0, i, width, 1, outputData[n], width, 1, GDT_Float64, 0, 0);
                    }
                }
                std::cout << " Complete.\n";
                
                GDALClose(outputImageDS);
                
                delete[] inputData;
                
                if(outputData != NULL)
                {
                    for(int i = 0; i < this->numOutBands; i++)
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

                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }
                
                if(outputRasterBands != NULL)
                {
                    delete[] outputRasterBands;
                }
                
                if(bandOffsets != NULL)
                {
                    for(int i = 0; i < numInBands; i++)
                    {
                        if(bandOffsets[i] != NULL)
                        {
                            delete[] bandOffsets[i];
                        }
                    }
                    delete[] bandOffsets;
                }
            }
		}
		catch(RSGISImageCalcException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
            
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}

	}
    
    /*
    void RSGISCalcImage::calcImageInEnv(GDALDataset **datasets, int numDS, std::string outputImage, geos::geom::Envelope *env, bool setOutNames, std::string *bandNames, std::string gdalFormat, GDALDataType gdalDataType) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputData = NULL;
		float **outputData = NULL;
		float *inDataColumn = NULL;
		float *outDataColumn = NULL;
		
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlapCut2Env(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env);
            
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
            
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw RSGISImageBandException("ENVI driver does not exists..");
			}
			std::cout << "New image width = " << width << " height = " << height << " bands = " << this->numOutBands << std::endl;

			outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, this->numOutBands, gdalDataType, NULL);
			
			if(outputImageDS == NULL)
			{
				throw RSGISImageBandException("Output image could not be created. Check filepath.");
			}
			outputImageDS->SetGeoTransform(gdalTranslation);
			if(useImageProj)
			{
				outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
			}
			else
			{
				outputImageDS->SetProjection(proj.c_str());
			}
            
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
				if (setOutNames) // Set output band names
				{
					outputRasterBands[i]->SetDescription(bandNames[i].c_str());
				}
			}
            
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumn = new float[numInBands];
            
			outputData = new float*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			outDataColumn = new float[this->numOutBands];
            
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				//std::cout << i << " of " << height << std::endl;
				
				if((i % feedback) == 0)
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], (bandOffsets[n][1]+i), width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
				}
				
				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumn[n] = inputData[n][j];
					}
					
					this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
					
					for(int n = 0; n < this->numOutBands; n++)
					{
						outputData[n][j] = outDataColumn[n];
					}
					
				}
				
				for(int n = 0; n < this->numOutBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Write, 0, i, width, 1, outputData[n], width, 1, GDT_Float32, 0, 0);
				}
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		
		GDALClose(outputImageDS);
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
		}
		
		if(inputData != NULL)
		{
			for(int i = 0; i < numInBands; i++)
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
			for(int i = 0; i < this->numOutBands; i++)
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
		
		if(inputRasterBands != NULL)
		{
			delete[] inputRasterBands;
		}
		
		if(outputRasterBands != NULL)
		{
			delete[] outputRasterBands;
		}
	}
	*/
    
    void RSGISCalcImage::calcImageInEnv(GDALDataset **datasets, int numDS, std::string outputImage, geos::geom::Envelope *env, bool setOutNames, std::string *bandNames, std::string gdalFormat, GDALDataType gdalDataType) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		float **inputData = NULL;
		double **outputData = NULL;
		float *inDataColumn = NULL;
		double *outDataColumn = NULL;
		
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
        
        try
		{
			// Find image overlap
            imgUtils.getImageOverlapCut2Env(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env, &xBlockSize, &yBlockSize);

            // Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
            
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw RSGISImageBandException("Requested GDAL driver does not exists..");
			}
			std::cout << "New image width = " << width << " height = " << height << " bands = " << this->numOutBands << std::endl;
			
			outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, this->numOutBands, gdalDataType, NULL);
			
			if(outputImageDS == NULL)
			{
				throw RSGISImageBandException("Output image could not be created. Check filepath.");
			}
			outputImageDS->SetGeoTransform(gdalTranslation);
			if(useImageProj)
			{
				outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
			}
			else
			{
				outputImageDS->SetProjection(proj.c_str());
			}
            
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					counter++;
				}
			}
            
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
				if (setOutNames) // Set output band names
				{
					outputRasterBands[i]->SetDescription(bandNames[i].c_str());
				}
			}
            int outXBlockSize = 0;
            int outYBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&outXBlockSize, &outYBlockSize);
            
            if(outYBlockSize > yBlockSize)
            {
                yBlockSize = outYBlockSize;
            }
            
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*(width*yBlockSize));
			}
			inDataColumn = new float[numInBands];
            
			outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*(width*yBlockSize));
			}
			outDataColumn = new double[this->numOutBands];
            
            int nYBlocks = floor(((double)height) / ((double)yBlockSize));
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10.0;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * i);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, yBlockSize, inputData[n], width, yBlockSize, GDT_Float32, 0, 0);
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
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                        
                    }
                }
				
				for(int n = 0; n < this->numOutBands; n++)
				{
                    rowOffset = yBlockSize * i;
					outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, yBlockSize, outputData[n], width, yBlockSize, GDT_Float64, 0, 0);
				}
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputData[n], width, remainRows, GDT_Float32, 0, 0);
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
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
                        
                        for(int n = 0; n < this->numOutBands; n++)
                        {
                            outputData[n][(m*width)+j] = outDataColumn[n];
                        }
                        
                    }
                }
				
				for(int n = 0; n < this->numOutBands; n++)
				{
                    rowOffset = (yBlockSize * nYBlocks);
					outputRasterBands[n]->RasterIO(GF_Write, 0, rowOffset, width, remainRows, outputData[n], width, remainRows, GDT_Float64, 0, 0);
				}
            }
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		
		GDALClose(outputImageDS);
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			}
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
		}
		
		if(inputData != NULL)
		{
			for(int i = 0; i < numInBands; i++)
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
			for(int i = 0; i < this->numOutBands; i++)
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
		
		if(inputRasterBands != NULL)
		{
			delete[] inputRasterBands;
		}
		
		if(outputRasterBands != NULL)
		{
			delete[] outputRasterBands;
		}
    }
    
    void RSGISCalcImage::calcImageInEnv(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, bool quiet) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		float **inputData = NULL;
		float *inDataColumn = NULL;
		
		GDALRasterBand **inputRasterBands = NULL;

        
        try
		{
			// Find image overlap
            imgUtils.getImageOverlapCut2Env(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env, &xBlockSize, &yBlockSize);
            
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
            
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*(width*yBlockSize));
			}
			inDataColumn = new float[numInBands];
            
            int nYBlocks = floor(((double)height) / ((double)yBlockSize));
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10.0;
			int feedbackCounter = 0;
            if(!quiet)
            {
                std::cout << "Started " << std::flush;
            }
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * i);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, yBlockSize, inputData[n], width, yBlockSize, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((!quiet) && (feedback != 0) && (((i*yBlockSize)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands);
                        
                    }
                }
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputData[n], width, remainRows, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < remainRows; ++m)
                {
                    if((!quiet) && (feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataColumn, numInBands);
                    }
                }
            }
            if(!quiet)
            {
                std::cout << " Complete.\n";
            }
		}
		catch(RSGISImageCalcException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			}
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
		}
		
		if(inputData != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(inputData[i] != NULL)
				{
					CPLFree(inputData[i]);
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
    
    void RSGISCalcImage::calcImageInEnv(GDALDataset **datasets, int numIntDS, int numFloatDS, geos::geom::Envelope *env, bool quiet) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
        RSGISImageUtils imgUtils;
        int numDS = numIntDS + numFloatDS;
        double *gdalTranslation = new double[6];
        int **dsOffsets = new int*[numDS];
        for(int i = 0; i < numDS; i++)
        {
            dsOffsets[i] = new int[2];
        }
        int **bandIntOffsets = NULL;
        int **bandFloatOffsets = NULL;
        int height = 0;
        int width = 0;
        int numIntBands = 0;
        int numFloatBands = 0;
        
        float **inputFloatData = NULL;
        float *inDataFloatColumn = NULL;
        unsigned int **inputIntData = NULL;
        long *inDataIntColumn = NULL;
        int xBlockSize = 0;
        int yBlockSize = 0;
        
        GDALRasterBand **inputRasterFloatBands = NULL;
        GDALRasterBand **inputRasterIntBands = NULL;
        
        try
        {
            // Find image overlap
            imgUtils.getImageOverlapCut2Env(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env, &xBlockSize, &yBlockSize);
            
            //std::cout << "height = " << height << std::endl;
            //std::cout << "Width = " << width << std::endl;
            
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
            // Count number of image bands
            for(int i = 0; i < numIntDS; i++)
            {
                numIntBands += datasets[i]->GetRasterCount();
            }
            for(int i = numIntDS; i < numDS; i++)
            {
                numFloatBands += datasets[i]->GetRasterCount();
            }
            
            //std::cout << "Number of Int Bands: " << numIntBands << std::endl;
            //std::cout << "Number of Float Bands: " << numFloatBands << std::endl;
            
            // Get Image Input Bands
            bandIntOffsets = new int*[numIntBands];
            inputRasterIntBands = new GDALRasterBand*[numIntBands];
            
            bandFloatOffsets = new int*[numFloatBands];
            inputRasterFloatBands = new GDALRasterBand*[numFloatBands];
            
            int counter = 0;
            for(int i = 0; i < numIntDS; i++)
            {
                for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
                {
                    inputRasterIntBands[counter] = datasets[i]->GetRasterBand(j+1);
                    bandIntOffsets[counter] = new int[2];
                    bandIntOffsets[counter][0] = dsOffsets[i][0];
                    bandIntOffsets[counter][1] = dsOffsets[i][1];
                    //std::cout << counter << ") dataset " << i << " (int) band " << j << " offset [" << bandIntOffsets[counter][0] << "," << bandIntOffsets[counter][1] << "]\n";
                    counter++;
                }
            }
            
            counter = 0;
            for(int i = numIntDS; i < numDS; i++)
            {
                for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
                {
                    inputRasterFloatBands[counter] = datasets[i]->GetRasterBand(j+1);
                    bandFloatOffsets[counter] = new int[2];
                    bandFloatOffsets[counter][0] = dsOffsets[i][0];
                    bandFloatOffsets[counter][1] = dsOffsets[i][1];
                    //std::cout << counter << ") dataset " << i << " (float) band " << j << " offset [" << bandFloatOffsets[counter][0] << "," << bandFloatOffsets[counter][1] << "]\n";
                    counter++;
                }
            }
            
            // Allocate memory
            inputIntData = new unsigned int*[numIntBands];
            for(int i = 0; i < numIntBands; i++)
            {
                inputIntData[i] = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
            }
            inDataIntColumn = new long[numIntBands];
            
            inputFloatData = new float*[numFloatBands];
            for(int i = 0; i < numFloatBands; i++)
            {
                inputFloatData[i] = (float *) CPLMalloc(sizeof(float)*width*yBlockSize);
            }
            inDataFloatColumn = new float[numFloatBands];
            
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            int feedback = height/10;
            int feedbackCounter = 0;
            if(!quiet)
            {
                std::cout << "Started " << std::flush;
            }
            // Loop images to process data
            for(int i = 0; i < nYBlocks; i++)
            {
                for(int n = 0; n < numIntBands; n++)
                {
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * i);
                    inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, yBlockSize, inputIntData[n], width, yBlockSize, GDT_UInt32, 0, 0);
                }
                
                for(int n = 0; n < numFloatBands; n++)
                {
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * i);
                    inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, yBlockSize, inputFloatData[n], width, yBlockSize, GDT_Float32, 0, 0);
                }
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((!quiet) && (feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands);
                    }
                }
            }
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numIntBands; n++)
                {
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * nYBlocks);
                    inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, remainRows, inputIntData[n], width, remainRows, GDT_UInt32, 0, 0);
                }
                
                
                for(int n = 0; n < numFloatBands; n++)
                {
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * nYBlocks);
                    inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, remainRows, inputFloatData[n], width, remainRows, GDT_Float32, 0, 0);
                }
                
                for(int m = 0; m < remainRows; ++m)
                {
                    if((!quiet) && (feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands);
                    }
                }
            }
            if(!quiet)
            {
                std::cout << " Complete.\n";
            }
        }
        catch(RSGISImageCalcException& e)
        {
            if(gdalTranslation != NULL)
            {
                delete[] gdalTranslation;
            }
            
            if(dsOffsets != NULL)
            {
                for(int i = 0; i < numDS; i++)
                {
                    if(dsOffsets[i] != NULL)
                    {
                        delete[] dsOffsets[i];
                    }
                }
                delete[] dsOffsets;
            }
            
            if(bandIntOffsets != NULL)
            {
                for(int i = 0; i < numIntBands; i++)
                {
                    if(bandIntOffsets[i] != NULL)
                    {
                        delete[] bandIntOffsets[i];
                    }
                }
                delete[] bandIntOffsets;
            }
            
            if(bandFloatOffsets != NULL)
            {
                for(int i = 0; i < numFloatBands; i++)
                {
                    if(bandFloatOffsets[i] != NULL)
                    {
                        delete[] bandFloatOffsets[i];
                    }
                }
                delete[] bandFloatOffsets;
            }
            
            if(inputIntData != NULL)
            {
                for(int i = 0; i < numIntBands; i++)
                {
                    if(inputIntData[i] != NULL)
                    {
                        CPLFree(inputIntData[i]);
                    }
                }
                delete[] inputIntData;
            }
            
            if(inputFloatData != NULL)
            {
                for(int i = 0; i < numFloatBands; i++)
                {
                    if(inputFloatData[i] != NULL)
                    {
                        delete[] inputFloatData[i];
                    }
                }
                delete[] inputFloatData;
            }
            
            if(inDataIntColumn != NULL)
            {
                delete[] inDataIntColumn;
            }
            
            if(inDataFloatColumn != NULL)
            {
                delete[] inDataFloatColumn;
            }
            
            if(inputRasterIntBands != NULL)
            {
                delete[] inputRasterIntBands;
            }
            
            if(inputRasterFloatBands != NULL)
            {
                delete[] inputRasterFloatBands;
            }
            throw e;
        }
        catch(RSGISImageBandException& e)
        {
            if(gdalTranslation != NULL)
            {
                delete[] gdalTranslation;
            }
            
            if(dsOffsets != NULL)
            {
                for(int i = 0; i < numDS; i++)
                {
                    if(dsOffsets[i] != NULL)
                    {
                        delete[] dsOffsets[i];
                    }
                }
                delete[] dsOffsets;
            }
            
            if(bandIntOffsets != NULL)
            {
                for(int i = 0; i < numIntBands; i++)
                {
                    if(bandIntOffsets[i] != NULL)
                    {
                        delete[] bandIntOffsets[i];
                    }
                }
                delete[] bandIntOffsets;
            }
            
            if(bandFloatOffsets != NULL)
            {
                for(int i = 0; i < numFloatBands; i++)
                {
                    if(bandFloatOffsets[i] != NULL)
                    {
                        delete[] bandFloatOffsets[i];
                    }
                }
                delete[] bandFloatOffsets;
            }
            
            if(inputIntData != NULL)
            {
                for(int i = 0; i < numIntBands; i++)
                {
                    if(inputIntData[i] != NULL)
                    {
                        CPLFree(inputIntData[i]);
                    }
                }
                delete[] inputIntData;
            }
            
            if(inputFloatData != NULL)
            {
                for(int i = 0; i < numFloatBands; i++)
                {
                    if(inputFloatData[i] != NULL)
                    {
                        delete[] inputFloatData[i];
                    }
                }
                delete[] inputFloatData;
            }
            
            if(inDataIntColumn != NULL)
            {
                delete[] inDataIntColumn;
            }
            
            if(inDataFloatColumn != NULL)
            {
                delete[] inDataFloatColumn;
            }
            
            if(inputRasterIntBands != NULL)
            {
                delete[] inputRasterIntBands;
            }
            
            if(inputRasterFloatBands != NULL)
            {
                delete[] inputRasterFloatBands;
            }
            throw e;
        }
        
        if(gdalTranslation != NULL)
        {
            delete[] gdalTranslation;
        }
        
        if(dsOffsets != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                if(dsOffsets[i] != NULL)
                {
                    delete[] dsOffsets[i];
                }
            }
            delete[] dsOffsets;
        }
        
        if(bandIntOffsets != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(bandIntOffsets[i] != NULL)
                {
                    delete[] bandIntOffsets[i];
                }
            }
            delete[] bandIntOffsets;
        }
        
        if(bandFloatOffsets != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(bandFloatOffsets[i] != NULL)
                {
                    delete[] bandFloatOffsets[i];
                }
            }
            delete[] bandFloatOffsets;
        }
        
        if(inputIntData != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(inputIntData[i] != NULL)
                {
                    CPLFree(inputIntData[i]);
                }
            }
            delete[] inputIntData;
        }
        
        if(inputFloatData != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(inputFloatData[i] != NULL)
                {
                    delete[] inputFloatData[i];
                }
            }
            delete[] inputFloatData;
        }
        
        if(inDataIntColumn != NULL)
        {
            delete[] inDataIntColumn;
        }
        
        if(inDataFloatColumn != NULL)
        {
            delete[] inDataFloatColumn;
        }
        
        if(inputRasterIntBands != NULL)
        {
            delete[] inputRasterIntBands;
        }
        
        if(inputRasterFloatBands != NULL)
        {
            delete[] inputRasterFloatBands;
        }
    }
    
    void RSGISCalcImage::calcImagePosPxl(GDALDataset **datasets, int numDS) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputData = NULL;
		float *inDataColumn = NULL;
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		GDALRasterBand **inputRasterBands = NULL;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width*yBlockSize);
			}
			inDataColumn = new float[numInBands];
            
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
			int feedback = height/10;
			int feedbackCounter = 0;
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            geos::geom::Envelope extent;
			std::cout << "Started " << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * i);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, yBlockSize, inputData[n], width, yBlockSize, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    xPxl = 0;
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        extent.init(xPxl, xPxl, yPxl, yPxl);
                        
                        this->calc->calcImageValue(inDataColumn, numInBands, extent);
                        ++xPxl;
                    }
                    ++yPxl;
                }
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numInBands; n++)
				{
                    rowOffset = bandOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputData[n], width, remainRows, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < remainRows; ++m)
                {
                    if((feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    xPxl = 0;
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numInBands; n++)
                        {
                            inDataColumn[n] = inputData[n][(m*width)+j];
                        }
                        
                        extent.init(xPxl, xPxl, yPxl, yPxl);
                        
                        this->calc->calcImageValue(inDataColumn, numInBands, extent);
                        ++xPxl;
                    }
                    ++yPxl;
                }
            }
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			}
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
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
    
    void RSGISCalcImage::calcImagePosPxl(GDALDataset **datasets, int numIntDS, int numFloatDS) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
        RSGISImageUtils imgUtils;
        int numDS = numIntDS + numFloatDS;
        double *gdalTranslation = new double[6];
        int **dsOffsets = new int*[numDS];
        for(int i = 0; i < numDS; i++)
        {
            dsOffsets[i] = new int[2];
        }
        int **bandIntOffsets = NULL;
        int **bandFloatOffsets = NULL;
        int height = 0;
        int width = 0;
        int numIntBands = 0;
        int numFloatBands = 0;
        
        float **inputFloatData = NULL;
        float *inDataFloatColumn = NULL;
        unsigned int **inputIntData = NULL;
        long *inDataIntColumn = NULL;
        int xBlockSize = 0;
        int yBlockSize = 0;
        
        GDALRasterBand **inputRasterFloatBands = NULL;
        GDALRasterBand **inputRasterIntBands = NULL;
        
        geos::geom::Envelope extent;
        unsigned int xPxl = 0;
        unsigned int yPxl = 0;
        
        try
        {
            // Find image overlap
            imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            //std::cout << "height = " << height << std::endl;
            //std::cout << "Width = " << width << std::endl;
            
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
            // Count number of image bands
            for(int i = 0; i < numIntDS; i++)
            {
                numIntBands += datasets[i]->GetRasterCount();
            }
            for(int i = numIntDS; i < numDS; i++)
            {
                numFloatBands += datasets[i]->GetRasterCount();
            }
            
            //std::cout << "Number of Int Bands: " << numIntBands << std::endl;
            //std::cout << "Number of Float Bands: " << numFloatBands << std::endl;
            
            // Get Image Input Bands
            bandIntOffsets = new int*[numIntBands];
            inputRasterIntBands = new GDALRasterBand*[numIntBands];
            
            bandFloatOffsets = new int*[numFloatBands];
            inputRasterFloatBands = new GDALRasterBand*[numFloatBands];
            
            int counter = 0;
            for(int i = 0; i < numIntDS; i++)
            {
                for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
                {
                    inputRasterIntBands[counter] = datasets[i]->GetRasterBand(j+1);
                    bandIntOffsets[counter] = new int[2];
                    bandIntOffsets[counter][0] = dsOffsets[i][0];
                    bandIntOffsets[counter][1] = dsOffsets[i][1];
                    //std::cout << counter << ") dataset " << i << " (int) band " << j << " offset [" << bandIntOffsets[counter][0] << "," << bandIntOffsets[counter][1] << "]\n";
                    counter++;
                }
            }
            
            counter = 0;
            for(int i = numIntDS; i < numDS; i++)
            {
                for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
                {
                    inputRasterFloatBands[counter] = datasets[i]->GetRasterBand(j+1);
                    bandFloatOffsets[counter] = new int[2];
                    bandFloatOffsets[counter][0] = dsOffsets[i][0];
                    bandFloatOffsets[counter][1] = dsOffsets[i][1];
                    //std::cout << counter << ") dataset " << i << " (float) band " << j << " offset [" << bandFloatOffsets[counter][0] << "," << bandFloatOffsets[counter][1] << "]\n";
                    counter++;
                }
            }
            
            // Allocate memory
            inputIntData = new unsigned int*[numIntBands];
            for(int i = 0; i < numIntBands; i++)
            {
                inputIntData[i] = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
            }
            inDataIntColumn = new long[numIntBands];
            
            inputFloatData = new float*[numFloatBands];
            for(int i = 0; i < numFloatBands; i++)
            {
                inputFloatData[i] = (float *) CPLMalloc(sizeof(float)*width*yBlockSize);
            }
            inDataFloatColumn = new float[numFloatBands];
            
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            int feedback = height/10;
            int feedbackCounter = 0;
            std::cout << "Started " << std::flush;
            // Loop images to process data
            for(int i = 0; i < nYBlocks; i++)
            {
                //std::cout << i << " of " << nYBlocks << std::endl;
                
                for(int n = 0; n < numIntBands; n++)
                {
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * i);
                    inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, yBlockSize, inputIntData[n], width, yBlockSize, GDT_UInt32, 0, 0);
                }
                
                for(int n = 0; n < numFloatBands; n++)
                {
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * i);
                    inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, yBlockSize, inputFloatData[n], width, yBlockSize, GDT_Float32, 0, 0);
                }
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    xPxl = 0;
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        extent.init(xPxl, xPxl, yPxl, yPxl);
                        //std::cout << "[" << xPxl << "," << yPxl << "]\n";
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands, extent);
                        
                        xPxl += 1;
                    }
                    yPxl += 1;
                }
            }
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numIntBands; n++)
                {
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * nYBlocks);
                    inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, remainRows, inputIntData[n], width, remainRows, GDT_UInt32, 0, 0);
                }
                
                
                for(int n = 0; n < numFloatBands; n++)
                {
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * nYBlocks);
                    inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, remainRows, inputFloatData[n], width, remainRows, GDT_Float32, 0, 0);
                }
                
                for(int m = 0; m < remainRows; ++m)
                {
                    if((feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    xPxl = 0;
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        extent.init(xPxl, xPxl, yPxl, yPxl);
                        //std::cout << "[" << xPxl << "," << yPxl << "]\n";
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands, extent);
                        xPxl += 1;
                    }
                    yPxl += 1;
                }
            }
            std::cout << " Complete.\n";
        }
        catch(RSGISImageCalcException& e)
        {
            if(gdalTranslation != NULL)
            {
                delete[] gdalTranslation;
            }
            
            if(dsOffsets != NULL)
            {
                for(int i = 0; i < numDS; i++)
                {
                    if(dsOffsets[i] != NULL)
                    {
                        delete[] dsOffsets[i];
                    }
                }
                delete[] dsOffsets;
            }
            
            if(bandIntOffsets != NULL)
            {
                for(int i = 0; i < numIntBands; i++)
                {
                    if(bandIntOffsets[i] != NULL)
                    {
                        delete[] bandIntOffsets[i];
                    }
                }
                delete[] bandIntOffsets;
            }
            
            if(bandFloatOffsets != NULL)
            {
                for(int i = 0; i < numFloatBands; i++)
                {
                    if(bandFloatOffsets[i] != NULL)
                    {
                        delete[] bandFloatOffsets[i];
                    }
                }
                delete[] bandFloatOffsets;
            }
            
            if(inputIntData != NULL)
            {
                for(int i = 0; i < numIntBands; i++)
                {
                    if(inputIntData[i] != NULL)
                    {
                        CPLFree(inputIntData[i]);
                    }
                }
                delete[] inputIntData;
            }
            
            if(inputFloatData != NULL)
            {
                for(int i = 0; i < numFloatBands; i++)
                {
                    if(inputFloatData[i] != NULL)
                    {
                        delete[] inputFloatData[i];
                    }
                }
                delete[] inputFloatData;
            }
            
            if(inDataIntColumn != NULL)
            {
                delete[] inDataIntColumn;
            }
            
            if(inDataFloatColumn != NULL)
            {
                delete[] inDataFloatColumn;
            }
            
            if(inputRasterIntBands != NULL)
            {
                delete[] inputRasterIntBands;
            }
            
            if(inputRasterFloatBands != NULL)
            {
                delete[] inputRasterFloatBands;
            }
            throw e;
        }
        catch(RSGISImageBandException& e)
        {
            if(gdalTranslation != NULL)
            {
                delete[] gdalTranslation;
            }
            
            if(dsOffsets != NULL)
            {
                for(int i = 0; i < numDS; i++)
                {
                    if(dsOffsets[i] != NULL)
                    {
                        delete[] dsOffsets[i];
                    }
                }
                delete[] dsOffsets;
            }
            
            if(bandIntOffsets != NULL)
            {
                for(int i = 0; i < numIntBands; i++)
                {
                    if(bandIntOffsets[i] != NULL)
                    {
                        delete[] bandIntOffsets[i];
                    }
                }
                delete[] bandIntOffsets;
            }
            
            if(bandFloatOffsets != NULL)
            {
                for(int i = 0; i < numFloatBands; i++)
                {
                    if(bandFloatOffsets[i] != NULL)
                    {
                        delete[] bandFloatOffsets[i];
                    }
                }
                delete[] bandFloatOffsets;
            }
            
            if(inputIntData != NULL)
            {
                for(int i = 0; i < numIntBands; i++)
                {
                    if(inputIntData[i] != NULL)
                    {
                        CPLFree(inputIntData[i]);
                    }
                }
                delete[] inputIntData;
            }
            
            if(inputFloatData != NULL)
            {
                for(int i = 0; i < numFloatBands; i++)
                {
                    if(inputFloatData[i] != NULL)
                    {
                        delete[] inputFloatData[i];
                    }
                }
                delete[] inputFloatData;
            }
            
            if(inDataIntColumn != NULL)
            {
                delete[] inDataIntColumn;
            }
            
            if(inDataFloatColumn != NULL)
            {
                delete[] inDataFloatColumn;
            }
            
            if(inputRasterIntBands != NULL)
            {
                delete[] inputRasterIntBands;
            }
            
            if(inputRasterFloatBands != NULL)
            {
                delete[] inputRasterFloatBands;
            }
            throw e;
        }
        
        if(gdalTranslation != NULL)
        {
            delete[] gdalTranslation;
        }
        
        if(dsOffsets != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                if(dsOffsets[i] != NULL)
                {
                    delete[] dsOffsets[i];
                }
            }
            delete[] dsOffsets;
        }
        
        if(bandIntOffsets != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(bandIntOffsets[i] != NULL)
                {
                    delete[] bandIntOffsets[i];
                }
            }
            delete[] bandIntOffsets;
        }
        
        if(bandFloatOffsets != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(bandFloatOffsets[i] != NULL)
                {
                    delete[] bandFloatOffsets[i];
                }
            }
            delete[] bandFloatOffsets;
        }
        
        if(inputIntData != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(inputIntData[i] != NULL)
                {
                    CPLFree(inputIntData[i]);
                }
            }
            delete[] inputIntData;
        }
        
        if(inputFloatData != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(inputFloatData[i] != NULL)
                {
                    delete[] inputFloatData[i];
                }
            }
            delete[] inputFloatData;
        }
        
        if(inDataIntColumn != NULL)
        {
            delete[] inDataIntColumn;
        }
        
        if(inDataFloatColumn != NULL)
        {
            delete[] inDataFloatColumn;
        }
        
        if(inputRasterIntBands != NULL)
        {
            delete[] inputRasterIntBands;
        }
        
        if(inputRasterFloatBands != NULL)
        {
            delete[] inputRasterFloatBands;
        }
    }
    
    void RSGISCalcImage::calcImageExtent(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, bool quiet) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputData = NULL;
		float *inDataColumn = NULL;
		
		GDALRasterBand **inputRasterBands = NULL;
        
        geos::geom::Envelope extent;
		double pxlTLX = 0;
		double pxlTLY = 0;
		double pxlWidth = 0;
		double pxlHeight = 0;
		
		try
		{
			// Find image overlap
            if(env == NULL)
            {
                imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation);
            }
            else
            {
                imgUtils.getImageOverlapCut2Env(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env);
            }
            
            //std::cout << "Width = " << width << std::endl;
            //std::cout << "Height = " << height << std::endl;
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			pxlTLX = gdalTranslation[0];
			pxlTLY = gdalTranslation[3];
			pxlWidth = gdalTranslation[1];
			pxlHeight = gdalTranslation[5];
            
            if(pxlHeight < 0)
            {
                pxlHeight *= (-1);
            }
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumn = new float[numInBands];
			
			int feedback = height/10;
			int feedbackCounter = 0;
            if(!quiet)
            {
                std::cout << "Started " << std::flush;
            }
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
                //std::cout << i << " of " << height << std::endl;
				
				if((!quiet) && (feedback != 0) && ((i % feedback) == 0))
				{
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], (bandOffsets[n][1]+i), width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
				}

                for(int j = 0; j < width; j++)
				{
                    for(int n = 0; n < numInBands; n++)
					{
						inDataColumn[n] = inputData[n][j];
					}
					
					extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
					
					this->calc->calcImageValue(inDataColumn, numInBands, extent);
					
					pxlTLX += pxlWidth;
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
			}
            if(!quiet)
            {
                std::cout << " Complete.\n";
            }
		}
		catch(RSGISImageCalcException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			}
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
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
    
    void RSGISCalcImage::calcImageExtent(GDALDataset **datasets, int numIntDS, int numFloatDS) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
		RSGISImageUtils imgUtils;
        int numDS = numIntDS + numFloatDS;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandIntOffsets = NULL;
        int **bandFloatOffsets = NULL;
		int height = 0;
		int width = 0;
		int numIntBands = 0;
        int numFloatBands = 0;
		
		float **inputFloatData = NULL;
		float *inDataFloatColumn = NULL;
        unsigned int **inputIntData = NULL;
		long *inDataIntColumn = NULL;
        int xBlockSize = 0;
        int yBlockSize = 0;
		
		GDALRasterBand **inputRasterFloatBands = NULL;
        GDALRasterBand **inputRasterIntBands = NULL;
        
        geos::geom::Envelope extent;
        double imgTLX = 0;
        double imgTLY = 0;
        double pxlTLX = 0;
        double pxlTLY = 0;
        double pxlWidth = 0;
        double pxlHeight = 0;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            
            imgTLX = gdalTranslation[0];
			imgTLY = gdalTranslation[3];
			pxlWidth = gdalTranslation[1];
			pxlHeight = gdalTranslation[5];
            
            if(pxlHeight < 0)
            {
                pxlHeight *= (-1);
            }
            
            //std::cout << "height = " << height << std::endl;
            //std::cout << "Width = " << width << std::endl;
            
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
			// Count number of image bands
			for(int i = 0; i < numIntDS; i++)
			{
				numIntBands += datasets[i]->GetRasterCount();
			}
            for(int i = numIntDS; i < numDS; i++)
			{
				numFloatBands += datasets[i]->GetRasterCount();
			}
            
            //std::cout << "Number of Int Bands: " << numIntBands << std::endl;
            //std::cout << "Number of Float Bands: " << numFloatBands << std::endl;
			
			// Get Image Input Bands
			bandIntOffsets = new int*[numIntBands];
			inputRasterIntBands = new GDALRasterBand*[numIntBands];
            
            bandFloatOffsets = new int*[numFloatBands];
			inputRasterFloatBands = new GDALRasterBand*[numFloatBands];
            
			int counter = 0;
			for(int i = 0; i < numIntDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterIntBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandIntOffsets[counter] = new int[2];
					bandIntOffsets[counter][0] = dsOffsets[i][0];
					bandIntOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " (int) band " << j << " offset [" << bandIntOffsets[counter][0] << "," << bandIntOffsets[counter][1] << "]\n";
					counter++;
				}
			}
            
            counter = 0;
            for(int i = numIntDS; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterFloatBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandFloatOffsets[counter] = new int[2];
					bandFloatOffsets[counter][0] = dsOffsets[i][0];
					bandFloatOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " (float) band " << j << " offset [" << bandFloatOffsets[counter][0] << "," << bandFloatOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			// Allocate memory
			inputIntData = new unsigned int*[numIntBands];
			for(int i = 0; i < numIntBands; i++)
			{
				inputIntData[i] = (unsigned int *) CPLMalloc(sizeof(unsigned int)*width*yBlockSize);
			}
			inDataIntColumn = new long[numIntBands];
            
            inputFloatData = new float*[numFloatBands];
			for(int i = 0; i < numFloatBands; i++)
			{
				inputFloatData[i] = (float *) CPLMalloc(sizeof(float)*width*yBlockSize);
			}
			inDataFloatColumn = new float[numFloatBands];
            
            
            int nYBlocks = height / yBlockSize;
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            pxlTLY = imgTLY;
            
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
				//std::cout << i << " of " << nYBlocks << std::endl;
                
				for(int n = 0; n < numIntBands; n++)
				{
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * i);
					inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, yBlockSize, inputIntData[n], width, yBlockSize, GDT_UInt32, 0, 0);
				}
                
                for(int n = 0; n < numFloatBands; n++)
				{
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * i);
					inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, yBlockSize, inputFloatData[n], width, yBlockSize, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    pxlTLX = imgTLX;
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands, extent);
                        
                        pxlTLX += pxlWidth;
                    }
                    pxlTLY -= pxlHeight;
                }
			}
            
            if(remainRows > 0)
            {
                for(int n = 0; n < numIntBands; n++)
				{
                    rowOffset = bandIntOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterIntBands[n]->RasterIO(GF_Read, bandIntOffsets[n][0], rowOffset, width, remainRows, inputIntData[n], width, remainRows, GDT_UInt32, 0, 0);
				}
                
                
                for(int n = 0; n < numFloatBands; n++)
				{
                    rowOffset = bandFloatOffsets[n][1] + (yBlockSize * nYBlocks);
					inputRasterFloatBands[n]->RasterIO(GF_Read, bandFloatOffsets[n][0], rowOffset, width, remainRows, inputFloatData[n], width, remainRows, GDT_Float32, 0, 0);
				}
                
                for(int m = 0; m < remainRows; ++m)
                {
                    if((feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    pxlTLX = imgTLX;
                    for(int j = 0; j < width; j++)
                    {
                        for(int n = 0; n < numIntBands; n++)
                        {
                            inDataIntColumn[n] = inputIntData[n][(m*width)+j];
                        }
                        
                        for(int n = 0; n < numFloatBands; n++)
                        {
                            inDataFloatColumn[n] = inputFloatData[n][(m*width)+j];
                        }
                        
                        extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
                        
                        this->calc->calcImageValue(inDataIntColumn, numIntBands, inDataFloatColumn, numFloatBands, extent);
                        pxlTLX += pxlWidth;
                    }
                    pxlTLY -= pxlHeight;
                }
            }
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandIntOffsets != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(bandIntOffsets[i] != NULL)
					{
						delete[] bandIntOffsets[i];
					}
				}
				delete[] bandIntOffsets;
			}
            
            if(bandFloatOffsets != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(bandFloatOffsets[i] != NULL)
					{
						delete[] bandFloatOffsets[i];
					}
				}
				delete[] bandFloatOffsets;
			}
			
			if(inputIntData != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(inputIntData[i] != NULL)
					{
						CPLFree(inputIntData[i]);
					}
				}
				delete[] inputIntData;
			}
            
            if(inputFloatData != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(inputFloatData[i] != NULL)
					{
						delete[] inputFloatData[i];
					}
				}
				delete[] inputFloatData;
			}
            
			if(inDataIntColumn != NULL)
			{
				delete[] inDataIntColumn;
			}
            
            if(inDataFloatColumn != NULL)
			{
				delete[] inDataFloatColumn;
			}
            
			if(inputRasterIntBands != NULL)
			{
				delete[] inputRasterIntBands;
			}
            
            if(inputRasterFloatBands != NULL)
			{
				delete[] inputRasterFloatBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				}
				delete[] dsOffsets;
			}
			
			if(bandIntOffsets != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(bandIntOffsets[i] != NULL)
					{
						delete[] bandIntOffsets[i];
					}
				}
				delete[] bandIntOffsets;
			}
            
            if(bandFloatOffsets != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(bandFloatOffsets[i] != NULL)
					{
						delete[] bandFloatOffsets[i];
					}
				}
				delete[] bandFloatOffsets;
			}
			
			if(inputIntData != NULL)
			{
				for(int i = 0; i < numIntBands; i++)
				{
					if(inputIntData[i] != NULL)
					{
						CPLFree(inputIntData[i]);
					}
				}
				delete[] inputIntData;
			}
            
            if(inputFloatData != NULL)
			{
				for(int i = 0; i < numFloatBands; i++)
				{
					if(inputFloatData[i] != NULL)
					{
						delete[] inputFloatData[i];
					}
				}
				delete[] inputFloatData;
			}
            
			if(inDataIntColumn != NULL)
			{
				delete[] inDataIntColumn;
			}
            
            if(inDataFloatColumn != NULL)
			{
				delete[] inDataFloatColumn;
			}
            
			if(inputRasterIntBands != NULL)
			{
				delete[] inputRasterIntBands;
			}
            
            if(inputRasterFloatBands != NULL)
			{
				delete[] inputRasterFloatBands;
			}
			throw e;
		}
        
		if(gdalTranslation != NULL)
        {
            delete[] gdalTranslation;
        }
        
        if(dsOffsets != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                if(dsOffsets[i] != NULL)
                {
                    delete[] dsOffsets[i];
                }
            }
            delete[] dsOffsets;
        }
        
        if(bandIntOffsets != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(bandIntOffsets[i] != NULL)
                {
                    delete[] bandIntOffsets[i];
                }
            }
            delete[] bandIntOffsets;
        }
        
        if(bandFloatOffsets != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(bandFloatOffsets[i] != NULL)
                {
                    delete[] bandFloatOffsets[i];
                }
            }
            delete[] bandFloatOffsets;
        }
        
        if(inputIntData != NULL)
        {
            for(int i = 0; i < numIntBands; i++)
            {
                if(inputIntData[i] != NULL)
                {
                    CPLFree(inputIntData[i]);
                }
            }
            delete[] inputIntData;
        }
        
        if(inputFloatData != NULL)
        {
            for(int i = 0; i < numFloatBands; i++)
            {
                if(inputFloatData[i] != NULL)
                {
                    delete[] inputFloatData[i];
                }
            }
            delete[] inputFloatData;
        }
        
        if(inDataIntColumn != NULL)
        {
            delete[] inDataIntColumn;
        }
        
        if(inDataFloatColumn != NULL)
        {
            delete[] inDataFloatColumn;
        }
        
        if(inputRasterIntBands != NULL)
        {
            delete[] inputRasterIntBands;
        }
        
        if(inputRasterFloatBands != NULL)
        {
            delete[] inputRasterFloatBands;
        }
    }
	
	void RSGISCalcImage::calcImageExtent(GDALDataset **datasets, int numDS, std::string outputImage, std::string gdalFormat, GDALDataType gdalDataType) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputData = NULL;
		double **outputData = NULL;
		float *inDataColumn = NULL;
		double *outDataColumn = NULL;
		
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
		geos::geom::Envelope extent;
		double pxlTLX = 0;
		double pxlTLY = 0;
		double pxlWidth = 0;
		double pxlHeight = 0;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation);
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw RSGISImageBandException(gdalFormat + std::string(" driver does not exists.."));
			}
			//std::cout << "New image width = " << width << " height = " << height << std::endl;

			outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, this->numOutBands, gdalDataType, NULL);
			
			if(outputImageDS == NULL)
			{
				throw RSGISImageBandException("Output image could not be created. Check filepath.");
			}
			outputImageDS->SetGeoTransform(gdalTranslation);
			if(useImageProj)
			{
				outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
			}
			else
			{
				outputImageDS->SetProjection(proj.c_str());
			}
			
			pxlTLX = gdalTranslation[0];
			pxlTLY = gdalTranslation[3];
			pxlWidth = gdalTranslation[1];
			pxlHeight = gdalTranslation[5];
            
            if(pxlHeight < 0)
            {
                pxlHeight *= (-1);
            }
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
			}
			
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumn = new float[numInBands];
			
			outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*width);
			}
			outDataColumn = new double[this->numOutBands];
			
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				//std::cout << i << " of " << height << std::endl;
				
				if((feedback != 0) && ((i % feedback) == 0))
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], (bandOffsets[n][1]+i), width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
				}
				
				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumn[n] = inputData[n][j];
					}
					
					extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
					
					this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn, extent);
					
					pxlTLX += pxlWidth;
					
					for(int n = 0; n < this->numOutBands; n++)
					{
						outputData[n][j] = outDataColumn[n];
					}
					
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
				
				for(int n = 0; n < this->numOutBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Write, 0, i, width, 1, outputData[n], width, 1, GDT_Float64, 0, 0);
				}
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISImageCalcException& e)
		{			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		
		GDALClose(outputImageDS);
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
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
		
		if(outputData != NULL)
		{
			for(int i = 0; i < this->numOutBands; i++)
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
		
		if(inputRasterBands != NULL)
		{
			delete[] inputRasterBands;
		}
		
		if(outputRasterBands != NULL)
		{
			delete[] outputRasterBands;
		}
	}
	
    void RSGISCalcImage::calcImageWindowData(GDALDataset **datasets, int numDS, int windowSize) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
        int xBlockSize = 0;
        int yBlockSize = 0;
        size_t numPxlsInBlock = 0;
		
        float **inputDataUpper = NULL;
		float **inputDataMain = NULL;
        float **inputDataLower = NULL;
		float ***inDataBlock = NULL;
		double *outDataColumn = NULL;
		
		GDALRasterBand **inputRasterBands = NULL;
		
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
			
            //std::cout << "Window Size: " << windowSize << std::endl;
            //std::cout << "Window Mid: " << windowMid << std::endl;
            
			// Find image overlap
            imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
            
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
            int numOfLines = yBlockSize;
            if(yBlockSize < windowSize)
            {
                numOfLines = ceil(((float)windowSize)/((float)yBlockSize))*yBlockSize;
            }
            //std::cout << "Number of Lines: " << numOfLines << std::endl;
            
            //std::cout << "numInBands = " << numInBands << std::endl;
            
			// Allocate memory
            numPxlsInBlock = width*numOfLines;
			inputDataUpper = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataUpper[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataUpper[i][k] = 0;
                }
			}
            
            inputDataMain = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataMain[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataMain[i][k] = 0;
                }
			}
            
            inputDataLower = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataLower[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataLower[i][k] = 0;
                }
			}
			
			inDataBlock = new float**[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inDataBlock[i] = new float*[windowSize];
				for(int j = 0; j < windowSize; j++)
				{
					inDataBlock[i][j] = new float[windowSize];
				}
			}
			
			outDataColumn = new double[this->numOutBands];
            
            //std::cout << "height: " << height << std::endl;
            int nYBlocks = floor(((double)height) / ((double)numOfLines));
            //std::cout << "nYBlocks: " << nYBlocks << std::endl;
            int remainRows = height - (nYBlocks * numOfLines);
            //std::cout << "remainRows: " << remainRows << std::endl;
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
                    //std::cout << "i: " << i << std::endl;
                    if(i == 0)
                    {
                        // Set Upper Block with Zeros.
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = 0;
                            }
                        }
                        
                        // Read Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            rowOffset = bandOffsets[n][1] + (numOfLines * i);
                            //std::cout << "rowOffset: " << rowOffset << std::endl;
                            //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                            //std::cout << "width: " << width << std::endl;
                            //std::cout << "numOfLines: " << numOfLines << std::endl;
                            
                            inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataMain[n], width, numOfLines, GDT_Float32, 0, 0);
                        }
                        // Read Lower Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            if(nYBlocks == 1)
                            {
                                if(remainRows > 0)
                                {
                                    rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                    //std::cout << "rowOffset: " << rowOffset << std::endl;
                                    //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                                    //std::cout << "width: " << width << std::endl;
                                    //std::cout << "remainRows = " << remainRows << std::endl;
                                    inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
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
                                rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                //std::cout << "rowOffset: " << rowOffset << std::endl;
                                //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                                //std::cout << "width: " << width << std::endl;
                                //std::cout << "remainRows = " << remainRows << std::endl;
                                inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                            }
                        }
                    }
                    else if(i == (nYBlocks-1))
                    {
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = inputDataMain[n][k];
                            }
                        }
                        
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataMain[n][k] = inputDataLower[n][k];
                            }
                        }
                        
                        // Set Lower Block with Zeros.
                        for(int n = 0; n < numInBands; n++)
                        {
                            if(remainRows > 0)
                            {
                                rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
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
                    }
                    else
                    {
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = inputDataMain[n][k];
                            }
                        }
                        
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataMain[n][k] = inputDataLower[n][k];
                            }
                        }
                        
                        // Read Lower Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                            //std::cout << "rowOffset: " << rowOffset << std::endl;
                            //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                            //std::cout << "width: " << width << std::endl;
                            //std::cout << "numOfLines: " << numOfLines << std::endl;
                            
                            inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                        }
                    }
                    
                    for(int m = 0; m < numOfLines; ++m)
                    {
                        line = (i*numOfLines)+m;
                        //std::cout << "line = " << line << std::endl;
                        if((feedback != 0) && (line % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        cLinePxl = m*width;
                        //std::cout << "cLine: " << cLinePxl << std::endl;
                        
                        for(int j = 0; j < width; j++)
                        {
                            cPxl = cLinePxl+j;
                            if(m < windowMid)
                            {
                                //std::cout << "Need Upper\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " (numPxlsInBlock+(cPxl+dLinePxls)) = " << (numPxlsInBlock+(cPxl+dLinePxls)) << std::endl;
                                    
                                    if((cPxl + dLinePxls) < 0)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Need Lower\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << "j = " << j << " y = " << y << ": " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " ((cPxl+dLinePxls)-numPxlsInBlock) = " << ((cPxl+dLinePxls)-numPxlsInBlock) << std::endl;
                                    
                                    if((cPxl + dLinePxls) >= numPxlsInBlock)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Within block\n";
                                
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << std::endl;
                                    
                                    for(int x = 0; x < windowSize; x++)
                                    {
                                        dWinX = x-windowMid;
                                        
                                        if((j+dWinX) < 0)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else if((j+dWinX) >= width)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                            }
                                        }
                                    }
                                }
                            }
                            
                            this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn);
                        }
                        
                    }
                }
                
                if(remainRows > 0)
                {
                    // Shift Lower Block to Main Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataUpper[n][k] = inputDataMain[n][k];
                        }
                    }
                    
                    // Shift Lower Block to Main Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataMain[n][k] = inputDataLower[n][k];
                        }
                    }
                    
                    // Read Lower Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataLower[n][k] = 0;
                        }
                    }
                    
                    for(int m = 0; m < remainRows; ++m)
                    {
                        line = (nYBlocks*numOfLines)+m;
                        //std::cout << "line = " << line << std::endl;
                        if((feedback != 0) && (line % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        cLinePxl = m*width;
                        //std::cout << "cLine: " << cLinePxl << std::endl;
                        
                        for(int j = 0; j < width; j++)
                        {
                            cPxl = cLinePxl+j;
                            if(m < windowMid)
                            {
                                //std::cout << "Need Upper\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " (numPxlsInBlock+(cPxl+dLinePxls)) = " << (numPxlsInBlock+(cPxl+dLinePxls)) << std::endl;
                                    
                                    if((cPxl + dLinePxls) < 0)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Need Lower\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << "j = " << j << " y = " << y << ": " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " ((cPxl+dLinePxls)-numPxlsInBlock) = " << ((cPxl+dLinePxls)-numPxlsInBlock) << std::endl;
                                    
                                    if((cPxl + dLinePxls) >= numPxlsInBlock)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Within block\n";
                                
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << std::endl;
                                    
                                    for(int x = 0; x < windowSize; x++)
                                    {
                                        dWinX = x-windowMid;
                                        
                                        if((j+dWinX) < 0)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else if((j+dWinX) >= width)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                            }
                                        }
                                    }
                                }
                            }
                            
                            this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn);
                        }
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
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete dsOffsets[i];
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete bandOffsets[i];
				}
				delete[] bandOffsets;
			}
			
			if(inputDataUpper != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataUpper[i];
				}
				delete[] inputDataUpper;
			}
            
            if(inputDataMain != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataMain[i];
				}
				delete[] inputDataMain;
			}
            
            if(inputDataLower != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataLower[i];
				}
				delete[] inputDataLower;
			}
			
			if(inDataBlock != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					for(int j = 0; j < windowSize; j++)
					{
						delete[] inDataBlock[i][j];
					}
					delete[] inDataBlock[i];
				}
				delete[] inDataBlock;
			}
			
			if(outDataColumn != NULL)
			{
				delete[] outDataColumn;
			}
            
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete dsOffsets[i];
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete bandOffsets[i];
				}
				delete[] bandOffsets;
			}
			
			if(inputDataUpper != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataUpper[i];
				}
				delete[] inputDataUpper;
			}
            
            if(inputDataMain != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataMain[i];
				}
				delete[] inputDataMain;
			}
            
            if(inputDataLower != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataLower[i];
				}
				delete[] inputDataLower;
			}
			
			if(inDataBlock != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					for(int j = 0; j < windowSize; j++)
					{
						delete[] inDataBlock[i][j];
					}
					delete[] inDataBlock[i];
				}
				delete[] inDataBlock;
			}
			
			if(outDataColumn != NULL)
			{
				delete[] outDataColumn;
			}
			
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete dsOffsets[i];
			}
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				delete bandOffsets[i];
			}
			delete[] bandOffsets;
		}
		
		if(inputDataUpper != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataUpper[i];
            }
            delete[] inputDataUpper;
        }
        
        if(inputDataMain != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataMain[i];
            }
            delete[] inputDataMain;
        }
        
        if(inputDataLower != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataLower[i];
            }
            delete[] inputDataLower;
        }
		
		if(inDataBlock != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				for(int j = 0; j < windowSize; j++)
				{
					delete[] inDataBlock[i][j];
				}
				delete[] inDataBlock[i];
			}
			delete[] inDataBlock;
		}
		
		if(outDataColumn != NULL)
		{
			delete[] outDataColumn;
		}
	}
    
    void RSGISCalcImage::calcImageWindowData(GDALDataset **datasets, int numDS, std::string outputImage, int windowSize, std::string gdalFormat, GDALDataType gdalDataType) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
        int xBlockSize = 0;
        int yBlockSize = 0;
        size_t numPxlsInBlock = 0;
		
        float **inputDataUpper = NULL;
		float **inputDataMain = NULL;
        float **inputDataLower = NULL;
		double **outputData = NULL;
		float ***inDataBlock = NULL;
		double *outDataColumn = NULL;
		
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
		
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
			
            //std::cout << "Window Size: " << windowSize << std::endl;
            //std::cout << "Window Mid: " << windowMid << std::endl;
            
			// Find image overlap
            imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw RSGISImageBandException("Driver does not exists..");
			}
			//std::cout << "New image width = " << width << " height = " << height << std::endl;
            
			outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, this->numOutBands, gdalDataType, NULL);
			
			if(outputImageDS == NULL)
			{
				throw RSGISImageBandException("Output image could not be created. Check filepath.");
			}
			outputImageDS->SetGeoTransform(gdalTranslation);
			if(useImageProj)
			{
				outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
			}
			else
			{
				outputImageDS->SetProjection(proj.c_str());
			}
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
			}
            
            int outXBlockSize = 0;
            int outYBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&outXBlockSize, &outYBlockSize);
            
            if(outYBlockSize > yBlockSize)
            {
                yBlockSize = outYBlockSize;
            }
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
			            
            int numOfLines = yBlockSize;
            if(yBlockSize < windowSize)
            {
                numOfLines = ceil(((float)windowSize)/((float)yBlockSize))*yBlockSize;
            }
            //std::cout << "Number of Lines: " << numOfLines << std::endl;
            
            //std::cout << "numInBands = " << numInBands << std::endl;
            
			// Allocate memory
            numPxlsInBlock = width*numOfLines;
			inputDataUpper = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataUpper[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataUpper[i][k] = 0;
                }
			}
            
            inputDataMain = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataMain[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataMain[i][k] = 0;
                }
			}
            
            inputDataLower = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataLower[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataLower[i][k] = 0;
                }
			}
			
			inDataBlock = new float**[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inDataBlock[i] = new float*[windowSize];
				for(int j = 0; j < windowSize; j++)
				{
					inDataBlock[i][j] = new float[windowSize];
				}
			}
			
			outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*numPxlsInBlock);
			}
			outDataColumn = new double[this->numOutBands];
			

            //std::cout << "height: " << height << std::endl;
            int nYBlocks = floor(((double)height) / ((double)numOfLines));
            //std::cout << "nYBlocks: " << nYBlocks << std::endl;
            int remainRows = height - (nYBlocks * numOfLines);
            //std::cout << "remainRows: " << remainRows << std::endl;
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
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = 0;
                            }
                        }
                        
                        // Read Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            rowOffset = bandOffsets[n][1] + (numOfLines * i);
                            //std::cout << "rowOffset: " << rowOffset << std::endl;
                            //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                            //std::cout << "width: " << width << std::endl;
                            //std::cout << "numOfLines: " << numOfLines << std::endl;
                            
                            inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataMain[n], width, numOfLines, GDT_Float32, 0, 0);
                        }
                        // Read Lower Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            if(nYBlocks == 1)
                            {
                                if(remainRows > 0)
                                {
                                    rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                    //std::cout << "rowOffset: " << rowOffset << std::endl;
                                    //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                                    //std::cout << "width: " << width << std::endl;
                                    //std::cout << "remainRows = " << remainRows << std::endl;
                                    inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
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
                                rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                //std::cout << "rowOffset: " << rowOffset << std::endl;
                                //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                                //std::cout << "width: " << width << std::endl;
                                //std::cout << "remainRows = " << remainRows << std::endl;
                                inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                            }
                        }
                    }
                    else if(i == (nYBlocks-1))
                    {
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = inputDataMain[n][k];
                            }
                        }
                        
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataMain[n][k] = inputDataLower[n][k];
                            }
                        }
                        
                        // Set Lower Block with Zeros.
                        for(int n = 0; n < numInBands; n++)
                        {
                            if(remainRows > 0)
                            {
                                rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
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
                    }
                    else
                    {
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = inputDataMain[n][k];
                            }
                        }
                        
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataMain[n][k] = inputDataLower[n][k];
                            }
                        }
                        
                        // Read Lower Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                            //std::cout << "rowOffset: " << rowOffset << std::endl;
                            //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                            //std::cout << "width: " << width << std::endl;
                            //std::cout << "numOfLines: " << numOfLines << std::endl;
                            
                            inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                        }
                    }
                    
                    for(int m = 0; m < numOfLines; ++m)
                    {
                        line = (i*numOfLines)+m;
                        //std::cout << "line = " << line << std::endl;
                        if((feedback != 0) && (line % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        cLinePxl = m*width;
                        //std::cout << "cLine: " << cLinePxl << std::endl;
                        
                        for(int j = 0; j < width; j++)
                        {
                            cPxl = cLinePxl+j;
                            if(m < windowMid)
                            {
                                //std::cout << "Need Upper\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " (numPxlsInBlock+(cPxl+dLinePxls)) = " << (numPxlsInBlock+(cPxl+dLinePxls)) << std::endl;
                                    
                                    if((cPxl + dLinePxls) < 0)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Need Lower\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << "j = " << j << " y = " << y << ": " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " ((cPxl+dLinePxls)-numPxlsInBlock) = " << ((cPxl+dLinePxls)-numPxlsInBlock) << std::endl;
                                    
                                    if((cPxl + dLinePxls) >= numPxlsInBlock)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Within block\n";
                                
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << std::endl;
                                    
                                    for(int x = 0; x < windowSize; x++)
                                    {
                                        dWinX = x-windowMid;
                                        
                                        if((j+dWinX) < 0)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else if((j+dWinX) >= width)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                            }
                                        }
                                    }
                                }
                            }
                            
                            this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn);
                            
                            for(int n = 0; n < this->numOutBands; n++)
                            {
                                outputData[n][cPxl] = outDataColumn[n];
                            }
                        }
                        
                    }
                    
                    for(int n = 0; n < this->numOutBands; n++)
                    {
                        outputRasterBands[n]->RasterIO(GF_Write, 0, (numOfLines * i), width, numOfLines, outputData[n], width, numOfLines, GDT_Float64, 0, 0);
                    }
                }
                                
                if(remainRows > 0)
                {
                    // Shift Lower Block to Main Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataUpper[n][k] = inputDataMain[n][k];
                        }
                    }
                    
                    // Shift Lower Block to Main Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            //std::cout << inputDataLower[n][k] << " ";
                            inputDataMain[n][k] = inputDataLower[n][k];
                        }
                        //std::cout << std::endl;
                    }
                    
                    // Read Lower Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataLower[n][k] = 0;
                        }
                    }
                    
                    for(int m = 0; m < remainRows; ++m)
                    {
                        line = (nYBlocks*numOfLines)+m;
                        //std::cout << "line = " << line << std::endl;
                        if((feedback != 0) && (line % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        cLinePxl = m*width;
                        //std::cout << "cLine: " << cLinePxl << std::endl;
                        
                        for(int j = 0; j < width; j++)
                        {
                            cPxl = cLinePxl+j;
                            if(m < windowMid)
                            {
                                //std::cout << "Need Upper\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " (numPxlsInBlock+(cPxl+dLinePxls)) = " << (numPxlsInBlock+(cPxl+dLinePxls)) << std::endl;
                                    
                                    if((cPxl + dLinePxls) < 0)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Need Lower\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << "j = " << j << " y = " << y << ": " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " ((cPxl+dLinePxls)-numPxlsInBlock) = " << ((cPxl+dLinePxls)-numPxlsInBlock) << std::endl;
                                    
                                    if((cPxl + dLinePxls) >= numPxlsInBlock)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Within block\n";
                                
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << std::endl;
                                    
                                    for(int x = 0; x < windowSize; x++)
                                    {
                                        dWinX = x-windowMid;
                                        
                                        if((j+dWinX) < 0)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else if((j+dWinX) >= width)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                            }
                                        }
                                    }
                                }
                            }
                            
                            this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn);
                            
                            for(int n = 0; n < this->numOutBands; n++)
                            {                                
                                outputData[n][cPxl] = outDataColumn[n];
                            }
                        }
                    }
                    
                    for(int n = 0; n < this->numOutBands; n++)
                    {
                        outputRasterBands[n]->RasterIO(GF_Write, 0, (nYBlocks*numOfLines), width, remainRows, outputData[n], width, remainRows, GDT_Float64, 0, 0);
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
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete dsOffsets[i];
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete bandOffsets[i];
				}
				delete[] bandOffsets;
			}
			
			if(inputDataUpper != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataUpper[i];
				}
				delete[] inputDataUpper;
			}
            
            if(inputDataMain != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataMain[i];
				}
				delete[] inputDataMain;
			}
            
            if(inputDataLower != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataLower[i];
				}
				delete[] inputDataLower;
			}
			
			if(inDataBlock != NULL)
			{
				for(int i = 0; i < numInBands; i++)
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
				for(int i = 0; i < numOutBands; i++)
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
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete dsOffsets[i];
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete bandOffsets[i];
				}
				delete[] bandOffsets;
			}
			
			if(inputDataUpper != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataUpper[i];
				}
				delete[] inputDataUpper;
			}
            
            if(inputDataMain != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataMain[i];
				}
				delete[] inputDataMain;
			}
            
            if(inputDataLower != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataLower[i];
				}
				delete[] inputDataLower;
			}
			
			if(inDataBlock != NULL)
			{
				for(int i = 0; i < numInBands; i++)
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
				for(int i = 0; i < numOutBands; i++)
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
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete dsOffsets[i];
			}
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				delete bandOffsets[i];
			}
			delete[] bandOffsets;
		}
		
		if(inputDataUpper != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataUpper[i];
            }
            delete[] inputDataUpper;
        }
        
        if(inputDataMain != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataMain[i];
            }
            delete[] inputDataMain;
        }
        
        if(inputDataLower != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataLower[i];
            }
            delete[] inputDataLower;
        }
		
		if(inDataBlock != NULL)
		{
			for(int i = 0; i < numInBands; i++)
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
			for(int i = 0; i < numOutBands; i++)
			{
				delete outputData[i];
			}
			delete outputData;
		}
		
		if(outDataColumn != NULL)
		{
			delete[] outDataColumn;
		}
		
		GDALClose(outputImageDS);
	}
    
    void RSGISCalcImage::calcImageWindowData(GDALDataset **datasets, int numDS, GDALDataset *outputImageDS, int windowSize, bool passPxlXY) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		if(outputImageDS == NULL)
        {
            throw RSGISImageBandException("Output image is not valid.");
        }
        
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
        int xBlockSize = 0;
        int yBlockSize = 0;
        size_t numPxlsInBlock = 0;
		
        float **inputDataUpper = NULL;
		float **inputDataMain = NULL;
        float **inputDataLower = NULL;
		double **outputData = NULL;
		float ***inDataBlock = NULL;
		double *outDataColumn = NULL;
		
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		
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
			
            //std::cout << "Window Size: " << windowSize << std::endl;
            //std::cout << "Window Mid: " << windowMid << std::endl;
            
			// Find image overlap
            imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			if(outputImageDS->GetRasterXSize() != width)
            {
                throw RSGISImageCalcException("Inputted dataset is not of the required width.");
            }
            
            if(outputImageDS->GetRasterYSize() != height)
            {
                throw RSGISImageCalcException("Inputted dataset is not of the required height.");
            }
			
            if(outputImageDS->GetRasterCount() != numOutBands)
            {
                throw RSGISImageCalcException("Inputted dataset does not have the required number of image bands.");
            }
            
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
			}
            
            int outXBlockSize = 0;
            int outYBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&outXBlockSize, &outYBlockSize);
            
            if(outYBlockSize > yBlockSize)
            {
                yBlockSize = outYBlockSize;
            }
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
            int numOfLines = yBlockSize;
            if(yBlockSize < windowSize)
            {
                numOfLines = ceil(((float)windowSize)/((float)yBlockSize))*yBlockSize;
            }
            //std::cout << "Number of Lines: " << numOfLines << std::endl;
            
            //std::cout << "numInBands = " << numInBands << std::endl;
            
			// Allocate memory
            numPxlsInBlock = width*numOfLines;
			inputDataUpper = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataUpper[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataUpper[i][k] = 0;
                }
			}
            
            inputDataMain = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataMain[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataMain[i][k] = 0;
                }
			}
            
            inputDataLower = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
                inputDataLower[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataLower[i][k] = 0;
                }
			}
			
			inDataBlock = new float**[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inDataBlock[i] = new float*[windowSize];
				for(int j = 0; j < windowSize; j++)
				{
					inDataBlock[i][j] = new float[windowSize];
				}
			}
			
			outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*numPxlsInBlock);
			}
			outDataColumn = new double[this->numOutBands];
			
            
            //std::cout << "height: " << height << std::endl;
            int nYBlocks = floor(((double)height) / ((double)numOfLines));
            //std::cout << "nYBlocks: " << nYBlocks << std::endl;
            int remainRows = height - (nYBlocks * numOfLines);
            //std::cout << "remainRows: " << remainRows << std::endl;
            int rowOffset = 0;
            unsigned int line = 0;
            long cLinePxl = 0;
            long cPxl = 0;
            long dLinePxls = 0;
            int dWinX = 0;
            int dWinY = 0;
            long xPxl = 0;
            long yPxl = 0;
            geos::geom::Envelope pxlPos;
            
            int feedback = height/10.0;
			int feedbackCounter = 0;
			std::cout << "Started " << std::flush;
			
            if(nYBlocks > 0)
            {
                for(int i = 0; i < nYBlocks; i++)
                {
                    //std::cout << "i: " << i << std::endl;
                    if(i == 0)
                    {
                        // Set Upper Block with Zeros.
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = 0;
                            }
                        }
                        
                        // Read Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            rowOffset = bandOffsets[n][1] + (numOfLines * i);
                            //std::cout << "rowOffset: " << rowOffset << std::endl;
                            //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                            //std::cout << "width: " << width << std::endl;
                            //std::cout << "numOfLines: " << numOfLines << std::endl;
                            
                            inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataMain[n], width, numOfLines, GDT_Float32, 0, 0);
                        }
                        // Read Lower Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            if(nYBlocks == 1)
                            {
                                if(remainRows > 0)
                                {
                                    rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                    //std::cout << "rowOffset: " << rowOffset << std::endl;
                                    //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                                    //std::cout << "width: " << width << std::endl;
                                    //std::cout << "remainRows = " << remainRows << std::endl;
                                    inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
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
                                rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                //std::cout << "rowOffset: " << rowOffset << std::endl;
                                //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                                //std::cout << "width: " << width << std::endl;
                                //std::cout << "remainRows = " << remainRows << std::endl;
                                inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                            }
                        }
                    }
                    else if(i == (nYBlocks-1))
                    {
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = inputDataMain[n][k];
                            }
                        }
                        
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataMain[n][k] = inputDataLower[n][k];
                            }
                        }
                        
                        // Set Lower Block with Zeros.
                        for(int n = 0; n < numInBands; n++)
                        {
                            if(remainRows > 0)
                            {
                                rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
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
                    }
                    else
                    {
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = inputDataMain[n][k];
                            }
                        }
                        
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataMain[n][k] = inputDataLower[n][k];
                            }
                        }
                        
                        // Read Lower Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                            //std::cout << "rowOffset: " << rowOffset << std::endl;
                            //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                            //std::cout << "width: " << width << std::endl;
                            //std::cout << "numOfLines: " << numOfLines << std::endl;
                            
                            inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                        }
                    }
                    
                    for(int m = 0; m < numOfLines; ++m)
                    {
                        line = (i*numOfLines)+m;
                        //std::cout << "line = " << line << std::endl;
                        if((feedback != 0) && (line % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        cLinePxl = m*width;
                        //std::cout << "cLine: " << cLinePxl << std::endl;
                        
                        xPxl = 0;
                        for(int j = 0; j < width; j++)
                        {
                            cPxl = cLinePxl+j;
                            if(m < windowMid)
                            {
                                //std::cout << "Need Upper\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " (numPxlsInBlock+(cPxl+dLinePxls)) = " << (numPxlsInBlock+(cPxl+dLinePxls)) << std::endl;
                                    
                                    if((cPxl + dLinePxls) < 0)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Need Lower\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << "j = " << j << " y = " << y << ": " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " ((cPxl+dLinePxls)-numPxlsInBlock) = " << ((cPxl+dLinePxls)-numPxlsInBlock) << std::endl;
                                    
                                    if((cPxl + dLinePxls) >= numPxlsInBlock)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Within block\n";
                                
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << std::endl;
                                    
                                    for(int x = 0; x < windowSize; x++)
                                    {
                                        dWinX = x-windowMid;
                                        
                                        if((j+dWinX) < 0)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else if((j+dWinX) >= width)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                            }
                                        }
                                    }
                                }
                            }
                            
                            if(passPxlXY)
                            {
                                pxlPos.init(xPxl, xPxl, yPxl, yPxl);
                                this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn, pxlPos);
                            }
                            else
                            {
                                this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn);
                            }
                            
                            for(int n = 0; n < this->numOutBands; n++)
                            {
                                outputData[n][cPxl] = outDataColumn[n];
                            }
                            ++xPxl;
                        }
                        ++yPxl;
                    }
                    
                    for(int n = 0; n < this->numOutBands; n++)
                    {
                        outputRasterBands[n]->RasterIO(GF_Write, 0, (numOfLines * i), width, numOfLines, outputData[n], width, numOfLines, GDT_Float64, 0, 0);
                    }
                }
                
                if(remainRows > 0)
                {
                    // Shift Lower Block to Main Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataUpper[n][k] = inputDataMain[n][k];
                        }
                    }
                    
                    // Shift Lower Block to Main Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataMain[n][k] = inputDataLower[n][k];
                        }
                    }
                    
                    // Read Lower Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataLower[n][k] = 0;
                        }
                    }
                    
                    for(int m = 0; m < remainRows; ++m)
                    {
                        line = (nYBlocks*numOfLines)+m;
                        //std::cout << "line = " << line << std::endl;
                        if((feedback != 0) && (line % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        cLinePxl = m*width;
                        //std::cout << "cLine: " << cLinePxl << std::endl;
                        
                        xPxl = 0;
                        for(int j = 0; j < width; j++)
                        {
                            cPxl = cLinePxl+j;
                            if(m < windowMid)
                            {
                                //std::cout << "Need Upper\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " (numPxlsInBlock+(cPxl+dLinePxls)) = " << (numPxlsInBlock+(cPxl+dLinePxls)) << std::endl;
                                    
                                    if((cPxl + dLinePxls) < 0)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Need Lower\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << "j = " << j << " y = " << y << ": " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " ((cPxl+dLinePxls)-numPxlsInBlock) = " << ((cPxl+dLinePxls)-numPxlsInBlock) << std::endl;
                                    
                                    if((cPxl + dLinePxls) >= numPxlsInBlock)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Within block\n";
                                
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << std::endl;
                                    
                                    for(int x = 0; x < windowSize; x++)
                                    {
                                        dWinX = x-windowMid;
                                        
                                        if((j+dWinX) < 0)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else if((j+dWinX) >= width)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                            }
                                        }
                                    }
                                }
                            }
                            
                            if(passPxlXY)
                            {
                                pxlPos.init(xPxl, xPxl, yPxl, yPxl);
                                this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn, pxlPos);
                            }
                            else
                            {
                                this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn);
                            }
                            
                            for(int n = 0; n < this->numOutBands; n++)
                            {
                                outputData[n][cPxl] = outDataColumn[n];
                            }
                            ++xPxl;
                        }
                        ++yPxl;
                    }
                    
                    for(int n = 0; n < this->numOutBands; n++)
                    {
                        outputRasterBands[n]->RasterIO(GF_Write, 0, (nYBlocks*numOfLines), width, remainRows, outputData[n], width, remainRows, GDT_Float64, 0, 0);
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
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete dsOffsets[i];
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete bandOffsets[i];
				}
				delete[] bandOffsets;
			}
			
			if(inputDataUpper != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataUpper[i];
				}
				delete[] inputDataUpper;
			}
            
            if(inputDataMain != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataMain[i];
				}
				delete[] inputDataMain;
			}
            
            if(inputDataLower != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataLower[i];
				}
				delete[] inputDataLower;
			}
			
			if(inDataBlock != NULL)
			{
				for(int i = 0; i < numInBands; i++)
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
				for(int i = 0; i < numOutBands; i++)
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
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete dsOffsets[i];
				}
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete bandOffsets[i];
				}
				delete[] bandOffsets;
			}
			
			if(inputDataUpper != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataUpper[i];
				}
				delete[] inputDataUpper;
			}
            
            if(inputDataMain != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataMain[i];
				}
				delete[] inputDataMain;
			}
            
            if(inputDataLower != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					delete[] inputDataLower[i];
				}
				delete[] inputDataLower;
			}
			
			if(inDataBlock != NULL)
			{
				for(int i = 0; i < numInBands; i++)
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
				for(int i = 0; i < numOutBands; i++)
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
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete dsOffsets[i];
			}
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				delete bandOffsets[i];
			}
			delete[] bandOffsets;
		}
		
		if(inputDataUpper != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataUpper[i];
            }
            delete[] inputDataUpper;
        }
        
        if(inputDataMain != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataMain[i];
            }
            delete[] inputDataMain;
        }
        
        if(inputDataLower != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataLower[i];
            }
            delete[] inputDataLower;
        }
		
		if(inDataBlock != NULL)
		{
			for(int i = 0; i < numInBands; i++)
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
			for(int i = 0; i < numOutBands; i++)
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
     
    /* Keeps returning a window of data based upon the supplied windowSize until all finished provides the extent on the central pixel (as envelope) at each iteration */
	void RSGISCalcImage::calcImageWindowDataExtent(GDALDataset **datasets, int numDS, std::string outputImage, int windowSize, std::string gdalFormat, GDALDataType gdalDataType) throw(RSGISImageCalcException,RSGISImageBandException)
	{
        GDALAllRegister();
        RSGISImageUtils imgUtils;
        double *gdalTranslation = new double[6];
        int **dsOffsets = new int*[numDS];
        for(int i = 0; i < numDS; i++)
        {
            dsOffsets[i] = new int[2];
        }
        int **bandOffsets = NULL;
        int height = 0;
        int width = 0;
        int numInBands = 0;
        int xBlockSize = 0;
        int yBlockSize = 0;
        size_t numPxlsInBlock = 0;
        
        float **inputDataUpper = NULL;
        float **inputDataMain = NULL;
        float **inputDataLower = NULL;
        double **outputData = NULL;
        float ***inDataBlock = NULL;
        double *outDataColumn = NULL;
        
        GDALDataset *outputImageDS = NULL;
        GDALRasterBand **inputRasterBands = NULL;
        GDALRasterBand **outputRasterBands = NULL;
        GDALDriver *gdalDriver = NULL;
        
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
            
            //std::cout << "Window Size: " << windowSize << std::endl;
            //std::cout << "Window Mid: " << windowMid << std::endl;
            
            // Find image overlap
            imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, &xBlockSize, &yBlockSize);
            
            // Count number of image bands
            for(int i = 0; i < numDS; i++)
            {
                numInBands += datasets[i]->GetRasterCount();
            }
            
            // Create new Image
            gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
            if(gdalDriver == NULL)
            {
                throw RSGISImageBandException("Driver does not exists..");
            }
            //std::cout << "New image width = " << width << " height = " << height << std::endl;
            
            outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, this->numOutBands, gdalDataType, NULL);
            
            if(outputImageDS == NULL)
            {
                throw RSGISImageBandException("Output image could not be created. Check filepath.");
            }
            outputImageDS->SetGeoTransform(gdalTranslation);
            if(useImageProj)
            {
                outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
            }
            else
            {
                outputImageDS->SetProjection(proj.c_str());
            }
            
            // Get Image Input Bands
            bandOffsets = new int*[numInBands];
            inputRasterBands = new GDALRasterBand*[numInBands];
            int counter = 0;
            for(int i = 0; i < numDS; i++)
            {
                for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
                {
                    inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
                    bandOffsets[counter] = new int[2];
                    bandOffsets[counter][0] = dsOffsets[i][0];
                    bandOffsets[counter][1] = dsOffsets[i][1];
                    //std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
                    counter++;
                }
            }
            
            //Get Image Output Bands
            outputRasterBands = new GDALRasterBand*[this->numOutBands];
            for(int i = 0; i < this->numOutBands; i++)
            {
                outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
            }
            
            int outXBlockSize = 0;
            int outYBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&outXBlockSize, &outYBlockSize);
            
            if(outYBlockSize > yBlockSize)
            {
                yBlockSize = outYBlockSize;
            }
            //std::cout << "Max. block size: " << yBlockSize << std::endl;
            
            int numOfLines = yBlockSize;
            if(yBlockSize < windowSize)
            {
                numOfLines = ceil(((float)windowSize)/((float)yBlockSize))*yBlockSize;
            }
            
            // Allocate memory
            numPxlsInBlock = width*numOfLines;
            inputDataUpper = new float*[numInBands];
            for(int i = 0; i < numInBands; i++)
            {
                inputDataUpper[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataUpper[i][k] = 0;
                }
            }
            
            inputDataMain = new float*[numInBands];
            for(int i = 0; i < numInBands; i++)
            {
                inputDataMain[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataMain[i][k] = 0;
                }
            }
            
            inputDataLower = new float*[numInBands];
            for(int i = 0; i < numInBands; i++)
            {
                inputDataLower[i] = (float *) CPLMalloc(sizeof(float)*numPxlsInBlock);
                for(int k = 0; k < numPxlsInBlock; k++)
                {
                    inputDataLower[i][k] = 0;
                }
            }
            
            inDataBlock = new float**[numInBands];
            for(int i = 0; i < numInBands; i++)
            {
                inDataBlock[i] = new float*[windowSize];
                for(int j = 0; j < windowSize; j++)
                {
                    inDataBlock[i][j] = new float[windowSize];
                }
            }
            
            outputData = new double*[this->numOutBands];
            for(int i = 0; i < this->numOutBands; i++)
            {
                outputData[i] = (double *) CPLMalloc(sizeof(double)*numPxlsInBlock);
            }
            outDataColumn = new double[this->numOutBands];
            
            
            //std::cout << "height: " << height << std::endl;
            int nYBlocks = floor(((double)height) / ((double)numOfLines));
            //std::cout << "nYBlocks: " << nYBlocks << std::endl;
            int remainRows = height - (nYBlocks * numOfLines);
            //std::cout << "remainRows: " << remainRows << std::endl;
            int rowOffset = 0;
            unsigned int line = 0;
            long cLinePxl = 0;
            long cPxl = 0;
            long dLinePxls = 0;
            int dWinX = 0;
            int dWinY = 0;
            double pxlTLX = 0;
            double pxlTLY = 0;
            double pxlWidth = 0;
            double pxlHeight = 0;
            geos::geom::Envelope pxlExt;
            
            pxlTLX = gdalTranslation[0];
            pxlTLY = gdalTranslation[3];
            pxlWidth = gdalTranslation[1];
            pxlHeight = gdalTranslation[5];
            
            if(pxlHeight < 0)
            {
                pxlHeight *= (-1);
            }
            
            int feedback = height/10.0;
            int feedbackCounter = 0;
            std::cout << "Started " << std::flush;
            
            if(nYBlocks > 0)
            {
                for(int i = 0; i < nYBlocks; i++)
                {
                    //std::cout << "i: " << i << std::endl;
                    if(i == 0)
                    {
                        // Set Upper Block with Zeros.
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = 0;
                            }
                        }
                        
                        // Read Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            rowOffset = bandOffsets[n][1] + (numOfLines * i);
                            //std::cout << "rowOffset: " << rowOffset << std::endl;
                            //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                            //std::cout << "width: " << width << std::endl;
                            //std::cout << "numOfLines: " << numOfLines << std::endl;
                            
                            inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataMain[n], width, numOfLines, GDT_Float32, 0, 0);
                        }
                        // Read Lower Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            if(nYBlocks == 1)
                            {
                                if(remainRows > 0)
                                {
                                    rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                    //std::cout << "rowOffset: " << rowOffset << std::endl;
                                    //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                                    //std::cout << "width: " << width << std::endl;
                                    //std::cout << "remainRows = " << remainRows << std::endl;
                                    inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
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
                                rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                //std::cout << "rowOffset: " << rowOffset << std::endl;
                                //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                                //std::cout << "width: " << width << std::endl;
                                //std::cout << "remainRows = " << remainRows << std::endl;
                                inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                            }
                        }
                    }
                    else if(i == (nYBlocks-1))
                    {
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = inputDataMain[n][k];
                            }
                        }
                        
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataMain[n][k] = inputDataLower[n][k];
                            }
                        }
                        
                        // Set Lower Block with Zeros.
                        for(int n = 0; n < numInBands; n++)
                        {
                            if(remainRows > 0)
                            {
                                rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                                inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, remainRows, inputDataLower[n], width, remainRows, GDT_Float32, 0, 0);
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
                    }
                    else
                    {
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataUpper[n][k] = inputDataMain[n][k];
                            }
                        }
                        
                        // Shift Lower Block to Main Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            for(int k = 0; k < numPxlsInBlock; k++)
                            {
                                inputDataMain[n][k] = inputDataLower[n][k];
                            }
                        }
                        
                        // Read Lower Block
                        for(int n = 0; n < numInBands; n++)
                        {
                            rowOffset = bandOffsets[n][1] + (numOfLines * (i+1));
                            //std::cout << "rowOffset: " << rowOffset << std::endl;
                            //std::cout << "bandOffsets["<<n<<"][0]: " << bandOffsets[n][0] << std::endl;
                            //std::cout << "width: " << width << std::endl;
                            //std::cout << "numOfLines: " << numOfLines << std::endl;
                            
                            inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], rowOffset, width, numOfLines, inputDataLower[n], width, numOfLines, GDT_Float32, 0, 0);
                        }
                    }
                    
                    for(int m = 0; m < numOfLines; ++m)
                    {
                        line = (i*numOfLines)+m;
                        //std::cout << "line = " << line << std::endl;
                        if((feedback != 0) && (line % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        cLinePxl = m*width;
                        //std::cout << "cLine: " << cLinePxl << std::endl;
                        
                        pxlTLX = gdalTranslation[0];
                        for(int j = 0; j < width; j++)
                        {
                            cPxl = cLinePxl+j;
                            if(m < windowMid)
                            {
                                //std::cout << "Need Upper\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " (numPxlsInBlock+(cPxl+dLinePxls)) = " << (numPxlsInBlock+(cPxl+dLinePxls)) << std::endl;
                                    
                                    if((cPxl + dLinePxls) < 0)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Need Lower\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << "j = " << j << " y = " << y << ": " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " ((cPxl+dLinePxls)-numPxlsInBlock) = " << ((cPxl+dLinePxls)-numPxlsInBlock) << std::endl;
                                    
                                    if((cPxl + dLinePxls) >= numPxlsInBlock)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Within block\n";
                                
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << std::endl;
                                    
                                    for(int x = 0; x < windowSize; x++)
                                    {
                                        dWinX = x-windowMid;
                                        
                                        if((j+dWinX) < 0)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else if((j+dWinX) >= width)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                            }
                                        }
                                    }
                                }
                            }
                            

                            pxlExt.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
                            this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn, pxlExt);
                            
                            for(int n = 0; n < this->numOutBands; n++)
                            {
                                outputData[n][cPxl] = outDataColumn[n];
                            }
                            pxlTLX += pxlWidth;
                        }
                        pxlTLY -= pxlHeight;
                    }
                    
                    for(int n = 0; n < this->numOutBands; n++)
                    {
                        outputRasterBands[n]->RasterIO(GF_Write, 0, (numOfLines * i), width, numOfLines, outputData[n], width, numOfLines, GDT_Float64, 0, 0);
                    }
                }
                
                if(remainRows > 0)
                {
                    // Shift Lower Block to Main Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataUpper[n][k] = inputDataMain[n][k];
                        }
                    }
                    
                    // Shift Lower Block to Main Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataMain[n][k] = inputDataLower[n][k];
                        }
                    }
                    
                    // Read Lower Block
                    for(int n = 0; n < numInBands; n++)
                    {
                        for(int k = 0; k < numPxlsInBlock; k++)
                        {
                            inputDataLower[n][k] = 0;
                        }
                    }
                    
                    for(int m = 0; m < remainRows; ++m)
                    {
                        line = (nYBlocks*numOfLines)+m;
                        //std::cout << "line = " << line << std::endl;
                        if((feedback != 0) && (line % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        cLinePxl = m*width;
                        //std::cout << "cLine: " << cLinePxl << std::endl;
                        
                        pxlTLX = gdalTranslation[0];
                        for(int j = 0; j < width; j++)
                        {
                            cPxl = cLinePxl+j;
                            if(m < windowMid)
                            {
                                //std::cout << "Need Upper\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " (numPxlsInBlock+(cPxl+dLinePxls)) = " << (numPxlsInBlock+(cPxl+dLinePxls)) << std::endl;
                                    
                                    if((cPxl + dLinePxls) < 0)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Need Lower\n";
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << "j = " << j << " y = " << y << ": " << dLinePxls << " Width = " << width << " (cPxl + dLinePxls) = " << (cPxl + dLinePxls) << " numPxlsInBlock = " << numPxlsInBlock << " ((cPxl+dLinePxls)-numPxlsInBlock) = " << ((cPxl+dLinePxls)-numPxlsInBlock) << std::endl;
                                    
                                    if((cPxl + dLinePxls) >= numPxlsInBlock)
                                    {
                                        for(int x = 0; x < windowSize; x++)
                                        {
                                            dWinX = x-windowMid;
                                            
                                            if((j+dWinX) < 0)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else if((j+dWinX) >= width)
                                            {
                                                for(int n = 0; n < numInBands; n++)
                                                {
                                                    inDataBlock[n][y][x] = 0;
                                                }
                                            }
                                            else
                                            {
                                                for(int n = 0; n < numInBands; n++)
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
                                //std::cout << "Within block\n";
                                
                                for(int y = 0; y < windowSize; y++)
                                {
                                    dWinY = y-windowMid;
                                    dLinePxls = dWinY * width;
                                    //std::cout << y << " Y  = " << dLinePxls << " Width = " << width << std::endl;
                                    
                                    for(int x = 0; x < windowSize; x++)
                                    {
                                        dWinX = x-windowMid;
                                        
                                        if((j+dWinX) < 0)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else if((j+dWinX) >= width)
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = 0;
                                            }
                                        }
                                        else
                                        {
                                            for(int n = 0; n < numInBands; n++)
                                            {
                                                inDataBlock[n][y][x] = inputDataMain[n][(cPxl+dLinePxls)+dWinX];
                                            }
                                        }
                                    }
                                }
                            }
                            

                            pxlExt.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
                            this->calc->calcImageValue(inDataBlock, numInBands, windowSize, outDataColumn, pxlExt);
                            
                            for(int n = 0; n < this->numOutBands; n++)
                            {
                                outputData[n][cPxl] = outDataColumn[n];
                            }
                            pxlTLX += pxlWidth;
                        }
                        pxlTLY -= pxlHeight;
                    }
                    
                    for(int n = 0; n < this->numOutBands; n++)
                    {
                        outputRasterBands[n]->RasterIO(GF_Write, 0, (nYBlocks*numOfLines), width, remainRows, outputData[n], width, remainRows, GDT_Float64, 0, 0);
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
            if(gdalTranslation != NULL)
            {
                delete[] gdalTranslation;
            }
            
            if(dsOffsets != NULL)
            {
                for(int i = 0; i < numDS; i++)
                {
                    delete dsOffsets[i];
                }
                delete[] dsOffsets;
            }
            
            if(bandOffsets != NULL)
            {
                for(int i = 0; i < numInBands; i++)
                {
                    delete bandOffsets[i];
                }
                delete[] bandOffsets;
            }
            
            if(inputDataUpper != NULL)
            {
                for(int i = 0; i < numInBands; i++)
                {
                    delete[] inputDataUpper[i];
                }
                delete[] inputDataUpper;
            }
            
            if(inputDataMain != NULL)
            {
                for(int i = 0; i < numInBands; i++)
                {
                    delete[] inputDataMain[i];
                }
                delete[] inputDataMain;
            }
            
            if(inputDataLower != NULL)
            {
                for(int i = 0; i < numInBands; i++)
                {
                    delete[] inputDataLower[i];
                }
                delete[] inputDataLower;
            }
            
            if(inDataBlock != NULL)
            {
                for(int i = 0; i < numInBands; i++)
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
                for(int i = 0; i < numOutBands; i++)
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
            if(gdalTranslation != NULL)
            {
                delete[] gdalTranslation;
            }
            
            if(dsOffsets != NULL)
            {
                for(int i = 0; i < numDS; i++)
                {
                    delete dsOffsets[i];
                }
                delete[] dsOffsets;
            }
            
            if(bandOffsets != NULL)
            {
                for(int i = 0; i < numInBands; i++)
                {
                    delete bandOffsets[i];
                }
                delete[] bandOffsets;
            }
            
            if(inputDataUpper != NULL)
            {
                for(int i = 0; i < numInBands; i++)
                {
                    delete[] inputDataUpper[i];
                }
                delete[] inputDataUpper;
            }
            
            if(inputDataMain != NULL)
            {
                for(int i = 0; i < numInBands; i++)
                {
                    delete[] inputDataMain[i];
                }
                delete[] inputDataMain;
            }
            
            if(inputDataLower != NULL)
            {
                for(int i = 0; i < numInBands; i++)
                {
                    delete[] inputDataLower[i];
                }
                delete[] inputDataLower;
            }
            
            if(inDataBlock != NULL)
            {
                for(int i = 0; i < numInBands; i++)
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
                for(int i = 0; i < numOutBands; i++)
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
        
        if(gdalTranslation != NULL)
        {
            delete[] gdalTranslation;
        }
        
        if(dsOffsets != NULL)
        {
            for(int i = 0; i < numDS; i++)
            {
                delete dsOffsets[i];
            }
            delete[] dsOffsets;
        }
        
        if(bandOffsets != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete bandOffsets[i];
            }
            delete[] bandOffsets;
        }
        
        if(inputDataUpper != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataUpper[i];
            }
            delete[] inputDataUpper;
        }
        
        if(inputDataMain != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataMain[i];
            }
            delete[] inputDataMain;
        }
        
        if(inputDataLower != NULL)
        {
            for(int i = 0; i < numInBands; i++)
            {
                delete[] inputDataLower[i];
            }
            delete[] inputDataLower;
        }
        
        if(inDataBlock != NULL)
        {
            for(int i = 0; i < numInBands; i++)
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
            for(int i = 0; i < numOutBands; i++)
            {
                delete outputData[i];
            }
            delete outputData;
        }
        
        if(outDataColumn != NULL)
        {
            delete[] outDataColumn;
        }
        
        GDALClose(outputImageDS);
    }
	
	void RSGISCalcImage::calcImageWithinPolygon(GDALDataset **datasets, int numDS, std::string outputImage, geos::geom::Envelope *env, geos::geom::Polygon *poly, float nodata, pixelInPolyOption pixelPolyOption, std::string gdalFormat,  GDALDataType gdalDataType) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputData = NULL;
		double **outputData = NULL;
		float *inDataColumn = NULL;
		double *outDataColumn = NULL;
		
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
		geos::geom::Envelope extent;
		geos::geom::Coordinate pxlCentre;
		geos::geom::GeometryFactory geomFactory;
		double pxlTLX = 0;
		double pxlTLY = 0;
		double pxlWidth = 0;
		double pxlHeight = 0;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env);
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
			if(gdalDriver == NULL)
			{
				throw RSGISImageBandException("ENVI driver does not exists..");
			}
			//std::cout << "New image width = " << width << " height = " << height << std::endl;

			outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, this->numOutBands, gdalDataType, NULL);
			
			if(outputImageDS == NULL)
			{
				throw RSGISImageBandException("Output image could not be created. Check filepath.");
			}
			outputImageDS->SetGeoTransform(gdalTranslation);
			if(useImageProj)
			{
				outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
			}
			else
			{
				outputImageDS->SetProjection(proj.c_str());
			}
			
			pxlTLX = gdalTranslation[0];
			pxlTLY = gdalTranslation[3];
			pxlWidth = gdalTranslation[1];
			pxlHeight = gdalTranslation[1];
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
			}
			
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumn = new float[numInBands];
			
			outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*width);
			}
			outDataColumn = new double[this->numOutBands];
			
			int feedback = height/10;
			if (feedback == 0) {feedback = 1;} // Set feedback to 1
			int feedbackCounter = 0;
			if(height > 100)
			{
				std::cout << "Started " << std::flush;
			}			// Loop images to process data
			for(int i = 0; i < height; i++)
			{				
				if((feedback != 0) && ((i % feedback) == 0))
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], (bandOffsets[n][1]+i), width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
				}
				
				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumn[n] = inputData[n][j];
					}
					
                    geos::geom::Coordinate pxlCentre;
					geos::geom::GeometryFactory geomFactory;
					geos::geom::Point *pt = NULL;
					
					extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
					extent.centre(pxlCentre);
					pt = geomFactory.createPoint(pxlCentre);
					
					if (pixelPolyOption == polyContainsPixelCenter) 
					{
						
						if(poly->contains(pt)) // If polygon contains pixel center
						{
							this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
						}
						else
						{
							for(int n = 0; n < this->numOutBands; n++)
							{
								outDataColumn[n] = nodata;
							}
						}
						
					}
					else if (pixelPolyOption == pixelAreaInPoly) 
					{
						geos::geom::CoordinateSequence *coords = NULL;
						geos::geom::LinearRing *ring = NULL;
						geos::geom::Polygon *pixelGeosPoly = NULL;
						geos::geom::Geometry *intersectionGeom;
						
						geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel();
						geos::geom::GeometryFactory *geomFactory = new geos::geom::GeometryFactory(pm);
						
						coords = new geos::geom::CoordinateArraySequence();
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY, 0));
						coords->add(geos::geom::Coordinate(pxlTLX + pxlWidth, pxlTLY, 0));
						coords->add(geos::geom::Coordinate(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0));
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY - pxlHeight, 0));
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY, 0));
						ring = geomFactory->createLinearRing(coords);
						pixelGeosPoly = geomFactory->createPolygon(ring, NULL);
						
						
						intersectionGeom = pixelGeosPoly->intersection(poly);
						double intersectionArea = intersectionGeom->getArea()  / pixelGeosPoly->getArea();
						
						if(intersectionArea > 0)
						{
							for(int n = 0; n < numInBands; n++)
							{
								this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
							}
						}
						else
						{
							for(int n = 0; n < this->numOutBands; n++)
							{
								outDataColumn[n] = nodata;
							}
						}
					}
					else 
					{
						RSGISPixelInPoly *pixelInPoly;
						OGRLinearRing *ring;
						OGRPolygon *pixelPoly;
						OGRPolygon *ogrPoly;
						OGRGeometry *ogrGeom;
						
						pixelInPoly = new RSGISPixelInPoly(pixelPolyOption);
						
						ring = new OGRLinearRing();
						ring->addPoint(pxlTLX, pxlTLY, 0);
						ring->addPoint(pxlTLX + pxlWidth, pxlTLY, 0);
						ring->addPoint(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0);
						ring->addPoint(pxlTLX, pxlTLY - pxlHeight, 0);
						ring->addPoint(pxlTLX, pxlTLY, 0);
						
						pixelPoly = new OGRPolygon();
						pixelPoly->addRingDirectly(ring);
						
						ogrPoly = new OGRPolygon();
						const geos::geom::LineString *line = poly->getExteriorRing();
						OGRLinearRing *ogrRing = new OGRLinearRing();
						const geos::geom::CoordinateSequence *coords = line->getCoordinatesRO();
						int numCoords = coords->getSize();
						geos::geom::Coordinate coord;
						for(int i = 0; i < numCoords; i++)
						{
							coord = coords->getAt(i);
							ogrRing->addPoint(coord.x, coord.y, coord.z);
						}
						ogrPoly->addRing(ogrRing);
						ogrGeom = (OGRGeometry *) ogrPoly;
						
						// Check if the pixel should be classed as part of the polygon using the specified method
						if (pixelInPoly->findPixelInPoly(ogrGeom, pixelPoly)) 
						{
							this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
						}
						else
						{
							for(int n = 0; n < this->numOutBands; n++)
							{
								outDataColumn[n] = nodata;
							}
						}
						
						
						// Tidy
						delete pixelInPoly;
						delete pixelPoly;
						delete ogrPoly;
					}
					
					delete pt;
					
					pxlTLX += pxlWidth;
					
					for(int n = 0; n < this->numOutBands; n++)
					{
						outputData[n][j] = outDataColumn[n];
					}
					
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
				
				for(int n = 0; n < this->numOutBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Write, 0, i, width, 1, outputData[n], width, 1, GDT_Float64, 0, 0);
				}
			}
			if (height > 100) 
			{
				std::cout << " Complete.\n";
			}
		}
		catch(RSGISImageCalcException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		
		GDALClose(outputImageDS);
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
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
		
		if(outputData != NULL)
		{
			for(int i = 0; i < this->numOutBands; i++)
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
		
		if(inputRasterBands != NULL)
		{
			delete[] inputRasterBands;
		}
		
		if(outputRasterBands != NULL)
		{
			delete[] outputRasterBands;
		}		
	}
	
	/* calcImageWithinPolygon - takes existing output image */
	void RSGISCalcImage::calcImageWithinPolygon(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, geos::geom::Polygon *poly, pixelInPolyOption pixelPolyOption) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		/* Input and output images as gdal datasets
		 * Stored as:
		 * Input DS
		 * Output DS
		 * numDS = numinput + 1 (output band)
		 */
		
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		//int numOutBands = 0;
		
		float **inputData = NULL;
		double **outputData = NULL;
		float *inDataColumn = NULL;
		double *outDataColumn = NULL;
		
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		geos::geom::Envelope extent;
		geos::geom::Coordinate pxlCentre;
		geos::geom::GeometryFactory geomFactory;
		double pxlTLX = 0;
		double pxlTLY = 0;
		double pxlWidth = 0;
		double pxlHeight = 0;
				
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env);
			
			// Count number of input image bands
			for(int i = 0; i < numDS - 1; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			// Count number of output image bands
			this->numOutBands = datasets[numDS-1]->GetRasterCount();
			
			// Get Pixel Size
			pxlTLX = gdalTranslation[0];
			pxlTLY = gdalTranslation[3];
			pxlWidth = gdalTranslation[1];
			pxlHeight = gdalTranslation[1];
			
			if(pxlHeight < 0) // Check resolution is positive (negative in Southern hemisphere).
			{
				pxlHeight = pxlHeight * (-1);
			}
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands+numOutBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS - 1; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			// Get Output Input Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int j = 0; j < datasets[numDS-1]->GetRasterCount(); j++)
			{
				outputRasterBands[j] = datasets[numDS-1]->GetRasterBand(j+1);
				bandOffsets[counter] = new int[2];
				bandOffsets[counter][0] = dsOffsets[numDS-1][0];
				bandOffsets[counter][1] = dsOffsets[numDS-1][1];
				//std::cout << counter << ") dataset " << numDS-1 << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
				counter++;
			}
			
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumn = new float[numInBands];
			
			outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*width);
			}
			outDataColumn = new double[this->numOutBands];
			
			int feedback = height/10;
			if (feedback == 0) {feedback = 1;} // Set feedback to 1
			int feedbackCounter = 0;
			if(height > 100)
			{
				std::cout << "\rStarted " << std::flush;
			}			
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{				
				if((feedback != 0) && ((i % feedback) == 0))
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], (bandOffsets[n][1]+i), width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
				}
				for(int n = 0; n < this->numOutBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], (bandOffsets[n][1]+i), width, 1, outputData[n], width, 1, GDT_Float64, 0, 0);
				}
				
				
				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumn[n] = inputData[n][j];
					}
					
					geos::geom::Coordinate pxlCentre;
					geos::geom::GeometryFactory geomFactory;
					geos::geom::Point *pt = NULL;
					
					extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
					extent.centre(pxlCentre);
					pt = geomFactory.createPoint(pxlCentre);
					
					if (pixelPolyOption == polyContainsPixelCenter) 
					{
						if(poly->contains(pt)) // If polygon contains pixel center
						{
							this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
						}
						else 
						{
							for (int n = 0; n < this->numOutBands; n++) 
							{
								outDataColumn[n] = outputData[n][j];
							}
						}

					}
					else if (pixelPolyOption == pixelAreaInPoly) 
					{
						geos::geom::CoordinateSequence *coords = NULL;
						geos::geom::LinearRing *ring = NULL;
						geos::geom::Polygon *pixelGeosPoly = NULL;
						geos::geom::Geometry *intersectionGeom;
						
						geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel();
						geos::geom::GeometryFactory *geomFactory = new geos::geom::GeometryFactory(pm);
						
						coords = new geos::geom::CoordinateArraySequence();
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY, 0));
						coords->add(geos::geom::Coordinate(pxlTLX + pxlWidth, pxlTLY, 0));
						coords->add(geos::geom::Coordinate(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0));
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY - pxlHeight, 0));
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY, 0));
						ring = geomFactory->createLinearRing(coords);
						pixelGeosPoly = geomFactory->createPolygon(ring, NULL);
						
						
						intersectionGeom = pixelGeosPoly->intersection(poly);
						double intersectionArea = intersectionGeom->getArea()  / pixelGeosPoly->getArea();
						
						if(intersectionArea > 0)
						{
							for(int n = 0; n < numInBands; n++)
							{
								this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
							}
						}
						else 
						{
							for (int n = 0; n < this->numOutBands; n++) 
							{
								outDataColumn[n] = outputData[n][j];
							}
						}
					}
					else 
					{
						RSGISPixelInPoly *pixelInPoly;
						OGRLinearRing *ring;
						OGRPolygon *pixelPoly;
						OGRPolygon *ogrPoly;
						OGRGeometry *ogrGeom;
						
						pixelInPoly = new RSGISPixelInPoly(pixelPolyOption);
						
						ring = new OGRLinearRing();
						ring->addPoint(pxlTLX, pxlTLY, 0);
						ring->addPoint(pxlTLX + pxlWidth, pxlTLY, 0);
						ring->addPoint(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0);
						ring->addPoint(pxlTLX, pxlTLY - pxlHeight, 0);
						ring->addPoint(pxlTLX, pxlTLY, 0);
						
						pixelPoly = new OGRPolygon();
						pixelPoly->addRingDirectly(ring);
						
						ogrPoly = new OGRPolygon();
						const geos::geom::LineString *line = poly->getExteriorRing();
						OGRLinearRing *ogrRing = new OGRLinearRing();
						const geos::geom::CoordinateSequence *coords = line->getCoordinatesRO();
						int numCoords = coords->getSize();
						geos::geom::Coordinate coord;
						for(int i = 0; i < numCoords; i++)
						{
							coord = coords->getAt(i);
							ogrRing->addPoint(coord.x, coord.y, coord.z);
						}
						ogrPoly->addRing(ogrRing);
						ogrGeom = (OGRGeometry *) ogrPoly;
						
						// Check if the pixel should be classed as part of the polygon using the specified method
						if (pixelInPoly->findPixelInPoly(ogrGeom, pixelPoly)) 
						{
							this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
						}
						else 
						{
							for (int n = 0; n < this->numOutBands; n++) 
							{
								outDataColumn[n] = outputData[n][j];
							}
						}
						
						// Tidy
						delete pixelInPoly;
						delete pixelPoly;
						delete ogrPoly;
					}
					
					delete pt;
					
					pxlTLX += pxlWidth;
					
					for(int n = 0; n < this->numOutBands; n++)
					{
						outputData[n][j] = outDataColumn[n];
					}
					
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
				
				
				for(int n = 0; n < this->numOutBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Write, bandOffsets[n][0], (bandOffsets[n][1]+i), width, 1, outputData[n], width, 1, GDT_Float64, 0, 0);
				}
			}
			if (height > 100) 
			{
				std::cout << "Complete\r" << std::flush;
				std::cout << "\r                                                                                                                            \r" << std::flush;
			}
			//std::cout << "Done the main bit, starting to tidy up!" << std::endl;
			
		}
		catch(RSGISImageCalcException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
				
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
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
		
		if(outputData != NULL)
		{
			for(int i = 0; i < this->numOutBands; i++)
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
		
		if(inputRasterBands != NULL)
		{
			delete[] inputRasterBands;
		}
		
		if(outputRasterBands != NULL)
		{
			delete[] outputRasterBands;
		}		
	}
    
    /* calcImageWithinPolygon - Does not use an output image */
	void RSGISCalcImage::calcImageWithinPolygonExtent(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, geos::geom::Polygon *poly, pixelInPolyOption pixelPolyOption) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputData = NULL;
		float *inDataColumn = NULL;
		
		GDALRasterBand **inputRasterBands = NULL;
		geos::geom::Envelope extent;
		geos::geom::Coordinate pxlCentre;
		geos::geom::GeometryFactory geomFactory;
		double pxlTLX = 0;
		double pxlTLY = 0;
		double pxlWidth = 0;
		double pxlHeight = 0;
        
		try
		{
			// CHECK ENVELOPE IS AT LEAST 1 x 1 Pixel
			/* For small polygons the the envelope can be smaller than a pixel, which will cause problems.
			 * To avoid errors a buffer is applied to the envelope and this buffered envelope is used for 'getImageOverlap'
			 * The buffered envelope is created and destroyed in this class and does not effect the passed in envelope
			 */
			bool buffer = false;
			
			double *transformations = new double[6];
			datasets[0]->GetGeoTransform(transformations);
			
			// Get pixel size
			pxlWidth = transformations[1];
			pxlHeight = transformations[5];
			
			if(pxlHeight < 0) // Check resolution is positive (negative in Southern hemisphere).
			{
				pxlHeight = pxlHeight * (-1);
			}
			
			delete[] transformations;
			
			geos::geom::Envelope *bufferedEnvelope = NULL;
			
			if ((env->getWidth() < pxlWidth) | (env->getHeight() < pxlHeight))
			{
				//std::cout << "BUFFERING\n";
				buffer = true;
				bufferedEnvelope = new geos::geom::Envelope(env->getMinX() - pxlWidth / 2, env->getMaxX() + pxlWidth / 2, env->getMinY() - pxlHeight / 2, env->getMaxY() + pxlHeight / 2);
			}
			
			// Find image overlap
			gdalTranslation = new double[6];
			
			if (buffer) // Use buffered envelope.
			{
				imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, bufferedEnvelope);
			}
			else // Use envelope passed in.
			{
				imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env);
			}
			
			// Count number of input image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
            
			// Get Pixel Size
			pxlTLX = gdalTranslation[0];
			pxlTLY = gdalTranslation[3];
			pxlWidth = gdalTranslation[1];
			pxlHeight = gdalTranslation[1];
			
			if(pxlHeight < 0) // Check resolution is positive (negative in Southern hemisphere).
			{
				pxlHeight = pxlHeight * (-1);
			}
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands+numOutBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumn = new float[numInBands];
            
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], (bandOffsets[n][1]+i), width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
				}
				
				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumn[n] = inputData[n][j];
					}
					
					geos::geom::Coordinate pxlCentre;
					geos::geom::GeometryFactory geomFactory;
					geos::geom::Point *pt = NULL;
					
					extent.init(pxlTLX, (pxlTLX+pxlWidth), pxlTLY, (pxlTLY-pxlHeight));
					extent.centre(pxlCentre);
					pt = geomFactory.createPoint(pxlCentre);
					
					if (pixelPolyOption == polyContainsPixelCenter) 
					{
						if(poly->contains(pt)) // If polygon contains pixel center
						{
							this->calc->calcImageValue(inDataColumn, numInBands, extent);
						}
					}
					else if (pixelPolyOption == pixelAreaInPoly) 
					{
						geos::geom::CoordinateSequence *coords = NULL;
						geos::geom::LinearRing *ring = NULL;
						geos::geom::Polygon *pixelGeosPoly = NULL;
						geos::geom::Geometry *intersectionGeom;
						
						geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel();
						geos::geom::GeometryFactory *geomFactory = new geos::geom::GeometryFactory(pm);
						
						coords = new geos::geom::CoordinateArraySequence();
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY, 0));
						coords->add(geos::geom::Coordinate(pxlTLX + pxlWidth, pxlTLY, 0));
						coords->add(geos::geom::Coordinate(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0));
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY - pxlHeight, 0));
						coords->add(geos::geom::Coordinate(pxlTLX, pxlTLY, 0));
						ring = geomFactory->createLinearRing(coords);
						pixelGeosPoly = geomFactory->createPolygon(ring, NULL);
						
						
						intersectionGeom = pixelGeosPoly->intersection(poly);
						double intersectionArea = intersectionGeom->getArea()  / pixelGeosPoly->getArea();
						
						if(intersectionArea > 0)
						{
							this->calc->calcImageValue(inDataColumn, numInBands, extent);
						}
					}
					else 
					{
						RSGISPixelInPoly *pixelInPoly;
						OGRLinearRing *ring;
						OGRPolygon *pixelPoly;
						OGRPolygon *ogrPoly;
						OGRGeometry *ogrGeom;
						
						pixelInPoly = new RSGISPixelInPoly(pixelPolyOption);
						
						ring = new OGRLinearRing();
						ring->addPoint(pxlTLX, pxlTLY, 0);
						ring->addPoint(pxlTLX + pxlWidth, pxlTLY, 0);
						ring->addPoint(pxlTLX + pxlWidth, pxlTLY - pxlHeight, 0);
						ring->addPoint(pxlTLX, pxlTLY - pxlHeight, 0);
						ring->addPoint(pxlTLX, pxlTLY, 0);
						
						pixelPoly = new OGRPolygon();
						pixelPoly->addRingDirectly(ring);
						
						ogrPoly = new OGRPolygon();
						const geos::geom::LineString *line = poly->getExteriorRing();
						OGRLinearRing *ogrRing = new OGRLinearRing();
						const geos::geom::CoordinateSequence *coords = line->getCoordinatesRO();
						int numCoords = coords->getSize();
						geos::geom::Coordinate coord;
						for(int i = 0; i < numCoords; i++)
						{
							coord = coords->getAt(i);
							ogrRing->addPoint(coord.x, coord.y, coord.z);
						}
						ogrPoly->addRing(ogrRing);
						ogrGeom = (OGRGeometry *) ogrPoly;
						
						// Check if the pixel should be classed as part of the polygon using the specified method
						if (pixelInPoly->findPixelInPoly(ogrGeom, pixelPoly)) 
						{
							this->calc->calcImageValue(inDataColumn, numInBands, extent);
						}
						
						// Tidy
						delete pixelInPoly;
						delete pixelPoly;
						delete ogrPoly;
					}
					
					delete pt;
					
					pxlTLX += pxlWidth;
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
			}
			//std::cout << "Done the main bit, starting to tidy up!" << std::endl;
			
		}
		catch(RSGISImageCalcException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
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
	
	/* calcImageWithinRasterPolygon - takes existing output image */
	void RSGISCalcImage::calcImageWithinRasterPolygon(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, long fid) throw(RSGISImageCalcException,RSGISImageBandException)
	{
		/** Input and output images as gdal datasets
		 * Stored as:
		 * 1) RasterisedVector DS
		 * 2) Input DS
		 * 3) Output DS
		 * numDS = numinput + 2 (mask + output band)
		 */
		
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		//int numOutBands = 0;
		
		float **inputMask = NULL;
		float **inputData = NULL;
		double **outputData = NULL;
		float *inDataColumn = NULL;
		double *outDataColumn = NULL;
		
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALRasterBand **inputMaskBand = NULL;

		geos::geom::Envelope extent;
		geos::geom::Coordinate pxlCentre;
		geos::geom::GeometryFactory geomFactory;
		double pxlTLX = 0;
		double pxlTLY = 0;
		double pxlWidth = 0;
		double pxlHeight = 0;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation, env);
			
			// Count number of input image bands
			for(int i = 1; i < numDS - 1; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Count number of output image bands
			this->numOutBands = datasets[numDS-1]->GetRasterCount();
			
			// Get Pixel Size
			pxlTLX = gdalTranslation[0];
			pxlTLY = gdalTranslation[3];
			pxlWidth = gdalTranslation[1];
			pxlHeight = gdalTranslation[1];
			
			if(pxlHeight < 0) // Check resolution is positive (negative in Southern hemisphere).
			{
				pxlHeight = pxlHeight * (-1);
			}
			
			int counter = 0;
			bandOffsets = new int*[numInBands+numOutBands+1];
			
			// Get Mask Band
			inputMaskBand = new GDALRasterBand*[1];
			inputMaskBand[counter] = datasets[0]->GetRasterBand(1);
			bandOffsets[counter] = new int[2];
			bandOffsets[counter][0] = dsOffsets[0][0];
			bandOffsets[counter][1] = dsOffsets[0][1];
			//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
			counter++;
			
			// Get Image Input Bands
			inputRasterBands = new GDALRasterBand*[numInBands];
			for(int i = 1; i < numDS - 1; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					bandOffsets[counter] = new int[2];
					inputRasterBands[j] = datasets[i]->GetRasterBand(j+1);
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//std::cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			// Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[this->numOutBands];
			for(int j = 0; j < datasets[numDS-1]->GetRasterCount(); j++)
			{
				bandOffsets[counter] = new int[2];
				outputRasterBands[j] = datasets[numDS-1]->GetRasterBand(j+1);
				bandOffsets[counter][0] = dsOffsets[numDS-1][0];
				bandOffsets[counter][1] = dsOffsets[numDS-1][1];
				//std::cout << counter << ") dataset " << numDS-1 << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
				counter++;
			}
			
			// Allocate memory
			inputMask = new float*[1];
			inputMask[0] = (float *) CPLMalloc(sizeof(float)*width);
			
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			inDataColumn = new float[numInBands];
			
			outputData = new double*[this->numOutBands];
			for(int i = 0; i < this->numOutBands; i++)
			{
				outputData[i] = (double *) CPLMalloc(sizeof(double)*width);
			}
			outDataColumn = new double[this->numOutBands];
			
			int feedback = height/10;
			if (feedback == 0) {feedback = 1;} // Set feedback to 1
			int feedbackCounter = 0;
			if(height > 100)
			{
				std::cout << "\rStarted (Object:" << fid << ")..";
			}	
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{				
				if((feedback != 0) && ((i % feedback) == 0))
				{
					std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				counter = 0;
				inputMaskBand[0]->RasterIO(GF_Read, bandOffsets[counter][0], (bandOffsets[counter][1]+i), width, 1, inputMask[0], width, 1, GDT_Float32, 0, 0);
				counter++;
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[counter][0], (bandOffsets[counter][1]+i), width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
					counter++;
				}
				for(int n = 0; n < this->numOutBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Read, bandOffsets[counter][0], (bandOffsets[counter][1]+i), width, 1, outputData[n], width, 1, GDT_Float32, 0, 0);
					counter++;
				}
				
				for(int j = 0; j < width; j++)
				{
					for(int n = 0; n < numInBands; n++)
					{
						inDataColumn[n] = inputData[n][j];
					}
					
					if (inputMask[0][j] == fid) 
					{
						this->calc->calcImageValue(inDataColumn, numInBands, outDataColumn);
					}
					else 
					{
						for (int n = 0; n < this->numOutBands; n++) 
						{
							outDataColumn[n] = outputData[n][j];
						}
					}
					
					for(int n = 0; n < this->numOutBands; n++)
					{
						outputData[n][j] = outDataColumn[n];
					}
					
					pxlTLX += pxlWidth;
					
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
				
				for(int n = 0; n < this->numOutBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Write, bandOffsets[numInBands+1+n][0], (bandOffsets[numInBands+1+n][1]+i), width, 1, outputData[n], width, 1, GDT_Float32, 0, 0);
				}
			}
			if (height > 100) 
			{
				std::cout << "Complete" << std::flush;
				std::cout << "\r                                                                                                                            \r" << std::flush;
			}
			//std::cout << "Done the main bit, starting to tidy up!" << std::endl;
			
		}
		catch(RSGISImageCalcException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands+numOutBands+1; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
				{
					if(outputData[i] != NULL)
					{
						delete[] outputData[i];
					}
				}
				delete[] outputData;
			}
			
			if(inputMask != NULL)
			{
				delete[] inputMask[0];
				delete[] inputMask;
			}
			
			if(inDataColumn != NULL)
			{
				delete[] inDataColumn;
			}
			
			if(outDataColumn != NULL)
			{
				delete[] outDataColumn;
			}
			
			if(inputMaskBand != NULL)
			{
				delete[] inputMaskBand;
			}
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		catch(RSGISImageBandException& e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(bandOffsets != NULL)
			{
				for(int i = 0; i < numInBands; i++)
				{
					if(bandOffsets[i] != NULL)
					{
						delete[] bandOffsets[i];
					}
				}
				delete[] bandOffsets;
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
			
			if(outputData != NULL)
			{
				for(int i = 0; i < this->numOutBands; i++)
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
			
			if(inputRasterBands != NULL)
			{
				delete[] inputRasterBands;
			}
			
			if(outputRasterBands != NULL)
			{
				delete[] outputRasterBands;
			}
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(bandOffsets != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(bandOffsets[i] != NULL)
				{
					delete[] bandOffsets[i];
				}
			}
			delete[] bandOffsets;
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
		
		if(outputData != NULL)
		{
			for(int i = 0; i < this->numOutBands; i++)
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
		
		if(inputRasterBands != NULL)
		{
			delete[] inputRasterBands;
		}
		
		if(outputRasterBands != NULL)
		{
			delete[] outputRasterBands;
		}		
	}
	
    void RSGISCalcImage::calcImageBorderPixels(GDALDataset *dataset, bool returnInt) throw(RSGISImageCalcException,RSGISImageBandException)
    {
        GDALAllRegister();
        
        try
        {
            unsigned int imgWidth = dataset->GetRasterXSize();
            unsigned int imgHeight = dataset->GetRasterYSize();
            unsigned int numBands = dataset->GetRasterCount();
            
            GDALRasterBand **gdalBands = new GDALRasterBand*[numBands];
            for(unsigned int i = 0; i < numBands; ++i)
            {
                gdalBands[i] = dataset->GetRasterBand(i+1);
            }
            
            unsigned int numfloatVals = 0;
            float *pxlFloatVals = NULL;
            unsigned int numIntVals = 0;
            long *pxlIntVals = NULL;
            if(returnInt)
            {
                numIntVals = numBands;
                pxlIntVals = new long[numIntVals];
            }
            else
            {
                numfloatVals = numBands;
                pxlFloatVals = new float[numfloatVals];
            }
            
            int tmpVal = 0;
            
            std::cout << "Processing Top and Bottom Pixels\n";
            // Top and Bottom pixels
            for(unsigned int x = 0; x < imgWidth; ++x)
            {
                // Top
                for(unsigned int b = 0; b < numBands; ++b)
                {
                    if(returnInt)
                    {
                        gdalBands[b]->RasterIO(GF_Read, x, 0, 1, 1, &tmpVal, 1, 1, GDT_Int32, 0, 0);
                        pxlIntVals[b] = tmpVal;
                    }
                    else
                    {
                        gdalBands[b]->RasterIO(GF_Read, x, 0, 1, 1, &pxlFloatVals[b], 1, 1, GDT_Float32, 0, 0);
                    }
                }
                //std::cout << "[" << x << ", " << 0 << "]" << std::endl;
                this->calc->calcImageValue(pxlIntVals, numIntVals, pxlFloatVals, numfloatVals);
                
                // Bottom
                for(unsigned int b = 0; b < numBands; ++b)
                {
                    if(returnInt)
                    {
                        gdalBands[b]->RasterIO(GF_Read, x, (imgHeight-1), 1, 1, &tmpVal, 1, 1, GDT_Int32, 0, 0);
                        pxlIntVals[b] = tmpVal;
                    }
                    else
                    {
                        gdalBands[b]->RasterIO(GF_Read, x, (imgHeight-1), 1, 1, &pxlFloatVals[b], 1, 1, GDT_Float32, 0, 0);
                    }
                }
                //std::cout << "[" << x << ", " << (imgHeight-1) << "]" << std::endl;
                this->calc->calcImageValue(pxlIntVals, numIntVals, pxlFloatVals, numfloatVals);
            }
            
            std::cout << "Processing Left and Right Pixels\n";
            // Left and right pixels
            for(unsigned int y = 0; y < imgHeight; ++y)
            {
                // Left
                for(unsigned int b = 0; b < numBands; ++b)
                {
                    if(returnInt)
                    {
                        gdalBands[b]->RasterIO(GF_Read, 0, y, 1, 1, &pxlIntVals[b], 1, 1, GDT_Int32, 0, 0);
                    }
                    else
                    {
                        gdalBands[b]->RasterIO(GF_Read, 0, y, 1, 1, &pxlFloatVals[b], 1, 1, GDT_Float32, 0, 0);
                    }
                }
                //std::cout << "[" << 0 << ", " << y << "]" << std::endl;
                this->calc->calcImageValue(pxlIntVals, numIntVals, pxlFloatVals, numfloatVals);
                
                // Right
                for(unsigned int b = 0; b < numBands; ++b)
                {
                    if(returnInt)
                    {
                        gdalBands[b]->RasterIO(GF_Read, (imgWidth-1), y, 1, 1, &pxlIntVals[b], 1, 1, GDT_Int32, 0, 0);
                    }
                    else
                    {
                        gdalBands[b]->RasterIO(GF_Read, (imgWidth-1), y, 1, 1, &pxlFloatVals[b], 1, 1, GDT_Float32, 0, 0);
                    }
                }
                //std::cout << "[" << (imgWidth-1) << ", " << 0 << "]" << std::endl;
                this->calc->calcImageValue(pxlIntVals, numIntVals, pxlFloatVals, numfloatVals);
            }
            
            delete[] gdalBands;
            delete[] pxlFloatVals;
            delete[] pxlIntVals;
        }
        catch (RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (RSGISImageBandException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        
        
    }
    
	RSGISCalcImage::~RSGISCalcImage()
	{
		
	}
	
}} //rsgis::img

