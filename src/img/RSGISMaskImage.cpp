/*
 *  RSGISMaskImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/05/2008.
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

#include "RSGISMaskImage.h"

namespace rsgis{namespace img{
	
	RSGISMaskImage::RSGISMaskImage()
	{
		
	}
	
	void RSGISMaskImage::maskImage(GDALDataset *dataset, GDALDataset *mask, string outputImage, double outputValue)throw(RSGISImageCalcException,RSGISImageBandException)
	{
		GDALDataset **datasets = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISApplyImageMask *applyMask = NULL;
		try
		{
			int numDS = 2;
			datasets = new GDALDataset*[numDS];
			datasets[0] = mask;
			datasets[1] = dataset;
			
			applyMask = new RSGISApplyImageMask(dataset->GetRasterCount(), outputValue);
			calcImg = new RSGISCalcImage(applyMask, "", true);
			calcImg->calcImage(datasets, numDS, outputImage);
			
		}
		catch(RSGISImageCalcException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			if(applyMask != NULL)
			{
				delete applyMask;
			}
			throw e;
		}
		catch(RSGISImageBandException e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			if(calcImg != NULL)
			{
				delete calcImg;
			}
			if(applyMask != NULL)
			{
				delete applyMask;
			}
			throw e;
		}
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
		if(calcImg != NULL)
		{
			delete calcImg;
		}
		if(applyMask != NULL)
		{
			delete applyMask;
		}
	}
	
	RSGISApplyImageMask::RSGISApplyImageMask(int numberOutBands, double outputValue) : RSGISCalcImageValue(numberOutBands)
	{
		this->outputValue = outputValue;
	}
	
	void RSGISApplyImageMask::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
	{
		if(bandValues[0] == 0)
		{
			for(int i = 0; i < numOutBands; i++)
			{
				output[i] = outputValue;
			}
		}
		else 
		{
			for(int i = 0; i < numOutBands; i++)
			{
				output[i] = bandValues[i+1];
			}
		}
	}
	
	void RSGISApplyImageMask::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISApplyImageMask::calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISApplyImageMask::calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISApplyImageMask::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}

	bool RSGISApplyImageMask::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	RSGISApplyImageMask::~RSGISApplyImageMask()
	{
		
	}
	
}}
