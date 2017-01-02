/*
 *  RSGISClumpImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/12/2011.
 *  Copyright 2011 RSGISLib.
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

#include "img/RSGISClumpImage.h"


namespace rsgis{namespace img{
            
    bool RSGISClumpImage::calcValue(bool firstIter, unsigned int numBands, unsigned int *dataCol, unsigned int **rowAbove, unsigned int **rowBelow, unsigned int *left, unsigned int *right)throw(RSGISImageCalcException)
    {
        bool valChange = false;
        
        for(unsigned int n = 0; n < numBands; ++n)
        {
            if(dataCol[n] != 0)
            {
                if((left[n] == 0) & (rowAbove[n][1] == 0) & (right[n] == 0) & (rowBelow[n][1] == 0))
                {
                    if(firstIter)
                    {
                        dataCol[n] = this->clumpCounter++;
                        valChange = true;
                    }
                }
                else
                {
                    unsigned int idx = 0;
                    if(left[n] != 0)
                    {
                        idx = left[n];
                    }
                    
                    if((right[n] != 0) & (idx == 0))
                    {
                        idx = right[n];
                    }
                    else if((right[n] != 0) & (right[n] < idx))
                    {
                        idx = right[n];
                    }
                    
                    if((rowAbove[n][1] != 0) & (idx == 0))
                    {
                        idx = rowAbove[n][1];
                    }
                    else if((rowAbove[n][1] != 0) & (rowAbove[n][1] < idx))
                    {
                        idx = rowAbove[n][1];
                    }
                    
                    if((rowBelow[n][1] != 0) & (idx == 0))
                    {
                        idx = rowBelow[n][1];
                    }
                    else if((rowBelow[n][1] != 0) & (rowBelow[n][1] < idx))
                    {
                        idx = rowBelow[n][1];
                    }
                    
                    if(dataCol[n] != idx)
                    {
                        dataCol[n] = idx;
                        valChange = true;
                    }
                }
            }
        }
        
        return valChange;
    }
    
    
    RSGISUniquePixelClumps::RSGISUniquePixelClumps(bool noDataDefined, float noDataVal) : RSGISCalcImageValue(1)
    {
        this->noDataDefined = noDataDefined;
        this->noDataVal = noDataVal;
        nextPixelVal = 1;
    }
    
    void RSGISUniquePixelClumps::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
    {
        bool validData = true;
        if(noDataDefined)
        {
            for(unsigned int i = 0; i < numBands; ++i)
            {
                if(bandValues[i] == noDataVal)
                {
                    validData = false;
                    break;
                }
            }
        }
        
        if(validData)
        {
            output[0] = nextPixelVal++;
        }
        else
        {
            output[0] = 0;
        }
    }
    
    RSGISUniquePixelClumps::~RSGISUniquePixelClumps()
    {
        
    }
    
}}

