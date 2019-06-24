/*
 *  RSGISColourUsingHue.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/11/2010.
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

#include "RSGISColourUsingHue.h"


namespace rsgis { namespace img {
	

	RSGISColourUsingHue::RSGISColourUsingHue(int numOutBands, int imageBand, float lowerRangeVal, float upperRangeVal, float background): RSGISCalcImageValue(numOutBands)
	{
		this->imageBand = imageBand;
		this->lowerRangeVal = lowerRangeVal;
		this->upperRangeVal = upperRangeVal;
		this->background = background;
	}
	
	void RSGISColourUsingHue::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		
		float inValue = bandValues[imageBand];
        if(boost::math::isnan(inValue))
        {
            output[0] = 0;
			output[1] = 0;
			output[2] = 0;
        }
        else if((inValue < lowerRangeVal) | (inValue > upperRangeVal) | (inValue == background))
		{
			output[0] = 0;
			output[1] = 0;
			output[2] = 0;
		}
		else 
		{
			float hueValue = ((inValue - lowerRangeVal) / (upperRangeVal - lowerRangeVal))*360;
			
			if(hueValue <= 60)
			{
				output[0] = 255;
				output[1] = ((hueValue)/60)*255;
				output[2] = 0;
			}
			else if((hueValue > 60) & (hueValue <= 120))
			{
				output[0] = 255 - (((hueValue-60)/60)*255);
				output[1] = 255;
				output[2] = 0;
			}
			else if((hueValue > 120) & (hueValue <= 180))
			{
				output[0] = 0;
				output[1] = 255;
				output[2] = (((hueValue-120)/60)*255);
			}
			else if((hueValue > 180) & (hueValue <= 240))
			{
				output[0] = 0;
				output[1] = 255 - (((hueValue-180)/60)*255);
				output[2] = 255;
			}
			else if((hueValue > 240) & (hueValue <= 300))
			{
				output[0] = (((hueValue-240)/60)*255);
				output[1] = 0; 
				output[2] = 255;
			}
			else if((hueValue > 300) & (hueValue <= 360))
			{
				output[0] = 255;
				output[1] = 0; 
				output[2] = 255 - (((hueValue-300)/60)*255);
			}
		}
		
	}
	
	RSGISColourUsingHue::~RSGISColourUsingHue()
	{
		
	}
	
}}





