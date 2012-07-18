/*
 *  RSGISImageMosaic.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/11/2008.
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
 *
 *  Modified by Dan Clewley on 21/11/2010
 *  Added 'mosaicSkipVals' and 'mosaicSkipThreash'
 *  to skip values in input image
 * 
 */

#include "RSGISImageMosaic.h"

namespace rsgis{namespace img{
	
	RSGISImageMosaic::RSGISImageMosaic()
	{
		
	}
	
	void RSGISImageMosaic::mosaic(std::string *inputImages, int numDS, std::string outputImage, float background, bool projFromImage, std::string proj) throw(RSGISImageException)
	{
		RSGISImageUtils imgUtils;
        GDALAllRegister();
        GDALDataset *dataset = NULL;
        GDALRasterBand *imgBand = NULL;
		int width;
		int height;
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection = proj;
		GDALDataset *outputDataset = NULL;
		GDALRasterBand *inputBand = NULL;
		GDALRasterBand *outputBand = NULL;
		float *imgData = NULL;
		
        std::vector<std::string> bandnames;
        
		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

				if(i == 0)
				{
					numberBands = dataset->GetRasterCount();
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        bandnames.push_back(std::string(imgBand->GetDescription()));
                    }
                    if(projFromImage)
                    {
                        projection = std::string(dataset->GetProjectionRef());
                    }
				}
				else
				{
					if(dataset->GetRasterCount() != numberBands)
					{
						throw RSGISImageBandException("All input images need to have the same number of bands.");
					}
				}
                GDALClose(dataset);
			}
            
			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);
			
			cout << "Create new image [" << width << "," << height << "] with projection: \n" << projection << endl;
			
			outputDataset = imgUtils.createBlankImage(outputImage, transformation, width, height, numberBands, projection, background, bandnames);
			
			// COPY IMAGE DATA INTO THE BLANK IMAGE
			
			cout << "Started (total " << numDS << ") ." << flush;
			
			for(int i = 0; i < numDS; i++)
			{
				cout << "." << i << "." << flush;
                
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();
				
				xDiff = imgTransform[0] - transformation[0];
				yDiff = transformation[3] - imgTransform[3];
				
				xStart = floor(xDiff/transformation[1]);
				yStart = floor(yDiff/transformation[1]);
				imgData = (float *) CPLMalloc(sizeof(float)*tileXSize);
				
				for(int n = 1; n <= numberBands; n++)
				{
					inputBand = dataset->GetRasterBand(n);
					outputBand = outputDataset->GetRasterBand(n);
					for(int m = 0; m < tileYSize; m++)
					{
						inputBand->RasterIO(GF_Read, 0, m, tileXSize, 1, imgData, tileXSize, 1, GDT_Float32, 0, 0);
						outputBand->RasterIO(GF_Write, xStart, (yStart + m), tileXSize, 1, imgData, tileXSize, 1, GDT_Float32, 0, 0);
					}
				}
				delete imgData;
                GDALClose(dataset);
			}
			cout << ".complete\n";
		}
		catch(RSGISImageBandException e)
		{
			if(imgData != NULL)
			{
				delete imgData;
			}
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}
		
		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
		GDALClose(outputDataset);
        GDALDestroyDriverManager();
	}
	
	void RSGISImageMosaic::mosaicSkipVals(std::string *inputImages, int numDS, std::string outputImage, float background, float skipVal, bool projFromImage, std::string proj, unsigned int skipBand) throw(RSGISImageException)
	{
		RSGISImageUtils imgUtils;
        GDALAllRegister();
        GDALDataset *dataset = NULL;
        GDALRasterBand *imgBand = NULL;
		int width;
		int height;
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection = proj;
		GDALDataset *outputDataset = NULL;
		GDALRasterBand *inputBand = NULL;
		GDALRasterBand *outputBand = NULL;
		float **imgData = new float*[numberBands];
		bool skip;
        std::vector<std::string> bandnames;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
				if(i == 0)
				{
					numberBands = dataset->GetRasterCount();
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        bandnames.push_back(std::string(imgBand->GetDescription()));
                    }
                    if(projFromImage)
                    {
                        projection = std::string(dataset->GetProjectionRef());
                    }
				}
				else
				{
					if(dataset->GetRasterCount() != numberBands)
					{
						throw RSGISImageBandException("All input images need to have the same number of bands.");
					}
				}
                GDALClose(dataset);
			}
			
			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);
			
			cout << "Create new image [" << width << "," << height << "] with projection: \n" << projection << endl;
			
			outputDataset = imgUtils.createBlankImage(outputImage, transformation, width, height, numberBands, projection, background, bandnames);
			
			// COPY IMAGE DATA INTO THE BLANK IMAGE
			
			cout << "Started (total " << numDS << ") ." << flush;
			
			for(int i = 0; i < numDS; i++)
			{
				cout << "." << i << "." << flush;
                
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();
				
				xDiff = imgTransform[0] - transformation[0];
				yDiff = transformation[3] - imgTransform[3];
				
				xStart = floor(xDiff/transformation[1]);
				yStart = floor(yDiff/transformation[1]);
				
				for(int n = 0; n < numberBands; n++)
				{
					imgData[n] = (float *) CPLMalloc(sizeof(float)*1);					
				}
				
				for(int y = 0; y < tileYSize; y++)
				{
					for (int x = 0; x < tileXSize; x++) 
					{
						skip = false;
						for(int n = 1; n <= numberBands; n++)
						{
							inputBand = dataset->GetRasterBand(n);
						
							inputBand->RasterIO(GF_Read, x, y, 1, 1, imgData[n-1], 1, 1, GDT_Float32, 0, 0);
						}
						//cout << "Skip Val = " << skipVal << " imgData[n][0] = " << imgData[skipBand][0] << endl;
						if(imgData[skipBand][0] == skipVal) // Check for skip value in band 1
						{
							skip = true;
						}
						if(!skip)
						{
							for(int n = 1; n <= numberBands; n++)
							{
								outputBand = outputDataset->GetRasterBand(n);
								outputBand->RasterIO(GF_Write, (xStart + x), (yStart + y), 1, 1, imgData[n-1], 1, 1, GDT_Float32, 0, 0);
							}
							
						}
					}
				}
				
				for(int n = 0; n < numberBands; n++)
				{
					delete imgData[n];					
				}

                GDALClose(dataset);
			}
			cout << ".complete\n";
		}
		catch(RSGISImageBandException e)
		{
			if(imgData != NULL)
			{
				delete imgData;
			}
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}
		
		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
		delete[] imgData;
		GDALClose(outputDataset);
        GDALDestroyDriverManager();
	}
	
	void RSGISImageMosaic::mosaicSkipThreash(std::string *inputImages, int numDS, std::string outputImage, float background, float skipLowerThreash, float skipUpperThreash, bool projFromImage, std::string proj, unsigned int threashBand) throw(RSGISImageException)
	{
		RSGISImageUtils imgUtils;
        GDALAllRegister();
        GDALDataset *dataset = NULL;
        GDALRasterBand *imgBand = NULL;
		int width;
		int height;
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection = proj;
		GDALDataset *outputDataset = NULL;
		GDALRasterBand *inputBand = NULL;
		GDALRasterBand *outputBand = NULL;
		float **imgData = new float*[numberBands];
		bool skip;
        std::vector<std::string> bandnames;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
				if(i == 0)
				{
					numberBands = dataset->GetRasterCount();
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        bandnames.push_back(std::string(imgBand->GetDescription()));
                    }
                    if(projFromImage)
                    {
                        projection = std::string(dataset->GetProjectionRef());
                    }
				}
				else
				{
					if(dataset->GetRasterCount() != numberBands)
					{
						throw RSGISImageBandException("All input images need to have the same number of bands.");
					}
				}
                GDALClose(dataset);
			}
			
			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);
			
			cout << "Create new image [" << width << "," << height << "] with projection: \n" << projection << endl;
			
			outputDataset = imgUtils.createBlankImage(outputImage, transformation, width, height, numberBands, projection, background, bandnames);
			
			// COPY IMAGE DATA INTO THE BLANK IMAGE
			
			cout << "Started (total " << numDS << ") ." << flush;
			
			for(int i = 0; i < numDS; i++)
			{
				cout << "." << i << "." << flush;
                
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();
				
				xDiff = imgTransform[0] - transformation[0];
				yDiff = transformation[3] - imgTransform[3];
				
				xStart = floor(xDiff/transformation[1]);
				yStart = floor(yDiff/transformation[1]);
				
				for(int n = 0; n < numberBands; n++)
				{
					imgData[n] = (float *) CPLMalloc(sizeof(float)*1);					
				}
				
				for(int y = 0; y < tileYSize; y++)
				{
					for (int x = 0; x < tileXSize; x++) 
					{
						skip = false;
						for(int n = 1; n <= numberBands; n++)
						{
							inputBand = dataset->GetRasterBand(n);
							inputBand->RasterIO(GF_Read, x, y, 1, 1, imgData[n-1], 1, 1, GDT_Float32, 0, 0);
						}
						//cout << "Skip Val = " << skipVal << " imgData[n][0] = " << imgData[skipBand][0] << endl;
						if((imgData[threashBand][0] > skipLowerThreash) && (imgData[threashBand][0] < skipUpperThreash)) // Check if pixel is outside threasholds
						{
							skip = true;
						}
						if(!skip)
						{
							for(int n = 1; n <= numberBands; n++)
							{
								outputBand = outputDataset->GetRasterBand(n);
								outputBand->RasterIO(GF_Write, (xStart + x), (yStart + y), 1, 1, imgData[n-1], 1, 1, GDT_Float32, 0, 0);
							}
						}
						
					}
				}
				for(int n = 0; n < numberBands; n++)
				{
					delete imgData[n];					
				}
                GDALClose(dataset);
			}
			cout << ".complete\n";
		}
		catch(RSGISImageBandException e)
		{
			if(imgData != NULL)
			{
				delete imgData;
			}
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}
		
		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
		delete[] imgData;
		GDALClose(outputDataset);
        GDALDestroyDriverManager();
	}
	
	void RSGISImageMosaic::includeDatasets(GDALDataset *baseImage, std::string *inputImages, int numDS, std::vector<int> bands, bool bandsDefined) throw(RSGISImageException)
	{
		RSGISImageUtils imgUtils;
        GDALDataset *dataset = NULL;
		int width;
		int height;
		
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		double *baseTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection;

		GDALRasterBand *inputBand = NULL;
		GDALRasterBand *outputBand = NULL;
		float *imgData = NULL;
		
		try
		{
			numberBands = baseImage->GetRasterCount();
            for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
                if(!bandsDefined)
                {
                    if(dataset->GetRasterCount() != numberBands)
                    {
                        throw RSGISImageBandException("All input images need to have the same number of bands.");
                    }
                }
                else 
                {
                    for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                    {
                        if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                        {
                            cerr << "Band = " << *iterBands << endl;
                            std::string message = std::string("Band is not within the input dataset ") + inputImages[i];
                            throw RSGISImageException(message.c_str());
                        }
                    }
                }
                GDALClose(dataset);
			}
            
            if(bandsDefined)
            {
                if(numberBands < bands.size())
                {
                    throw RSGISImageException("The base image does not have enough image bands for the output data specificed.");
                }
            }
            
            projection = std::string(baseImage->GetProjectionRef());
			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);
			
			baseImage->GetGeoTransform(baseTransform);
			
			double baseExtentX = baseTransform[0] + (baseImage->GetRasterXSize() * baseTransform[1]);
			double baseExtentY = baseTransform[3] + (baseImage->GetRasterYSize() * baseTransform[5]);
			double imgExtentX = transformation[0] + (width * transformation[1]);
			double imgExtentY = transformation[3] + (height * transformation[5]);
			
			/*cout << "Transformation[0] = " << transformation[0] << endl;
			cout << "Transformation[1] = " << transformation[1] << endl;
			cout << "Transformation[2] = " << transformation[2] << endl;
			cout << "Transformation[3] = " << transformation[3] << endl;
			cout << "Transformation[4] = " << transformation[4] << endl;
			cout << "Transformation[5] = " << transformation[5] << endl;
			
			cout << "baseTransform[0] = " << baseTransform[0] << endl;
			cout << "baseTransform[1] = " << baseTransform[1] << endl;
			cout << "baseTransform[2] = " << baseTransform[2] << endl;
			cout << "baseTransform[3] = " << baseTransform[3] << endl;
			cout << "baseTransform[4] = " << baseTransform[4] << endl;
			cout << "baseTransform[5] = " << baseTransform[5] << endl;
			
			cout << "baseImage->GetRasterXSize() = " << baseImage->GetRasterXSize() << endl;
			cout << "baseImage->GetRasterYSize() = " << baseImage->GetRasterYSize() << endl;
			
			cout << "height = " << height << endl;
			cout << "Width = " << width << endl;
			
			cout << "baseExtentX = " << baseExtentX << endl;
			cout << "baseExtentY = " << baseExtentY << endl;
			cout << "imgExtentX = " << imgExtentX << endl;
			cout << "imgExtentY = " << imgExtentY << endl;*/
			
			// Check datasets fit within the base image.
			if(transformation[0] < baseTransform[0])
			{
				throw RSGISImageException("Images do not fit within the base image (Eastings Min)");
			}
			if(transformation[3] > baseTransform[3])
			{
				throw RSGISImageException("Images do not fit within the base image (Northings Max)");
			}
			if(imgExtentX > baseExtentX)
			{
				throw RSGISImageException("Images do not fit within the base image (Eastings Max)");
			}
			if(imgExtentY < baseExtentY)
			{
				throw RSGISImageException("Images do not fit within the base image (Northings Min)");
			}

			height = baseImage->GetRasterYSize();
			width = baseImage->GetRasterXSize();
			
			cout << "Started (total " << numDS << ") ." << flush;
			
			for(int i = 0; i < numDS; i++)
			{
				cout << "." << i << "." << flush;
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();
				
				xDiff = imgTransform[0] - baseTransform[0];
				yDiff = baseTransform[3] - imgTransform[3];
				
				xStart = floor(xDiff/baseTransform[1]);
				yStart = floor(yDiff/baseTransform[1]);
				imgData = (float *) CPLMalloc(sizeof(float)*tileXSize);
				
                if(bandsDefined)
                {
                    int nBand = 1;
                    for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                    {
                        if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                        {
                            throw RSGISImageException("Band is not within the input dataset.");
                        }
                        inputBand = dataset->GetRasterBand(*iterBands);
                        outputBand = baseImage->GetRasterBand(nBand);
                        for(int m = 0; m < tileYSize; m++)
                        {
                            inputBand->RasterIO(GF_Read, 0, m, tileXSize, 1, imgData, tileXSize, 1, GDT_Float32, 0, 0);
                            outputBand->RasterIO(GF_Write, xStart, (yStart + m), tileXSize, 1, imgData, tileXSize, 1, GDT_Float32, 0, 0);
                        }
                        ++nBand;
                    }
                }
                else 
                {
                    if(numberBands != dataset->GetRasterCount())
                    {
                        throw RSGISImageException("The number of bands in the input datasets and base dataset need to be the same.");
                    }
                    
                    for(int n = 1; n <= numberBands; n++)
                    {
                        inputBand = dataset->GetRasterBand(n);
                        outputBand = baseImage->GetRasterBand(n);
                        for(int m = 0; m < tileYSize; m++)
                        {
                            inputBand->RasterIO(GF_Read, 0, m, tileXSize, 1, imgData, tileXSize, 1, GDT_Float32, 0, 0);
                            outputBand->RasterIO(GF_Write, xStart, (yStart + m), tileXSize, 1, imgData, tileXSize, 1, GDT_Float32, 0, 0);
                        }
                    }
                }
				
				delete imgData;
                GDALClose(dataset);
			}
			cout << ".complete\n";
		}
		catch(RSGISImageBandException e)
		{
			if(imgData != NULL)
			{
				delete imgData;
			}
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}
		
		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
	}
	
	RSGISImageMosaic::~RSGISImageMosaic()
	{
		
	}
}}

