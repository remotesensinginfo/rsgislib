/*
 *  RSGISGenerateFilter.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/05/2008.
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

#include "RSGISGenerateFilter.h"

namespace rsgis{namespace filter{

	RSGISGenerateFilter::RSGISGenerateFilter(RSGISCalcFilter *calcFilter)
	{
		this->calcFilter = calcFilter;
	}
	
	ImageFilter* RSGISGenerateFilter::generateFilter(int size)
	{
		if(size % 2 == 0)
		{
			throw RSGISImageFilterException("The filter size needs to be an odd number.");
		}
		
		ImageFilter *filter = new ImageFilter;
		
		float halfSize = size/2;
		int halfSizeFloor = floor(halfSize);
		
		int x = halfSizeFloor * (-1);
		int y = halfSizeFloor * (-1);
		
		filter->size = size;
				
		filter->filter = new float*[size];
		for(int i = 0; i < size; i++)
		{
			filter->filter[i] = new float[size];
			x = halfSizeFloor * (-1);
			for(int j = 0; j < size; j++)
			{
				filter->filter[i][j] = calcFilter->calcFilterValue(x, y);
				x++;
			}
			y++;
		}
		
		return filter;
	}
	
	RSGISGenerateFilter::~RSGISGenerateFilter()
	{
		
	}
	
}}
