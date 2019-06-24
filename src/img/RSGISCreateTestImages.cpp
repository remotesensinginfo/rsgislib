/*
 *  RSGISCreateTestImages.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/12/2008.
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

#include "RSGISCreateTestImages.h"


namespace rsgis{namespace img{
	
	RSGISCreateTestImages::RSGISCreateTestImages()
	{
		
	}
	
	void RSGISCreateTestImages::createRowMajorNumberedImage(std::string outputImage, int width, int height)
	{
		GDALAllRegister();
		GDALDriver *poDriver = NULL;
		GDALRasterBand *imgBand = NULL;
		GDALDataset *outputImageDS = NULL;
		
		float *imgData = NULL;
		int counter = 0;
	
		try
		{
			if(width < 1)
			{
				throw RSGISImageException("Width is less than 1 pixel");
			}
			
			if(height < 1)
			{
				throw RSGISImageException("Height is less than 1 pixel");
			}
			
			poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(poDriver == NULL)
			{
				throw RSGISImageException("ENVI image driver is not available.");
			}
			
			// Create new file. 
			outputImageDS = poDriver->Create(outputImage.c_str(), width, height, 1, GDT_Float32, poDriver->GetMetadata());
			if(outputImageDS == NULL)
			{
				throw RSGISImageException("Image could not be created.");
			}
			
			imgData = (float *) CPLMalloc(sizeof(float)*width);
			counter = 0;
			imgBand = outputImageDS->GetRasterBand(1);
			for(int i = 0; i < height; i++)
			{
				for(int j = 0; j < width; j++)
				{
					imgData[j] = counter++;
				}
				
				imgBand->RasterIO(GF_Write, 0, i, width, 1, imgData, width, 1, GDT_Float32, 0, 0);
			}
		}
		catch(RSGISImageException &e)
		{
			if(imgData != NULL)
			{
				delete imgData;
			}
			throw e;
		}
		
		if(imgData != NULL)
		{
			delete imgData;
		}
		
		GDALClose(outputImageDS);
	}
			
	RSGISCreateTestImages::~RSGISCreateTestImages()
	{
		
	}
}}
