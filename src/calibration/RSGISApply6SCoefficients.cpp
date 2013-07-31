/*
 *  RSGISApply6SCoefficients.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/07/2011.
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

#include "RSGISApply6SCoefficients.h"

namespace rsgis{namespace calib{
    
    RSGISApply6SCoefficientsSingleParam::RSGISApply6SCoefficientsSingleParam(unsigned int *imageBands, float *aX, float *bX, float *cX, int numValues, float noDataVal, bool useNoDataVal, float scaleFactor):rsgis::img::RSGISCalcImageValue(numValues)
    {
		this->imageBands = imageBands;
        this->aX = aX;
        this->bX = bX;
        this->cX = cX;
        this->numValues = numValues;
        this->scaleFactor = scaleFactor;
        this->noDataVal = noDataVal;
        this->useNoDataVal = useNoDataVal;
    }
    
    void RSGISApply6SCoefficientsSingleParam::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numValues != this->numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input image bands needs to be equal to the number of output image bands.");
        }
        
        if(numBands != numValues)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input values needs to be equal to the number of input image bands.");
        }
        
        double tmpVal = 0;
		
        bool nodata = true;
        if(this->useNoDataVal)
        {
            for(int i = 0; i < numBands; ++i)
            {
                if(bandValues[i] != this->noDataVal)
                {
                    nodata = false;
                    break;
                }
            }
        }
        else
        {
            nodata = false;
        }
        
        
		
        if(nodata)
        {
            for(unsigned int i = 0; i < this->numValues; ++i)
            {
                output[i] = 0;
            }
        }
        else
        {            
            for(unsigned int i = 0; i < this->numValues; ++i)
            {
                if(imageBands[i] > numBands)
                {
                    std::cout << "Image band: " << imageBands[i] << std::endl;
                    throw rsgis::img::RSGISImageCalcException("Image band is not within image.");
                }
                
                tmpVal=aX[i]*bandValues[imageBands[i]]-bX[i];
                output[i] = (tmpVal/(1.0+cX[i]*tmpVal))*this->scaleFactor;
                
                if(this->useNoDataVal & (this->noDataVal == 0.0))
                {
                    if(output[i] < 1)
                    {
                        output[i] = 1.0;
                    }
                    else
                    {
                        output[i] = output[i] + 1.0;
                    }
                }
            }
        }
        
    }
    
    RSGISApply6SCoefficientsSingleParam::~RSGISApply6SCoefficientsSingleParam()
    {
        
    }
    
    
    
    
    RSGISApply6SCoefficients::RSGISApply6SCoefficients(int numberOutBands, unsigned int *imageBands, float **aX, float **bX, float **cX, int numValues, float *elevationThresh, int numElevation, float scaleFactor):rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->useTopo6S = true;
        this->bandOffset = 1;
		this->imageBands = imageBands;
        this->aX = aX;
        this->bX = bX;
        this->cX = cX;
		this->elevationThresh = elevationThresh;
        this->numValues = numValues;
        this->scaleFactor = scaleFactor;
		this->numElevation = numElevation;
		if (numElevation == 0)
		{
			this->useTopo6S = false;
            this->bandOffset = 0;
		}
    }
    
    void RSGISApply6SCoefficients::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numValues != this->numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input image bands needs to be equal to the number of output image bands.");
        }
        
        if(numBands <= numValues)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input values needs to be equal to or less than the number of input image bands.");
        }
                
        double tmpVal = 0;
		
		unsigned int elv = 0;
		
        if (bandValues[this->bandOffset] == 0) // If first band == 0, assume image border
        {
            for(unsigned int i = 0; i < this->numValues; ++i)
            {
                output[i] = 0;
            }
        }
        else
        {
            if(this->useTopo6S)
            {
                // Round to nearest 50 m
                double elevationScale = bandValues[0] / 100.0;
                elevationScale = int(elevationScale + 0.5);
                int elevationInt = elevationScale * 100;
                
                if (elevationInt < this->elevationThresh[0]) 
                {
                    elv = 0;
                }
                else 
                {
                    for (unsigned int d = 1; d < numElevation; ++d) 
                    {
                        if((elevationInt >= this->elevationThresh[d - 1]) && (elevationInt < this->elevationThresh[d]))
                        {
                            elv = d;
                        }
                    }
                }
            }
            
            for(unsigned int i = 0; i < this->numValues; ++i)
            {
                if(imageBands[i]+bandOffset > numBands)
                {
                    std::cout << "Image band: " << imageBands[i] << std::endl;
                    throw rsgis::img::RSGISImageCalcException("Image band is not within image.");
                }
                
                tmpVal=aX[i][elv]*bandValues[imageBands[i]+bandOffset]-bX[i][elv];
                output[i] = (tmpVal/(1.0+cX[i][elv]*tmpVal))*this->scaleFactor;
            }
        }

    }
        
    RSGISApply6SCoefficients::~RSGISApply6SCoefficients()
    {
        
    }

}}


