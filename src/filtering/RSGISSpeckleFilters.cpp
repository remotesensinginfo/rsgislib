/*
 *  RSGISSpeckleFilters.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/12/2008.
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

#include "RSGISSpeckleFilters.h"

namespace rsgis{namespace filter{
    
	RSGISLeeFilter::RSGISLeeFilter(int numberOutBands, int size, std::string filenameEnding, unsigned int nLooks, float internalScaleFactor) : RSGISImageFilter(numberOutBands, size, filenameEnding)
    {
        this->nLooks = nLooks;
        this->internalScaleFactor = internalScaleFactor;
    }
	
	void RSGISLeeFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
	
        unsigned int middleVal = floor(((float)winSize) / 2);
		unsigned int numVal = 0; // Number of values counted
        float outI = 0;
        float iVal = 0;
        float iMean = 0;
        float iVar = 0;
      	float cU = sqrt(1. / this->nLooks); // Noise variation coefficient;
		float nNoiseMean = 1; // Mean multiplicative noise
		float k = 0;
        
        for(int i = 0; i < numBands; i++)
        {
            iMean = 0;
            iVar = 0;
			numVal = 0;

            // Calculate mean
            for(int j = 0; j < size; j++)
            {
                for(int k = 0; k < size; k++)
                {
                    if(dataBlock[i][j][k] != 0)
					{
							iMean = iMean + dataBlock[i][j][k]*this->internalScaleFactor;
							if((j == middleVal) && (k == middleVal))
							{
								iVal = dataBlock[i][j][k]*this->internalScaleFactor;
							}
							numVal++;
					}
                }
            }
            
            iMean = iMean / numVal;
            
            // Calculate variance
            for(int j = 0; j < size; j++)
            {
                for(int k = 0; k < size; k++)
                {
                     if(dataBlock[i][j][k] != 0)
					 {
					     iVar = iVar + pow((dataBlock[i][j][k]*this->internalScaleFactor - iMean), 2);
					 }
                }
            }
            
            iVar = iVar / numVal;
            
            k = (nNoiseMean * iVar) / (iMean*iMean*cU + nNoiseMean*nNoiseMean*iVar);
            outI = iMean + k*(iVal - nNoiseMean + iMean);
            output[i] = outI / this->internalScaleFactor;
        }
	}
	
	RSGISLeeFilter::~RSGISLeeFilter()
	{
		
	}
}}
