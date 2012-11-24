/*
 *  RSGISClassMask.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/09/2012.
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

#include "RSGISClassMask.h"

namespace rsgis{namespace rastergis{
    
    
    RSGISClassMask::RSGISClassMask(const GDALRasterAttributeTable *attTable, unsigned int colIdx, std::string className): rsgis::img::RSGISCalcImageValue(1)
    {
        this->attTable = attTable;
        this->colIdx = colIdx;
        this->className = boost::trim_all_copy(className);
    }
    
    void RSGISClassMask::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            
            output[0] = 0;
            if(boost::trim_all_copy(std::string(attTable->GetValueAsString(fid, colIdx))) == className)
            {
                output[0] = 1;
            }
        }
    }
    
}}





