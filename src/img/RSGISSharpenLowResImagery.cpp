/*
 *  RSGISSharpenLowResImagery.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/05/2017.
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

#include "RSGISSharpenLowResImagery.h"

namespace rsgis { namespace img {

    RSGISSharpenLowResBands::RSGISSharpenLowResBands(int numberOutBands, RSGISSharpenBandInfo *bandInfo, unsigned int nBandInfo, unsigned int nLowResBands, unsigned int nHighResBands, unsigned int defWinSize, int noDataVal, double *imgMinVal, double *imgMaxVal):RSGISCalcImageValue(numberOutBands)
    {
        this->bandInfo = bandInfo;
        this->nBandInfo = nBandInfo;
        this->nLowResBands = nLowResBands;
        this->nHighResBands = nHighResBands;
        this->defWinSize = defWinSize;
        this->noDataVal = noDataVal;
        this->imgMinVal = imgMinVal;
        this->imgMaxVal = imgMaxVal;
        this->nWinPxls = defWinSize * defWinSize;
        this->lowResPxlVals = new double*[nLowResBands];
        this->highResPxlVals = new double*[nHighResBands];
        for(int i = 0; i < nLowResBands; ++i)
        {
            this->lowResPxlVals[i] = new double[this->nWinPxls];
        }
        for(int i = 0; i < nHighResBands; ++i)
        {
            this->highResPxlVals[i] = new double[this->nWinPxls];
        }
        
        this->lowResBandIdxs = new unsigned int[nLowResBands];
        this->highResBandIdxs = new unsigned int[nHighResBands];
        unsigned int lbIdx = 0;
        unsigned int hbIdx = 0;
        for(unsigned int i = 0; i < nBandInfo; ++i)
        {
            if(this->bandInfo[i].status == rsgis_sharp_band_highres)
            {
                this->highResBandIdxs[hbIdx] = this->bandInfo[i].band-1;
                ++hbIdx;
            }
            else if(this->bandInfo[i].status == rsgis_sharp_band_lowres)
            {
                this->lowResBandIdxs[lbIdx] = this->bandInfo[i].band-1;
                ++lbIdx;
            }
        }
        
        linFits = new rsgis::math::RSGISLinearFitVals*[nHighResBands];
        for(unsigned int i = 0; i < nHighResBands; ++i)
        {
            linFits[i] = new rsgis::math::RSGISLinearFitVals();
            linFits[i]->coeff = 0.0;
            linFits[i]->intercept = 0.0;
            linFits[i]->pvar = 0.0;
            linFits[i]->slope = 0.0;
        }
        
        this->mathUtils = rsgis::math::RSGISMathsUtils();
    }
    
    void RSGISSharpenLowResBands::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException)
    {
        if(this->nBandInfo != numBands)
        {
            throw RSGISImageCalcException("The number of input bands and the number of band info objects must be the same.");
        }
        unsigned int winHSize = (winSize-1)/2;
        
        bool isNoData = true;
        for(unsigned int i = 0; i < numBands; ++i)
        {
            if((int(dataBlock[i][winHSize][winHSize])) != this->noDataVal)
            {
                isNoData = false;
            }
        }
        
        if(!isNoData)
        {
            unsigned int idx = 0;
            unsigned int hBIdx = 0;
            unsigned int lBIdx = 0;
            for(unsigned int i = 0; i < numBands; ++i)
            {
                if(this->bandInfo[i].status == rsgis_sharp_band_ignore)
                {
                    output[i] = dataBlock[i][winHSize][winHSize];
                }
                else if(this->bandInfo[i].status == rsgis_sharp_band_highres)
                {
                    output[i] = dataBlock[i][winHSize][winHSize];
                    idx = 0;
                    for(unsigned int n = 0; n < winSize; ++n)
                    {
                        for(unsigned int m = 0; m < winSize; ++m)
                        {
                            this->highResPxlVals[hBIdx][idx] = dataBlock[i][n][m];
                            ++idx;
                        }
                    }
                    hBIdx++;
                }
                else if(this->bandInfo[i].status == rsgis_sharp_band_lowres)
                {
                    idx = 0;
                    for(unsigned int n = 0; n < winSize; ++n)
                    {
                        for(unsigned int m = 0; m < winSize; ++m)
                        {
                            this->lowResPxlVals[lBIdx][idx] = dataBlock[i][n][m];
                            ++idx;
                        }
                    }
                    ++lBIdx;
                }
                else
                {
                    throw RSGISImageCalcException("Band status is not recognised - must be either ignore, low res or high res.");
                }
            }
            
            bool first = true;
            double maxCoeff = 0.0;
            unsigned int maxHRIdx = 0;
            for(unsigned int i = 0; i < nLowResBands; ++i)
            {
                first = true;
                maxCoeff = 0.0;
                maxHRIdx = 0;
                for(unsigned int j = 0; j < nHighResBands; ++j)
                {
                    linFits[j]->coeff = 0.0;
                    linFits[j]->intercept = 0.0;
                    linFits[j]->pvar = 0.0;
                    linFits[j]->slope = 0.0;
                    
                    this->mathUtils.performLinearFit(this->highResPxlVals[j], this->lowResPxlVals[i], this->nWinPxls, this->noDataVal, this->linFits[j]);
                    if(first)
                    {
                        first = false;
                        maxCoeff = this->linFits[j]->coeff;
                        maxHRIdx = j;
                    }
                    else if(this->linFits[j]->coeff > maxCoeff)
                    {
                        maxCoeff = this->linFits[j]->coeff;
                        maxHRIdx = j;
                    }
                }
                
                if( (!first) & (maxCoeff > 0.5))
                {
                    output[this->lowResBandIdxs[i]] = this->mathUtils.predFromLinearFit(dataBlock[this->highResBandIdxs[maxHRIdx]][winHSize][winHSize], this->linFits[maxHRIdx], this->imgMinVal[this->lowResBandIdxs[i]], this->imgMaxVal[this->lowResBandIdxs[i]]);
                }
                else
                {
                    output[this->lowResBandIdxs[i]] = dataBlock[this->lowResBandIdxs[i]][winHSize][winHSize];
                }
            }
        }
        else
        {
            for(unsigned int i = 0; i < numBands; ++i)
            {
                output[i] = this->noDataVal;
            }
        }
    }
    
    RSGISSharpenLowResBands::~RSGISSharpenLowResBands()
    {
        
    }
    
    
}}
