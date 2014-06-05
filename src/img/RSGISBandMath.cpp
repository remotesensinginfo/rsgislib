/*
 *  RSGISBandMath.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/11/2008.
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

#include "RSGISBandMath.h"

namespace rsgis{namespace img{

	RSGISBandMath::RSGISBandMath(int numberOutBands, VariableBands **variables, int numVariables, mu::Parser *muParser) : RSGISCalcImageValue(numberOutBands)
	{
		this->variables = variables;
		this->numVariables = numVariables;
		
		this->muParser = muParser;
		this->inVals = new mu::value_type[numVariables];
		for(int i = 0; i < numVariables; ++i)
		{
			muParser->DefineVar(_T(variables[i]->name.c_str()), &inVals[i]);
		}
		
	}

	void RSGISBandMath::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		if(numOutBands != 1)
		{
			throw RSGISImageCalcException("Incorrect number of output Image bands (should be equal to 1).");
		}
		
		try 
		{
			for(int i = 0; i < numVariables; ++i)
			{
				inVals[i] = bandValues[variables[i]->band];
			}
            mu::value_type result = 0;
			result = muParser->Eval();
			
			output[0] = result;
		}
		catch (mu::ParserError &e) 
		{
            std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
			throw RSGISImageCalcException(message);
		}
	}

	void RSGISBandMath::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}
	
	void RSGISBandMath::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}

	void RSGISBandMath::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}

	void RSGISBandMath::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}

	bool RSGISBandMath::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}

	RSGISBandMath::~RSGISBandMath()
	{

	}
    
}}

