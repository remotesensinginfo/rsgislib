/*
 *  RSGISPrewittFilter.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/12/2008.
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

#include "RSGISPrewittFilter.h"


namespace rsgis{namespace filter{
	
	RSGISPrewittFilter::RSGISPrewittFilter(int numberOutBands, int size, std::string filenameEnding, FilterDirection filterType) : RSGISImageFilter(numberOutBands, size, filenameEnding)
	{
		this->filterType = filterType;
		this->filterX = new int*[3];
		this->filterY = new int*[3];
		this->filterX[0] = new int[3];
		this->filterY[0] = new int[3];
		this->filterX[1] = new int[3];
		this->filterY[1] = new int[3];
		this->filterX[2] = new int[3];
		this->filterY[2] = new int[3];
		
		this->filterX[0][0] = -1;
		this->filterX[0][1] = -1;
		this->filterX[0][2] = -1;
		this->filterX[1][0] = 0;
		this->filterX[1][1] = 0;
		this->filterX[1][2] = 0;
		this->filterX[2][0] = 1;
		this->filterX[2][1] = 1;
		this->filterX[2][2] = 1;
		
		this->filterY[0][0] = -1;
		this->filterY[0][1] = 0;
		this->filterY[0][2] = 1;
		this->filterY[1][0] = -1;
		this->filterY[1][1] = 0;
		this->filterY[1][2] = 1;
		this->filterY[2][0] = -1;
		this->filterY[2][1] = 0;
		this->filterY[2][2] = 1;
	}
	
	void RSGISPrewittFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		if(this->size != 3 & winSize != 3)
		{
			throw rsgis::img::RSGISImageCalcException("Window size is required to be 3.");
		}
		//dataBlock[i][j][k]; i = band; j = y axis; k = x axis
		
		double outputValue = 0;
		
		if(filterType == RSGISPrewittFilter::x)
		{
			for(int i = 0; i < numBands; i++)
			{
				outputValue = 0;
				for(int j = 0; j < size; j++)
				{
					for(int k = 0; k < size; k++)
					{
						outputValue = outputValue + (dataBlock[i][j][k] * filterX[j][k]);
					}
				}
				output[i] = outputValue;
			}
		}
		else if(filterType == RSGISPrewittFilter::y)
		{
			for(int i = 0; i < numBands; i++)
			{
				outputValue = 0;
				for(int j = 0; j < size; j++)
				{
					for(int k = 0; k < size; k++)
					{
						outputValue = outputValue + (dataBlock[i][j][k] * filterY[j][k]);
					}
				}
				output[i] = outputValue;
			}
		}
		else if(filterType == RSGISPrewittFilter::xy)
		{
			for(int i = 0; i < numBands; i++)
			{
				outputValue = 0;
				for(int j = 0; j < size; j++)
				{
					for(int k = 0; k < size; k++)
					{
						outputValue = outputValue + (((dataBlock[i][j][k] * filterX[j][k]) + (dataBlock[i][j][k] * filterY[j][k]))/2);
					}
				}
				output[i] = outputValue;
			}
		}
		
	}

	bool RSGISPrewittFilter::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented!");
	}
	
	void RSGISPrewittFilter::exportAsImage(std::string filename)
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
			outputImageDS = gdalDriver->Create(filename.c_str(), ((this->size * 2) + 1), this->size, 1, GDT_Float32, gdalDriver->GetMetadata());
			outputRasterBand = outputImageDS->GetRasterBand(1);
			outputData = (float *) CPLMalloc(sizeof(float)*((this->size * 2) + 1));
			
			for(int i = 0; i < this->size; i++)
			{
				outputData[0] = filterX[i][0];
				outputData[1] = filterX[i][1];
				outputData[2] = filterX[i][2];
				outputData[3] = -1;
				outputData[4] = filterY[i][0];
				outputData[5] = filterY[i][1];
				outputData[6] = filterY[i][2];
				
				outputRasterBand->RasterIO(GF_Write, 0, i, this->size, 1, outputData, this->size, 1, GDT_Float32, 0, 0);
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
	
	RSGISPrewittFilter::~RSGISPrewittFilter()
	{
		
	}
}}


