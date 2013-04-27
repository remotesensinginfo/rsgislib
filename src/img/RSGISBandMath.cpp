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

	void RSGISBandMath::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
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

	void RSGISBandMath::calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}

	void RSGISBandMath::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}

	bool RSGISBandMath::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("No implemented");
	}

	RSGISBandMath::~RSGISBandMath()
	{

	}
    
    void RSGISBandMath::executeBandMaths(VariableStruct *variables, unsigned int numVars, std::string outputImage, std::string mathsExpression, std::string gdalFormat, GDALDataType outDataType)throw(rsgis::RSGISImageException)
    {
        std::string *outBandName = new std::string[1];
        outBandName[0] = mathsExpression;
        
        GDALAllRegister();
        GDALDataset **datasets = NULL;
        rsgis::img::RSGISBandMath *bandmaths = NULL;
        rsgis::img::RSGISCalcImage *calcImage = NULL;
        mu::Parser *muParser = new mu::Parser();
        
        try
        {
            rsgis::img::VariableBands **processVaribles = new rsgis::img::VariableBands*[numVars];
            datasets = new GDALDataset*[numVars];
            
            int numRasterBands = 0;
            int totalNumRasterBands = 0;
            
            for(int i = 0; i < numVars; ++i)
            {
                std::cout << variables[i].image << std::endl;
                datasets[i] = (GDALDataset *) GDALOpen(variables[i].image.c_str(), GA_ReadOnly);
                if(datasets[i] == NULL)
                {
                    std::string message = std::string("Could not open image ") + variables[i].image;
                    throw rsgis::RSGISImageException(message.c_str());
                }
                
                numRasterBands = datasets[i]->GetRasterCount();
                
                if((variables[i].bandNum < 0) | (variables[i].bandNum > numRasterBands))
                {
                    throw rsgis::RSGISImageException("You have specified a band which is not within the image");
                }
                
                processVaribles[i] = new rsgis::img::VariableBands();
                processVaribles[i]->name = variables[i].name;
                processVaribles[i]->band = totalNumRasterBands + (variables[i].bandNum - 1);
                
                totalNumRasterBands += numRasterBands;
            }
            
            mu::value_type *inVals = new mu::value_type[numVars];
            for(int i = 0; i < numVars; ++i)
            {
                inVals[i] = 0;
                muParser->DefineVar(_T(processVaribles[i]->name.c_str()), &inVals[i]);
            }
            
            muParser->SetExpr(mathsExpression.c_str());
            
            bandmaths = new rsgis::img::RSGISBandMath(1, processVaribles, numVars, muParser);
            
            calcImage = new rsgis::img::RSGISCalcImage(bandmaths, "", true);
            calcImage->calcImage(datasets, numVars, outputImage, true, outBandName, gdalFormat, outDataType);
            
            for(int i = 0; i < numVars; ++i)
            {
                GDALClose(datasets[i]);
                delete processVaribles[i];
            }
            delete[] datasets;
            delete[] processVaribles;
            
            delete[] inVals;
            
            delete muParser;
            delete bandmaths;
            delete calcImage;
            delete[] outBandName;
        }
        catch(rsgis::RSGISException e)
        {
            throw RSGISImageException(e.what());
        }
        catch (mu::ParserError &e)
        {
            std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
            throw rsgis::RSGISImageException(message);
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }

}}

