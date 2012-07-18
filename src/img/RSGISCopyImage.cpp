/*
 *  RSGISCopyImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/11/2008.
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

#include "RSGISCopyImage.h"

namespace rsgis{namespace img{
	
	RSGISCopyImage::RSGISCopyImage(int numberOutBands) : RSGISCalcImageValue(numberOutBands)
	{
		
	}
	
	void RSGISCopyImage::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
	{
		if(numBands != numOutBands)
		{
			RSGISImageCalcException("The number of input bands should be equal to the number of output bands..");
		}
		
		for(int i = 0; i < numBands; i++)
		{
			output[i] = bandValues[i];
		}
	}
	
	void RSGISCopyImage::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}
	
	void RSGISCopyImage::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCopyImage::calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}
	
	void RSGISCopyImage::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}

	bool RSGISCopyImage::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}
	
	RSGISCopyImage::~RSGISCopyImage()
	{
		
	}
	
}}


