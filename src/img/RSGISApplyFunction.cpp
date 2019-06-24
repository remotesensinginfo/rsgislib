/*
 *  RSGISApplyFunction.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 27/01/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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

#include "RSGISApplyFunction.h"

namespace rsgis{namespace img{

	RSGISApplySingleVarFunction::RSGISApplySingleVarFunction(int numOutputBands, rsgis::math::RSGISMathFunction *imagefunction, float ignoreVal) : RSGISCalcImageValue(numOutputBands)
	{
		this->imagefunction = imagefunction;
		this->ignoreVal = ignoreVal;
	}
	void RSGISApplySingleVarFunction::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if (bandValues[0] != ignoreVal) 
		{
			output[0] = imagefunction->calcFunction(bandValues[0]);
		}
		else 
		{
			output[0] = 0;
		}
		
	}
	RSGISApplySingleVarFunction::~RSGISApplySingleVarFunction()
	{
		
	}
	
	RSGISApplyTwoVarFunction::RSGISApplyTwoVarFunction(int numOutputBands, rsgis::math::RSGISMathTwoVariableFunction *imagefunction, float ignoreVal) : RSGISCalcImageValue(numOutputBands)
	{
		this->imagefunction = imagefunction;
		this->ignoreVal = ignoreVal;
	}
	void RSGISApplyTwoVarFunction::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if ((bandValues[0] != ignoreVal) && (bandValues[1] != ignoreVal)) 
		{
			output[0] = imagefunction->calcFunction(bandValues[0],bandValues[1]);
		}
		else 
		{
			output[0] = 0;
		}
		
	}
	RSGISApplyTwoVarFunction::~RSGISApplyTwoVarFunction()
	{
		
	}
	
	RSGISApplyThreeVarFunction::RSGISApplyThreeVarFunction(int numOutputBands, rsgis::math::RSGISMathThreeVariableFunction *imagefunction, float ignoreVal) : RSGISCalcImageValue(numOutputBands)
	{
		this->imagefunction = imagefunction;
		this->ignoreVal = ignoreVal;
	}
	void RSGISApplyThreeVarFunction::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if ((bandValues[0] != ignoreVal) && (bandValues[1] != ignoreVal) && (bandValues[2] != ignoreVal)) 
		{
			output[0] = imagefunction->calcFunction(bandValues[0],bandValues[1],bandValues[2]);
		}
		else 
		{
			output[0] = 0;
		}
		
	}
	RSGISApplyThreeVarFunction::~RSGISApplyThreeVarFunction()
	{
		
	}
	
}}
