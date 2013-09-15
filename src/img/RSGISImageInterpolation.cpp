/*
 *  RSGISImageInterpolation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/05/2008.
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

#include "RSGISImageInterpolation.h"

namespace rsgis{namespace img{
	
	RSGISImageInterpolation::RSGISImageInterpolation(RSGISInterpolator *interpolator)
	{
		this->interpolator = interpolator;
	}
	
	void RSGISImageInterpolation::interpolateNewImage(GDALDataset *data,
															  double outputXResolution, 
															  double outputYResolution, 
															  std::string filename) throw(rsgis::RSGISFileException, rsgis::RSGISImageException)
	{
		// Image Data Stores.
		float *scanline0 = NULL;
		float *scanline1 = NULL;
		float *scanline2 = NULL;
		double *transformation = NULL;
		float *newLine = NULL;
		double *pixels = NULL;
		GDALDriver *poDriver = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		GDALRasterBand *rasterband = NULL;
		try
		{
			GDALDataset *output;
			/********************** Calculate Scaling *************************/
			transformation = new double[6];
			data->GetGeoTransform(transformation);	
			
			int dataXSize = data->GetRasterXSize();
			int dataYSize = data->GetRasterYSize();
			
			double inputXResolution = transformation[1];
			double inputYResolution = transformation[5];
			
			/*if(inputXResolution < 0)
			{
				inputXResolution = inputXResolution * (-1);
			}
			
			if(inputYResolution < 0)
			{
				inputYResolution = inputYResolution * (-1);
			}*/
			
			double xScale = inputXResolution/outputXResolution;
			double yScale = inputYResolution/outputYResolution;
			
			int xSize = static_cast<int>(dataXSize*xScale);  //mathUtils.round(dataXSize*xScale);
			int ySize = static_cast<int>(dataYSize*yScale); //mathUtils.round(dataYSize*yScale);;
			int bands = data->GetRasterCount();
			
			std::cout << "size [" << xSize << "," << ySize << "]\n";
			
			transformation[1] = outputXResolution;
			transformation[5] = outputYResolution;
			/*******************************************************************/
			
			/************ Output Image with the New registration *******************/   
			poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(poDriver == NULL)
			{
				throw rsgis::RSGISImageException("Could not find ENVI driver");
			}
			
			output = poDriver->Create(filename.c_str(), xSize, ySize, bands, GDT_Float32, poDriver->GetMetadata());
			output->SetGeoTransform(transformation);
			output->SetProjection(data->GetProjectionRef());
			
			
			scanline0 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			scanline1 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			scanline2 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			
			newLine = (float *)CPLMalloc(sizeof(float)*xSize);
			pixels = new double[9];
			
			int column = 0;
			int row = 0;
			double xShift = 0;
			double yShift = 0;
						
			for(int n = 1; n <= bands; n++)
			{
				std::cout << "Interpolating band "  << n << ".." << std::endl;
				outputRasterBand = output->GetRasterBand(n);
				rasterband = data->GetRasterBand(n);
				
				int feedback = ySize/10;
				int feedbackCounter = 0;
				std::cout << "Started " << std::flush;
				for( int i = 0; i < ySize; i++)
				{
					if((ySize > 10) && (i % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
					
					
					yShift = this->findFloatingPointComponent(((i*outputYResolution)/inputYResolution),
															  &row);
					if(row == 0)
					{
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline0, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline1, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row+1, 
											 dataXSize, 
											 1, 
											 scanline2, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
					}
					else if(row == (dataYSize-1))
					{
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row-1, 
											 dataXSize, 
											 1, 
											 scanline0, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline1, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline2, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
					}
					else
					{
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row-1, 
											 dataXSize, 
											 1, 
											 scanline0, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline1, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row+1, 
											 dataXSize, 
											 1, 
											 scanline2, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
					}
					for(int j = 0; j < xSize; j++)
					{
						xShift = this->findFloatingPointComponent(((j*outputXResolution)/inputXResolution), 
																  &column);
						if(column == 0)
						{
							//Column 1
							pixels[0] = scanline0[column];
							pixels[3] = scanline1[column];
							pixels[6] = scanline2[column];
							//Column 2
							pixels[1] = scanline0[column];
							pixels[4] = scanline1[column];
							pixels[7] = scanline2[column];
							//Column 1
							pixels[2] = scanline0[column+1];
							pixels[5] = scanline1[column+1];
							pixels[8] = scanline2[column+1];
						}
						else if(column == (dataXSize-1))
						{
							//Column 1
							pixels[0] = scanline0[column-1];
							pixels[3] = scanline1[column-1];
							pixels[6] = scanline2[column-1];
							//Column 2
							pixels[1] = scanline0[column];
							pixels[4] = scanline1[column];
							pixels[7] = scanline2[column];
							//Column 1
							pixels[2] = scanline0[column];
							pixels[5] = scanline1[column];
							pixels[8] = scanline2[column];
						}
						else
						{
							//Column 1
							pixels[0] = scanline0[column-1];
							pixels[3] = scanline1[column-1];
							pixels[6] = scanline2[column-1];
							//Column 2
							pixels[1] = scanline0[column];
							pixels[4] = scanline1[column];
							pixels[7] = scanline2[column];
							//Column 1
							pixels[2] = scanline0[column+1];
							pixels[5] = scanline1[column+1];
							pixels[8] = scanline2[column+1];
						}
						newLine[j] = interpolator->interpolate(xShift,yShift,pixels);
					}
					outputRasterBand->RasterIO(GF_Write, 0, i, xSize, 1, newLine, xSize, 1, GDT_Float32, 0, 0);
				}
				std::cout << " Completed\n";
			}
			GDALClose(output);
			std::cout << "Interpolation complete\n";
		}
		catch(rsgis::RSGISFileException e)
		{
			if( transformation != NULL )
			{
				delete transformation;
			}
			if(newLine != NULL)
			{
				delete newLine;
			}
			if( pixels != NULL )
			{
				delete pixels;
			}
			if( scanline0 != NULL )
			{
				delete scanline0;
			}
			if( scanline1 != NULL )
			{
				delete scanline1;
			}
			if( scanline2 != NULL )
			{
				delete scanline2;
			}
			throw e;
		}
		catch(rsgis::RSGISImageException e)
		{
			if( transformation != NULL )
			{
				delete transformation;
			}
			if(newLine != NULL)
			{
				delete newLine;
			}
			if( pixels != NULL )
			{
				delete pixels;
			}
			if( scanline0 != NULL )
			{
				delete scanline0;
			}
			if( scanline1 != NULL )
			{
				delete scanline1;
			}
			if( scanline2 != NULL )
			{
				delete scanline2;
			}
			throw e;
		}
		
		if( transformation != NULL )
		{
			delete transformation;
		}
		if(newLine != NULL)
		{
			delete newLine;
		}
		if( pixels != NULL )
		{
			delete pixels;
		}
		if( scanline0 != NULL )
		{
			delete scanline0;
		}
		if( scanline1 != NULL )
		{
			delete scanline1;
		}
		if( scanline2 != NULL )
		{
			delete scanline2;
		}
	}
	
	void RSGISImageInterpolation::interpolateNewImage(GDALDataset *data,
															  double outputXResolution, 
															  double outputYResolution, 
															  std::string filename,
															  int band) throw(rsgis::RSGISFileException, rsgis::RSGISImageException)
	{
		// Image Data Stores.
		float *scanline0 = NULL;
		float *scanline1 = NULL;
		float *scanline2 = NULL;
		double *transformation = NULL;
		float *newLine = NULL;
		double *pixels = NULL;
		GDALDriver *poDriver = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		GDALRasterBand *rasterband = NULL;
		try
		{
			GDALDataset *output;
			/********************** Calculate Scaling *************************/
			transformation = new double[6];
			data->GetGeoTransform(transformation);	
			
			int dataXSize = data->GetRasterXSize();
			int dataYSize = data->GetRasterYSize();
			
			double inputXResolution = transformation[1];
			if(inputXResolution < 0)
			{
				inputXResolution = inputXResolution * (-1);
			}
			double inputYResolution = transformation[5];
			if(inputYResolution < 0)
			{
				inputYResolution = inputYResolution * (-1);
			}
			
			double xScale = inputXResolution/outputXResolution;
			double yScale = inputYResolution/outputYResolution;
			
			int xSize = static_cast<int>(dataXSize*xScale);  //mathUtils.round(dataXSize*xScale);
			int ySize = static_cast<int>(dataYSize*yScale); //mathUtils.round(dataYSize*yScale);;
			
			transformation[1] = outputXResolution;
			transformation[5] = outputYResolution;
			/*******************************************************************/
			
			/************ Output Image with the New registration *******************/   
			poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(poDriver == NULL)
			{
				throw rsgis::RSGISImageException("Could not find ENVI driver");
			}
			
			output = poDriver->Create(filename.c_str(), xSize, ySize, 1, GDT_Float32, poDriver->GetMetadata());
			output->SetGeoTransform(transformation);
			output->SetProjection(data->GetProjectionRef());
			
			
			scanline0 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			scanline1 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			scanline2 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			
			newLine = (float *)CPLMalloc(sizeof(float)*xSize);
			pixels = new double[9];
			
			int column = 0;
			int row = 0;
			double xShift = 0;
			double yShift = 0;
			
			outputRasterBand = output->GetRasterBand(1);
			rasterband = data->GetRasterBand(band);
			
			
			int feedback = ySize/10;
			int feedbackCounter = 0;
			std::cout << "Started Interpolating";
			
			for( int i = 0; i < ySize; i++)
			{
				if((ySize > 10) && (i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << "..";
					feedbackCounter = feedbackCounter + 10;
				}
				yShift = this->findFloatingPointComponent(((i*outputYResolution)/inputYResolution),
														  &row);
				if(row == 0)
				{
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline0, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline1, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row+1, 
										 dataXSize, 
										 1, 
										 scanline2, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
				}
				else if(row == (dataYSize-1))
				{
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row-1, 
										 dataXSize, 
										 1, 
										 scanline0, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline1, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline2, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
				}
				else
				{
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row-1, 
										 dataXSize, 
										 1, 
										 scanline0, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline1, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row+1, 
										 dataXSize, 
										 1, 
										 scanline2, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
				}
				for(int j = 0; j < xSize; j++)
				{
					xShift = this->findFloatingPointComponent(((j*outputXResolution)/inputXResolution), 
															  &column);
					if(column == 0)
					{
						//Column 1
						pixels[0] = scanline0[column];
						pixels[3] = scanline1[column];
						pixels[6] = scanline2[column];
						//Column 2
						pixels[1] = scanline0[column];
						pixels[4] = scanline1[column];
						pixels[7] = scanline2[column];
						//Column 1
						pixels[2] = scanline0[column+1];
						pixels[5] = scanline1[column+1];
						pixels[8] = scanline2[column+1];
					}
					else if(column == (dataXSize-1))
					{
						//Column 1
						pixels[0] = scanline0[column-1];
						pixels[3] = scanline1[column-1];
						pixels[6] = scanline2[column-1];
						//Column 2
						pixels[1] = scanline0[column];
						pixels[4] = scanline1[column];
						pixels[7] = scanline2[column];
						//Column 1
						pixels[2] = scanline0[column];
						pixels[5] = scanline1[column];
						pixels[8] = scanline2[column];
					}
					else
					{
						//Column 1
						pixels[0] = scanline0[column-1];
						pixels[3] = scanline1[column-1];
						pixels[6] = scanline2[column-1];
						//Column 2
						pixels[1] = scanline0[column];
						pixels[4] = scanline1[column];
						pixels[7] = scanline2[column];
						//Column 1
						pixels[2] = scanline0[column+1];
						pixels[5] = scanline1[column+1];
						pixels[8] = scanline2[column+1];
					}
					newLine[j] = interpolator->interpolate(xShift,yShift,pixels);
				}
				outputRasterBand->RasterIO(GF_Write, 0, i, xSize, 1, newLine, xSize, 1, GDT_Float32, 0, 0);
			}
			GDALClose(output);
			std::cout << ".. Complete." << std::endl;
		}
		catch(rsgis::RSGISFileException e)
		{
			if( transformation != NULL )
			{
				delete transformation;
			}
			if(newLine != NULL)
			{
				delete newLine;
			}
			if( pixels != NULL )
			{
				delete pixels;
			}
			if( scanline0 != NULL )
			{
				delete scanline0;
			}
			if( scanline1 != NULL )
			{
				delete scanline1;
			}
			if( scanline2 != NULL )
			{
				delete scanline2;
			}
			throw e;
		}
		catch(rsgis::RSGISImageException e)
		{
			if( transformation != NULL )
			{
				delete transformation;
			}
			if(newLine != NULL)
			{
				delete newLine;
			}
			if( pixels != NULL )
			{
				delete pixels;
			}
			if( scanline0 != NULL )
			{
				delete scanline0;
			}
			if( scanline1 != NULL )
			{
				delete scanline1;
			}
			if( scanline2 != NULL )
			{
				delete scanline2;
			}
			throw e;
		}
		
		if( transformation != NULL )
		{
			delete transformation;
		}
		if(newLine != NULL)
		{
			delete newLine;
		}
		if( pixels != NULL )
		{
			delete pixels;
		}
		if( scanline0 != NULL )
		{
			delete scanline0;
		}
		if( scanline1 != NULL )
		{
			delete scanline1;
		}
		if( scanline2 != NULL )
		{
			delete scanline2;
		}

	}
	
	double RSGISImageInterpolation::findFloatingPointComponent(double floatingPointNum, int *integer)
	{
		//std::std::cout << "Starting find floating point\n";
		*integer = 0;
		bool negative = false;
		if(floatingPointNum < 0)
		{
			floatingPointNum = floatingPointNum * (-1);
			negative = true;
		}
		
		int reduction = 100000000;
		
		while(floatingPointNum > 1)
		{
			while(floatingPointNum > reduction & reduction != 0)
			{
				//std::std::cout << "floatingPointNum = " << floatingPointNum << " reduction = " << reduction << std::std::endl;
				floatingPointNum = floatingPointNum - reduction;
				*integer = *integer + reduction;
			}
			//std::std::cout << "Adjust reduction: floatingPointNum = " << floatingPointNum << std::std::endl;
			reduction = reduction / 10;
		}
		if(floatingPointNum > -0.000000001 & floatingPointNum < 0.000000001)
		{
			floatingPointNum = 0;
		}
		if(floatingPointNum > 0.999999999 & floatingPointNum < 1.000000001)
		{
			floatingPointNum = 0;
			*integer = *integer + 1;
		}
		
		if( negative )
		{
			*integer = (*integer) * (-1);
		}
		//std::std::cout << "finished floating point\n";
		return floatingPointNum;
	}
	
	void RSGISImageInterpolation::findOutputResolution(GDALDataset *dataset, float scale, int *outResolutionX, int *outResolutionY)
	{
		double *transformation = new double[6];
		dataset->GetGeoTransform(transformation);	
		int inResolutionX = transformation[1];
		int inResolutionY = transformation[5];
		
		*outResolutionX = ceil(inResolutionX * scale);
		*outResolutionY = ceil(inResolutionY * scale);
		delete[] transformation;
	}
	
	RSGISImageInterpolation::~RSGISImageInterpolation()
	{
		
	}
    
    
    
    
    
    RSGISPopulateImageFromInterpolator::RSGISPopulateImageFromInterpolator()
    {
        
    }
    
    void RSGISPopulateImageFromInterpolator::populateImage(rsgis::math::RSGIS2DInterpolator *interpolator, GDALDataset *image)throw(rsgis::RSGISImageException, rsgis::math::RSGISInterpolationException)
    {
        try
        {
            int xBlockSize = 0;
            int yBlockSize = 0;
            int height = 0;
            int width = 0;
            float *imgData = NULL;
            GDALRasterBand *outputRasterBand = image->GetRasterBand(1);
            double *gdalTransform = new double[6];
            image->GetGeoTransform(gdalTransform);
            width = image->GetRasterXSize();
            height = image->GetRasterYSize();
            
            outputRasterBand->GetBlockSize(&xBlockSize, &yBlockSize);
            
            int bufferSize = yBlockSize * width;
            imgData = (float *) CPLMalloc(sizeof(float)*(bufferSize));
            
            int nYBlocks = floor(((double)height) / ((double)yBlockSize));
            int remainRows = height - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            double tlX = gdalTransform[0];
            double tlY = gdalTransform[3];
            double xRes = gdalTransform[1];
            double yRes = gdalTransform[5];
            
            //std::cout << "Resolution: [" << xRes << ", " << yRes << "]\n";
            //std::cout << "TL: [" << tlX << ", " << tlY << "]\n";
            
            double cX = 0.0;
            double cY = 0.0;
            
            
			int feedback = height/10.0;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
            cY = tlY;
            for(int i = 0; i < nYBlocks; ++i)
			{
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && (((i*yBlockSize)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    cX = tlX;
                    //std::cout << "cY = " << cY << std::endl;
                    for(int j = 0; j < width; ++j)
                    {
                        imgData[(m*width)+j] = interpolator->getValue(cX, cY);
                        //std::cout << "[" << cX << "," << cY << "] = " << imgData[(m*width)+j] << std::endl;
                        cX += xRes;
                    }
                    cY += yRes;
                }
				
				rowOffset = yBlockSize * i;
                outputRasterBand->RasterIO(GF_Write, 0, rowOffset, width, yBlockSize, imgData, width, yBlockSize, GDT_Float32, 0, 0);
			}
            
            if(remainRows > 0)
            {
                for(int m = 0; m < remainRows; ++m)
                {
                    if((feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    cX = tlX;
                    for(int j = 0; j < width; ++j)
                    {
                        imgData[(m*width)+j] = interpolator->getValue(cX, cY);
                        //std::cout << "[" << cX << "," << cY << "] = " << imgData[(m*width)+j] << std::endl;
                        cX += xRes;
                    }
                    cY += yRes;
                }
				
				rowOffset = (yBlockSize * nYBlocks);
                outputRasterBand->RasterIO(GF_Write, 0, rowOffset, width, remainRows, imgData, width, remainRows, GDT_Float32, 0, 0);
            }
			std::cout << " Complete.\n";
                        
            delete[] gdalTransform;
            delete[] imgData;
            
        }
        catch(rsgis::math::RSGISInterpolationException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::math::RSGISInterpolationException(e.what());
        }
    }
    
    RSGISPopulateImageFromInterpolator::~RSGISPopulateImageFromInterpolator()
    {
        
    }
	
}}
