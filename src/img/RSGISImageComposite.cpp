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
    
    
    
    
    RSGISTimeseriesFillRefImgImageComposite::RSGISTimeseriesFillRefImgImageComposite(std::vector<rsgis::img::RSGISCompositeInfo*> compInfoVec) : RSGISCalcImageValue(1)
    {
        this->compInfoVec = compInfoVec;
        int fillBandIdx = 1;
        for(std::vector<rsgis::img::RSGISCompositeInfo*>::iterator iterInfo = compInfoVec.begin(); iterInfo != compInfoVec.end(); ++iterInfo)
        {
            if((*iterInfo)->outRef)
            {
                break;
            }
            ++fillBandIdx;
        }
        
        if(fillBandIdx != 1)
        {
            std::cout << "this->fillBandIdx = " << fillBandIdx << std::endl;
            throw RSGISImageCalcException("The image being filled should be the first in the list");
        }
        maxRefPxlVal = 0;
    }

    void RSGISTimeseriesFillRefImgImageComposite::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException)
    {
        if(numIntVals != (compInfoVec.size()+1))
        {
            throw RSGISImageCalcException("The number of inputted int bands is not equal to the number of dates+1 (for validation image)");
        }
        output[0] = 0;
        
        if((intBandValues[0] == 1) & (intBandValues[1] == 0))
        {
            for(unsigned int i = 2; i < numIntVals; ++i)
            {
                if(intBandValues[i] > 0)
                {
                    output[0] = i;
                    compInfoVec.at(i-1)->usedInComp = true;
                    compInfoVec.at(i-1)->pxlRefContrib2Fill.insert(intBandValues[i]);
                    break;
                }
            }
        }
        
        if(intBandValues[1] > maxRefPxlVal)
        {
            maxRefPxlVal = intBandValues[1];
        }
    }
    
    
    
    
    RSGISTimeseriesFillImgImageComposite::RSGISTimeseriesFillImgImageComposite(std::vector<rsgis::img::RSGISCompositeInfo*> compInfoVec, unsigned int *imgIdxLUT, unsigned int nLUT, unsigned int nBands) : RSGISCalcImageValue(nBands)
    {
        this->compInfoVec = compInfoVec;
        this->imgIdxLUT = imgIdxLUT;
        this->nLUT = nLUT;
        int fillBandIdx = 1;
        for(std::vector<rsgis::img::RSGISCompositeInfo*>::iterator iterInfo = compInfoVec.begin(); iterInfo != compInfoVec.end(); ++iterInfo)
        {
            if((*iterInfo)->outRef)
            {
                break;
            }
            ++fillBandIdx;
        }
        
        if(fillBandIdx != 1)
        {
            std::cout << "this->fillBandIdx = " << fillBandIdx << std::endl;
            throw RSGISImageCalcException("The image being filled should be the first in the list");
        }
    }
    
    void RSGISTimeseriesFillImgImageComposite::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException)
    {
        if(numIntVals != 1)
        {
            throw RSGISImageCalcException("There should only be a single integer band.");
        }
        if((numfloatVals % this->getNumOutBands()) != 0)
        {
            throw RSGISImageCalcException("The number of float bands should be divisable by the number of output bands");
        }
        
        if(intBandValues[0] > 0)
        {
            unsigned int imgIdx = 0;
            imgIdx = this->imgIdxLUT[(intBandValues[0])];
            //std::cout << "this->imgIdxLUT["<<intBandValues[0]<<"] = " << this->imgIdxLUT[intBandValues[0]] << std::endl;
            if((imgIdx >= this->nLUT))
            {
                throw RSGISImageCalcException("LUT has incorrect valid.");
            }
            if(imgIdx > 0)
            {
                imgIdx = imgIdx - 1;
            }
            
            int offset = (this->getNumOutBands() * imgIdx);
            
            //std::cout << "offset = " << offset << std::endl;
            for(unsigned int i = 0; i < this->getNumOutBands(); ++i)
            {
                output[i] = floatBandValues[offset+i];
            }
        }
        else
        {
            for(unsigned int i = 0; i < this->getNumOutBands(); ++i)
            {
                output[i] = floatBandValues[i];
            }
        }
    }
    
    
    
    RSGISCombineImgBands2SingleBand::RSGISCombineImgBands2SingleBand(double noDataVal) : RSGISCalcImageValue(1)
    {
        this->noDataVal = noDataVal;
    }
    
    void RSGISCombineImgBands2SingleBand::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException)
    {
        if(numIntVals != 1)
        {
            throw RSGISImageCalcException("Should only have a single integer reference band.");
        }
        
        if(((intBandValues[0]-1)>=0) & ((intBandValues[0]-1) < numfloatVals))
        {
            output[0] = floatBandValues[intBandValues[0]-1];
        }
        else
        {
            output[0] = noDataVal;
        }
        
    }
    
    RSGISCombineImgBands2SingleBand::~RSGISCombineImgBands2SingleBand()
    {
        
    }
    
    
    
    
    
}}

