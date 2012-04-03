/*
 *  RSGISBaysianStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 27/12/2008.
 *  Copyright 2008  RSGISLib.
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

#include "RSGISBaysianStatsPrior.h"

namespace rsgis{namespace math{
	
	RSGISBaysianStatsPrior::RSGISBaysianStatsPrior(RSGISMathFunction *function, RSGISProbDistro *probDist, double variance, double interval, double minVal, double maxVal, double lowerLimit, double upperLimit, deltatypedef deltatype) throw(RSGISBaysianStatsException)
	{
		this->variance = variance;
		this->interval = interval;
		this->minVal = minVal;
		this->maxVal = maxVal;
		this->upperLimit = upperLimit;
		this->lowerLimit = lowerLimit;
		this->deltatype = deltatype;
		this->baysianFunction = new RSGISBaysianIntergrateFunctionPrior(function, probDist, variance);
	}
	
	/***
	 * returns a double[3] 
	 * 
	 * output[0] - Value
	 * output[1] - Lower
	 * output[2] - Upper
	 *
	 */
	
	double* RSGISBaysianStatsPrior::calcImageValuePrior(float value) throw(RSGISBaysianStatsException)
	{
		/***
		 * Calculates the maximum likelyhood of B given A.
		 * The errors are given as values of B at the lower and
		 * upper proportions of the total areas under the probability
		 * function.
		 */ 
		
		double *outputValues = new double[3];
		
		this->baysianFunction->updateValue(value);
		
		// Intergration
		TrapeziumIntegration *intergrat = new TrapeziumIntegration(baysianFunction, interval);
		
		intergrat->calcArea(minVal, maxVal, true);
		
		if(this->deltatype == area)
		{
			outputValues[0] = intergrat->calcMaxValue(); // Maximum likelyhood value
			outputValues[1] = intergrat->calcValue4ProportionArea(lowerLimit); // Calculate lower limit
			outputValues[2] = intergrat->calcValue4ProportionArea(upperLimit); // Calculate Upper limit
			//cout << "Maximum Likelyhood Biomass = " << outputValues[0] << " Lower Val = " << outputValues[1] << " Upper Val = " << outputValues[2] << endl;
			
		}
		
		else if(this->deltatype == prob)
		{
			outputValues[0] = intergrat->calcMaxValue(); // Maximum likelyhood value
			intergrat->getUpperLowerValues(outputValues[1], outputValues[2], lowerLimit);
		}
		
		else
		{
			throw RSGISBaysianStatsException("Unknown delta type. Valid types are area and prob");
		}
		
		delete intergrat;
		return outputValues;
	}
	
	
}}

