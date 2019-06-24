/*
 *  RSGISCumulativeAreaClassifier.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2010.
 *  Copyright 2010 RSGISLib.
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

#include "RSGISCumulativeAreaClassifier.h"

namespace rsgis { namespace classifier {
	
	
	RSGISCumulativeAreaClassifierGenRules::RSGISCumulativeAreaClassifierGenRules(int numOutBands, rsgis::math::Matrix *bandValuesWidths, rsgis::math::Matrix *samples) : RSGISCalcImageValue(numOutBands)
	{
		this->bandValuesWidths = bandValuesWidths;
		this->samples = samples;
	}
	
	void RSGISCumulativeAreaClassifierGenRules::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if(this->samples->m != this->numOutBands)
		{
			throw rsgis::img::RSGISImageCalcException("The number of output image bands needs to be equal to the number of samples.");
		}
		
		if(bandValuesWidths->n != numBands)
		{
			throw rsgis::img::RSGISImageCalcException("Band values (i.e., wavelength) and widths need to be defined for all image bands");
		}
		
		float *cumulativeArea = new float[numBands];
		bool first = true;
		for(int i = 0; i < numBands; ++i)
		{
			if(first)
			{
				cumulativeArea[i] = bandValuesWidths->matrix[(i*2)+1] * bandValues[i];
				first = false;
			}
			else
			{
				cumulativeArea[i] = cumulativeArea[i-1] + (bandValuesWidths->matrix[(i*2)+1] * bandValues[i]);
			}
		}

		
		for(int i = 0; i < this->samples->m; ++i)
		{
			output[i] = this->calcEuclideanDistance(this->samples, i, cumulativeArea);
		}
		
		delete[] cumulativeArea;
	}
	
	float RSGISCumulativeAreaClassifierGenRules::calcEuclideanDistance(rsgis::math::Matrix *samples, int sampleNum, float *data)
	{
		float eucDist = 0;
		double sumSQs = 0;
		float tempVal = 0;
		
		for(int i = 0; i < samples->n; ++i)
		{
			tempVal = samples->matrix[((i*samples->m)+sampleNum)] - data[i];
			sumSQs += (tempVal * tempVal);
		}
		
		eucDist = sqrt(sumSQs);
		
		return eucDist;
	}
	
	RSGISCumulativeAreaClassifierGenRules::~RSGISCumulativeAreaClassifierGenRules()
	{

	}
	
	RSGISCumulativeAreaClassifierDecide::RSGISCumulativeAreaClassifierDecide(int numOutBands, double threshold) : rsgis::img::RSGISCalcImageValue(numOutBands)
	{
		this->threshold = threshold;
	}
	
	void RSGISCumulativeAreaClassifierDecide::calcImageValue(float *bandValues, int numBands, double *output) 
	{		
		bool first = true;
		int minIdx = 0;
		float minVal = 0;
		
		for(int i = 0; i < numBands; ++i)
		{
			if(first)
			{
				minIdx = i;
				minVal = bandValues[i];
				first = false;
			}
			else if(bandValues[i] < minVal)
			{
				minIdx = i;
				minVal = bandValues[i];
			}
		}
		
		if(minVal < threshold)
		{
			output[0] = minIdx+1;
		}
		else 
		{
			output[0] = -1; //1/0;
		}

		
	}
	
	RSGISCumulativeAreaClassifierDecide::~RSGISCumulativeAreaClassifierDecide()
	{
		
	}
	
}}

