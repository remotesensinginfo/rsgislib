/*
 *  RSGISHydroDEMFillSoilleGratin94.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/07/2016.
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

#include "RSGISHydroDEMFillSoilleGratin94.h"

namespace rsgis{namespace calib{
    
    RSGISHydroDEMFillSoilleGratin94::RSGISHydroDEMFillSoilleGratin94()
    {
        
    }
    
    void RSGISHydroDEMFillSoilleGratin94::performSoilleGratin94Fill(GDALDataset *inDEMImgDS, GDALDataset *inValidImgDS, GDALDataset *outImgDS, bool calcBorderVal, long borderVal)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            int numBands = inDEMImgDS->GetRasterCount();
            if(numBands != 1)
            {
                rsgis::img::RSGISImageCalcException("The image to be filled should only have 1 image band.");
            }
            
            
            GDALDataset **datasets = new GDALDataset*[3];
            datasets[0] = inDEMImgDS;
            datasets[1] = inValidImgDS;
            datasets[2] = outImgDS;
            
            rsgis::img::RSGISImageUtils imgUtils;
            if(!imgUtils.doImageSpatAndExtMatch(datasets, 3))
            {
                rsgis::img::RSGISImageCalcException("The images provided do not all have the same size and/or spaital header. The input image (e.g., DEM) and valid area image must be excatly the same.");
            }
            delete[] datasets;
            
            rsgis::img::ImageStats *stats = new rsgis::img::ImageStats();
            
            int useNoData = false;
            double noDataVal = inDEMImgDS->GetRasterBand(1)->GetNoDataValue(&useNoData);
            
            if(useNoData)
            {
                std::cout << "Fill layer has a no data value of " << noDataVal << std::endl;
            }
            
            if(calcBorderVal)
            {
                rsgis::img::RSGISImageStatistics imgStats;
                imgStats.calcImageStatisticsMask(inDEMImgDS, inValidImgDS, 1, &stats, &noDataVal, useNoData, 1, true);
                
                if((stats->mean - stats->stddev) > stats->min)
                {
                    borderVal = floor((stats->mean - stats->stddev)+0.5);
                }
                else
                {
                    borderVal = floor((stats->mean)+0.5);
                }
                std::cout << "Calculated Border Value is " << borderVal << std::endl;
            }
            else
            {
                rsgis::img::RSGISImageStatistics imgStats;
                imgStats.calcImageStatisticsMask(inDEMImgDS, inValidImgDS, 1, &stats, &noDataVal, useNoData, 1, false);
            }
            
            minVal = (long)stats->min;
            maxVal = (long)stats->max;
            delete stats;
           
            numLevels = (maxVal - minVal)+1;
            
            std::cout << "Range of Values [" << minVal << ", " << maxVal << "] Needs " << numLevels << " Levels." << std::endl;
            
            if(!useNoData)
            {
                noDataVal = 0.0;
            }
            
            // Create the hierarchical queue.
            pxQ = new std::list<Q2DPxl>*[numLevels];
            for(long n = 0; n < numLevels; ++n)
            {
                pxQ[n] = new std::list<Q2DPxl>();
            }
            
            
            // Initialise the output image.
            std::cout << "Initalise the Output Image and find boundary pixels.\n";
            RSGISInitOutputImageSoilleGratin94 initOutImg = RSGISInitOutputImageSoilleGratin94(noDataVal, maxVal, borderVal, pxQ[0]);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&initOutImg);
            calcImage.calcImageWindowData(&inValidImgDS, 1, outImgDS, 3, true);
            
            Q2DPxl pxl;
            long hcrt = minVal;
            long imgVal = 0;
            long img2Val = 0;
            int feedback = numLevels/10.0;
            int feedbackCounter = 0;
            std::cout << "Perform Fill:\n";
            std::cout << "Started ." << std::flush;
            for(long n = 0; n < numLevels; ++n)
            {
                if((feedback != 0) && ((n % feedback) == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                while(!this->qEmpty(hcrt))
                {
                    pxl = this->qPopFront(hcrt);

                    // Get Neightbours
                    std::list<Q2DPxl> *nPxls = this->getNeighbours(pxl, inValidImgDS->GetRasterBand(1));
                    for(std::list<Q2DPxl>::iterator iterNPxls = nPxls->begin(); iterNPxls != nPxls->end(); ++iterNPxls)
                    {
                        imgVal = this->getPxlVal((*iterNPxls), inDEMImgDS->GetRasterBand(1));
                        img2Val = this->getPxlVal((*iterNPxls), outImgDS->GetRasterBand(1));

                        if(img2Val == maxVal)
                        {
                            img2Val = this->rtnMax(hcrt, imgVal);
                            this->setPxlVal((*iterNPxls), img2Val, outImgDS->GetRasterBand(1));
                            if(img2Val < maxVal)
                            {
                                this->qPushBack(img2Val, (*iterNPxls));
                            }
                        }
                    }
                    delete nPxls;
                    
                }
                
                ++hcrt;
            }
            std::cout << ". Complete.\n";
            
            for(long n = 0; n < numLevels; ++n)
            {
                delete pxQ[n];
            }
            delete[] pxQ;            
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    bool RSGISHydroDEMFillSoilleGratin94::qEmpty(long hcrt)
    {
        long qIdx = hcrt - this->minVal;
        bool empty = pxQ[qIdx]->empty();
        return empty;
    }
    
    Q2DPxl RSGISHydroDEMFillSoilleGratin94::qPopFront(long hcrt)
    {
        long qIdx = hcrt - this->minVal;
        Q2DPxl pxl = pxQ[qIdx]->front();
        pxQ[qIdx]->pop_front();
        return pxl;
    }
    
    void RSGISHydroDEMFillSoilleGratin94::qPushBack(long hcrt, Q2DPxl pxl)
    {
        long qIdx = hcrt - this->minVal;
        pxQ[qIdx]->push_back(pxl);
    }
    
    std::list<Q2DPxl>* RSGISHydroDEMFillSoilleGratin94::getNeighbours(Q2DPxl pxl, GDALRasterBand *inValidImg) throw(rsgis::img::RSGISImageCalcException)
    {
        std::list<Q2DPxl> *nPxls = new std::list<Q2DPxl>();
        
        long width = inValidImg->GetXSize();
        long height = inValidImg->GetYSize();
        
        long minXPxl = pxl.x - 1;
        long maxXPxl = pxl.x + 1;
        long minYPxl = pxl.y - 1;
        long maxYPxl = pxl.y + 1;
        
        if(minXPxl < 0)
        {
            minXPxl = 0;
        }
        if(minYPxl < 0)
        {
            minYPxl = 0;
        }
        if(maxXPxl >= width)
        {
            maxXPxl = width-1;
        }
        if(maxYPxl >= height)
        {
            maxYPxl = height-1;
        }

        int nWidth = (maxXPxl - minXPxl)+1;
        int nHeight = (maxYPxl - minYPxl)+1;
        
        float dataVal = 0;
        long cPxlX = 0;
        long cPxlY = minYPxl;
        for(int i = 0; i < nHeight; ++i)
        {
            cPxlX = minXPxl;
            for(int j = 0; j < nWidth; ++j)
            {
                if(!((cPxlX == pxl.x) & (cPxlY == pxl.y)))
                {
                    inValidImg->RasterIO(GF_Read, cPxlX, cPxlY, 1, 1, &dataVal, 1, 1, GDT_Float32, 0, 0);

                    if(dataVal == 1)
                    {
                        nPxls->push_back(Q2DPxl(cPxlX, cPxlY));
                    }
                }
                ++cPxlX;
            }
            ++cPxlY;
        }
        
        return nPxls;
    }
    
    long RSGISHydroDEMFillSoilleGratin94::getPxlVal(Q2DPxl pxl, GDALRasterBand *imgData)throw(rsgis::img::RSGISImageCalcException)
    {
        float outVal = 0;
        imgData->RasterIO(GF_Read, pxl.x, pxl.y, 1, 1, &outVal, 1, 1, GDT_Float32, 0, 0);
        return (long)outVal;
    }
    
    void RSGISHydroDEMFillSoilleGratin94::setPxlVal(Q2DPxl pxl, long val, GDALRasterBand *imgData)throw(rsgis::img::RSGISImageCalcException)
    {
        float outVal = val;
        imgData->RasterIO(GF_Write, pxl.x, pxl.y, 1, 1, &outVal, 1, 1, GDT_Float32, 0, 0);
    }
    
    long RSGISHydroDEMFillSoilleGratin94::rtnMax(long val1, long val2)
    {
        long outVal = val1;
        if(val2 > val1)
        {
            outVal = val2;
        }
        return outVal;
    }
    
    RSGISHydroDEMFillSoilleGratin94::~RSGISHydroDEMFillSoilleGratin94()
    {
        
    }
    

    RSGISInitOutputImageSoilleGratin94::RSGISInitOutputImageSoilleGratin94(double noDataVal, double dataVal, double borderVal, std::list<Q2DPxl> *pxQ): rsgis::img::RSGISCalcImageValue(1)
    {
        this->noDataVal = noDataVal;
        this->dataVal = dataVal;
        this->borderVal = borderVal;
        this->pxQ = pxQ;
    }
    
    void RSGISInitOutputImageSoilleGratin94::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
    {
        if(winSize != 3)
        {
            throw rsgis::img::RSGISImageCalcException("Window Size must be 3.");
        }
        
        int hWinSize = (winSize-1)/2;
        
        if(dataBlock[0][hWinSize][hWinSize] == 1)
        {
            output[0] = dataVal;
        }
        else
        {
            bool boundary = false;
            for(int i = 0; i < winSize; ++i)
            {
                for(int j = 0; j < winSize; ++j)
                {
                    if(dataBlock[0][i][j] == 1)
                    {
                        boundary = true;
                        break;
                    }
                }
                if(boundary)
                {
                    break;
                }
            }
            
            if(boundary)
            {
                output[0] = borderVal;
                this->pxQ->push_back(Q2DPxl(extent.getMaxX(), extent.getMaxY()));
            }
            else
            {
                output[0] = noDataVal;
            }
        }
    }
    
    RSGISInitOutputImageSoilleGratin94::~RSGISInitOutputImageSoilleGratin94()
    {
        
    }
    
    
    
}}


