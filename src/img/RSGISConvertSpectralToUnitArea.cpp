/*
 *  RSGISConvertSpectralToUnitArea.cpp
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

#include "RSGISConvertSpectralToUnitArea.h"

namespace rsgis{namespace img{
	
	
	RSGISConvertSpectralToUnitArea::RSGISConvertSpectralToUnitArea(int numberOutBands, rsgis::math::Matrix *bandValuesWidths) : RSGISCalcImageValue(numberOutBands)
	{
		this->bandValuesWidths = bandValuesWidths;
	}
	
	void RSGISConvertSpectralToUnitArea::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		if(numBands != numOutBands)
		{
			throw RSGISImageCalcException("The number of input and output image bands needs to be equal.");
		}
		
		if(bandValuesWidths->n != numBands)
		{
			throw RSGISImageCalcException("Band values (i.e., wavelength) and widths need to be defined for all image bands");
		}
		
		if(bandValuesWidths->m != 2)
		{
			throw RSGISImageCalcException("The inputted matrix (wavelength,width) should have two columns");
		}
		
		double totalArea = 0;
		
		for(int i = 0; i < numBands; ++i)
		{
			if(boost::math::isnan(bandValues[i]))
			{
				bandValues[i] = 0;
			}
			totalArea += bandValuesWidths->matrix[(i*2)+1] * bandValues[i];
		}
		
		double areaMultiplier = 0;
		
		if(totalArea == 0)
		{
			areaMultiplier = 0;
		}
		else 
		{
			areaMultiplier = 1/totalArea;
		}
		
		for(int i = 0; i < numBands; ++i)
		{
			output[i] = bandValues[i] * areaMultiplier;
		}
		
	}
	
	void RSGISConvertSpectralToUnitArea::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISConvertSpectralToUnitArea::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISConvertSpectralToUnitArea::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISConvertSpectralToUnitArea::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	bool RSGISConvertSpectralToUnitArea::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	RSGISConvertSpectralToUnitArea::~RSGISConvertSpectralToUnitArea()
	{
		
	}
}}


