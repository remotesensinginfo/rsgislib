/*
 *  RSGISCalcRMSE.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 21/12/2010.
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

#include "RSGISCalcRMSE.h"

namespace rsgis{namespace img{
	
	RSGISCalcRMSE::RSGISCalcRMSE(int numOutputValues) : RSGISCalcImageSingleValue(numOutputValues)
	{
		this->numOutputValues = numOutputValues;
		this->sumSqDiff = 0;
		this->rmseReturn = new double[1];
		this->numVal = 0;
	}
	
	void RSGISCalcRMSE::calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException)
	{
		this->sumSqDiff = this->sumSqDiff + (bandValuesImageA[bandA] - bandValuesImageB[bandB])*(bandValuesImageA[bandA] - bandValuesImageB[bandB]);
		this->numVal++;
	}
	
	double* RSGISCalcRMSE::getOutputValues() throw(RSGISImageCalcException)
	{
		rmseReturn[0] = sqrt(sumSqDiff / numVal);
		return rmseReturn;
	}
	
	void RSGISCalcRMSE::reset()
	{
		this->sumSqDiff = 0;
		this->rmseReturn[0] = 0;
		this->numVal = 0;
	}
	
	RSGISCalcRMSE::~RSGISCalcRMSE()
	{
		delete this->rmseReturn;
	}
	
}}
