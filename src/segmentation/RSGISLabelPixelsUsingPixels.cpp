/*
 *  RSGISLabelPixelsUsingPixels.cpp
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 30/05/2012.
 *  Copyright 2012 RSGISLib.
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

#include "RSGISLabelPixelsUsingPixels.h"


namespace rsgis{namespace segment{

    RSGISLabelPixelsUsingPixelsCalcImg::RSGISLabelPixelsUsingPixelsCalcImg(int numberOutBands, bool ignoreZeros) : RSGISCalcImageValue(numberOutBands)
    {
        this->ignoreZeros = ignoreZeros;
        this->pixelCount = 0;
    }
    
    void RSGISLabelPixelsUsingPixelsCalcImg::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {

        bool nonZeroFound = false;
        if(ignoreZeros) // Check for zero values
        {
            for(int i = 0; i < numBands; ++i)
            {
                if(bandValues[i] != 0)
                {
                    nonZeroFound = true;
                    break;
                }
            }  
        }
        
        if(ignoreZeros & !nonZeroFound)
        {
            output[0] = 0;
        }
        else
        {
            output[0] = this->pixelCount;
            ++this->pixelCount;
        }
    }
    
    RSGISLabelPixelsUsingPixelsCalcImg::~RSGISLabelPixelsUsingPixelsCalcImg()
    {
        
    }
    
}}




