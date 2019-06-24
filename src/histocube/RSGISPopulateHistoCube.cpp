/*
 *  RSGISPopulateHistoCube.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/02/2017.
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

#include "RSGISPopulateHistoCube.h"

namespace rsgis {namespace histocube{
    
    
    RSGISPopHistoCubeLayerFromImgBand::RSGISPopHistoCubeLayerFromImgBand(RSGISHistoCubeFile *hcFile, std::string layerName, unsigned int bandIdx, unsigned int maxRow, float scale, float offset, std::vector<int> bins) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->hcFile = hcFile;
        this->layerName = layerName;
        this->bandIdx = bandIdx;
        this->maxRow = maxRow;
        this->scale = scale;
        this->offset = offset;
        this->bins = bins;
        this->hcUtils = RSGISHistoCubeUtils();
        this->dataArrLen = bins.size();
        this->dataArr = new unsigned int[this->dataArrLen];
    }
    
    void RSGISPopHistoCubeLayerFromImgBand::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) 
    {
        try
        {
            if((intBandValues[0] >= 0) & (intBandValues[0] <= maxRow))
            {
                unsigned int row = intBandValues[0];
                float bandVal = (floatBandValues[bandIdx] * scale) + offset;
                int bandValInt = floor(bandVal + 0.5);
                long idx = this->hcUtils.getBinsIndex(bandValInt, this->bins);
                
                if((idx >= 0) | (idx < this->dataArrLen))
                {
                    this->hcFile->getHistoRow(this->layerName, row, this->dataArr, this->dataArrLen);
                    this->dataArr[idx] = this->dataArr[idx] + 1;
                    this->hcFile->setHistoRow(this->layerName, row, this->dataArr, this->dataArrLen);
                }
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    RSGISPopHistoCubeLayerFromImgBand::~RSGISPopHistoCubeLayerFromImgBand()
    {
        delete[] this->dataArr;
    }
    
    
    
    RSGISPopHistoCubeLayerFromImgBandInMem::RSGISPopHistoCubeLayerFromImgBandInMem(unsigned int *dataArr, unsigned long dataArrLen, unsigned int bandIdx, unsigned int maxRow, float scale, float offset, std::vector<int> bins) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->dataArrLen = dataArrLen;
        this->rowLen = bins.size();
        if((this->dataArrLen % this->rowLen) != 0)
        {
            rsgis::RSGISHistoCubeException("The data array did not a multiple of the number of bins.");
        }
        
        if((this->dataArrLen / this->rowLen) != maxRow)
        {
            rsgis::RSGISHistoCubeException("The data array is not the same length as the number of rows.");
        }
        
        this->dataArr = dataArr;
        this->layerName = layerName;
        this->bandIdx = bandIdx;
        this->maxRow = maxRow;
        this->scale = scale;
        this->offset = offset;
        this->bins = bins;
        this->hcUtils = RSGISHistoCubeUtils();
    }
    
    void RSGISPopHistoCubeLayerFromImgBandInMem::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) 
    {
        try
        {
            if((intBandValues[0] >= 0) & (intBandValues[0] <= maxRow))
            {
                unsigned int row = intBandValues[0];
                float bandVal = (floatBandValues[bandIdx] * scale) + offset;
                int bandValInt = floor(bandVal + 0.5);
                long binIdx = this->hcUtils.getBinsIndex(bandValInt, this->bins);
                
                if((binIdx >= 0) | (binIdx < this->rowLen))
                {
                    if(row == 0)
                    {
                        this->dataArr[binIdx] = this->dataArr[binIdx] + 1;
                    }
                    else
                    {
                        unsigned long arrBinIdx = (row * this->rowLen) + binIdx;
                        this->dataArr[arrBinIdx] = this->dataArr[arrBinIdx] + 1;
                    }
                }
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    RSGISPopHistoCubeLayerFromImgBandInMem::~RSGISPopHistoCubeLayerFromImgBandInMem()
    {

    }
    
    
}}


