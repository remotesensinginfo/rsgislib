/*
 *  RSGISImageComposite.cpp
 *  RSGISLIB
 *
 *  Created by Daniel Clewley on 27/02/2012.
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

#include "RSGISImageComposite.h"

namespace rsgis { namespace img {
    
RSGISImageComposite::RSGISImageComposite(int numberOutBands, unsigned int nCompositeBands, compositeStat outStat) : RSGISCalcImageValue(numberOutBands)
{
    this->numberOutBands = numberOutBands;
    this->nCompositeBands = nCompositeBands;
    this->outStat = outStat;
}

void RSGISImageComposite::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
{
    for(int i = 0; i < this->numberOutBands; ++i)
    {
        unsigned int startBand = this->nCompositeBands * i;
        float minVal = 0;
        float maxVal = 0;
        float sum = 0;
        unsigned int realNCompositeBands = 0; // Count composite bands, if not exactly devisable
        
        // Calculate stats from input bands
        for(unsigned int j = 0; j < this->nCompositeBands; ++j)
        {
            if((startBand + j) < numBands)
            {
                ++realNCompositeBands;
                float pixelVal = bandValues[startBand + j];
                sum = sum + pixelVal;
                if (j == 0) // If first band, set min and max to pixel values
                {
                    minVal = pixelVal;
                    maxVal = pixelVal;
                }
                else 
                {
                    if(pixelVal < minVal){minVal = pixelVal;}
                    if(pixelVal > maxVal){maxVal = pixelVal;}
                }

            }
            
        }
        
        // Add stats to output image
        if (this->outStat == compositeMean)
        {
            if(sum != 0)
            {
                 output[i] = sum / realNCompositeBands;
            }
            else 
            {
                output[i] = 0;
            }
        }
        else if (this->outStat == compositeMin) {output[i] = minVal;}
        else if (this->outStat == compositeMax) {output[i] = maxVal;}
        else if (this->outStat == compositeRange) {output[i] = maxVal - minVal;}
        
    }    
}
    
}}

