/*
 *  RSGISCountValsAboveThresInCol.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/12/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISCountValsAboveThresInCol.h"

namespace rsgis{namespace img{
	 
	RSGISCountValsAboveThresInCol::RSGISCountValsAboveThresInCol(int numberOutBands, float upper, float lower): RSGISCalcImageValue(numberOutBands)
	{
		this->upper = upper;
		this->lower = lower;
	}
	
	void RSGISCountValsAboveThresInCol::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		int countVals = 0;
		for(int i = 0; i < numBands; ++i)
		{
			if((bandValues[i] < upper) & (bandValues[i] > lower))
			{
				++countVals;
			}
		}
		
		output[0] = countVals;
	}
	
	RSGISCountValsAboveThresInCol::~RSGISCountValsAboveThresInCol()
	{
		
	}
	
}}

