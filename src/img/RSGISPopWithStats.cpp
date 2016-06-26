/*
 *  RSGISPopWithStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/04/2012.
 *  Rewritten 26/06/2016
 *  Copyright 2016 RSGISLib. All rights reserved.
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

#include "RSGISPopWithStats.h"

namespace rsgis { namespace img {

    
    void RSGISPopWithStats::calcPopStats( GDALDataset *imgDS, bool useNoDataVal, float noDataVal, bool calcPyramid, std::vector<int> decimatFactors ) throw(rsgis::RSGISImageException)
    {
        rsgis::utils::RSGISTextUtils textUtils;
        
        int numBands = imgDS->GetRasterCount();
        GDALRasterBand *band = NULL;
        const char *layerType = imgDS->GetRasterBand( 1 )->GetMetadataItem( "LAYER_TYPE" );
        if( layerType != NULL )
        {
            std::string bandType = std::string(layerType);
            if( bandType != "athematic" )
            {
                rsgis::RSGISImageException("Use rsgislib.rastergis.populateStats for thematic data.");
            }
        }
        
        // Iteration 1 through image: Min, Max and Mean
        double *minVal = new double[numBands];
        double *maxVal = new double[numBands];
        double *meanVal = new double[numBands];
        unsigned long *nVals = new unsigned long[numBands];
        
        for(int i = 0; i < numBands; ++i)
        {
            minVal[i] = 0;
            maxVal[i] = 0;
            meanVal[i] = 0;
            nVals[i] = 0;
        }
        
        RSGISCalcImageMinMaxMean calcImageStats = RSGISCalcImageMinMaxMean(numBands, useNoDataVal, noDataVal, minVal, maxVal, meanVal, nVals);
        RSGISCalcImage calcImg = RSGISCalcImage(&calcImageStats, "", true);
        calcImg.calcImage(&imgDS, 1);
        
        for(int i = 0; i < numBands; ++i)
        {
            //std::cout << "Band " << (i+1) << std::endl;
            //std::cout << "\tMax = " << maxVal[i] << std::endl;
            //std::cout << "\tMin = " << minVal[i] << std::endl;
            meanVal[i] = meanVal[i] / nVals[i];
            //std::cout << "\tMean = " << meanVal[i] << std::endl;
            //std::cout << "\nVals = " << nVals[i] << std::endl;
            
            band = imgDS->GetRasterBand(i+1);
            band->SetMetadataItem( "STATISTICS_MINIMUM", textUtils.doubletostring(minVal[i]).c_str(), NULL );
            band->SetMetadataItem( "STATISTICS_MAXIMUM", textUtils.doubletostring(maxVal[i]).c_str(), NULL );
            band->SetMetadataItem( "STATISTICS_MEAN", textUtils.doubletostring(meanVal[i]).c_str(), NULL );
            
            if(useNoDataVal)
            {
                band->SetMetadataItem( "STATISTICS_EXCLUDEDVALUES", textUtils.floattostring(noDataVal).c_str(), NULL );
                band->SetNoDataValue(noDataVal);
            }
        }
        
        // Iteration 2 through image: Std Dev + Histogram
        unsigned int numHistBins = 256;
        unsigned int **bandHist = new unsigned int*[numBands];
        
        std::string *histoType = new std::string[numBands];
        double *histMin = new double[numBands];
        double *histMax = new double[numBands];
        double *histWidth = new double[numBands];
        double range = 0.0;
        
        for(int i = 0; i < numBands; ++i)
        {
            GDALDataType dataType = imgDS->GetRasterBand( i+1 )->GetRasterDataType();
            
            histMin[i] = -0.5;
            histMax[i] = 255.5;
            histWidth[i] = 1.0;
            histoType[i] = "direct";
            
            if( dataType == GDT_Byte )
            {
                histMin[i] = -0.5;
                histMax[i] = 255.5;
                histWidth[i] = 1.0;
                histoType[i] = "direct";
            }
            else if( (dataType == GDT_UInt16) | (dataType == GDT_Int16) | (dataType == GDT_UInt32) | (dataType == GDT_Int32))
            {
                range = maxVal[i] - minVal[i];
                
                if(range < 256)
                {
                    histMin[i] = minVal[i]-0.5;
                    histMax[i] = minVal[i]+256;
                    histWidth[i] = 1.0;
                    histoType[i] = "direct";
                }
                else
                {
                    histWidth[i] = range/256;
                    histMin[i] = minVal[i] - (histWidth[i]/2);
                    histMax[i] = maxVal[i] + (histWidth[i]/2);
                    histoType[i] = "linear";
                }
            }
            else
            {
                range = maxVal[i] - minVal[i];
                histWidth[i] = range/256;
                histMin[i] = minVal[i] - (histWidth[i]/2);
                histMax[i] = maxVal[i] + (histWidth[i]/2);
                histoType[i] = "linear";
            }
            
            //std::cout << "Hist Range [" << histMin[i] << ", " << histMax[i] << "] : " << histWidth[i] << "\n";
        }
        
        double *stdDevVal = new double[numBands];
        unsigned long *nVals2 = new unsigned long[numBands];
        for(int i = 0; i < numBands; ++i)
        {
            stdDevVal[i] = 0;
            nVals2[i] = 0;
            bandHist[i] = new unsigned int[numHistBins];
            for(unsigned int j = 0; j < numHistBins; ++j)
            {
                bandHist[i][j] = 0;
            }
        }
        RSGISCalcImageStdDevPopHist calcImageHistSD = RSGISCalcImageStdDevPopHist(numBands, useNoDataVal, noDataVal, minVal, maxVal, meanVal, stdDevVal, nVals2, histMin, histMax, histWidth, bandHist, numHistBins);
        calcImg = RSGISCalcImage(&calcImageHistSD, "", true);
        calcImg.calcImage(&imgDS, 1);
        
        
        for(int i = 0; i < numBands; ++i)
        {
            //std::cout << "Band " << (i+1) << std::endl;
            //std::cout << "\nVals2 = " << nVals2[i] << std::endl;
            stdDevVal[i] = sqrt(stdDevVal[i] / nVals2[i]);
            //std::cout << "\tStdDev = " << stdDevVal[i] << std::endl;
            
            band = imgDS->GetRasterBand(i+1);
            band->SetMetadataItem( "STATISTICS_STDDEV", textUtils.doubletostring(stdDevVal[i]).c_str(), NULL );
            
            
            band->SetMetadataItem( "STATISTICS_HISTOMIN", textUtils.doubletostring(minVal[i]).c_str(), NULL );
            band->SetMetadataItem( "STATISTICS_HISTOMAX", textUtils.doubletostring(maxVal[i]).c_str(), NULL );
            band->SetMetadataItem( "STATISTICS_HISTONUMBINS", "256", NULL );
            band->SetMetadataItem( "STATISTICS_HISTOBINFUNCTION", histoType[i].c_str(), NULL );
            
            // Calc Mode and Median:
            double modeVal = 0.0;
            double medianVal = 0.0;
            long pxlCount = 0;
            bool foundMedian = false;
            long medianPxl = nVals2[i]/2;
            unsigned long modeBinFreq = 0;
            std::string histBinsStr = "";
            for(int j = 0; j < 256; ++j)
            {
                if(j == 0)
                {
                    modeBinFreq = bandHist[i][j];
                    modeVal = minVal[i] + (j * histWidth[i]);
                }
                else if(bandHist[i][j] >  modeBinFreq)
                {
                    modeBinFreq = bandHist[i][j];
                    modeVal = minVal[i] + (j * histWidth[i]);
                }
                
                if(j == 0)
                {
                    histBinsStr = histBinsStr + textUtils.uInt64bittostring(bandHist[i][j]);
                }
                else
                {
                    histBinsStr = histBinsStr + "|" + textUtils.uInt64bittostring(bandHist[i][j]);
                }
                
                pxlCount = pxlCount + bandHist[i][j];
                
                if((pxlCount > medianPxl) & (!foundMedian))
                {
                    if( labs(pxlCount-medianPxl) > labs((pxlCount-bandHist[i][j])-medianPxl) )
                    {
                        medianVal = minVal[i] + ((j-1) * histWidth[i]);
                    }
                    else
                    {
                        medianVal = minVal[i] + (j * histWidth[i]);
                    }
                    foundMedian = true;
                }
            }
            
            band->SetMetadataItem( "STATISTICS_MODE", textUtils.doubletostring(modeVal).c_str(), NULL );
            band->SetMetadataItem( "STATISTICS_MEDIAN", textUtils.doubletostring(medianVal).c_str(), NULL );
            band->SetMetadataItem( "STATISTICS_HISTOBINVALUES", histBinsStr.c_str(), NULL );
            
            GDALRasterAttributeTable *attTable = imgDS->GetRasterBand( i+1 )->GetDefaultRAT();
            if(attTable == NULL)
            {
                attTable = new GDALDefaultRasterAttributeTable();
            }
            attTable->SetRowCount(256);
            
            unsigned int histoColIdx = this->findColumnIndexOrCreate(attTable, "Histogram", GFT_Real, GFU_PixelCount);
            attTable->ValuesIO(GF_Write, histoColIdx, 0, 256, (int*) bandHist[i]);
        }
        
        if(calcPyramid)
        {
            std::cout << "Calculating Image Pyramids.\n";
            if(decimatFactors.size() == 0)
            {
                int minOverviewDim = 33;
                
                int minDim = imgDS->GetRasterXSize();
                if(imgDS->GetRasterYSize() < minDim)
                {
                    minDim = imgDS->GetRasterYSize();
                }
                
                int nLevels[] = { 4, 8, 16, 32, 64, 128, 256, 512 };
                for(int i = 0; i < 8; i++)
                {
                    if( (minDim/nLevels[i]) > minOverviewDim )
                    {
                        decimatFactors.push_back(nLevels[i]);
                    }
                }
            }
            
            this->addPyramids(imgDS, decimatFactors);
        }
    }
    
    void RSGISPopWithStats::addPyramids(GDALDataset *imgDS, std::vector<int> decimatFactors) throw(rsgis::RSGISImageException)
    {
        int nOverviews = decimatFactors.size();
        
        int nLastProgress = -1;
        imgDS->BuildOverviews("AVERAGE", nOverviews, decimatFactors.data(), 0, NULL,  (GDALProgressFunc)StatsTextProgress, &nLastProgress );
    }
    
    unsigned int RSGISPopWithStats::findColumnIndexOrCreate(GDALRasterAttributeTable *gdalATT, std::string colName, GDALRATFieldType dType, GDALRATFieldUsage dUsage) throw(rsgis::RSGISImageException)
    {
        int numColumns = gdalATT->GetColumnCount();
        bool foundCol = false;
        unsigned int colIdx = 0;
        for(int i = 0; i < numColumns; ++i)
        {
            if(std::string(gdalATT->GetNameOfCol(i)) == colName)
            {
                foundCol = true;
                colIdx = i;
                break;
            }
        }
        
        if(!foundCol)
        {
            gdalATT->CreateColumn(colName.c_str(), dType, dUsage);
            colIdx = numColumns;
        }
        
        return colIdx;
    }
    
    
    RSGISCalcImageMinMaxMean::RSGISCalcImageMinMaxMean(int numVals, bool useNoData, double noDataVal, double *minVal, double *maxVal, double *sumVal, unsigned long *nVals): RSGISCalcImageValue(0)
    {
        this->numVals = numVals;
        this->useNoData = useNoData;
        this->noDataVal = noDataVal;
        this->minVal = minVal;
        this->maxVal = maxVal;
        this->sumVal = sumVal;
        this->nVals = nVals;
        
        this->first = new bool[numVals];
        for(int i = 0; i < numVals; ++i)
        {
            this->first[i] = true;
        }
        
    }
    
    void RSGISCalcImageMinMaxMean::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        if(numVals != numBands)
        {
            throw RSGISImageCalcException("Number of values and bands are difference; error somewhere.");
        }
        
        for(int i = 0; i < numBands; ++i)
        {
            if(this->useNoData)
            {
                if(bandValues[i] != this->noDataVal)
                {
                    if(first[i])
                    {
                        this->minVal[i] = bandValues[i];
                        this->maxVal[i] = bandValues[i];
                        this->sumVal[i] = bandValues[i];
                        ++this->nVals[i];
                        first[i] = false;
                    }
                    else
                    {
                        if(bandValues[i] < this->minVal[i])
                        {
                            this->minVal[i] = bandValues[i];
                        }
                        else if(bandValues[i] > this->maxVal[i])
                        {
                            this->maxVal[i] = bandValues[i];
                        }
                        this->sumVal[i] += bandValues[i];
                        ++this->nVals[i];
                    }
                }
            }
            else
            {
                if(first[i])
                {
                    this->minVal[i] = bandValues[i];
                    this->maxVal[i] = bandValues[i];
                    this->sumVal[i] = bandValues[i];
                    ++this->nVals[i];
                    first[i] = false;
                }
                else
                {
                    if(bandValues[i] < this->minVal[i])
                    {
                        this->minVal[i] = bandValues[i];
                    }
                    else if(bandValues[i] > this->maxVal[i])
                    {
                        this->maxVal[i] = bandValues[i];
                    }
                    this->sumVal[i] += bandValues[i];
                    ++this->nVals[i];
                }
            }
        }
        
    }
    
    RSGISCalcImageMinMaxMean::~RSGISCalcImageMinMaxMean()
    {
        
    }
    
    
    RSGISCalcImageStdDevPopHist::RSGISCalcImageStdDevPopHist(int numVals, bool useNoData, double noDataVal, double *minVal, double *maxVal, double *meanVal, double *sumVal, unsigned long *nVals, double *histMin, double *histMax, double *histWidth, unsigned int **bandHist, unsigned int numBins): RSGISCalcImageValue(0)
    {
        this->numVals = numVals;
        this->useNoData = useNoData;
        this->noDataVal = noDataVal;
        this->minVal = minVal;
        this->maxVal = maxVal;
        this->meanVal = meanVal;
        this->sumVal = sumVal;
        this->nVals = nVals;
        this->histMin = histMin;
        this->histMax = histMax;
        this->histWidth = histWidth;
        this->bandHist = bandHist;
        this->numBins = numBins;
        
        this->first = new bool[numVals];
        for(int i = 0; i < numVals; ++i)
        {
            this->first[i] = true;
        }
        
    }
    
    void RSGISCalcImageStdDevPopHist::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        if(numVals != numBands)
        {
            throw RSGISImageCalcException("Number of values and bands are difference; error somewhere.");
        }
        
        for(int i = 0; i < numBands; ++i)
        {
            if(this->useNoData)
            {
                if(bandValues[i] != this->noDataVal)
                {
                    if(first[i])
                    {
                        double diff = (bandValues[i] - this->meanVal[i]);
                        this->sumVal[i] = this->sumVal[i] + (diff * diff);
                        ++this->nVals[i];
                        first[i] = false;
                    }
                    else
                    {
                        double diff = (bandValues[i] - this->meanVal[i]);
                        this->sumVal[i] = this->sumVal[i] + (diff * diff);
                        ++this->nVals[i];
                    }
                    
                    unsigned int histIdx = floor(((bandValues[i] - this->minVal[i]) / this->histWidth[i])+0.5);
                    if(histIdx >= this->numBins)
                    {
                        histIdx = (this->numBins-1);
                    }
                    ++bandHist[i][histIdx];
                }
            }
            else
            {
                if(first[i])
                {
                    double diff = (bandValues[i] - this->meanVal[i]);
                    this->sumVal[i] = this->sumVal[i] + (diff * diff);
                    ++this->nVals[i];
                    first[i] = false;
                }
                else
                {
                    double diff = (bandValues[i] - this->meanVal[i]);
                    this->sumVal[i] = this->sumVal[i] + (diff * diff);
                    ++this->nVals[i];
                }
            }
        }
        
    }
    
    RSGISCalcImageStdDevPopHist::~RSGISCalcImageStdDevPopHist()
    {
        
    }
    
}}
 

