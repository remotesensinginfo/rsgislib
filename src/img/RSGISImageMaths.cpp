/*
 *  RSGISImageMaths.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/08/2010.
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

#include "RSGISImageMaths.h"


namespace rsgis{namespace img{
	
	RSGISImageMaths::RSGISImageMaths(int numberOutBands, mu::Parser *muParser) : RSGISCalcImageValue(numberOutBands)
	{
		
		this->muParser = muParser;
		inVal = 0;
		muParser->DefineVar(_T("b1"), &inVal);
		
	}
	
	void RSGISImageMaths::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		if(numOutBands != numBands)
		{
			throw RSGISImageCalcException("The number of output image bands must be equal to the number of input bands.");
		}
		
		try 
		{
            mu::value_type result = 0;
			for(int i = 0; i < numBands; ++i)
			{
				inVal = bandValues[i];
				result = muParser->Eval();
				output[i] = result;
			}
			
		}
		catch (mu::ParserError &e) 
		{
            std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
			throw RSGISImageCalcException(message);
		}
	}

	
	RSGISImageMaths::~RSGISImageMaths()
	{
		
	}
    
    
    RSGISAllBandsEqualTo::RSGISAllBandsEqualTo(int numberOutBands, float value, float outTrueVal, float outFalseVal) : RSGISCalcImageValue(numberOutBands)
	{
		this->value = value;
		this->outTrueVal = outTrueVal;
		this->outFalseVal = outFalseVal;
	}
	
	void RSGISAllBandsEqualTo::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
	{
		if(!(numOutBands > 0))
		{
			throw RSGISImageCalcException("The number of output image bands must great or equal to 1.");
		}
		
		bool valNotFound = false;
        
        for(int i = 0; i < numBands; ++i)
        {
            if(bandValues[i] != value)
            {
                valNotFound = true;
                break;
            }
        }
        
        if(valNotFound)
        {
            output[0] = outFalseVal;
        }
        else
        {
            output[0] = outTrueVal;
        }
	}
	
    
	
	RSGISAllBandsEqualTo::~RSGISAllBandsEqualTo()
	{
		
	}
	
}}



