/*
 *  RSGISExportColumns2Image.cpp
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

#include "RSGISExportColumns2Image.h"

namespace rsgis{namespace rastergis{
    

    RSGISExportColumns2ImageCalcImage::RSGISExportColumns2ImageCalcImage(int numberOutBands, const GDALRasterAttributeTable *attTable, std::vector<unsigned int> *colIdxs): rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->attTable = attTable;
        this->colIdxs = colIdxs;
    }
    
    void RSGISExportColumns2ImageCalcImage::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            
            for(size_t i = 0; i < colIdxs->size(); ++i)
            {
                output[i] = attTable->GetValueAsDouble(fid, colIdxs->at(i));
            }
        }
    }

}}





