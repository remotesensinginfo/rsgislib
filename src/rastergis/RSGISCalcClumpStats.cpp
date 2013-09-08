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
        try
        {
            // Get Attribute table
            const GDALRasterAttributeTable *attTableTmp = clumpDS->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                attTable = new GDALRasterAttributeTable();
            }
            
            // Make sure it is long enough and extend if required.
            int numRows = attTable->GetRowCount();
            
            double maxVal = 0;
            clumpDS->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            
            if(maxVal > numRows)
            {                
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
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
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISCalcClumpStats::calcImageClumpPercentiles(GDALDataset *clumpDS, GDALDataset *imageDS, std::vector<rsgis::rastergis::RSGISBandAttPercentiles*> *bandPercentiles) throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            // Get Attribute table
            const GDALRasterAttributeTable *attTableTmp = clumpDS->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                attTable = new GDALRasterAttributeTable();
            }
            
            // Make sure it is long enough and extend if required.
            int numRows = attTable->GetRowCount();
            int numImageBands = imageDS->GetRasterCount();
            
            double maxVal = 0;
            clumpDS->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            
            if(maxVal > numRows)
            {                
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
            
            int numColumns = attTable->GetColumnCount();
            std::string colName = "";
            for(int i = 0; i < numColumns; ++i)
            {
                colName = std::string(attTable->GetNameOfCol(i));
                for(std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>::iterator iterBands = bandPercentiles->begin(); iterBands != bandPercentiles->end(); ++iterBands)
                {
                    if((!(*iterBands)->fieldIdxDef) && (colName == (*iterBands)->fieldName))
                    {
                        (*iterBands)->fieldIdx = i;
                        (*iterBands)->fieldIdxDef = true;
                    }
                }
            }
            
            for(std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>::iterator iterBands = bandPercentiles->begin(); iterBands != bandPercentiles->end(); ++iterBands)
            {
                if(!(*iterBands)->fieldIdxDef)
                {
                    attTable->CreateColumn((*iterBands)->fieldName.c_str(), GFT_Real, GFU_Generic);
                    (*iterBands)->fieldIdx = numColumns++;
                    (*iterBands)->fieldIdxDef = true;
                }
            }
            
            std::vector<double> ***data = new std::vector<double>**[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                data[i] = new std::vector<double>*[numImageBands];
                for(size_t j = 0; j < numImageBands; ++j)
                {
                    data[i][j] = new std::vector<double>();
                }
            }
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpDS;
            datasets[1] = imageDS;
            
            RSGISPopDataWithClusterPxlValue *calcImgValStats = new RSGISPopDataWithClusterPxlValue(data);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgValStats);
            calcImageStats.calcImage(datasets, 2);
            delete calcImgValStats;
            delete[] datasets;
                        
            for(size_t i = 0; i < numRows; ++i)
            {
                for(std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>::iterator iterBands = bandPercentiles->begin(); iterBands != bandPercentiles->end(); ++iterBands)
                {
                    std::sort(data[i][(*iterBands)->band-1]->begin(), data[i][(*iterBands)->band-1]->end());
                    attTable->SetValue(i, (*iterBands)->fieldIdx, gsl_stats_quantile_from_sorted_data(&(*data[i][(*iterBands)->band-1])[0], 1, data[i][(*iterBands)->band-1]->size(), ((double)(*iterBands)->percentile)/100.0));
                }
                
                for(size_t j = 0; j < numImageBands; ++j)
                {
                    delete data[i][j];
                }
                delete[] data[i];
            }
            delete[] data;
            
            clumpDS->GetRasterBand(1)->SetDefaultRAT(attTable);
        }
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISCalcClumpStats::populateColourTable(GDALDataset *clumpDS, GDALDataset *imageDS, unsigned int red, unsigned int green, unsigned int blue) throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            unsigned int numBands = imageDS->GetRasterCount();
            if(red > numBands)
            {
                throw rsgis::RSGISAttributeTableException("The red band is not within the image.");
            }
            if(green > numBands)
            {
                throw rsgis::RSGISAttributeTableException("The green band is not within the image.");
            }
            if(blue > numBands)
            {
                throw rsgis::RSGISAttributeTableException("The blue band is not within the image.");
            }
            
            double maxVal = 0;
            clumpDS->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            
            size_t numClumps = boost::lexical_cast<size_t>(maxVal)+1;
            
            size_t *redVals = new size_t[numClumps];
            size_t *greenVals = new size_t[numClumps];
            size_t *blueVals = new size_t[numClumps];
            size_t *count = new size_t[numClumps];
            
            for(size_t i = 0; i < numClumps; ++i)
            {
                redVals[i] = 0;
                greenVals[i] = 0;
                blueVals[i] = 0;
                count[i] = 0;
            }
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpDS;
            datasets[1] = imageDS;
            
            RSGISGetColourTableValues *calcImgValStats = new RSGISGetColourTableValues(redVals, greenVals, blueVals, count, red, green, blue);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgValStats);
            calcImageStats.calcImage(datasets, 2);
            delete calcImgValStats;
            delete[] datasets;
            
            
            GDALColorTable *clrTab = clumpDS->GetRasterBand(1)->GetColorTable();
            bool newClrTab = false;
            if(clrTab == NULL)
            {
                clrTab = new GDALColorTable();
                newClrTab = true;
            }
            GDALColorEntry *clr = NULL;
            for(size_t i = 0; i < numClumps; ++i)
            {
                clr = new GDALColorEntry();
                if(count[i] > 0)
                {
                    clr->c1 = redVals[i]/count[i];
                    clr->c2 = greenVals[i]/count[i];
                    clr->c3 = blueVals[i]/count[i];
                }
                else
                {
                    clr->c1 = 0;
                    clr->c2 = 0;
                    clr->c3 = 0;
                }
                clr->c4 = 255;
                clrTab->SetColorEntry(i, clr);
            }
            clumpDS->GetRasterBand(1)->SetColorTable(clrTab);
            if(newClrTab)
            {
                delete clrTab;
            }
            
        }
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISCalcClumpStats::calcValidPixelCount(GDALDataset *clumpDS, GDALDataset *imageDS, float noData, std::string validPixelCount) throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            // Get Attribute table
            const GDALRasterAttributeTable *attTableTmp = clumpDS->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                attTable = new GDALRasterAttributeTable();
            }
            
            // Make sure it is long enough and extend if required.
            int numRows = attTable->GetRowCount();
            
            double maxVal = 0;
            clumpDS->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            
            if(maxVal > numRows)
            {
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
            size_t *validPxlCount = new size_t[numRows];
            
            for(size_t i = 0; i < numRows; ++i)
            {
                validPxlCount[i] = 0;
            }
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpDS;
            datasets[1] = imageDS;
            
            RSGISGetValidPxlCounts *calcImgValStats = new RSGISGetValidPxlCounts(validPxlCount, noData);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgValStats);
            calcImageStats.calcImage(datasets, 2);
            delete calcImgValStats;
            delete[] datasets;
            
            RSGISRasterAttUtils attUtils;
            unsigned int colIdx = attUtils.findColumnIndexOrCreate(attTable, validPixelCount, GFT_Integer);
            unsigned int colRatioIdx = attUtils.findColumnIndexOrCreate(attTable, "ValidPxlRatio", GFT_Real);
            unsigned int histColIdx = attUtils.findColumnIndex(attTable, "Histogram");
            
            for(size_t i = 0; i < numRows; ++i)
            {
                //std::cout << i << ":\t" << validPxlCount[i] << std::endl;
                attTable->SetValue(i, colIdx, (int)validPxlCount[i]);
                if(validPxlCount[i] > 0)
                {
                    attTable->SetValue(i, colRatioIdx, ((double)validPxlCount[i])/attTable->GetValueAsDouble(i, histColIdx));
                }
                else
                {
                    attTable->SetValue(i, colRatioIdx, 0.0);
                }
            }
            
            clumpDS->GetRasterBand(1)->SetDefaultRAT(attTable);
        }
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
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
        //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            //std::cout << "FID: " << fid << std::endl;
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if(boost::math::isfinite(bandValues[(*iterBands)->band]))
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
                if(boost::math::isfinite(bandValues[(*iterBands)->band]))
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
    
    
    RSGISPopDataWithClusterPxlValue::RSGISPopDataWithClusterPxlValue(std::vector<double> ***data) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->data = data;
    }
    
    void RSGISPopDataWithClusterPxlValue::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            for(int i = 1; i < numBands; ++i)
            {
                data[fid][i-1]->push_back(bandValues[i]);
            }
        }
    }
    
    RSGISPopDataWithClusterPxlValue::~RSGISPopDataWithClusterPxlValue()
    {
        
    }
    
    
    
    RSGISGetColourTableValues::RSGISGetColourTableValues(size_t *redSum, size_t *greenSum, size_t *blueSum, size_t *countVals, unsigned int redIdx, unsigned int greenIdx, unsigned int blueIdx) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->redSum = redSum;
        this->greenSum = greenSum;
        this->blueSum = blueSum;
        this->countVals = countVals;
        this->redIdx = redIdx;
        this->greenIdx = greenIdx;
        this->blueIdx = blueIdx;
    }
		
    void RSGISGetColourTableValues::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            redSum[fid] += bandValues[redIdx];
            greenSum[fid] += bandValues[greenIdx];
            blueSum[fid] += bandValues[blueIdx];
            ++countVals[fid];
        }
    }
    
    RSGISGetColourTableValues::~RSGISGetColourTableValues()
    {
        
    }
    

    RSGISGetValidPxlCounts::RSGISGetValidPxlCounts(size_t *validPxlCount, float noDataValue) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->validPxlCount = validPxlCount;
        this->noDataValue = noDataValue;
    }
    
    void RSGISGetValidPxlCounts::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            
            bool validPxl = true;
            for(int i = 0; i < numBands; ++i)
            {
                if(bandValues[i] == noDataValue)
                {
                    validPxl = false;
                    break;
                }
            }
            
            if(validPxl)
            {
                ++validPxlCount[fid];
            }
        }
    }
    
    RSGISGetValidPxlCounts::~RSGISGetValidPxlCounts()
    {
        
    }
    
    
    
    
    
    RSGISPopulateWithImageStats::RSGISPopulateWithImageStats()
    {
        
    }
    
    void RSGISPopulateWithImageStats::populateImageWithRasterGISStats(GDALDataset *clumpsDataset, bool addColourTable, bool calcImagePyramids) throw(rsgis::RSGISImageException)
    {
        try
        {
            rsgis::utils::RSGISTextUtils txtUtils;
            RSGISRasterAttUtils attUtils;
            
            GDALRasterBand *band = clumpsDataset->GetRasterBand(1);
            
            double max = 0;
            double min = 0;
            
            std::cout << "Get Image Min and Max.\n";
            band->ComputeStatistics(false, &min, &max, NULL, NULL, StatsTextProgress, NULL);
            
            if(min < 0)
            {
                throw rsgis::RSGISImageException("The minimum value is less than zero.");
            }
            
            unsigned int maxHistVal = ceil(max);
            unsigned int *histo = new unsigned int[maxHistVal];
            
            for(unsigned int i = 0; i < maxHistVal; ++i)
            {
                histo[i] = 0;
            }
            
            std::cout << "Get Image Histogram.\n";
            RSGISGetClumpsHistogram *calcImgHisto = new RSGISGetClumpsHistogram(histo, maxHistVal);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgHisto);
            calcImageStats.calcImage(&clumpsDataset, 1);
            delete calcImgHisto;
            
            const GDALRasterAttributeTable *attTableTmp = band->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                attTable = new GDALRasterAttributeTable();
            }
            attTable->SetRowCount(maxHistVal);
            
            band->SetMetadataItem("STATISTICS_HISTOBINFUNCTION", "direct");
            band->SetMetadataItem("STATISTICS_HISTOMIN", "0");
            band->SetMetadataItem("STATISTICS_HISTOMAX", txtUtils.int64bittostring(maxHistVal).c_str());
            band->SetMetadataItem("STATISTICS_HISTONUMBINS", txtUtils.int64bittostring(maxHistVal).c_str());
            
            unsigned int histoColIdx = attUtils.findColumnIndexOrCreate(attTable, "Histogram", GFT_Integer);
            
            std::string histoVals = "";
            for(unsigned int i = 0; i < maxHistVal; ++i)
            {
                if(i == 0)
                {
                    histoVals = txtUtils.int64bittostring(histo[i]);
                }
                else
                {
                    histoVals += std::string("|") + txtUtils.int64bittostring(histo[i]);
                }
                
                attTable->SetValue(i, histoColIdx, (int)histo[i]);
            }
            
            band->SetMetadataItem("STATISTICS_HISTOBINVALUES", histoVals.c_str());
            
            if(addColourTable)
            {
                std::cout << "Adding a colour table.\n";
                GDALColorTable *clrTab = new GDALColorTable();
                GDALColorEntry *clrEntry = new GDALColorEntry();
                srand(time(NULL));
                for(unsigned int i = 0; i < maxHistVal; ++i)
                {
                    if(i == 0)
                    {
                        clrEntry->c1 = 0;
                        clrEntry->c2 = 0;
                        clrEntry->c3 = 0;
                        clrEntry->c4 = 255;
                    }
                    else
                    {
                        clrEntry->c1 = rand() % 255 + 1;
                        clrEntry->c2 = rand() % 255 + 1;
                        clrEntry->c3 = rand() % 255 + 1;
                        clrEntry->c4 = 255;
                    }
                    clrTab->SetColorEntry(i, clrEntry);
                }
                band->SetColorTable(clrTab);
                delete clrTab;
            }
            
            std::cout << "Writing RAT to file.\n";
            band->SetDefaultRAT(attTable);
            delete attTable;
            
            if(calcImagePyramids)
            {
                std::cout << "Calculating Image Pyramids.\n";
                int nLevels[] = { 4, 8, 16, 32, 64, 128, 256, 512 };
                int nOverviews = 0;
                int mindim = 0;
                const char *pszType = "NEAREST";
                
                /* first we work out how many overviews to build based on the size */
                if(clumpsDataset->GetRasterXSize() < clumpsDataset->GetRasterYSize())
                {
                    mindim = clumpsDataset->GetRasterXSize();
                }
                else
                {
                    mindim = clumpsDataset->GetRasterYSize();
                }
                
                nOverviews = 0;
                for(int i = 0; i < 8; i++)
                {
                    if( (mindim/nLevels[i]) > 33 )
                    {
                        ++nOverviews;
                    }
                }
                
                clumpsDataset->BuildOverviews(pszType, nOverviews, nLevels, 0, NULL, StatsTextProgress, NULL);
            }
                        
            delete[] histo;
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    RSGISPopulateWithImageStats::~RSGISPopulateWithImageStats()
    {
        
    }
    
    
    
    RSGISGetClumpsHistogram::RSGISGetClumpsHistogram(unsigned int *histogram, unsigned int maxVal):rsgis::img::RSGISCalcImageValue(0)
    {
        this->histogram = histogram;
        this->maxVal = maxVal;
    }

    void RSGISGetClumpsHistogram::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if((bandValues[0] > 0) & (bandValues[0] < maxVal))
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            ++histogram[fid];
        }
    }
    
    RSGISGetClumpsHistogram::~RSGISGetClumpsHistogram()
    {
        
    }
    
    
	
}}



