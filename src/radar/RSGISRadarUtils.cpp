/*
 *  RSGISRadarUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 22/07/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#include "RSGISRadarUtils.h"

namespace rsgis { namespace radar{
	
	RSGISConvert2dB::RSGISConvert2dB(int numOutputBands, double calFactor) : rsgis::img::RSGISCalcImageValue(numOutputBands)
	{
		this->calFactor = calFactor;
	}
	
	void RSGISConvert2dB::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		double intensity = 0;
		double dB = 0;
		
		for(int i = 0; i < numBands; i++)
		{
			intensity = bandValues[i];
			if(intensity > 0)
			{
				dB = (10 * log10(intensity)) + this->calFactor;
				output[i] = dB;
			}
			else 
			{
				output[i] = 0;
			}
		}
	}
	
	RSGISConvert2dB::~RSGISConvert2dB()
	{
		
	}
	
}}
