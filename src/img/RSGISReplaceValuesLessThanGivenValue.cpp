/*
 *  RSGISReplaceValuesLessThanGivenValue.cpp
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

#include "RSGISReplaceValuesLessThanGivenValue.h"

namespace rsgis{namespace img{
	
	
	RSGISReplaceValuesLessThanGivenValue::RSGISReplaceValuesLessThanGivenValue(int numberOutBands, float threshold, float value) : RSGISCalcImageValue(numberOutBands)
	{
		this->threshold = threshold;
		this->value = value;
	}
	
	void RSGISReplaceValuesLessThanGivenValue::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if(numBands != numOutBands)
		{
			throw RSGISImageCalcException("The number of input and output image bands needs to be equal.");
		}
		
		for(int i = 0; i < numBands; ++i)
		{
			if(bandValues[i] < threshold)
			{
				output[i] = value;
			}
			else
			{
				output[i] = bandValues[i];
			}
		}
	}
	
	void RSGISReplaceValuesLessThanGivenValue::calcImageValue(float *bandValues, int numBands) 
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISReplaceValuesLessThanGivenValue::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) 
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISReplaceValuesLessThanGivenValue::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) 
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISReplaceValuesLessThanGivenValue::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	bool RSGISReplaceValuesLessThanGivenValue::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	RSGISReplaceValuesLessThanGivenValue::~RSGISReplaceValuesLessThanGivenValue()
	{
		
	}
}}


