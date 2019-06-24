/*
 *  RSGISApplyGainOffset2Img.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/03/2017.
 *  Copyright 2017 RSGISLib. All rights reserved.
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

#include "RSGISApplyGainOffset2Img.h"

namespace rsgis{namespace img{
    
    
    RSGISRescaleImageData::RSGISRescaleImageData(int numOutputBands, float cNoDataVal, float cOffset, float cGain, float nNoDataVal, float nOffset, float nGain) : RSGISCalcImageValue(numOutputBands)
    {
        this->cNoDataVal = cNoDataVal;
        this->cOffset = cOffset;
        this->cGain = cGain;
        
        this->nNoDataVal = nNoDataVal;
        this->nOffset = nOffset;
        this->nGain = nGain;
    }
    
    void RSGISRescaleImageData::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        for(int i = 0; i < numBands; ++i)
        {
            if(bandValues[i] == this->cNoDataVal)
            {
                output[i] = this->nNoDataVal;
            }
            else
            {
                output[i] = (((bandValues[i]-cOffset)/cGain) * nGain) + nOffset;
            }
        }
    }
    
    RSGISRescaleImageData::~RSGISRescaleImageData()
    {
        
    }
    
}}

