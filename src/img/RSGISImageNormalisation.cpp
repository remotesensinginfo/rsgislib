/*
 *  RSGISImageNormalisation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/05/2008.
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

#include "RSGISImageNormalisation.h"


namespace rsgis{namespace img{

	RSGISImageNormalisation::RSGISImageNormalisation()
	{
		
	}
	
	void RSGISImageNormalisation::normaliseImage(GDALDataset *dataset, double *imageMax, double *imageMin, double *outMax, double *outMin, bool calcStats, std::string outputImage)
	{
		GDALDataset **datasets = NULL;
		RSGISImageStatistics *calcImageStats = NULL;
		ImageStats **stats = NULL;
		RSGISCalcImage *calcImg = NULL;
		RSGISNormaliseImage *normImage = NULL;

		try
		{
			int numBands = dataset->GetRasterCount();
			datasets = new GDALDataset*[1];
			datasets[0] = dataset;
			if(calcStats)
			{
				if(imageMax == NULL)
				{
					imageMax = new double[numBands];
				}
				if(imageMin == NULL)
				{
					imageMin = new double[numBands];
				}
				
				stats = new ImageStats*[numBands];
				for(int i = 0; i < numBands; i++)
				{
					stats[i] = new ImageStats();
				}
				calcImageStats = new RSGISImageStatistics();
				calcImageStats->calcImageStatistics(datasets, 1, stats, numBands, false);
				
				for(int i = 0; i < numBands; i++)
				{
					std::cout << "band " << i << " Min = " << stats[i]->min << " Max = " << stats[i]->max << std::endl;
					imageMin[i] = stats[i]->min;
					imageMax[i] = stats[i]->max;
				}
				
				for(int i = 0; i < numBands; i++)
				{
					delete stats[i];
				}
				delete[] stats;
				delete calcImageStats;
			}
			
			normImage = new RSGISNormaliseImage(numBands, imageMax, imageMin, outMax, outMin); //??? creates what we are to do with the calc image?
			calcImg = new RSGISCalcImage(normImage, "", true);
			calcImg->calcImage(datasets, 1, outputImage);
			
		}
		catch(RSGISImageCalcException &e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		catch(RSGISImageBandException &e)
		{
			if(datasets != NULL)
			{
				delete[] datasets;
			}
			throw e;
		}
		
		if(datasets != NULL)
		{
			delete[] datasets;
		}
	}
	
	RSGISNormaliseImage::RSGISNormaliseImage(int numberOutBands, double *imageMaxIn, double *imageMinIn, double *outMaxIn, double *outMinIn) : RSGISCalcImageValue(numberOutBands)
	{
		this->imageMax = imageMaxIn;
		this->imageMin = imageMinIn;
		this->outMax = outMaxIn;
		this->outMin = outMinIn;
	}
	
	void RSGISNormaliseImage::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		double inDiff = 0;
		double norm2min = 0;
		double outDiff = 0;
		for(int i = 0; i < numBands; i++)
		{
			if(bandValues[i] < imageMin[i])
			{
				output[i] = outMin[i];
			}
			else if(bandValues[i] > imageMax[i])
			{
				output[i] = outMax[i];
			}
			else 
			{
				inDiff = imageMax[i] - imageMin[i];
				norm2min = bandValues[i] - imageMin[i];
				outDiff = outMax[i] - outMin[i];
				output[i] = ((norm2min/inDiff)*outDiff)+outMin[i];
			}
		}
	}
	
	void RSGISNormaliseImage::calcImageValue(float *bandValues, int numBands) 
	{
		throw RSGISImageCalcException("No implemented");
	}
	
	void RSGISNormaliseImage::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) 
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISNormaliseImage::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) 
	{
		throw RSGISImageCalcException("No implemented");
	}
	
	void RSGISNormaliseImage::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw RSGISImageCalcException("No implemented");
	}

	bool RSGISNormaliseImage::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw RSGISImageCalcException("No implemented");
	}
	
	RSGISNormaliseImage::~RSGISNormaliseImage()
	{
		
	}

}}
