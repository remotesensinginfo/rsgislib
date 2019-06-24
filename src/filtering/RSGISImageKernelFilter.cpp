/*
 *  RSGISImageKernelFilter.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/12/2008.
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

#include "RSGISImageKernelFilter.h"

namespace rsgis{namespace filter{

	RSGISImageKernelFilter::RSGISImageKernelFilter(int numberOutBands, int size, std::string filenameEnding, ImageFilter *filter) : RSGISImageFilter(numberOutBands, size, filenameEnding)
	{
		this->filter = filter;
	}
	
	void RSGISImageKernelFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{		
		if(winSize != size)
		{
			throw rsgis::img::RSGISImageCalcException("Filter Size and window size do not match.");
		}
		
		//dataBlock[i][j][k]; i = band; j = y axis; k = x axis
		double outputValue = 0;
		for(int i = 0; i < numBands; i++)
		{
			outputValue = 0;
			for(int j = 0; j < size; j++)
			{
				for(int k = 0; k < size; k++)
				{
					outputValue = outputValue + (dataBlock[i][j][k] * filter->filter[j][k]);
				}
			}
			output[i] = outputValue;
		}
	}
	
	bool RSGISImageKernelFilter::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}

	void RSGISImageKernelFilter::exportAsImage(std::string filename)
	{
		GDALAllRegister();
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		GDALDriver *gdalDriver = NULL;
		float *outputData = NULL;
		
		try
		{
			gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(gdalDriver == NULL)
			{
				throw RSGISImageException("ENVI driver does not exists..");
			}
			outputImageDS = gdalDriver->Create(filename.c_str(), filter->size, filter->size, 1, GDT_Float32, gdalDriver->GetMetadata());
			outputRasterBand = outputImageDS->GetRasterBand(1);
			outputData = (float *) CPLMalloc(sizeof(float)*filter->size);
			
			for(int i = 0; i < filter->size; i++)
			{
				for(int j = 0; j < filter->size; j++)
				{
					outputData[j] = filter->filter[i][j];
				}
				outputRasterBand->RasterIO(GF_Write, 0, i, filter->size, 1, outputData, filter->size, 1, GDT_Float32, 0, 0);
			}	
		}
		catch(rsgis::RSGISImageException &e)
		{
			if(outputData == NULL)
			{
				delete outputData;
			}
			if(gdalDriver == NULL)
			{
				delete gdalDriver;
			}
			throw e;
		}
		
		if(outputData != NULL)
		{
			delete outputData;
		}
		if(gdalDriver == NULL)
		{
			delete gdalDriver;
		}
		GDALClose(outputImageDS);
	}
	
	RSGISImageKernelFilter::~RSGISImageKernelFilter()
	{
		
	}
	
}}


