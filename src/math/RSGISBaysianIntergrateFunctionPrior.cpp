/*
 *  RSGISBaysianIntergrateFunctionNoPrior.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 12/12/2008.
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

#include "RSGISBaysianIntergrateFunctionPrior.h"

namespace rsgis{namespace math{
	
	RSGISBaysianIntergrateFunctionPrior::RSGISBaysianIntergrateFunctionPrior(RSGISMathFunction *function, RSGISProbDistro *probDistro, double variance) : RSGISMathFunction()
	{
		this->function = function;
		this->probDistro = probDistro;
		this->variance = variance;
		this->value = 0;
	}
	
	void RSGISBaysianIntergrateFunctionPrior::updateValue(float value)
	{
		this->value = value;
	}
	
	double RSGISBaysianIntergrateFunctionPrior::calcFunction(double predictVal)
	{		
		double fVal = function->calcFunction(predictVal);
		double diff = value - fVal;
		double diffsq = diff * diff;
		double var2 = 2 * (variance * variance);
		double diffvar = diffsq / var2;
		double probAB = exp ((-1)* diffvar);
		double probABPrior = probDistro->calcProb(predictVal);
		return probAB * probABPrior;
		
	}
}}
