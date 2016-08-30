/*
 *  RSGISApplySubtractOffsets.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/01/2014.
 *  Copyright 2014 RSGISLib. All rights reserved.
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

#include "RSGISApplySubtractOffsets.h"

namespace rsgis{namespace calib{
    
    
    RSGISApplyDarkObjSubtractOffsets::RSGISApplyDarkObjSubtractOffsets(unsigned int numImageBands, bool nonNegative, float noDataVal, bool useNoDataVal, float darkObjReflVal):rsgis::img::RSGISCalcImageValue(numImageBands)
    {
        this->numImageBands = numImageBands;
        this->nonNegative = nonNegative;
        this->noDataVal = noDataVal;
        this->useNoDataVal = useNoDataVal;
        this->darkObjReflVal = darkObjReflVal;
    }
    
    void RSGISApplyDarkObjSubtractOffsets::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if( (numBands/2) != this->numImageBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of image bands in the calcImageValue function needs to be half the input image... - Something has gone wrong and should have been caught eariler to provide a useless error message!");
        }
        
        for(unsigned int i = 0; i < this->numImageBands; ++i)
        {
            if(this->useNoDataVal)
            {
                if(bandValues[i] != noDataVal)
                {
                    output[i] = (bandValues[i] - bandValues[i+this->numImageBands]) + this->darkObjReflVal;
                    
                    if(nonNegative)
                    {
                        if(output[i] <= 0)
                        {
                            output[i] = 1.0;
                        }
                    }
                }
                else
                {
                    output[i] = noDataVal;
                }
            }
            else
            {
                output[i] = (bandValues[i] - bandValues[i+this->numImageBands]) + this->darkObjReflVal;
                
                if(nonNegative)
                {
                    if(output[i] <= 0)
                    {
                        output[i] = 1.0;
                    }
                }
            }
        }
        
    }
    
    RSGISApplyDarkObjSubtractOffsets::~RSGISApplyDarkObjSubtractOffsets()
    {
        
    }
    
    
    RSGISApplyDarkObjSubtractSingleOffsets::RSGISApplyDarkObjSubtractSingleOffsets(unsigned int numImageBands, std::vector<double> offsetValues, bool nonNegative, float noDataVal, bool useNoDataVal, float darkObjReflVal):rsgis::img::RSGISCalcImageValue(numImageBands)
    {
        this->numImageBands = numImageBands;
        this->nonNegative = nonNegative;
        this->noDataVal = noDataVal;
        this->useNoDataVal = useNoDataVal;
        this->darkObjReflVal = darkObjReflVal;
        this->offsetValues = offsetValues;
    }
    
    void RSGISApplyDarkObjSubtractSingleOffsets::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        for(unsigned int i = 0; i < this->numImageBands; ++i)
        {
            if(this->useNoDataVal)
            {
                if(bandValues[i] != noDataVal)
                {
                    output[i] = (bandValues[i] - this->offsetValues.at(i)) + this->darkObjReflVal;
                    
                    if(nonNegative)
                    {
                        if(output[i] <= 0)
                        {
                            output[i] = 1.0;
                        }
                    }
                }
                else
                {
                    output[i] = noDataVal;
                }
            }
            else
            {
                output[i] = (bandValues[i] - bandValues[i+this->numImageBands]) + this->darkObjReflVal;
                
                if(nonNegative)
                {
                    if(output[i] <= 0)
                    {
                        output[i] = 1.0;
                    }
                }
            }
        }
        
    }
    
    RSGISApplyDarkObjSubtractSingleOffsets::~RSGISApplyDarkObjSubtractSingleOffsets()
    {
        
    }
    
}}

