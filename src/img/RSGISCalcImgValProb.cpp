/*
 *  RSGISCalcImgValProb.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/06/2016.
 *  Copyright 2016 RSGISLib.
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


#include "RSGISCalcImgValProb.h"

namespace rsgis{namespace img{
    

    void RSGISCalcImgValProb::calcMaskImgPxlValProb(GDALDataset *inImgDS, std::vector<unsigned int> inImgBandIdxs, GDALDataset *inMaskDS, int maskVal, std::string outputImage, std::string gdalFormat, std::vector<float> histBinWidths, bool calcHistBinWidth, bool useImgNoData, bool rescaleProbs)throw(RSGISImageCalcException)
    {
        try
        {
            if(!calcHistBinWidth)
            {
                if(inImgBandIdxs.size() != histBinWidths.size())
                {
                    throw RSGISImageCalcException("The number of bin widths specified must equal to number of bands specified for the histogram.");
                }
            }
            
            
            unsigned int numHistDIMS = inImgBandIdxs.size();
            
            std::cout << "Calculate Image Statistics\n";
            
            int numBands = inImgDS->GetRasterCount();
            
            double *noDataVals = new double[numBands];
            ImageStats **stats = new ImageStats*[numBands];
            for(int i = 0; i < numBands; ++i)
            {
                stats[i] = new ImageStats();
                stats[i]->max = 0;
                stats[i]->min = 0;
                stats[i]->mean = 0;
                stats[i]->sum = 0;
                stats[i]->stddev = 0;
                
                noDataVals[i] = inImgDS->GetRasterBand(i+1)->GetNoDataValue();
            }
            delete[] noDataVals;
            rsgis::img::RSGISImageStatistics imgStats;
            imgStats.calcImageStatisticsMask(inImgDS, inMaskDS, maskVal, stats, noDataVals, useImgNoData, numBands, false, false);
            
            
            double *bandMin = new double[numHistDIMS];
            double *bandMax = new double[numHistDIMS];
            unsigned long *numBins = new unsigned long[numHistDIMS];
            noDataVals = new double[numHistDIMS];
            for(unsigned int i = 0; i < numHistDIMS; ++i)
            {
                bandMin[i] = stats[inImgBandIdxs.at(i)-1]->min;
                bandMax[i] = stats[inImgBandIdxs.at(i)-1]->max;
                double range = bandMax[i] - bandMin[i];
                double numBinsFloatTmp = range/histBinWidths.at(i);
                int numBinsTmp = ceil(numBinsFloatTmp);
                double calcHistRange = numBinsTmp*histBinWidths.at(i);
                if((bandMin[i] + calcHistRange) < bandMax[i])
                {
                    numBinsTmp = numBinsTmp + 1;
                }
                numBins[i] = numBinsTmp;
                noDataVals[i] = inImgDS->GetRasterBand(inImgBandIdxs.at(i))->GetNoDataValue();
                std::cout << "Band No Data = " << noDataVals[i] << std::endl;
                std::cout << "Band " << inImgBandIdxs.at(i) << ":\t[" << bandMin[i] << ", " << bandMax[i] << "] (" << histBinWidths.at(i) << "): " << numBins[i] << std::endl;
            }
                
            for(int i = 0; i < numBands; ++i)
            {
                delete stats[i];
            }
            delete[] stats;
            
            
            std::cout << "Create and Populate n-d Histogram\n";
            
            unsigned long totalNumBins = numBins[0];
            for(unsigned int i = 1; i < numHistDIMS; ++i)
            {
                totalNumBins = totalNumBins * numBins[i];
            }
            double *hist = new double[totalNumBins];
            for(unsigned long i = 0; i < totalNumBins; ++i)
            {
                hist[i] = 0.0;
            }
            
            RSGISCalcImagePopNDHist calcImageStats = RSGISCalcImagePopNDHist(inImgBandIdxs, maskVal, noDataVals, useImgNoData, bandMin, bandMax, histBinWidths, numBins, hist, totalNumBins);
            RSGISCalcImage calcImg = RSGISCalcImage(&calcImageStats, "", true);
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = inMaskDS;
            datasets[1] = inImgDS;
            calcImg.calcImage(datasets, 1, 1);
            delete[] datasets;
            
            unsigned long nPxl = 0;
            for(unsigned long i = 0; i < totalNumBins; ++i)
            {
                nPxl += hist[i];
            }
            
            for(unsigned long i = 0; i < totalNumBins; ++i)
            {
                if(hist[i] > 0)
                {
                    hist[i] = hist[i] / nPxl;
                }
                else
                {
                    hist[i] = 0.0;
                }
            }
            
            if(rescaleProbs)
            {
                double maxVal = 0;
                bool first = true;
                for(unsigned long i = 0; i < totalNumBins; ++i)
                {
                    if(hist[i] > 0)
                    {
                        if(first)
                        {
                            maxVal = hist[i];
                            first = false;
                        }
                        else if(hist[i] > maxVal)
                        {
                            maxVal = hist[i];
                        }
                    }
                }
                
                double mulVal = 1/maxVal;
                for(unsigned long i = 0; i < totalNumBins; ++i)
                {
                    if(hist[i] > 0)
                    {
                        hist[i] = hist[i] * mulVal;
                    }
                }
                
            }
            
            std::cout << "Populate the output image\n";
            calcImg.calcImage(&inImgDS, 1, outputImage, false, NULL, gdalFormat, GDT_Float32);
            
            delete[] hist;
            delete[] bandMin;
            delete[] bandMax;
            delete[] numBins;
            delete[] noDataVals;
        }
        catch(RSGISImageCalcException e)
        {
            throw e;
        }
        catch(RSGISImageBandException e)
        {
            throw RSGISImageCalcException(e.what());
        }
        catch(RSGISException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISImageCalcException(e.what());
        }
    }

    
    
    
    RSGISCalcImagePopNDHist::RSGISCalcImagePopNDHist(std::vector<unsigned int> inImgBandIdxs, long maskVal, double *noDataVals, bool useNoData, double *bandMin, double *bandMax, std::vector<float> histBinWidths, unsigned long *numBins, double *hist, unsigned long totalNumBins):RSGISCalcImageValue(1)
    {
        this->inImgBandIdxs = inImgBandIdxs;
        this->maskVal = maskVal;
        this->noDataVals = noDataVals;
        this->useNoData = useNoData;
        this->bandMin = bandMin;
        this->bandMax = bandMax;
        this->histBinWidths = histBinWidths;
        this->numBins = numBins;
        this->hist = hist;
        this->totalNumBins = totalNumBins;
    }
    
    void RSGISCalcImagePopNDHist::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals)throw(RSGISImageCalcException)
    {
        if(numIntVals != 1)
        {
            throw RSGISImageCalcException("The number of mask bands should be 1.");
        }
        
        if(intBandValues[0] == this->maskVal)
        {
            unsigned int bIdx = 0;
            unsigned long *binIdxs = new unsigned long[inImgBandIdxs.size()];
            bool notValid = false;
            for(unsigned int i = 0; i < inImgBandIdxs.size(); ++i)
            {
                bIdx = inImgBandIdxs.at(i)-1;
                if(!boost::math::isnan(floatBandValues[bIdx]))
                {
                    if((!useNoData) || (useNoData && (floatBandValues[bIdx] != this->noDataVals[i])))
                    {
                        double binF = (floatBandValues[bIdx] - bandMin[i])/histBinWidths.at(i);
                        binIdxs[i] = floor(binF+0.5);
                    }
                    else
                    {
                        notValid = true;
                        break;
                    }
                }
                else
                {
                    notValid = true;
                    break;
                }
                
            }
            
            if(!notValid)
            {
                unsigned long idx = 0;
                unsigned long tDIMS = 0;
                for(unsigned int i = 0; i < inImgBandIdxs.size(); ++i)
                {
                    if(i == 0)
                    {
                        idx = binIdxs[i];
                        tDIMS = this->numBins[i];
                    }
                    else
                    {
                        idx = idx + (binIdxs[i] * tDIMS);
                        tDIMS = tDIMS * this->numBins[i];
                    }
                }
                if(idx < totalNumBins)
                {
                    hist[idx] = hist[idx] + 1;
                }
            }
            
            delete[] binIdxs;
        }
    }
    
    void RSGISCalcImagePopNDHist::calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException)
    {
        output[0] = 0.0;
        unsigned long *binIdxs = new unsigned long[inImgBandIdxs.size()];
        bool notValid = false;
        unsigned int bIdx = 0;
        for(unsigned int i = 0; i < inImgBandIdxs.size(); ++i)
        {
            bIdx = inImgBandIdxs.at(i)-1;
            if(!boost::math::isnan(bandValues[bIdx]))
            {
                if((bandValues[bIdx] >= bandMin[i]) & (bandValues[bIdx] <= bandMax[i]))
                {
                    if((!useNoData) || (useNoData && (bandValues[bIdx] != this->noDataVals[i])))
                    {
                        double binF = (bandValues[bIdx] - bandMin[i])/histBinWidths.at(i);
                        binIdxs[i] = floor(binF+0.5);
                    }
                    else
                    {
                        notValid = true;
                        break;
                    }
                }
                else
                {
                    notValid = true;
                    break;
                }
            }
            else
            {
                notValid = true;
                break;
            }
            
        }
        
        if(!notValid)
        {
            unsigned long idx = 0;
            unsigned long tDIMS = 0;
            for(unsigned int i = 0; i < inImgBandIdxs.size(); ++i)
            {
                if(i == 0)
                {
                    idx = binIdxs[i];
                    tDIMS = this->numBins[i];
                }
                else
                {
                    idx = idx + (binIdxs[i] * tDIMS);
                    tDIMS = tDIMS * this->numBins[i];
                }
            }
            if(idx < totalNumBins)
            {
                output[0] = hist[idx];
            }
            else
            {
                output[0] = 0;
            }
        }
        
        delete[] binIdxs;
    }
    
    RSGISCalcImagePopNDHist::~RSGISCalcImagePopNDHist()
    {
        
    }
}}

