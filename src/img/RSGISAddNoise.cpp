/*
 *  RSGISAddNoise.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/08/2008.
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

#include "RSGISAddNoise.h"

namespace rsgis{namespace img{
	
	RSGISAddRandomNoise::RSGISAddRandomNoise(int numberOutBands, float scale) : RSGISCalcImageValue(numberOutBands)
	{
		this->scale = scale;
	}
	
	void RSGISAddRandomNoise::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if(numOutBands != numBands)
		{
			RSGISImageCalcException("Expecting the number of output bands to be equal to number of input bands.");
		}
		
		for(int i = 0; i < numBands; i++)
		{
			output[i] = bandValues[i] + (rand() * scale);
		}
	}
	
	RSGISAddRandomNoise::~RSGISAddRandomNoise()
	{
		
	}
	
	RSGISAddRandomGaussianNoisePercent::RSGISAddRandomGaussianNoisePercent(int numberOutBands, float scale) : RSGISCalcImageValue(numberOutBands)
	{
		this->scale = scale;
		this->gRand = new rsgis::math::RSGISRandDistroGaussian(0,1);
	}
	
	void RSGISAddRandomGaussianNoisePercent::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if(numOutBands != numBands)
		{
			RSGISImageCalcException("Expecting the number of output bands to be equal to number of input bands.");
		}
		
		for(int i = 0; i < numBands; i++)
		{
			output[i] = bandValues[i] + (bandValues[i] * gRand->calcRand() * scale);
		}
	}
	
	RSGISAddRandomGaussianNoisePercent::~RSGISAddRandomGaussianNoisePercent()
	{
		delete gRand;
	}
	
}}


