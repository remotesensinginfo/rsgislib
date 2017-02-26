/*
 *  RSGISExportHistoCube2Img.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/02/2017.
 *
 *  Copyright 2017 RSGISLib.
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

#include "RSGISExportHistoCube2Img.h"

namespace rsgis {namespace histocube{
    
    
    
    RSGISExportBins2ImgBands::RSGISExportBins2ImgBands(unsigned numOutBands, unsigned int *dataArr, unsigned long dataArrLen, unsigned int nBins, std::vector<unsigned int> binsIdxs): rsgis::img::RSGISCalcImageValue(numOutBands)
    {
        this->dataArr = dataArr;
        this->dataArrLen = dataArrLen;
        this->nBins = nBins;
        this->binsIdxs = binsIdxs;
    }
    
    void RSGISExportBins2ImgBands::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            if((intBandValues[0] >= 0) & (intBandValues[0] <= dataArrLen))
            {
                unsigned int row = intBandValues[0];
                unsigned int rowIdx = row * this->nBins;
                
                unsigned int oIdx = 0;
                for(std::vector<unsigned int>::iterator iterBins = binsIdxs.begin(); iterBins != binsIdxs.end(); ++iterBins)
                {
                    output[oIdx] = this->dataArr[rowIdx + (*iterBins)];
                    ++oIdx;
                }
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    RSGISExportBins2ImgBands::~RSGISExportBins2ImgBands()
    {
        
    }
    
}}


