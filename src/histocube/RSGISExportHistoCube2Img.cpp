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
    
    void RSGISExportBins2ImgBands::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
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
    
    RSGISExportHistSummaryStats2ImgBands::RSGISExportHistSummaryStats2ImgBands(unsigned numOutBands, unsigned int *dataArr, unsigned long dataArrLen, unsigned int nBins, float binScale, float binOffset, std::vector<rsgis::math::rsgissummarytype> summaryStats): rsgis::img::RSGISCalcImageValue(numOutBands)
    {
        this->dataArr = dataArr;
        this->dataArrLen = dataArrLen;
        this->nBins = nBins;
        this->binScale = binScale;
        this->binOffset = binOffset;
        this->summaryStats = summaryStats;
    }
    
    void RSGISExportHistSummaryStats2ImgBands::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
    {
        try
        {
            if((intBandValues[0] >= 0) & (intBandValues[0] <= dataArrLen))
            {
                unsigned int row = intBandValues[0];
                unsigned int rowIdx = row * this->nBins;
                
                unsigned int oIdx = 0;
                double outVal = 0.0;
                for(std::vector<rsgis::math::rsgissummarytype>::iterator iterStats = summaryStats.begin(); iterStats != summaryStats.end(); ++iterStats)
                {
                    outVal = 0.0;
                    if((*iterStats) == rsgis::math::sumtype_min)
                    {
                        unsigned int firstBin = 0;
                        for(unsigned int i = 1; i < nBins; ++i)
                        {
                            if(this->dataArr[rowIdx + i] > 0)
                            {
                                firstBin = i;
                                break;
                            }
                        }
                        outVal = (firstBin*this->binScale)+this->binOffset;
                    }
                    else if((*iterStats) == rsgis::math::sumtype_max)
                    {
                        unsigned int lastBin = 0;
                        for(unsigned int i = 1; i < nBins; ++i)
                        {
                            if(this->dataArr[rowIdx + i] > 0)
                            {
                                lastBin = i;
                            }
                        }
                        outVal = (lastBin*this->binScale)+this->binOffset;
                    }
                    else if((*iterStats) == rsgis::math::sumtype_mean)
                    {
                        double countVals = 0.0;
                        for(unsigned int i = 1; i < nBins; ++i)
                        {
                            countVals += this->dataArr[rowIdx + i];
                            outVal += (((i*this->binScale)+this->binOffset) * this->dataArr[rowIdx + i]);
                        }
                        if(countVals > 0)
                        {
                            outVal = outVal/countVals;
                        }
                        else
                        {
                            outVal = 0.0;
                        }
                    }
                    else if((*iterStats) == rsgis::math::sumtype_stddev)
                    {
                        double countVals = 0.0;
                        double meanVal = 0.0;
                        for(unsigned int i = 1; i < nBins; ++i)
                        {
                            countVals += this->dataArr[rowIdx + i];
                            meanVal += (((i*this->binScale)+this->binOffset) * this->dataArr[rowIdx + i]);
                        }
                        if(countVals > 0)
                        {
                            meanVal = meanVal/countVals;
                            
                            for(unsigned int i = 1; i < nBins; ++i)
                            {
                                countVals += this->dataArr[rowIdx + i];
                                outVal += (meanVal - (((i*this->binScale)+this->binOffset) * this->dataArr[rowIdx + i])) * (meanVal - (((i*this->binScale)+this->binOffset) * this->dataArr[rowIdx + i]));
                            }
                            outVal = outVal/countVals;
                        }
                        else
                        {
                            outVal = 0.0;
                        }
                    }
                    else if((*iterStats) == rsgis::math::sumtype_median)
                    {
                        unsigned long countVals = 0;
                        for(unsigned int i = 1; i < nBins; ++i)
                        {
                            countVals += this->dataArr[rowIdx + i];
                            /*
                            if(i == 0)
                            {
                                std::cout << this->dataArr[rowIdx + i];
                            }
                            else
                            {
                                std::cout << ", " << this->dataArr[rowIdx + i];
                            }
                            */
                        }
                        //std::cout << std::endl;
                        //std::cout << "countVals = " << countVals << std::endl;
                        if(countVals > 0)
                        {
                            unsigned long thres = countVals/2;
                            unsigned long countValsLow = 0;
                            unsigned long countValsUp = 0;
                            unsigned int medBin = 0;
                            //std::cout << "thres = " << thres << std::endl;
                            for(unsigned int i = 1; i < nBins; ++i)
                            {
                                countValsLow = countValsUp;
                                countValsUp += this->dataArr[rowIdx + i];
                                if( (countValsLow < thres) & (countValsUp >= thres))
                                {
                                    medBin = i;
                                }
                            }
                            //std::cout << "medBin = " << medBin << std::endl;
                            outVal = (medBin*this->binScale)+this->binOffset;
                            //std::cout << "outVal = " << outVal << std::endl;
                        }
                        else
                        {
                            outVal = 0.0;
                        }
                        //std::cout << std::endl;
                    }
                    else if((*iterStats) == rsgis::math::sumtype_range)
                    {
                        unsigned int firstBin = 0;
                        unsigned int lastBin = 0;
                        bool first = true;
                        for(unsigned int i = 1; i < nBins; ++i)
                        {
                            if(this->dataArr[rowIdx + i] > 0)
                            {
                                if(first)
                                {
                                    firstBin = i;
                                    first = false;
                                }
                                lastBin = i;
                            }
                        }
                        outVal = ((lastBin*this->binScale)+this->binOffset) - ((firstBin*this->binScale)+this->binOffset);
                    }
                    else if((*iterStats) == rsgis::math::sumtype_mode)
                    {
                        unsigned int maxBin = 0;
                        unsigned int maxBinVal = 0;
                        for(unsigned int i = 1; i < nBins; ++i)
                        {
                            if(i == 1)
                            {
                                maxBin = i;
                                maxBinVal = this->dataArr[rowIdx + i];
                            }
                            else if(this->dataArr[rowIdx + i] > maxBinVal)
                            {
                                maxBin = i;
                                maxBinVal = this->dataArr[rowIdx + i];
                            }
                        }
                        outVal = ((maxBin*this->binScale)+this->binOffset);
                    }
                    else if((*iterStats) == rsgis::math::sumtype_sum)
                    {
                        for(unsigned int i = 1; i < nBins; ++i)
                        {
                            outVal += (((i*this->binScale)+this->binOffset) * this->dataArr[rowIdx + i]);
                        }
                    }
                    else
                    {
                        throw rsgis::img::RSGISImageCalcException("Summary static was not recognised.");
                    }
                    
                    output[oIdx] = outVal;
                    ++oIdx;
                }
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    RSGISExportHistSummaryStats2ImgBands::~RSGISExportHistSummaryStats2ImgBands()
    {
        
    }
    
}}


