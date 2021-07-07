/*
 *  RSGISPolygonPixelCount.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/11/2008.
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

#include "RSGISPolygonPixelCount.h"


namespace rsgis{namespace img{
	
	RSGISPolygonPixelCount::RSGISPolygonPixelCount(int numOutputValues) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->n = 0;
	}
	
	void RSGISPolygonPixelCount::calcImageValue(float *bandValuesImage, double interceptArea, int numBands, OGRPolygon *poly, OGRPoint *pt)
	{
		if(poly->Contains(pt))
		{
			n++;
		}
	}
	
	double* RSGISPolygonPixelCount::getOutputValues() 
	{
		this->outputValues[0] = n;
		return outputValues;
	}
	
	void RSGISPolygonPixelCount::reset()
	{
		this->n = 0;
	}
	
	RSGISPolygonPixelCount::~RSGISPolygonPixelCount()
	{

	}
	
}}


