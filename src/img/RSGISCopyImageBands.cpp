/*
 *  RSGISCopyImageBands.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/05/2008.
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

#include "RSGISCopyImageBands.h"

namespace rsgis{namespace img{

	RSGISCopyImageBands::RSGISCopyImageBands()
	{
		
	}
	
	GDALDataset* RSGISCopyImageBands::outputImageBands(GDALDataset *inputDS, std::string outputFile, int *outBands, int numOutBands, std::string outputProj, bool useInProj)
	{
		RSGISIdentifyImageValues *copyImageValues = NULL;
		RSGISCalcImage *calcImage = NULL;
		GDALDataset **datasets = NULL;
		
		try
		{
			std::cout << "Copying Image Data ";
			datasets = new GDALDataset*[1];
			datasets[0] = inputDS;
			
			copyImageValues = new RSGISIdentifyImageValues(numOutBands, outBands);
			calcImage = new RSGISCalcImage(copyImageValues, outputProj, useInProj);
			calcImage->calcImage(datasets, 1, outputFile);
		}
		catch(RSGISImageCalcException &e)
		{
			if(copyImageValues != NULL)
			{
				delete copyImageValues;
			}
			if(calcImage != NULL)
			{
				delete calcImage;
			}
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		catch(RSGISImageBandException &e)
		{
			if(copyImageValues != NULL)
			{
				delete copyImageValues;
			}
			if(calcImage != NULL)
			{
				delete calcImage;
			}
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		
		if(copyImageValues != NULL)
		{
			delete copyImageValues;
		}
		if(calcImage != NULL)
		{
			delete calcImage;
		}
		if(datasets != NULL)
		{
			delete[] datasets;
		}
		
		GDALDataset *ds = (GDALDataset *) GDALOpen(outputFile.c_str(), GA_ReadOnly);
		if(ds == NULL)
		{
			throw RSGISImageException("Failed to copy image data.");
		}
		return ds;
	}
	
	RSGISIdentifyImageValues::RSGISIdentifyImageValues(int numberOutBands, int *outBands) : RSGISCalcImageValue(numberOutBands)
	{
		this->outBands = outBands;
	}
	
	void RSGISIdentifyImageValues::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if(numBands < numOutBands)
		{
			throw RSGISImageCalcException("Insufficient number of input bands provided.");
		}
		
		for(int i = 0; i < numOutBands; i++)
		{
			if(outBands[i] > numBands)
			{
				throw RSGISImageCalcException("Insufficient number of input bands provided.");
			}
			output[i] = bandValues[outBands[i]];
		}
	}
	
	void RSGISIdentifyImageValues::calcImageValue(float *bandValues, int numBands) 
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISIdentifyImageValues::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) 
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISIdentifyImageValues::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) 
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISIdentifyImageValues::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw RSGISImageCalcException("No implemented");
	}

	bool RSGISIdentifyImageValues::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw RSGISImageCalcException("No implemented");
	}	
	
	
}}
