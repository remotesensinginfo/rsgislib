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
    
    RSGISApply6SCoefficients::RSGISApply6SCoefficients(int numberOutBands, unsigned int *imageBands, float **aX, float **bX, float **cX, int numValues, float *elevationThreash, int numElevation, float scaleFactor):RSGISCalcImageValue(numberOutBands)
    {
        this->useTopo6S = true;
		this->imageBands = imageBands;
        this->aX = aX;
        this->bX = bX;
        this->cX = cX;
		this->elevationThreash = elevationThreash;
        this->numValues = numValues;
        this->scaleFactor = scaleFactor;
		this->numElevation = numElevation;
        this->bandOffset = 0;
		if (numElevation == 0) 
		{
			this->useTopo6S = false;
            bandOffset = 1;
		}
    }
    
    void RSGISApply6SCoefficients::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
    {
        if(numValues != this->numOutBands)
        {
            throw RSGISImageCalcException("The number of input image bands needs to be equal to the number of output image bands.");
        }
        
        if(numBands <= numValues)
        {
            throw RSGISImageCalcException("The number of input values needs to be equal to or less than the number of input image bands.");
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
                
                if (elevationInt < this->elevationThreash[0]) 
                {
                    elv = 0;
                }
                else 
                {
                    for (unsigned int d = 1; d < numElevation; ++d) 
                    {
                        if((elevationInt >= this->elevationThreash[d - 1]) && (elevationInt < this->elevationThreash[d])){elv = d;}
                    }
                }
            }
            
            for(unsigned int i = 0; i < this->numValues; ++i)
            {
                if(imageBands[i]+bandOffset > numBands)
                {
                    cout << "Image band: " << imageBands[i] << endl;
                    throw RSGISImageCalcException("Image band is not within image.");
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


