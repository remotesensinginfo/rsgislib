/*
 *  RSGISCalcGammaZero.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/04/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#include "RSGISCalcGammaZero.h"


namespace rsgis{namespace radar{
	
	RSGISCalcGammaZero::RSGISCalcGammaZero(int numberOutBands, bool deg) : RSGISCalcImageValue(numberOutBands)
	{
		this->deg = deg;
	}
	
	void RSGISCalcGammaZero::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		// Input bands
		// Incidence Angle
		// SAR data
		
		if(this->numOutBands != numBands-1)
		{
			throw rsgis::img::RSGISImageCalcException("Number of input and output bands are the same!");
		}
		
		double pi = 3.14159265358979323846;
		double angle = 0;
		if(deg)
		{
			
			angle = bandValues[0] * (pi/180);
		}
		else
		{
			angle = bandValues[0];
		}
		
		for(int i = 1; i < numBands; i++)
		{
			output[i-1] = 10*log10(bandValues[i]/cos(angle));
		}
	}
	
	void RSGISCalcGammaZero::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not implemented");
	}
	
	void RSGISCalcGammaZero::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
	{
		throw rsgis::img::RSGISImageCalcException("Not Implemented");
	}
		
	RSGISCalcGammaZero::~RSGISCalcGammaZero()
	{
		
	}
	
}}
