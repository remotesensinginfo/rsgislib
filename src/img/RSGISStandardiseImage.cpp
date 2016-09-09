/*
 *  RSGISStandardiseImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/08/2008.
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

#include "RSGISStandardiseImage.h"


namespace rsgis{namespace img{
	
	RSGISStandardiseImage::RSGISStandardiseImage(int numberOutBands, rsgis::math::Matrix *meanVector) : RSGISCalcImageValue(numberOutBands)
	{
		this->meanVector = meanVector;
	}
	
	void RSGISStandardiseImage::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		if(this->numOutBands != this->meanVector->n)
		{
			RSGISImageCalcException("The mean vector needs to contain the same number of enteries as the output data.");
		}
		if(this->numOutBands != numBands)
		{
			RSGISImageCalcException("The input and output images need to have the same number of bands.");
		}
		
		for(int i = 0; i < numBands; i++)
		{
			output[i] = bandValues[i] - meanVector->matrix[i];
		}
	}
	
	void RSGISStandardiseImage::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISStandardiseImage::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISStandardiseImage::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISStandardiseImage::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	bool RSGISStandardiseImage::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	RSGISStandardiseImage::~RSGISStandardiseImage()
	{
		
	}
	
}}

