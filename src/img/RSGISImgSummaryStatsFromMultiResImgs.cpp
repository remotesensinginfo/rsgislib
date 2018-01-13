/*
 *  RSGISImgSummaryStatsFromMultiResImgs.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/01/2018.
 *  Copyright 2018 RSGISLib.
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

#include "RSGISImgSummaryStatsFromMultiResImgs.h"


namespace rsgis{namespace img{
    
    RSGISImgSummaryStatsFromMultiResImgs::RSGISImgSummaryStatsFromMultiResImgs()
    {
        
    }
    
    void RSGISImgSummaryStatsFromMultiResImgs::calcSummaryStats4LowResPxlsFromHighResImg(GDALDataset *refDataset, GDALDataset *statsDataset, unsigned int statsImgBand, std::string outImg, std::string gdalFormat, GDALDataType gdalDataType, bool useNoData, std::vector<rsgis::math::rsgissummarytype> sumStats, unsigned int xIOGrid, unsigned int yIOGrid) throw(RSGISImageException)
    {
        try
        {
            RSGISCalcHighResImgSummaryStats calcImgValSums = RSGISCalcHighResImgSummaryStats(sumStats.size(), sumStats);
            rsgis::img::RSGISCalcImageMultiImgRes calcImg = rsgis::img::RSGISCalcImageMultiImgRes(&calcImgValSums);
            calcImg.calcImageHighResForLowRegions(refDataset, statsDataset, statsImgBand, outImg, gdalFormat, gdalDataType, useNoData, xIOGrid, yIOGrid);
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    RSGISImgSummaryStatsFromMultiResImgs::~RSGISImgSummaryStatsFromMultiResImgs()
    {
        
    }
    
    
    RSGISCalcHighResImgSummaryStats::RSGISCalcHighResImgSummaryStats(int numberOutBands, std::vector<rsgis::math::rsgissummarytype> sumStats) : rsgis::img::RSGISCalcValuesFromMultiResInputs(numberOutBands)
    {
        this->sumStats = sumStats;
    }
    
    void RSGISCalcHighResImgSummaryStats::calcImageValue(float *bandValues, int numInVals, bool useNoData, float noDataVal, double *output) throw(RSGISImageCalcException)
    {
        //std::cout << "HERE - RSGISCalcHighResImgSummaryStats::calcImageValue\n";
        
        if(this->sumStats.size() != this->getNumOutBands())
        {
            throw RSGISImageCalcException("The number of output image bands and summary stats is not equal.");
        }
        
        bool needBasic = false;
        bool needStdDev = false;
        for(int i = 0; i < this->sumStats.size(); ++i)
        {
            if(this->sumStats.at(i) == rsgis::math::sumtype_min)
            {
                needBasic = true;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_max)
            {
                needBasic = true;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_mean)
            {
                needBasic = true;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_median)
            {
                throw RSGISImageCalcException("Sorry. Median is not yet implemented.");
                needBasic = true;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_range)
            {
                needBasic = true;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_stddev)
            {
                needBasic = true;
                needStdDev = true;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_sum)
            {
                needBasic = true;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_mode)
            {
                throw RSGISImageCalcException("Sorry. Mode is not yet implemented.");
                needBasic = true;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_count)
            {
                needBasic = true;
            }
            else
            {
                throw RSGISImageCalcException("The summary type specified is unknown.");
            }
        }
        
        double min = 0;
        double max = 0;
        double mean = 0;
        double stdDev = 0;
        double sum = 0;
        double count = 0;
        
        if(needBasic)
        {
            bool first = true;
            
            for(int i = 0; i < numInVals; ++i)
            {
                if(!useNoData || (bandValues[i] != noDataVal))
                {
                    if(first)
                    {
                        min = bandValues[i];
                        max = bandValues[i];
                        first = true;
                    }
                    
                    if(bandValues[i] < min)
                    {
                        min = bandValues[i];
                    }
                    
                    if(bandValues[i] > max)
                    {
                        max = bandValues[i];
                    }
                    
                    sum += bandValues[i];
                    
                    count = count + 1;
                }
            }
            
            mean = sum / count;
            
            if(needStdDev)
            {
                double sqDiffSum = 0.0;
                for(int i = 0; i < numInVals; ++i)
                {
                    if(!useNoData || (bandValues[i] != noDataVal))
                    {
                        sqDiffSum += ((bandValues[i] - mean) * (bandValues[i] - mean));
                    }
                }
                stdDev = std::sqrt(sqDiffSum);
            }
        }
        
        for(int i = 0; i < this->sumStats.size(); ++i)
        {
            if(this->sumStats.at(i) == rsgis::math::sumtype_min)
            {
                output[i] = min;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_max)
            {
                output[i] = max;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_mean)
            {
                output[i] = mean;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_median)
            {
                throw RSGISImageCalcException("Sorry. Median is not yet implemented.");
                //output[i] = min;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_range)
            {
                output[i] = max - min;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_stddev)
            {
                output[i] = stdDev;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_sum)
            {
                output[i] = sum;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_mode)
            {
                throw RSGISImageCalcException("Sorry. Mode is not yet implemented.");
                //output[i] = min;
            }
            else if(this->sumStats.at(i) == rsgis::math::sumtype_count)
            {
                output[i] = count;
            }
            else
            {
                throw RSGISImageCalcException("The summary type specified is unknown.");
            }
        }
    }
    
    RSGISCalcHighResImgSummaryStats::~RSGISCalcHighResImgSummaryStats()
    {
        
    }
    
}}



