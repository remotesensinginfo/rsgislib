/*
 *  RSGISImageCalcValueBaysianPrior.cpp
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

#include "RSGISImageCalcValueBaysianPrior.h"

namespace rsgis{namespace img{
	
	RSGISImageCalcValueBaysianPrior::RSGISImageCalcValueBaysianPrior(int numberOutBands, rsgis::math::RSGISMathFunction *function, rsgis::math::RSGISProbDistro *probDistro, double variance, double interval, double minVal, double maxVal, double lowerLimit, double upperLimit, rsgis::math::deltatypedef deltatype) : RSGISCalcImageValue(numberOutBands)
	{
		this->function = function;
		this->probDistro = probDistro;
		this->variance = variance;
		this->interval = interval;
		this->minVal = minVal;
		this->maxVal = maxVal;
		this->upperLimit = upperLimit;
		this->lowerLimit = lowerLimit;
		this->deltatype = deltatype;
		baysianStats = new rsgis::math::RSGISBaysianStatsPrior(function, probDistro, variance, interval, minVal, maxVal, upperLimit, lowerLimit, deltatype);
		std::cout << "Delta type " << deltatype << std::endl;
	}
	
	void RSGISImageCalcValueBaysianPrior::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		
		outputVals = baysianStats->calcImageValuePrior(bandValues[0]);
		
		output[1] = outputVals[0]; // Maximum Likelyhood Value
		output[0] = outputVals[1]; // Lower value
		output[2] = outputVals[2]; // Upper value
		
		// Calculate delta- and delta +
		output[3] = sqrt((output[1] - output[0])*(output[1] - output[0]));
		output[4] = sqrt((output[2] - output[1])*(output[2] - output[1]));	
		delete[] outputVals;
	}
	
	void RSGISImageCalcValueBaysianPrior::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISImageCalcValueBaysianPrior::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISImageCalcValueBaysianPrior::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	void RSGISImageCalcValueBaysianPrior::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}

	bool RSGISImageCalcValueBaysianPrior::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not implemented");
	}
	
	RSGISImageCalcValueBaysianPrior::~RSGISImageCalcValueBaysianPrior()
	{
		delete baysianStats;
	}
}}
