/*
 *  RSGISRelabelPixelValuesFromLUT.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/02/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
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

#include "RSGISRelabelPixelValuesFromLUT.h"


namespace rsgis { namespace img {

    RSGISRelabelPixelValuesFromLUT::RSGISRelabelPixelValuesFromLUT()
    {
        
    }
    
    void RSGISRelabelPixelValuesFromLUT::relabelPixelValues(GDALDataset *inData, std::string outputFile, std::string matrixLUTFile, std::string imageFormat)
    {
        try
        {
            rsgis::math::RSGISMatrices matrixUtils;
            gsl_matrix *lut = matrixUtils.readGSLMatrixFromGridTxt(matrixLUTFile);
            
			RSGISCalcImageValue *calcImageValue = new RSGISRelabelPixelValuesFromLUTCalcVal(inData->GetRasterCount(), lut);            
            RSGISCalcImage calcImage = RSGISCalcImage(calcImageValue, "", true);
            
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = inData;
            
            calcImage.calcImage(datasets, 1, outputFile, false, NULL, imageFormat);
            
            delete[] datasets;
            delete calcImageValue;
            gsl_matrix_free(lut);
        }
        catch(RSGISException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
    }
    
    RSGISRelabelPixelValuesFromLUT::~RSGISRelabelPixelValuesFromLUT()
    {
        
    }
    
	

    RSGISRelabelPixelValuesFromLUTCalcVal::RSGISRelabelPixelValuesFromLUTCalcVal(int numOutBands, gsl_matrix *lut):RSGISCalcImageValue(numOutBands)
    {
        this->lut = lut;
    }
		
    void RSGISRelabelPixelValuesFromLUTCalcVal::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        if(numBands != numOutBands)
        {
            throw RSGISImageCalcException("The number of output and input image bands should be the same.");
        }
        bool found = false;
        for(int i = 0; i < numBands; ++i)
        {
            found = false;
            for(int j = 0; j < lut->size1; ++j)
            {
                if(gsl_matrix_get(lut, j, 0) == bandValues[i])
                {
                    output[i] = gsl_matrix_get(lut, j, 1);
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                output[i] = bandValues[i];
            }
        }
        
    }
    
    
    RSGISRelabelPixelValuesFromLUTCalcVal::~RSGISRelabelPixelValuesFromLUTCalcVal()
    {
        
    }
	
}}


