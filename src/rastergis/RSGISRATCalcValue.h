/*
 *  RSGISRATCalcValue.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 4/05/2014.
 *  Copyright 2014 RSGISLib.
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

#ifndef RSGISRATCalcValue_H
#define RSGISRATCalcValue_H

#include <iostream>
#include <string>
#include <math.h>

#include "common/RSGISAttributeTableException.h"

#include "rastergis/RSGISRasterAttUtils.h"


namespace rsgis{namespace rastergis{
    
    class DllExport RSGISRATCalcValue
    {
    public:
        RSGISRATCalcValue(){};
        virtual void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException) = 0;
        virtual ~RSGISRATCalcValue(){};
    };
    
    
}}

#endif


