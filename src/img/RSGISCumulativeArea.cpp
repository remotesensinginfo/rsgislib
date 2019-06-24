/*
 *  RSGISCumulativeArea.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/08/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISCumulativeArea.h"


namespace rsgis { namespace img {
	
	
	RSGISCumulativeArea::RSGISCumulativeArea(int numOutBands, rsgis::math::Matrix *bandValuesWidths) : RSGISCalcImageValue(numOutBands)
	{
		this->bandValuesWidths = bandValuesWidths;
	}
	
	void RSGISCumulativeArea::calcImageValue(float *bandValues, int numBands, double *output) 
	{	
		if(bandValuesWidths->n != numBands)
		{
			throw RSGISImageCalcException("Band values (i.e., wavelength) and widths need to be defined for all image bands");
		}
		if(numBands != this->numOutBands)
		{
			throw RSGISImageCalcException("The number of output bands needs to be equal to the number of input bands");
		}
		
		bool first = true;
		for(int i = 0; i < numBands; ++i)
		{
			if(first)
			{
				output[i] = bandValuesWidths->matrix[(i*2)+1] * bandValues[i];
				first = false;
			}
			else
			{
				output[i] = output[i-1] + (bandValuesWidths->matrix[(i*2)+1] * bandValues[i]);
			}
		}

	}
	
	RSGISCumulativeArea::~RSGISCumulativeArea()
	{
		
	}
	
}}



