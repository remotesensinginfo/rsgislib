/*
 *  RSGISSpeckleFilters.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 09/03/2012.
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

#ifndef RSGISSpeckleFilters_H
#define RSGISSpeckleFilters_H

#include <iostream>

#include "common/RSGISImageException.h"

#include "filtering/RSGISImageFilterException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "filtering/RSGISImageFilter.h"

namespace rsgis{namespace filter{
	
	class DllExport RSGISLeeFilter : public RSGISImageFilter
    {
        /**
         
         Lee (1980) filter. Implemented for multiplicative noise, as described in Lopes et. al. (1990).
         nLooks is to the equivalent number of looks and is passed in as a filter parameter.
         
         J. Lee, 1980, “Digital Image-Enhancement and Noise Filtering by Use of Local Statistics,” IEEE Transactions on Pattern Analysis and Machine Intelligence, vol. 2, no. 2, pp. 165–168
         
         The implementation of the algorithm is as described:

         http://resources.arcgis.com/en/help/main/10.1/index.html#/Speckle_function/009t000001z7000000/

         */
        
    public: 
        
        RSGISLeeFilter(int numberOutBands, int size, std::string filenameEnding, unsigned int nLooks, float internalScaleFactor=100);
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException);
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw RSGISImageFilterException("Not implemented for Lee filter!");};;
        virtual void exportAsImage(std::string filename) throw(RSGISImageFilterException){throw RSGISImageFilterException("No image to output!");};
        ~RSGISLeeFilter();
    protected:
        unsigned int nLooks;
        float internalScaleFactor;
    };
}}

#endif



