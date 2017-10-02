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
    
    RSGISMaxNDVIImageComposite::RSGISMaxNDVIImageComposite(int numberOutBands, unsigned int redBand, unsigned int nirBand, unsigned int numInImgs) : RSGISCalcImageValue(numberOutBands)
    {
        this->redBand = redBand-1;
        this->nirBand = nirBand-1;
        this->numInImgs = numInImgs;
    }
    
    void RSGISMaxNDVIImageComposite::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
    {
        bool noData = true;
        for(int i = 0; i < numBands; ++i)
        {
            if(bandValues[i] != 0.0)
            {
                noData = false;
            }
        }
        
        if(noData)
        {
            for(int i = 0; i < this->getNumOutBands(); ++i)
            {
                output[i] = 0.0;
            }
        }
        else
        {
            int nImgBands = this->getNumOutBands();
            float maxNDVI = 0.0;
            int maxImgIdx = 0;
            
            float ndviVal = 0.0;
            bool first = true;
            for(int i = 0; i < numInImgs; ++i)
            {
                if((bandValues[(nImgBands*i)+this->nirBand] != 0) & (bandValues[(nImgBands*i)+this->redBand] != 0))
                {
                    ndviVal = (bandValues[(nImgBands*i)+this->nirBand]-bandValues[(nImgBands*i)+this->redBand])/(bandValues[(nImgBands*i)+this->nirBand]+bandValues[(nImgBands*i)+this->redBand]);
                    if(first)
                    {
                        maxNDVI = ndviVal;
                        maxImgIdx = i;
                        first = true;
                    }
                    else if(ndviVal > maxNDVI)
                    {
                        maxNDVI = ndviVal;
                        maxImgIdx = i;
                    }
                }
            }
            
            int outImgBandIdx = maxImgIdx * this->getNumOutBands();
            for(int i = 0; i < this->getNumOutBands(); ++i)
            {
                output[i] = bandValues[outImgBandIdx++];
            }
            
        }
        
    }
    
    
    RSGISRefImgImageComposite::RSGISRefImgImageComposite(int numberOutBands, unsigned int numInImgs, float outNoDataVal) : RSGISCalcImageValue(numberOutBands)
    {
        this->numInImgs = numInImgs;
        this->outNoDataVal = outNoDataVal;
    }
    
    void RSGISRefImgImageComposite::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException)
    {
        if(intBandValues[0] == 0)
        {
            for(int i = 0; i < this->getNumOutBands(); ++i)
            {
                output[i] = this->outNoDataVal;
            }
        }
        else if(intBandValues[0] > 0)
        {
            if((intBandValues[0]-1) < this->numInImgs)
            {
                int outImgBandIdx = (intBandValues[0]-1) * this->getNumOutBands();
                for(int i = 0; i < this->getNumOutBands(); ++i)
                {
                    output[i] = floatBandValues[outImgBandIdx++];
                }
            }
            else
            {
                std::cerr << "Reference pixel = " << intBandValues[0] << std::endl;
                throw RSGISImageCalcException("Reference image is not within the stack.");
            }
        }
        else
        {
            std::cerr << "Reference pixel = " << intBandValues[0] << std::endl;
            throw RSGISImageCalcException("Reference pixel values cannot be negative");
        }
    }
    
}}

