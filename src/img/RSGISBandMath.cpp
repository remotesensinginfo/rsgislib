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

	void RSGISBandMath::calcImageValue(float *bandValues, int numBands, double *output) 
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

	RSGISBandMath::~RSGISBandMath()
	{
        delete[] inVals;
	}
    
    
    
    
    
    RSGISCalcPropExpTruePxls::RSGISCalcPropExpTruePxls(VariableBands **variables, int numVariables, mu::Parser *muParser, bool useMask):RSGISCalcImageValue(0)
    {
        this->variables = variables;
        this->numVariables = numVariables;
        this->useMask = useMask;
        
        this->muParser = muParser;
        this->inVals = new mu::value_type[numVariables];
        for(int i = 0; i < numVariables; ++i)
        {
            muParser->DefineVar(_T(variables[i]->name.c_str()), &inVals[i]);
        }
        
        this->truePxlCount = 0.0;
        this->totalPxlCount = 0.0;
    }

    void RSGISCalcPropExpTruePxls::calcImageValue(float *bandValues, int numBands) 
    {
        try
        {
            if((!this->useMask) | (this->useMask & (bandValues[0] == 1)))
            {
                for(int i = 0; i < numVariables; ++i)
                {
                    inVals[i] = bandValues[variables[i]->band];
                }
                mu::value_type result = 0;
                result = muParser->Eval();
                
                if(1 == floor(result))
                {
                    this->truePxlCount = this->truePxlCount + 1.0;
                }
                this->totalPxlCount = this->totalPxlCount + 1.0;
            }
            
        }
        catch (mu::ParserError &e)
        {
            std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
            throw RSGISImageCalcException(message);
        }
    }
    
    float RSGISCalcPropExpTruePxls::getPropPxlVal()
    {
        return this->truePxlCount / this->totalPxlCount;
    }
    
    RSGISCalcPropExpTruePxls::~RSGISCalcPropExpTruePxls()
    {
        delete[] inVals;
    }

    
    
    
}}

