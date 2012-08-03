/*
 *  RSGISCalcClumpStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2012.
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

#include "RSGISCalcClumpStats.h"

namespace rsgis{namespace rastergis{
	
    RSGISCalcClumpStats::RSGISCalcClumpStats()
    {
        
    }
    
    void RSGISCalcClumpStats::calcImageClumpStatistic(GDALDataset *clumpDS, GDALDataset *imageDS, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats) throw(rsgis::RSGISAttributeTableException)
    {
        // Get Attribute table
        GDALRasterAttributeTable *attTable = new GDALRasterAttributeTable(*clumpDS->GetRasterBand(1)->GetDefaultRAT());
        
        // Make sure it is long enough and extend if required.
        int numRows = attTable->GetRowCount();
        
        double maxVal = 0;
        clumpDS->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
        
        if(maxVal > numRows)
        {                
            attTable->SetRowCount(maxVal+1);
        }
        
        // Check whether a standard deviation is to be created (requires second iteration of image).
        bool calcStdDevs = false;
        for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
        {
            if(((*iterBands)->calcStdDev) & (!(*iterBands)->calcMean))
            {
                throw rsgis::RSGISAttributeTableException("If the standard deviation is required to be calculated then the mean must also be calculated.");
            }
            else if((*iterBands)->calcStdDev)
            {
                calcStdDevs = true;
            }
        }
        
        // Check whether columns already exist and if not create them.
        int numColumns = attTable->GetColumnCount();
        std::string colName = "";
        bool foundPxlCount = false;
        int pxlCountIdx = 0;
        for(int i = 0; i < numColumns; ++i)
        {
            colName = std::string(attTable->GetNameOfCol(i));
            if(colName == "PxlCount")
            {
                pxlCountIdx = i;
                foundPxlCount = true;
            }
            else
            {
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    if(((*iterBands)->calcMin) && (colName == (*iterBands)->minField))
                    {
                        (*iterBands)->minIdx = i;
                        (*iterBands)->minIdxDef = true;
                    }
                    else if(((*iterBands)->calcMax) && (colName == (*iterBands)->maxField))
                    {
                        (*iterBands)->maxIdx = i;
                        (*iterBands)->maxIdxDef = true;
                    }
                    else if(((*iterBands)->calcMean) && (colName == (*iterBands)->meanField))
                    {
                        (*iterBands)->meanIdx = i;
                        (*iterBands)->meanIdxDef = true;
                    }
                    else if(((*iterBands)->calcStdDev) && (colName == (*iterBands)->stdDevField))
                    {
                        (*iterBands)->stdDevIdx = i;
                        (*iterBands)->stdDevIdxDef = true;
                    }
                    else if(((*iterBands)->calcSum) && (colName == (*iterBands)->sumField))
                    {
                        (*iterBands)->sumIdx = i;
                        (*iterBands)->sumIdxDef = true;
                    }
                }
            }
        }
        
        if(!foundPxlCount)
        {
            attTable->CreateColumn("PxlCount", GFT_Integer, GFU_PixelCount);
            pxlCountIdx = numColumns++;
        }
        
        for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
        {
            if(((*iterBands)->calcMin) && (!(*iterBands)->minIdxDef))
            {
                attTable->CreateColumn((*iterBands)->minField.c_str(), GFT_Real, GFU_Generic);
                (*iterBands)->minIdx = numColumns++;
                (*iterBands)->minIdxDef = true;
            }
            if(((*iterBands)->calcMax) && (!(*iterBands)->maxIdxDef))
            {
                attTable->CreateColumn((*iterBands)->maxField.c_str(), GFT_Real, GFU_Generic);
                (*iterBands)->maxIdx = numColumns++;
                (*iterBands)->maxIdxDef = true;
            }
            if(((*iterBands)->calcMean) && (!(*iterBands)->meanIdxDef))
            {
                attTable->CreateColumn((*iterBands)->meanField.c_str(), GFT_Real, GFU_Generic);
                (*iterBands)->meanIdx = numColumns++;
                (*iterBands)->meanIdxDef = true;
            }
            if(((*iterBands)->calcStdDev) && (!(*iterBands)->stdDevIdxDef))
            {
                attTable->CreateColumn((*iterBands)->stdDevField.c_str(), GFT_Real, GFU_Generic);
                (*iterBands)->stdDevIdx = numColumns++;
                (*iterBands)->stdDevIdxDef = true;
            }
            if(((*iterBands)->calcSum) && (!(*iterBands)->sumIdxDef))
            {
                attTable->CreateColumn((*iterBands)->sumField.c_str(), GFT_Real, GFU_Generic);
                (*iterBands)->sumIdx = numColumns++;
                (*iterBands)->sumIdxDef = true;
            }
        }
        
        // Calculate statistics
        bool *firstVal = new bool[numRows];
        for(int i = 0; i < numRows; ++i)
        {
            firstVal[i] = true;
        }
        
        // Get Image Values
        GDALDataset **datasets = new GDALDataset*[2];
        datasets[0] = clumpDS;
        datasets[1] = imageDS;
        
        RSGISCalcClusterPxlValueStats *calcImgValStats = new RSGISCalcClusterPxlValueStats(attTable, pxlCountIdx, bandStats, firstVal);
        rsgis::img::RSGISCalcImage calcImageStats(calcImgValStats);
        calcImageStats.calcImage(datasets, 2);
        delete calcImgValStats;
        
        for(int i = 0; i < numRows; ++i)
        {
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->calcMean)
                {
                    if(attTable->GetValueAsInt(i, pxlCountIdx) == 0)
                    {
                        attTable->SetValue(i, (*iterBands)->meanIdx, 0);
                    }
                    else
                    {
                        attTable->SetValue(i, 
                                           (*iterBands)->meanIdx, 
                                           attTable->GetValueAsDouble(i, (*iterBands)->meanIdx)/attTable->GetValueAsDouble(i, pxlCountIdx)
                                           );
                    }
                }
            }
        }
        
        if(calcStdDevs)
        {
            for(int i = 0; i < numRows; ++i)
            {
                firstVal[i] = true;
            }
            
            // Get Image Values
            RSGISCalcClusterPxlValueStdDev *calcImgValStdDev = new RSGISCalcClusterPxlValueStdDev(attTable, bandStats, firstVal);
            rsgis::img::RSGISCalcImage calcImageStdDev(calcImgValStdDev);
            calcImageStdDev.calcImage(datasets, 2);
            delete calcImgValStdDev;
            
            for(int i = 0; i < numRows; ++i)
            {
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    if((*iterBands)->calcStdDev)
                    {
                        if(attTable->GetValueAsInt(i, pxlCountIdx) == 0)
                        {
                            attTable->SetValue(i, (*iterBands)->stdDevIdx, 0);
                        }
                        else
                        {
                            attTable->SetValue(i, 
                                               (*iterBands)->stdDevIdx, 
                                               sqrt(attTable->GetValueAsDouble(i, (*iterBands)->stdDevIdx)/attTable->GetValueAsDouble(i, pxlCountIdx))
                                               );
                        }
                    }
                }
            }
        }
        
        delete[] datasets;
        
        clumpDS->GetRasterBand(1)->SetDefaultRAT(attTable);
    }
        
    RSGISCalcClumpStats::~RSGISCalcClumpStats()
    {
        
    }
    
    
    
    
    
    RSGISCalcClusterPxlValueStats::RSGISCalcClusterPxlValueStats(GDALRasterAttributeTable *attTable, int countColIdx, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->countColIdx = countColIdx;
        this->bandStats = bandStats;
        this->firstVal = firstVal;
    }
    
    void RSGISCalcClusterPxlValueStats::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->calcMin)
                {
                    if(firstVal[fid])
                    {
                        attTable->SetValue(fid, (*iterBands)->minIdx, bandValues[(*iterBands)->band]);
                    }
                    else if(bandValues[(*iterBands)->band] < attTable->GetValueAsDouble(fid, (*iterBands)->minIdx))
                    {
                        attTable->SetValue(fid, (*iterBands)->minIdx, bandValues[(*iterBands)->band]);
                    }
                }
                
                if((*iterBands)->calcMax)
                {
                    if(firstVal[fid])
                    {
                        attTable->SetValue(fid, (*iterBands)->maxIdx, bandValues[(*iterBands)->band]);
                    }
                    else if(bandValues[(*iterBands)->band] > attTable->GetValueAsDouble(fid, (*iterBands)->maxIdx))
                    {
                        attTable->SetValue(fid, (*iterBands)->maxIdx, bandValues[(*iterBands)->band]);
                    }
                }
                
                if((*iterBands)->calcMean)
                {
                    if(firstVal[fid])
                    {
                        attTable->SetValue(fid, (*iterBands)->meanIdx, bandValues[(*iterBands)->band]);
                    }
                    else
                    {
                        attTable->SetValue(fid, (*iterBands)->meanIdx, (attTable->GetValueAsDouble(fid, (*iterBands)->meanIdx) + bandValues[(*iterBands)->band]));
                    }
                }
                
                if((*iterBands)->calcSum)
                {
                    if(firstVal[fid])
                    {
                        attTable->SetValue(fid, (*iterBands)->sumIdx, bandValues[(*iterBands)->band]);
                    }
                    else
                    {
                        attTable->SetValue(fid, (*iterBands)->sumIdx, (attTable->GetValueAsDouble(fid, (*iterBands)->sumIdx) + bandValues[(*iterBands)->band]));
                    }
                }
                
            }
            
            if(firstVal[fid])
            {
                attTable->SetValue(fid, countColIdx, 1);
                firstVal[fid] = false;
            }
            else
            {
                attTable->SetValue(fid, countColIdx, (attTable->GetValueAsInt(fid, countColIdx)+1));
            }
        }
    }
    
    RSGISCalcClusterPxlValueStats::~RSGISCalcClusterPxlValueStats()
    {
        
    }
    
    
    
    RSGISCalcClusterPxlValueStdDev::RSGISCalcClusterPxlValueStdDev(GDALRasterAttributeTable *attTable, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->bandStats = bandStats;
        this->firstVal = firstVal;
    }
    
    void RSGISCalcClusterPxlValueStdDev::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((*iterBands)->calcStdDev)
                {
                    double mean = attTable->GetValueAsDouble(fid, (*iterBands)->meanIdx);
                    double stdDevComp = pow(((double)(bandValues[(*iterBands)->band] - mean)), 2.0);
                    if(firstVal[fid])
                    {
                        attTable->SetValue(fid, (*iterBands)->stdDevIdx, stdDevComp);
                    }
                    else
                    {
                        attTable->SetValue(fid, (*iterBands)->stdDevIdx, (attTable->GetValueAsDouble(fid, (*iterBands)->stdDevIdx)+stdDevComp));
                    }
                }                
            }
            
            if(firstVal[fid])
            {
                firstVal[fid] = false;
            }
        }
    }
    
    RSGISCalcClusterPxlValueStdDev::~RSGISCalcClusterPxlValueStdDev()
    {
        
    }
    
    
    
	
}}



