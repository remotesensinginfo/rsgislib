/*
 *  RSGISCalculateImageMovementSpeed.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/11/2010.
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

#include "RSGISCalculateImageMovementSpeed.h"

namespace rsgis{namespace img{
	 
	RSGISCalculateImageMovementSpeed::RSGISCalculateImageMovementSpeed(int numberOutBands, int numImages, unsigned int *imageBands, float *times, float upper, float lower): RSGISCalcImageValue(numberOutBands)
	{
		this->numImages = numImages;
		this->imageBands = imageBands;
		this->times = times;
		this->lower = lower;
		this->upper = upper;
	}
	
	void RSGISCalculateImageMovementSpeed::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		float meanSum = 0;
		float min = 0;
		float max = 0;
		
		bool first = false;
		float displacement = 0;
		float movement = 0;
		float time = 0;
		unsigned int valCount = 0;
		
        int idx = 0;
		for(int i = 0; i < (this->numImages-1); ++i)
		{
            idx = (i * 2) + 3;
            
            if(((boost::math::isnan)(bandValues[imageBands[i]])) | ((boost::math::isnan)(bandValues[imageBands[i+1]])))
            {
                displacement = std::numeric_limits<float>::signaling_NaN();
                movement = std::numeric_limits<float>::signaling_NaN();
            }
            else
            {
                displacement = bandValues[imageBands[i]] - bandValues[imageBands[i+1]];
                time = times[i+1] - times[i];
                movement = displacement/time;
            }
			
			//cout << "[" << displacement << ", " << time << ", " << movement << "]" << endl;
			output[idx] = displacement;
            output[idx+1] = movement;
			
			if((displacement > lower) & (displacement < upper))
			{
				if(first)
				{
					meanSum = movement;
					min = movement;
					max = movement;
					first = true;
				}
				else
				{
					if(movement < min)
					{
						min = movement;
					}
					else if(movement > max)
					{
						max = movement;
					}
					meanSum += movement;
				}
				++valCount;
			}
		}
		
		if(valCount > 0)
		{
			output[0] = meanSum/(valCount);
			output[1] = min;
			output[2] = max;
		}
		else
		{
			output[0] = std::numeric_limits<float>::signaling_NaN();
			output[1] = std::numeric_limits<float>::signaling_NaN();
			output[2] = std::numeric_limits<float>::signaling_NaN();
		}
		
	}

	RSGISCalculateImageMovementSpeed::~RSGISCalculateImageMovementSpeed()
	{
		
	}
	
}}

