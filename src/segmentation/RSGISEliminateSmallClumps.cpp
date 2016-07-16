/*
 *  RSGISEliminateSmallClumps.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/01/2012.
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

#include "RSGISEliminateSmallClumps.h"

namespace rsgis{namespace segment{
    
    RSGISEliminateSmallClumps::RSGISEliminateSmallClumps()
    {
        
    }
    
    void RSGISEliminateSmallClumps::eliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold) throw(rsgis::img::RSGISImageCalcException)
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
                
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        unsigned int *clumpIdxs = new unsigned int[width];
        float **spectralVals = new float*[numSpecBands];
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            spectralVals[n] = new float[width];
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        //unsigned long clumpIdx = 0;
        unsigned int uiPxlVal = 0;
        
        double maxVal = 0;
        clumpBand->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
        unsigned long maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal);
        /*
        for(unsigned int i = 0; i < height; ++i)
        {
            for(unsigned int j = 0; j < width; ++j)
            {
                clumpBand->RasterIO(GF_Read, j, i, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                if((i == 0) & (j == 0))
                {
                    maxClumpIdx = clumpIdx;
                }
                else if(clumpIdx > maxClumpIdx)
                {
                    maxClumpIdx = clumpIdx;
                }
            }
        }
         */
        std::vector<rsgis::img::ImgClump*> *clumpTable = new std::vector<rsgis::img::ImgClump*>();
        clumpTable->reserve(maxClumpIdx);
        std::deque<rsgis::img::ImgClump*> smallClumps;
        
        rsgis::img::ImgClump *cClump = NULL;
        rsgis::img::ImgClump *tClump = NULL;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new rsgis::img::ImgClump(i+1);
            cClump->sumVals = new float[numSpecBands];
            cClump->meanVals = new float[numSpecBands];
            cClump->pxls = new std::vector<rsgis::img::PxlLoc>();
            cClump->active = true;
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                cClump->sumVals[n] = 0;
                cClump->meanVals[n] = 0;
            }
            clumpTable->push_back(cClump);
        }
        
        std::cout << "Calculate Initial Clump Means\n";
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                spectralBands[n]->RasterIO(GF_Read, 0, i, width, 1, spectralVals[n], width, 1, GDT_Float32, 0, 0);
            }
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    cClump = clumpTable->at(clumpIdxs[j] - 1);
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        cClump->sumVals[n] += spectralVals[n][j];
                    }
                    cClump->pxls->push_back(rsgis::img::PxlLoc(j, i));
                }
            }
        }
        
        for(std::vector<rsgis::img::ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->pxls->size();
            }
            if((*iterClumps)->pxls->size() < minClumpSize)
            {
                smallClumps.push_back((*iterClumps));
            } 
        }
        
        std::cout << "There are " << clumpTable->size() << " clumps. " << smallClumps.size() << " are too small\n";
        
        rsgis::img::PxlLoc tLoc;
        std::list<unsigned long> neighbours;
        unsigned long closestNeighbour = 0;
        bool firstNeighbourTested = true;
        float closestNeighbourDist = 0;
        float distance = 0;
        
        std::cout << "Eliminating Small Clumps." << std::endl;
        long smallClumpsCounter = 0;
        while(smallClumps.size() > 0)
        {
            if((smallClumpsCounter % 10000) == 0)
            {
                std::cout << "Eliminated " << smallClumpsCounter << " > " << smallClumps.size() << " to go...\r";
            }
            
            cClump = smallClumps.front();
            smallClumps.pop_front();
            // Check that the clump was not selected for a merging already and therefore over minimum size...
            if((cClump->active) & (cClump->pxls->size() < minClumpSize))
            {
                // Get std::list of neighbours.
                neighbours.clear();
                for(std::vector<rsgis::img::PxlLoc>::iterator iterPxls = cClump->pxls->begin(); iterPxls != cClump->pxls->end(); ++iterPxls)
                {
                    // Above
                    if(((long)(*iterPxls).yPos)-1 >= 0)
                    {
                        clumpBand->RasterIO(GF_Read, (*iterPxls).xPos, (*iterPxls).yPos-1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                        if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                        {
                            neighbours.push_back(uiPxlVal);
                        }
                    }
                    // Below
                    if(((long)(*iterPxls).yPos)+1 < height)
                    {
                        clumpBand->RasterIO(GF_Read, (*iterPxls).xPos, (*iterPxls).yPos+1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                        if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                        {
                            neighbours.push_back(uiPxlVal);
                        }
                    }
                    // Left
                    if(((long)(*iterPxls).xPos-1) >= 0)
                    {
                        clumpBand->RasterIO(GF_Read, (*iterPxls).xPos-1, (*iterPxls).yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                        if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                        {
                            neighbours.push_back(uiPxlVal);
                        }
                    }
                    // Right
                    if(((long)(*iterPxls).xPos+1) < width)
                    {
                        clumpBand->RasterIO(GF_Read, (*iterPxls).xPos+1, (*iterPxls).yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                        if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                        {
                            neighbours.push_back(uiPxlVal);
                        }
                    }
                }
                neighbours.sort();
                neighbours.unique();
                
                // Decide on which neighbour to measure with.
                firstNeighbourTested = true;
                for(std::list<unsigned long>::iterator iterClumps = neighbours.begin(); iterClumps != neighbours.end(); ++iterClumps)
                {
                    for(unsigned int b = 0; b < numSpecBands; ++b)
                    {
                        if(b == 0)
                        {
                            distance = (cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b])*(cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b]);
                        }
                        else
                        {
                            distance += (cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b])*(cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b]);
                        }
                    }
                    distance = sqrt(distance);
                    if(firstNeighbourTested)
                    {
                        closestNeighbour = *iterClumps;
                        closestNeighbourDist = distance;
                        firstNeighbourTested = false;
                    }
                    else if(distance < closestNeighbourDist)
                    {
                        closestNeighbour = *iterClumps;
                        closestNeighbourDist = distance;
                    }
                }
                
                //std::cout << "For " << cClump->clumpID << "(size = " << cClump->pxls->size() << ") the closest neighbour is " << closestNeighbour << " with distance " << closestNeighbourDist << std::endl;
                
                // Perform Merge
                if(!firstNeighbourTested)
                {
                    if(closestNeighbourDist < specThreshold)
                    {
                        // Update Clump Table
                        tClump = clumpTable->at(closestNeighbour-1);
                        
                        for(size_t n = 0; n < cClump->pxls->size(); ++n)
                        {
                            tLoc = cClump->pxls->at(n);
                            tClump->pxls->push_back(tLoc);
                            // Update Pixel Values - in clump image.
                            clumpBand->RasterIO(GF_Write, tLoc.xPos, tLoc.yPos, 1, 1, &closestNeighbour, 1, 1, GDT_UInt32, 0, 0);
                        }
                        for(unsigned int b = 0; b < numSpecBands; ++b)
                        {
                            tClump->sumVals[b] += cClump->sumVals[b];
                            tClump->meanVals[b] = tClump->sumVals[b]/tClump->pxls->size();
                        }
                        
                        delete cClump->sumVals;
                        delete cClump->meanVals;
                        delete cClump->pxls;
                        cClump->active = false;

                        // Is the new clump still to small? 
                            // YES - add to queue...
                        if(tClump->pxls->size() < minClumpSize)
                        {
                            smallClumps.push_back(tClump);
                        }
                    }
                }
            }
            
            ++smallClumpsCounter;
        }
        std::cout << "Eliminated " << smallClumpsCounter << " small clumps\n";
        
        
        
        for(std::vector<rsgis::img::ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
        {
            if((*iterClumps) != NULL)
            {
                if((*iterClumps)->active)
                {
                    delete (*iterClumps)->sumVals;
                    delete (*iterClumps)->meanVals;
                    delete (*iterClumps)->pxls;
                }
                delete (*iterClumps);
            }
        }
        delete clumpTable;
        
        delete[] spectralBands;
        
        delete[] clumpIdxs;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete[] spectralVals[n];
        }
        delete[] spectralVals;
    }
    
    void RSGISEliminateSmallClumps::stepwiseEliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail) throw(rsgis::img::RSGISImageCalcException)
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        double *stretch2reflOffs = NULL;
        double *stretch2reflGains = NULL;
        if(bandStatsAvail && (numSpecBands != bandStretchStats->size()))
        {
            throw rsgis::img::RSGISImageCalcException("The number of image bands and the number band statistics are not the same.");
        }
        else if(bandStatsAvail)
        {
            stretch2reflOffs = new double[numSpecBands];
            stretch2reflGains = new double[numSpecBands];
            for(unsigned int i = 0; i < numSpecBands; ++i)
            {
                stretch2reflOffs[i] = bandStretchStats->at(i).origMin;
                stretch2reflGains[i] = (bandStretchStats->at(i).origMax - bandStretchStats->at(i).origMin) / (bandStretchStats->at(i).imgMax - bandStretchStats->at(i).imgMin);
            }
        }
        
        //unsigned int *clumpIdxs = new unsigned int[width];
        //float **spectralVals = new float*[numSpecBands];
        //GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        //for(unsigned int n = 0; n < numSpecBands; ++n)
        //{
        //    spectralBands[n] = spectral->GetRasterBand(n+1);
        //    spectralVals[n] = new float[width];
        //}
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned int uiPxlVal = 0;
        
        std::cout << "Calc Number of clumps\n";
        double maxVal = 0;
        clumpBand->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
        unsigned long maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal);
        /*for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if((i == 0) & (j == 0))
                {
                    maxClumpIdx = clumpIdxs[j];
                }
                else if(clumpIdxs[j] > maxClumpIdx)
                {
                    maxClumpIdx = clumpIdxs[j];
                }
            }
        }*/
        std::vector<rsgis::img::ImgClump*> *clumpTable = new std::vector<rsgis::img::ImgClump*>();
        clumpTable->reserve(maxClumpIdx);
        
        std::cout << "Build clump table\n";
        rsgis::img::ImgClump *cClump = NULL;
        rsgis::img::ImgClump *tClump = NULL;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new rsgis::img::ImgClump(i+1);
            cClump->sumVals = new float[numSpecBands];
            cClump->meanVals = new float[numSpecBands];
            cClump->pxls = new std::vector<rsgis::img::PxlLoc>();
            cClump->active = true;
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                cClump->sumVals[n] = 0;
                cClump->meanVals[n] = 0;
            }
            clumpTable->push_back(cClump);
        }
        
        std::deque<rsgis::img::ImgClump*> smallClumps;
        std::vector< std::pair<rsgis::img::ImgClump*, rsgis::img::ImgClump*> > mergeLookupTab;
        
        std::cout << "Calculate Initial Clump Means\n";
        /*
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                spectralBands[n]->RasterIO(GF_Read, 0, i, width, 1, spectralVals[n], width, 1, GDT_Float32, 0, 0);
            }
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    cClump = clumpTable->at(clumpIdxs[j] - 1);
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        cClump->sumVals[n] += spectralVals[n][j];
                    }
                    cClump->pxls->push_back(rsgis::img::PxlLoc(j, i));
                }
            }
        }
        */
        
        RSGISPopulateMeansPxlLocs *calcMeans = new RSGISPopulateMeansPxlLocs(clumpTable, numSpecBands);
        rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(calcMeans);
        
        GDALDataset **datasets = new  GDALDataset*[2];
        datasets[0] = clumps;
        datasets[1] = spectral;
        calcImg.calcImagePosPxl(datasets, 2);
        delete[] datasets;
        delete calcMeans;
        
        for(std::vector<rsgis::img::ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->pxls->size();
            }
        }
        std::cout << "There are " << clumpTable->size() << " clumps.\n";
        
        rsgis::img::PxlLoc tLoc;
        std::list<unsigned long> neighbours;
        unsigned long closestNeighbour = 0;
        bool firstNeighbourTested = true;
        float closestNeighbourDist = 0;
        float distance = 0;
        
        std::cout << "Eliminating Small Clumps." << std::endl;
        long smallClumpsCounter = 0;
        for(unsigned int clumpArea = 1; clumpArea <= minClumpSize; ++clumpArea)
        {
            std::cout << "Eliminating clumps of size " << clumpArea << std::endl;
            for(std::vector<rsgis::img::ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
            {
                if(((*iterClumps) != NULL) && ((*iterClumps)->active))
                {
                    if((*iterClumps)->pxls->size() <= clumpArea)
                    {
                        smallClumps.push_back(*iterClumps);
                    }
                }
            }
            
            std::cout << "Found " << smallClumps.size() << " small clumps to be eliminated." << std::endl;
            
            mergeLookupTab.clear();
            while(smallClumps.size() > 0)
            {
                cClump = smallClumps.front();
                smallClumps.pop_front();
                // Check that the clump was not selected for a merging already and therefore over minimum size...
                if((cClump->active) & (cClump->pxls->size() < minClumpSize))
                {
                    // Get std::list of neighbours.
                    neighbours.clear();
                    for(std::vector<rsgis::img::PxlLoc>::iterator iterPxls = cClump->pxls->begin(); iterPxls != cClump->pxls->end(); ++iterPxls)
                    {
                        // Above
                        if(((long)(*iterPxls).yPos)-1 >= 0)
                        {
                            clumpBand->RasterIO(GF_Read, (*iterPxls).xPos, (*iterPxls).yPos-1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                            {
                                neighbours.push_back(uiPxlVal);
                            }
                        }
                        // Below
                        if(((long)(*iterPxls).yPos)+1 < height)
                        {
                            clumpBand->RasterIO(GF_Read, (*iterPxls).xPos, (*iterPxls).yPos+1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                            {
                                neighbours.push_back(uiPxlVal);
                            }
                        }
                        // Left
                        if(((long)(*iterPxls).xPos-1) >= 0)
                        {
                            clumpBand->RasterIO(GF_Read, (*iterPxls).xPos-1, (*iterPxls).yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                            {
                                neighbours.push_back(uiPxlVal);
                            }
                        }
                        // Right
                        if(((long)(*iterPxls).xPos+1) < width)
                        {
                            clumpBand->RasterIO(GF_Read, (*iterPxls).xPos+1, (*iterPxls).yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                            {
                                neighbours.push_back(uiPxlVal);
                            }
                        }
                    }
                    neighbours.sort();
                    neighbours.unique();
                    
                    // Decide on which neighbour to measure with.
                    firstNeighbourTested = true;
                    for(std::list<unsigned long>::iterator iterClumps = neighbours.begin(); iterClumps != neighbours.end(); ++iterClumps)
                    {
                        if(clumpTable->at((*iterClumps)-1)->pxls->size() > cClump->pxls->size())
                        {
                            for(unsigned int b = 0; b < numSpecBands; ++b)
                            {
                                if(b == 0)
                                {
                                    distance = (cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b])*(cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b]);
                                }
                                else
                                {
                                    distance += (cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b])*(cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b]);
                                }
                            }
                            distance = sqrt(distance);
                            if(firstNeighbourTested)
                            {
                                closestNeighbour = *iterClumps;
                                closestNeighbourDist = distance;
                                firstNeighbourTested = false;
                            }
                            else if(distance < closestNeighbourDist)
                            {
                                closestNeighbour = *iterClumps;
                                closestNeighbourDist = distance;
                            }
                        }
                    }
                    
                    // Perform Merge
                    if(!firstNeighbourTested)
                    {
                        if(bandStatsAvail)
                        {
                            for(unsigned int b = 0; b < numSpecBands; ++b)
                            {
                                if(b == 0)
                                {
                                    distance = ((stretch2reflOffs[b]+(cClump->meanVals[b]*stretch2reflGains[b])) - (stretch2reflOffs[b]+(clumpTable->at(closestNeighbour-1)->meanVals[b]*stretch2reflGains[b])))*((stretch2reflOffs[b]+(cClump->meanVals[b]*stretch2reflGains[b])) - (stretch2reflOffs[b]+(clumpTable->at(closestNeighbour-1)->meanVals[b]*stretch2reflGains[b])));
                                }
                                else
                                {
                                    distance += ((stretch2reflOffs[b]+(cClump->meanVals[b]*stretch2reflGains[b])) - (stretch2reflOffs[b]+(clumpTable->at(closestNeighbour-1)->meanVals[b]*stretch2reflGains[b])))*((stretch2reflOffs[b]+(cClump->meanVals[b]*stretch2reflGains[b])) - (stretch2reflOffs[b]+(clumpTable->at(closestNeighbour-1)->meanVals[b]*stretch2reflGains[b])));
                                }
                            }
                            closestNeighbourDist = sqrt(distance);
                        }
                        
                        //std::cout << "For " << cClump->clumpID << "(size = " << cClump->pxls->size() << ") the closest neighbour is " << closestNeighbour << " with distance " << closestNeighbourDist << std::endl;
                        
                        if(closestNeighbourDist < specThreshold)
                        {
                            // PUT INTO LOOK UP TABLE TO BE APPLIED AFTERWARDS.
                            tClump = clumpTable->at(closestNeighbour-1);
                            std::pair<rsgis::img::ImgClump*, rsgis::img::ImgClump*> pair2Merge = std::pair<rsgis::img::ImgClump*, rsgis::img::ImgClump*>(cClump, tClump);
                            mergeLookupTab.push_back(pair2Merge);
                            ++smallClumpsCounter;
                        }
                    }
                }
            }
            
            // Update Clump Table
            std::pair<rsgis::img::ImgClump*, rsgis::img::ImgClump*> pair2Merge;
            for(size_t idx = 0; idx < mergeLookupTab.size(); ++idx)
            {
                pair2Merge = mergeLookupTab.at(idx);
                
                closestNeighbour = pair2Merge.second->clumpID;
                for(size_t n = 0; n < pair2Merge.first->pxls->size(); ++n)
                {
                    tLoc = pair2Merge.first->pxls->at(n);
                    pair2Merge.second->pxls->push_back(tLoc);
                     
                    // Update Pixel Values - in clump image.
                    clumpBand->RasterIO(GF_Write, tLoc.xPos, tLoc.yPos, 1, 1, &closestNeighbour, 1, 1, GDT_UInt32, 0, 0);
                }
                
                for(unsigned int b = 0; b < numSpecBands; ++b)
                {
                    pair2Merge.second->sumVals[b] += pair2Merge.first->sumVals[b];
                    pair2Merge.second->meanVals[b] = pair2Merge.second->sumVals[b]/pair2Merge.second->pxls->size();
                }

                delete[] pair2Merge.first->sumVals;
                delete[] pair2Merge.first->meanVals;
                delete pair2Merge.first->pxls;
                pair2Merge.first->active = false;
            }
            std::cout << "Eliminated " << smallClumpsCounter << " small clumps\n\n";
            smallClumps.clear();
            smallClumpsCounter = 0;
        }        
        
        
        for(std::vector<rsgis::img::ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
        {
            if((*iterClumps) != NULL)
            {
                if((*iterClumps)->active)
                {
                    delete[] (*iterClumps)->sumVals;
                    delete[] (*iterClumps)->meanVals;
                    delete (*iterClumps)->pxls;
                }
                delete (*iterClumps);
            }
        }
        delete clumpTable;
        
        if(bandStatsAvail)
        {
            delete[] stretch2reflOffs;
            delete[] stretch2reflGains;
        }
        
        //delete[] spectralBands;
        
        //delete[] clumpIdxs;
        //for(unsigned int n = 0; n < numSpecBands; ++n)
        //{
        //    delete[] spectralVals[n];
        //}
        //delete[] spectralVals;
    }
    
    void RSGISEliminateSmallClumps::stepwiseIterativeEliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail) throw(rsgis::img::RSGISImageCalcException)
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        double *stretch2reflOffs = NULL;
        double *stretch2reflGains = NULL;
        if(bandStatsAvail && (numSpecBands != bandStretchStats->size()))
        {
            throw rsgis::img::RSGISImageCalcException("The number of image bands and the number band statistics are not the same.");
        }
        else if(bandStatsAvail)
        {
            stretch2reflOffs = new double[numSpecBands];
            stretch2reflGains = new double[numSpecBands];
            for(unsigned int i = 0; i < numSpecBands; ++i)
            {
                stretch2reflOffs[i] = bandStretchStats->at(i).origMin;
                stretch2reflGains[i] = (bandStretchStats->at(i).origMax - bandStretchStats->at(i).origMin) / (bandStretchStats->at(i).imgMax - bandStretchStats->at(i).imgMin);
            }
        }
        
        /*unsigned int *clumpIdxs = new unsigned int[width];
        float **spectralVals = new float*[numSpecBands];
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            spectralVals[n] = new float[width];
        }
        */
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned int uiPxlVal = 0;
        
        std::cout << "Calc Number of clumps\n";
        double maxVal = 0;
        clumpBand->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
        unsigned long maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal);
        std::cout << "There are " << maxClumpIdx << " initial clumps." << std::endl;
        /*
        maxClumpIdx = 0;
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if((i == 0) & (j == 0))
                {
                    maxClumpIdx = clumpIdxs[j];
                }
                else if(clumpIdxs[j] > maxClumpIdx)
                {
                    maxClumpIdx = clumpIdxs[j];
                }
            }
        }
        std::cout << "maxClumpIdx = " << maxClumpIdx << std::endl;
        */
        std::vector<rsgis::img::ImgClump*> *clumpTable = new std::vector<rsgis::img::ImgClump*>();
        clumpTable->reserve(maxClumpIdx);
        
        std::cout << "Build clump table\n";
        rsgis::img::ImgClump *cClump = NULL;
        rsgis::img::ImgClump *tClump = NULL;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new rsgis::img::ImgClump(i+1);
            cClump->sumVals = new float[numSpecBands];
            cClump->meanVals = new float[numSpecBands];
            cClump->pxls = new std::vector<rsgis::img::PxlLoc>();
            cClump->active = true;
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                cClump->sumVals[n] = 0;
                cClump->meanVals[n] = 0;
            }
            clumpTable->push_back(cClump);
        }
        
        std::deque<rsgis::img::ImgClump*> smallClumps;
        std::vector< std::pair<rsgis::img::ImgClump*, rsgis::img::ImgClump*> > mergeLookupTab;
        
        std::cout << "Calculate Initial Clump Means\n";
        /*
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                spectralBands[n]->RasterIO(GF_Read, 0, i, width, 1, spectralVals[n], width, 1, GDT_Float32, 0, 0);
            }
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    cClump = clumpTable->at(clumpIdxs[j] - 1);
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        cClump->sumVals[n] += spectralVals[n][j];
                    }
                    cClump->pxls->push_back(rsgis::img::PxlLoc(j, i));
                }
            }
        }
        */
        
        RSGISPopulateMeansPxlLocs *calcMeans = new RSGISPopulateMeansPxlLocs(clumpTable, numSpecBands);
        rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(calcMeans);
        
        GDALDataset **datasets = new  GDALDataset*[2];
        datasets[0] = clumps;
        datasets[1] = spectral;
        calcImg.calcImagePosPxl(datasets, 2);
        delete[] datasets;
        delete calcMeans;
        
        for(std::vector<rsgis::img::ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->pxls->size();
            }
        }
        std::cout << "There are " << clumpTable->size() << " clumps.\n";
        
        rsgis::img::PxlLoc tLoc;
        std::list<unsigned long> neighbours;
        unsigned long closestNeighbour = 0;
        bool firstNeighbourTested = true;
        float closestNeighbourDist = 0;
        float distance = 0;
        
        std::cout << "Eliminating Small Clumps." << std::endl;
        unsigned long smallClumpsCounter = 0;
        unsigned long clumpsBelowThresCounter = 0;
        unsigned long initClumpsBelowThresCounter = 0;
        bool continueElim = true;
        for(unsigned int clumpArea = 1; clumpArea <= minClumpSize; ++clumpArea)
        {
            continueElim = true;
            while(continueElim)
            {
            
                std::cout << "Eliminating clumps of size " << clumpArea << std::endl;
                for(std::vector<rsgis::img::ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
                {
                    if(((*iterClumps) != NULL) && ((*iterClumps)->active))
                    {
                        if((*iterClumps)->pxls->size() <= clumpArea)
                        {
                            smallClumps.push_back(*iterClumps);
                        }
                    }
                }
                initClumpsBelowThresCounter = smallClumps.size();
                std::cout << "Found " << initClumpsBelowThresCounter << " small clumps to be eliminated." << std::endl;
                
                mergeLookupTab.clear();
                while(smallClumps.size() > 0)
                {
                    cClump = smallClumps.front();
                    smallClumps.pop_front();
                    // Check that the clump was not selected for a merging already and therefore over minimum size...
                    if((cClump->active) & (cClump->pxls->size() < minClumpSize))
                    {
                        // Get std::list of neighbours.
                        neighbours.clear();
                        for(std::vector<rsgis::img::PxlLoc>::iterator iterPxls = cClump->pxls->begin(); iterPxls != cClump->pxls->end(); ++iterPxls)
                        {
                            // Above
                            if(((long)(*iterPxls).yPos)-1 >= 0)
                            {
                                clumpBand->RasterIO(GF_Read, (*iterPxls).xPos, (*iterPxls).yPos-1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                                {
                                    neighbours.push_back(uiPxlVal);
                                }
                            }
                            // Below
                            if(((long)(*iterPxls).yPos)+1 < height)
                            {
                                clumpBand->RasterIO(GF_Read, (*iterPxls).xPos, (*iterPxls).yPos+1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                                {
                                    neighbours.push_back(uiPxlVal);
                                }
                            }
                            // Left
                            if(((long)(*iterPxls).xPos-1) >= 0)
                            {
                                clumpBand->RasterIO(GF_Read, (*iterPxls).xPos-1, (*iterPxls).yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                                {
                                    neighbours.push_back(uiPxlVal);
                                }
                            }
                            // Right
                            if(((long)(*iterPxls).xPos+1) < width)
                            {
                                clumpBand->RasterIO(GF_Read, (*iterPxls).xPos+1, (*iterPxls).yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if((uiPxlVal != cClump->clumpID) & (uiPxlVal != 0))
                                {
                                    neighbours.push_back(uiPxlVal);
                                }
                            }
                        }
                        neighbours.sort();
                        neighbours.unique();
                        
                        // Decide on which neighbour to measure with.
                        firstNeighbourTested = true;
                        //std::cout << "Clump " << cClump->clumpID << std::endl;
                        //std::cout << "Neighbours: ";
                        for(std::list<unsigned long>::iterator iterClumps = neighbours.begin(); iterClumps != neighbours.end(); ++iterClumps)
                        {
                            //std::cout << "\'" << *iterClumps << "\'";
                            if(clumpTable->at((*iterClumps)-1)->pxls->size() > cClump->pxls->size())
                            {
                                for(unsigned int b = 0; b < numSpecBands; ++b)
                                {
                                    if(b == 0)
                                    {
                                        distance = (cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b])*(cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b]);
                                    }
                                    else
                                    {
                                        distance += (cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b])*(cClump->meanVals[b] - clumpTable->at((*iterClumps)-1)->meanVals[b]);
                                    }
                                }
                                distance = sqrt(distance);
                                //std::cout << "(" << distance << ") ";
                                if(firstNeighbourTested)
                                {
                                    closestNeighbour = *iterClumps;
                                    closestNeighbourDist = distance;
                                    firstNeighbourTested = false;
                                }
                                else if(distance < closestNeighbourDist)
                                {
                                    closestNeighbour = *iterClumps;
                                    closestNeighbourDist = distance;
                                }
                            }
                        }
                        //std::cout << std::endl;
                        
                        // Perform Merge
                        if(!firstNeighbourTested)
                        {
                            if(bandStatsAvail)
                            {
                                for(unsigned int b = 0; b < numSpecBands; ++b)
                                {
                                    if(b == 0)
                                    {
                                        distance = ((stretch2reflOffs[b]+(cClump->meanVals[b]*stretch2reflGains[b])) - (stretch2reflOffs[b]+(clumpTable->at(closestNeighbour-1)->meanVals[b]*stretch2reflGains[b])))*((stretch2reflOffs[b]+(cClump->meanVals[b]*stretch2reflGains[b])) - (stretch2reflOffs[b]+(clumpTable->at(closestNeighbour-1)->meanVals[b]*stretch2reflGains[b])));
                                    }
                                    else
                                    {
                                        distance += ((stretch2reflOffs[b]+(cClump->meanVals[b]*stretch2reflGains[b])) - (stretch2reflOffs[b]+(clumpTable->at(closestNeighbour-1)->meanVals[b]*stretch2reflGains[b])))*((stretch2reflOffs[b]+(cClump->meanVals[b]*stretch2reflGains[b])) - (stretch2reflOffs[b]+(clumpTable->at(closestNeighbour-1)->meanVals[b]*stretch2reflGains[b])));
                                    }
                                }
                                closestNeighbourDist = sqrt(distance);
                            }
                            
                            //std::cout << "For " << cClump->clumpID << "(size = " << cClump->pxls->size() << ") the closest neighbour is " << closestNeighbour << " with distance " << closestNeighbourDist << std::endl;
                            
                            if(closestNeighbourDist < specThreshold)
                            {
                                // PUT INTO LOOK UP TABLE TO BE APPLIED AFTERWARDS.
                                tClump = clumpTable->at(closestNeighbour-1);
                                std::pair<rsgis::img::ImgClump*, rsgis::img::ImgClump*> pair2Merge = std::pair<rsgis::img::ImgClump*, rsgis::img::ImgClump*>(cClump, tClump);
                                mergeLookupTab.push_back(pair2Merge);
                                ++smallClumpsCounter;
                            }
                        }
                    }
                }
                
                // Update Clump Table
                std::pair<rsgis::img::ImgClump*, rsgis::img::ImgClump*> pair2Merge;
                for(size_t idx = 0; idx < mergeLookupTab.size(); ++idx)
                {
                    pair2Merge = mergeLookupTab.at(idx);
                    
                    closestNeighbour = pair2Merge.second->clumpID;
                    for(size_t n = 0; n < pair2Merge.first->pxls->size(); ++n)
                    {
                        tLoc = pair2Merge.first->pxls->at(n);
                        pair2Merge.second->pxls->push_back(tLoc);
                        
                        // Update Pixel Values - in clump image.
                        clumpBand->RasterIO(GF_Write, tLoc.xPos, tLoc.yPos, 1, 1, &closestNeighbour, 1, 1, GDT_UInt32, 0, 0);
                    }
                    
                    for(unsigned int b = 0; b < numSpecBands; ++b)
                    {
                        pair2Merge.second->sumVals[b] += pair2Merge.first->sumVals[b];
                        pair2Merge.second->meanVals[b] = pair2Merge.second->sumVals[b]/pair2Merge.second->pxls->size();
                    }
                    
                    delete[] pair2Merge.first->sumVals;
                    delete[] pair2Merge.first->meanVals;
                    delete pair2Merge.first->pxls;
                    pair2Merge.first->active = false;
                }
                std::cout << "Eliminated " << smallClumpsCounter << " small clumps\n";
                smallClumps.clear();
                smallClumpsCounter = 0;
                
                clumpsBelowThresCounter = 0;
                for(std::vector<rsgis::img::ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
                {
                    if(((*iterClumps) != NULL) && ((*iterClumps)->active))
                    {
                        if((*iterClumps)->pxls->size() <= clumpArea)
                        {
                            ++clumpsBelowThresCounter;
                        }
                    }
                }
                std::cout << "There are " << clumpsBelowThresCounter << " small clumps below " << clumpArea << " still to be eliminated\n";
                
                if((clumpsBelowThresCounter > 0) && (clumpsBelowThresCounter != initClumpsBelowThresCounter))
                {
                    continueElim = true;
                }
                else
                {
                    continueElim = false;
                }
            }
            
            std::cout << std::endl;
        }
        
        
        for(std::vector<rsgis::img::ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
        {
            if((*iterClumps) != NULL)
            {
                if((*iterClumps)->active)
                {
                    delete[] (*iterClumps)->sumVals;
                    delete[] (*iterClumps)->meanVals;
                    delete (*iterClumps)->pxls;
                }
                delete (*iterClumps);
            }
        }
        delete clumpTable;
        
        if(bandStatsAvail)
        {
            delete[] stretch2reflOffs;
            delete[] stretch2reflGains;
        }
        
        //delete[] spectralBands;
        
        //delete[] clumpIdxs;
        //for(unsigned int n = 0; n < numSpecBands; ++n)
        //{
        //    delete[] spectralVals[n];
        //}
        //delete[] spectralVals;
    }
    
    void RSGISEliminateSmallClumps::stepwiseEliminateSmallClumpsNoMean(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail) throw(rsgis::img::RSGISImageCalcException)
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        unsigned int *clumpIdxs = new unsigned int[width];
        float **spectralVals = new float*[numSpecBands];
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            spectralVals[n] = new float[width];
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned int uiPxlVal = 0;
        
        std::cout << "Calc Number of clumps\n";
        double maxVal = 0;
        clumpBand->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
        unsigned long maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal);
        /*
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if((i == 0) & (j == 0))
                {
                    maxClumpIdx = clumpIdxs[j];
                }
                else if(clumpIdxs[j] > maxClumpIdx)
                {
                    maxClumpIdx = clumpIdxs[j];
                }
            }
        */
        rsgis::img::ImgClumpSum **clumpTable = new rsgis::img::ImgClumpSum*[maxClumpIdx];
        
        std::cout << "Build clump table\n";
        //rsgis::img::ImgClumpSum *cClump = NULL;
        rsgis::img::ImgClumpSum *tClump = NULL;
        unsigned int cClumpIdx = 0;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            clumpTable[i] = new rsgis::img::ImgClumpSum(i+1);
            clumpTable[i]->sumVals = new float[numSpecBands];
            clumpTable[i]->pxls = new std::vector<rsgis::img::PxlLoc>();
            clumpTable[i]->active = true;
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                clumpTable[i]->sumVals[n] = 0;
            }
        }
        
        std::deque<unsigned int> smallClumps;
        std::vector< std::pair<unsigned int, unsigned int> > mergeLookupTab;
        
        std::cout << "Calculate Initial Clump Means\n";
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                spectralBands[n]->RasterIO(GF_Read, 0, i, width, 1, spectralVals[n], width, 1, GDT_Float32, 0, 0);
            }
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    cClumpIdx = clumpIdxs[j] - 1;
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        clumpTable[cClumpIdx]->sumVals[n] += spectralVals[n][j];
                    }
                    clumpTable[cClumpIdx]->pxls->push_back(rsgis::img::PxlLoc(j, i));
                }
            }
        }        

        std::cout << "There are " << maxClumpIdx << " clumps.\n";
        
        rsgis::img::PxlLoc tLoc;
        std::list<unsigned int> neighbours;
        unsigned int closestNeighbour = 0;
        bool firstNeighbourTested = true;
        float closestNeighbourDist = 0;
        float distance = 0;
        float cMean = 0;
        float tMean = 0;
        
        std::cout << "Eliminating Small Clumps." << std::endl;
        long smallClumpsCounter = 0;
        for(unsigned int clumpArea = 1; clumpArea <= minClumpSize; ++clumpArea)
        {
            std::cout << "Eliminating clumps of size " << clumpArea << std::endl;

            for(unsigned int i = 0; i < maxClumpIdx; ++i)
            {
                if((clumpTable[i] != NULL) && (clumpTable[i]->active))
                {
                    if(clumpTable[i]->pxls->size() <= clumpArea)
                    {
                        smallClumps.push_back(i);
                    }
                }
            }
            std::cout << "Found " << smallClumps.size() << " small clumps to be eliminated." << std::endl;
            
            mergeLookupTab.clear();
            while(smallClumps.size() > 0)
            {
                cClumpIdx = smallClumps.front();
                //cClump = clumpTable[cClumpIdx];
                smallClumps.pop_front();
                // Check that the clump was not selected for a merging already and therefore over minimum size...
                if((clumpTable[cClumpIdx]->active) & (clumpTable[cClumpIdx]->pxls->size() < minClumpSize))
                {
                    // Get std::list of neighbours.
                    neighbours.clear();
                    for(std::vector<rsgis::img::PxlLoc>::iterator iterPxls = clumpTable[cClumpIdx]->pxls->begin(); iterPxls != clumpTable[cClumpIdx]->pxls->end(); ++iterPxls)
                    {
                        // Above
                        if(((long)(*iterPxls).yPos)-1 >= 0)
                        {
                            clumpBand->RasterIO(GF_Read, (*iterPxls).xPos, (*iterPxls).yPos-1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if((uiPxlVal != clumpTable[cClumpIdx]->clumpID) & (uiPxlVal != 0))
                            {
                                neighbours.push_back(uiPxlVal);
                            }
                        }
                        // Below
                        if(((long)(*iterPxls).yPos)+1 < height)
                        {
                            clumpBand->RasterIO(GF_Read, (*iterPxls).xPos, (*iterPxls).yPos+1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if((uiPxlVal != clumpTable[cClumpIdx]->clumpID) & (uiPxlVal != 0))
                            {
                                neighbours.push_back(uiPxlVal);
                            }
                        }
                        // Left
                        if(((long)(*iterPxls).xPos-1) >= 0)
                        {
                            clumpBand->RasterIO(GF_Read, (*iterPxls).xPos-1, (*iterPxls).yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if((uiPxlVal != clumpTable[cClumpIdx]->clumpID) & (uiPxlVal != 0))
                            {
                                neighbours.push_back(uiPxlVal);
                            }
                        }
                        // Right
                        if(((long)(*iterPxls).xPos+1) < width)
                        {
                            clumpBand->RasterIO(GF_Read, (*iterPxls).xPos+1, (*iterPxls).yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if((uiPxlVal != clumpTable[cClumpIdx]->clumpID) & (uiPxlVal != 0))
                            {
                                neighbours.push_back(uiPxlVal);
                            }
                        }
                    }
                    neighbours.sort();
                    neighbours.unique();
                    
                    // Decide on which neighbour to measure with.
                    firstNeighbourTested = true;
                    for(std::list<unsigned int>::iterator iterClumps = neighbours.begin(); iterClumps != neighbours.end(); ++iterClumps)
                    {
                        if(clumpTable[(*iterClumps)-1]->pxls->size() > clumpTable[cClumpIdx]->pxls->size())
                        {
                            for(unsigned int b = 0; b < numSpecBands; ++b)
                            {
                                cMean = clumpTable[cClumpIdx]->sumVals[b] / clumpTable[cClumpIdx]->pxls->size();
                                tMean = clumpTable[(*iterClumps)-1]->sumVals[b] / clumpTable[(*iterClumps)-1]->pxls->size();
                                if(b == 0)
                                {
                                    distance = (cMean - tMean)*(cMean - tMean);
                                }
                                else
                                {
                                    distance += (cMean - tMean)*(cMean - tMean);
                                }
                            }
                            distance = sqrt(distance);
                            if(firstNeighbourTested)
                            {
                                closestNeighbour = *iterClumps;
                                closestNeighbourDist = distance;
                                firstNeighbourTested = false;
                            }
                            else if(distance < closestNeighbourDist)
                            {
                                closestNeighbour = *iterClumps;
                                closestNeighbourDist = distance;
                            }
                        }
                    }
                    
                    //std::cout << "For " << cClump->clumpID << "(size = " << cClump->pxls->size() << ") the closest neighbour is " << closestNeighbour << " with distance " << closestNeighbourDist << std::endl;
                    
                    // Perform Merge
                    if(!firstNeighbourTested)
                    {
                        if(closestNeighbourDist < specThreshold)
                        {
                            // PUT INTO LOOK UP TABLE TO BE APPLIED AFTERWARDS.
                            tClump = clumpTable[closestNeighbour-1];
                            std::pair<unsigned int, unsigned int> pair2Merge = std::pair<unsigned int, unsigned int>(cClumpIdx, tClump->clumpID-1);
                            mergeLookupTab.push_back(pair2Merge);
                        }
                    }
                }
                
                ++smallClumpsCounter;
            }
            
            // Update Clump Table
            std::pair<unsigned int, unsigned int> pair2Merge;
            for(size_t idx = 0; idx < mergeLookupTab.size(); ++idx)
            {
                pair2Merge = mergeLookupTab.at(idx);
                
                closestNeighbour = clumpTable[pair2Merge.second]->clumpID;
                for(size_t n = 0; n < clumpTable[pair2Merge.first]->pxls->size(); ++n)
                {
                    tLoc = clumpTable[pair2Merge.first]->pxls->at(n);
                    clumpTable[pair2Merge.second]->pxls->push_back(tLoc);
                    
                    // Update Pixel Values - in clump image.
                    clumpBand->RasterIO(GF_Write, tLoc.xPos, tLoc.yPos, 1, 1, &closestNeighbour, 1, 1, GDT_UInt32, 0, 0);
                }
                for(unsigned int b = 0; b < numSpecBands; ++b)
                {
                    clumpTable[pair2Merge.second]->sumVals[b] += clumpTable[pair2Merge.first]->sumVals[b];
                }
                
                delete[] clumpTable[pair2Merge.first]->sumVals;
                delete clumpTable[pair2Merge.first]->pxls;
                clumpTable[pair2Merge.first]->active = false;
                delete clumpTable[pair2Merge.first];
                clumpTable[pair2Merge.first] = NULL;
            }
            std::cout << "Eliminated " << smallClumpsCounter << " small clumps\n";
        }
        std::cout << "Finshed Elimination. " << smallClumpsCounter << " small clumps eliminated\n";
        
        
        
        //for(std::vector<rsgis::img::ImgClumpSum*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            if(clumpTable[i] != NULL)
            {
                if(clumpTable[i]->active)
                {
                    delete[] clumpTable[i]->sumVals;
                    delete clumpTable[i]->pxls;
                }
                delete clumpTable[i];
            }
        }
        delete[] clumpTable;
        
        delete[] spectralBands;
        
        delete[] clumpIdxs;
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            delete[] spectralVals[n];
        }
        delete[] spectralVals;
    }
   /*
    void RSGISEliminateSmallClumps::stepwiseEliminateSmallClumpsWithAtt(GDALDataset *spectral, GDALDataset *clumps, std::string outputImageFile, std::string imageFormat, bool useImageProj, std::string proj, rsgis::rastergis::RSGISAttributeTable *attTable, unsigned int minClumpSize, float specThreshold, bool outputWithConsecutiveFIDs, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            rsgis::utils::RSGISTextUtils textUtils;
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumps;
            datasets[1] = spectral;
            
            unsigned int numBands = spectral->GetRasterCount();
            double *stretch2reflOffs = NULL;
            double *stretch2reflGains = NULL;
            if(bandStatsAvail && (numBands != bandStretchStats->size()))
            {
                throw rsgis::img::RSGISImageCalcException("The number of image bands and the number band statistics are not the same.");
            }
            else if(bandStatsAvail)
            {
                stretch2reflOffs = new double[numBands];
                stretch2reflGains = new double[numBands];
                for(unsigned int i = 0; i < numBands; ++i)
                {
                    stretch2reflOffs[i] = bandStretchStats->at(i).origMin;
                    stretch2reflGains[i] = (bandStretchStats->at(i).origMax - bandStretchStats->at(i).origMin) / (bandStretchStats->at(i).imgMax - bandStretchStats->at(i).imgMin);
                }
            }
            
            
            std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats = new std::vector<rsgis::rastergis::RSGISBandAttStats*>();
            bandStats->reserve(numBands);
            rsgis::rastergis::RSGISBandAttStats *tmpBand = NULL;
            for(unsigned int i = 0; i < numBands; ++i)
            {
                tmpBand = new rsgis::rastergis::RSGISBandAttStats();
                tmpBand->band = i + 1;
                tmpBand->calcMean = false;//true;
                //tmpBand->meanField = std::string("b") + textUtils.uInt32bittostring(i) + std::string("_Mean");
                tmpBand->calcSum = true;
                tmpBand->sumField = std::string("b") + textUtils.uInt32bittostring(i) + std::string("_Sum");
                tmpBand->calcMax = false;
                tmpBand->calcMedian = false;
                tmpBand->calcMin = false;
                tmpBand->calcStdDev = false;
                bandStats->push_back(tmpBand);
            }
            
            std::cout << "Populate Image Stats:\n";
            rsgis::rastergis::RSGISPopulateAttributeTableBandWithSumAndMeans popAttStats;
            popAttStats.populateWithBandStatistics(attTable, datasets, 2, bandStats);
            delete[] datasets;
            
            std::cout << "Find neighbours:\n";
            rsgis::rastergis::RSGISFindClumpNeighbours findNeighbours;
            if(attTable->attInMemory())
            {
                findNeighbours.findNeighbours(clumps, attTable);
            }
            else
            {
                findNeighbours.findNeighboursInBlocks(clumps, attTable);
            }
                                    
            std::cout << "Create extra attribute tables columns\n";
            if(!attTable->hasAttribute("Eliminated"))
            {
                attTable->addAttBoolField("Eliminated", false);
            }
            else if(attTable->getDataType("Eliminated") != rsgis::rastergis::rsgis_bool)
            {
                throw rsgis::img::RSGISImageCalcException("Cannot proceed as \'Eliminated\' field is not of type boolean.");
            }
            else
            {
                attTable->setBoolValue("Eliminated", false);
            }
            unsigned int eliminatedFieldIdx = attTable->getFieldIndex("Eliminated");
            
            if(!attTable->hasAttribute("OutputFIDSet"))
            {
                attTable->addAttBoolField("OutputFIDSet", false);
            }
            else if(attTable->getDataType("OutputFIDSet") != rsgis::rastergis::rsgis_bool)
            {
                throw rsgis::img::RSGISImageCalcException("Cannot proceed as \'OutputFIDSet\' field is not of type boolean.");
            }
            else
            {
                attTable->setBoolValue("OutputFIDSet", false);
            }
            unsigned int outFIDSetFieldIdx = attTable->getFieldIndex("OutputFIDSet");
            
            
            if(!attTable->hasAttribute("OutputFID"))
            {
                attTable->addAttIntField("OutputFID", 0);
            }
            else if(attTable->getDataType("OutputFID") != rsgis::rastergis::rsgis_int)
            {
                throw rsgis::img::RSGISImageCalcException("Cannot proceed as \'OutputFID\' field is not of type integer.");
            }
            unsigned int outFIDIdx = attTable->getFieldIndex("OutputFID");
            
            if(!attTable->hasAttribute("MergedToFID"))
            {
                attTable->addAttIntField("MergedToFID", 0);
            }
            else if(attTable->getDataType("MergedToFID") != rsgis::rastergis::rsgis_int)
            {
                throw rsgis::img::RSGISImageCalcException("Cannot proceed as \'MergedToFID\' field is not of type integer.");
            }
            unsigned int mergedToFIDIdx = attTable->getFieldIndex("MergedToFID");
            
            unsigned int pxlCountIdx = attTable->getFieldIndex("pxlcount");
            
            rsgis::rastergis::RSGISAttExpressionLessThanConstEq *areaThreshold = new rsgis::rastergis::RSGISAttExpressionLessThanConstEq("pxlcount", pxlCountIdx, rsgis::rastergis::rsgis_int, 1);
            rsgis::rastergis::RSGISAttExpressionNotBoolField *boolEliminatedExp = new rsgis::rastergis::RSGISAttExpressionNotBoolField("Eliminated", eliminatedFieldIdx, rsgis::rastergis::rsgis_bool);
            
            std::vector<rsgis::rastergis::RSGISAttExpression*> *exps = new std::vector<rsgis::rastergis::RSGISAttExpression*>();
            exps->push_back(areaThreshold);
            exps->push_back(boolEliminatedExp);
            
            rsgis::rastergis::RSGISIfStatement *ifStat = new rsgis::rastergis::RSGISIfStatement();
            ifStat->exp = new rsgis::rastergis::RSGISAttExpressionAND(exps);
            ifStat->field = "";
            ifStat->dataType = rsgis::rastergis::rsgis_na;
            ifStat->fldIdx = 0;
            ifStat->value = 0;
            ifStat->noExp = false;
            ifStat->ignore = false;
            
            std::vector<std::pair<size_t, size_t> > *eliminationPairs = new std::vector<std::pair<size_t, size_t> >();
            
            rsgis::rastergis::RSGISProcessFeature *processFeature = new RSGISEliminateFeature(eliminatedFieldIdx, mergedToFIDIdx, specThreshold, pxlCountIdx, bandStats, eliminationPairs, bandStatsAvail, stretch2reflOffs, stretch2reflGains);
            
            if(attTable->attInMemory())
            {
                std::cout << "Eliminating features of size " << std::flush;
                for(unsigned int i = 1; i <= minClumpSize; ++i)
                {
                    std::cout << i << ", " << std::flush;
                    areaThreshold->setValue(i);
                    attTable->processIfStatements(ifStat, processFeature, NULL);
                    this->performElimination(attTable, eliminationPairs, eliminatedFieldIdx, mergedToFIDIdx, pxlCountIdx, bandStats);
                }
                std::cout << "Completed\n";
            }
            else
            {
                std::cout << "Eliminating features: " << std::endl;
                for(unsigned int i = 1; i <= minClumpSize; ++i)
                {
                    std::cout << "Process features with area " << i << std::endl;
                    areaThreshold->setValue(i);
                    attTable->processIfStatementsInBlocks(ifStat, processFeature, NULL);
                    std::cout << "Update table with elimination\n";
                    this->performElimination(attTable, eliminationPairs, eliminatedFieldIdx, mergedToFIDIdx, pxlCountIdx, bandStats);
                    std::cout << std::endl;
                }
                std::cout << "Completed\n";
            }
            
            delete ifStat->exp;
            delete ifStat;
            delete processFeature;
            
            // Relabel output FIDs to remove gaps.
            if(outputWithConsecutiveFIDs)
            {
                unsigned long fidIdxes = 0;
                for(attTable->start(); attTable->end(); ++(*attTable))
                {
                    if(!(*(*attTable))->boolFields->at(eliminatedFieldIdx))
                    {
                        (*(*attTable))->intFields->at(outFIDIdx) = fidIdxes++;
                        (*(*attTable))->boolFields->at(outFIDSetFieldIdx) = true;
                    }
                }
                std::cout << "There are " << fidIdxes << " clumps following elimination\n";
            }
            
            // Find output FIDs for relabelling.
            std::cout << "Defining output FIDs within table\n";
            for(attTable->start(); attTable->end(); ++(*attTable))
            {
                if(!(*(*attTable))->boolFields->at(outFIDSetFieldIdx))
                {
                    this->defineOutputFID(attTable, *(*attTable), eliminatedFieldIdx, mergedToFIDIdx, outFIDIdx, outFIDSetFieldIdx);
                }
            }
                        
            // Generate output image from original and output FIDs.
            std::cout << "Generating output clumps file\n";
            datasets = new GDALDataset*[1];
            datasets[0] = clumps;
            RSGISApplyOutputFIDs *applyOutFIDs = new RSGISApplyOutputFIDs(attTable, outFIDIdx, outFIDSetFieldIdx);
            rsgis::img::RSGISCalcImage calcImage(applyOutFIDs, proj, useImageProj);
            calcImage.calcImage(datasets, 1, outputImageFile, false, NULL, imageFormat, GDT_Float32);
            delete applyOutFIDs;
            delete[] datasets;
            
            if(bandStatsAvail)
            {
                delete[] stretch2reflOffs;
                delete[] stretch2reflGains;
            }
        }
        catch(RSGISAttributeTableException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    void RSGISEliminateSmallClumps::defineOutputFID(rsgis::rastergis::RSGISAttributeTable *attTable, rsgis::rastergis::RSGISFeature *feat, unsigned int eliminatedFieldIdx, unsigned int mergedToFIDIdx, unsigned int outFIDIdx, unsigned int outFIDSetFieldIdx) throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            if(!feat->boolFields->at(outFIDSetFieldIdx))
            {
                if(feat->boolFields->at(eliminatedFieldIdx))
                {
                    rsgis::rastergis::RSGISFeature *tmpFeat = attTable->getFeature(feat->intFields->at(mergedToFIDIdx));
                    if(!tmpFeat->boolFields->at(outFIDSetFieldIdx))
                    {
                        this->defineOutputFID(attTable, tmpFeat, eliminatedFieldIdx, mergedToFIDIdx, outFIDIdx, outFIDSetFieldIdx);
                    }
                    feat->intFields->at(outFIDIdx) = tmpFeat->intFields->at(outFIDIdx);
                    feat->boolFields->at(outFIDSetFieldIdx) = true;
                }
                else
                {
                    feat->intFields->at(outFIDIdx) = feat->fid;
                    feat->boolFields->at(outFIDSetFieldIdx) = true;
                }
            }            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISEliminateSmallClumps::performElimination(rsgis::rastergis::RSGISAttributeTable *attTable, std::vector<std::pair<size_t, size_t> > *eliminationPairs, unsigned int eliminatedFieldIdx, unsigned int mergedToFIDIdx, unsigned int pxlCountIdx, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats) throw(RSGISAttributeTableException)
    {
        try 
        {
            rsgis::rastergis::RSGISFeature* pFeat = NULL; // parent
            rsgis::rastergis::RSGISFeature* mFeat = NULL; // child to be merged
            bool alreadyNeighbour = false;
            for(std::vector<std::pair<size_t, size_t> >::iterator iterPairs = eliminationPairs->begin(); iterPairs != eliminationPairs->end(); ++iterPairs)
            {
                pFeat = attTable->getFeature((*iterPairs).first);
                attTable->holdFID((*iterPairs).first);
                mFeat = attTable->getFeature((*iterPairs).second);
                attTable->removeHoldFID((*iterPairs).first);
                
                if(pFeat->boolFields->at(eliminatedFieldIdx))
                {
                    pFeat = getEliminatedNeighbour(pFeat, attTable, eliminatedFieldIdx, mergedToFIDIdx);
                }
                
                pFeat->intFields->at(pxlCountIdx) += mFeat->intFields->at(pxlCountIdx);
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    pFeat->floatFields->at((*iterBands)->sumIdx) += mFeat->floatFields->at((*iterBands)->sumIdx);
                    //pFeat->floatFields->at((*iterBands)->meanIdx) = pFeat->floatFields->at((*iterBands)->sumIdx) / pFeat->intFields->at(pxlCountIdx);
                }
                
                for(std::vector<boost::uint_fast32_t>::iterator iterNeigh = pFeat->neighbours->begin(); iterNeigh != pFeat->neighbours->end(); )
                {
                    if((*iterNeigh) == mFeat->fid)
                    {
                        iterNeigh = pFeat->neighbours->erase(iterNeigh);
                    }
                    else
                    {
                        ++iterNeigh;
                    }
                }
                
                alreadyNeighbour = false;
                for(std::vector<boost::uint_fast32_t>::iterator iterFeatNeigh = mFeat->neighbours->begin(); iterFeatNeigh != mFeat->neighbours->end(); ++iterFeatNeigh)
                {
                    if((*iterFeatNeigh) != pFeat->fid)
                    {
                        alreadyNeighbour = false;
                        for(std::vector<boost::uint_fast32_t>::iterator iterNeigh = pFeat->neighbours->begin(); iterNeigh != pFeat->neighbours->end(); ++iterNeigh)
                        {
                            if((*iterFeatNeigh) == (*iterNeigh))
                            {
                                alreadyNeighbour = true;
                                break;
                            }
                        }
                        if(!alreadyNeighbour)
                        {
                            pFeat->neighbours->push_back(*iterFeatNeigh);
                        }
                    }
                }
                mFeat->neighbours->clear();
                
                mFeat->intFields->at(mergedToFIDIdx) = (*iterPairs).first;
                mFeat->boolFields->at(eliminatedFieldIdx) = true;
            }
            
            eliminationPairs->clear();
        }
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    rsgis::rastergis::RSGISFeature* RSGISEliminateSmallClumps::getEliminatedNeighbour(rsgis::rastergis::RSGISFeature *feat, rsgis::rastergis::RSGISAttributeTable *attTable, unsigned int eliminatedFieldIdx, unsigned int mergedToFIDIdx)throw(RSGISAttributeTableException)
    {
        rsgis::rastergis::RSGISFeature *nFeat = NULL;
        try 
        {
            if(!feat->boolFields->at(eliminatedFieldIdx))
            {
                nFeat = feat;
            }
            else
            {
                nFeat = attTable->getFeature(feat->intFields->at(mergedToFIDIdx));
                if(nFeat->boolFields->at(eliminatedFieldIdx))
                {
                    nFeat = this->getEliminatedNeighbour(nFeat, attTable, eliminatedFieldIdx, mergedToFIDIdx);
                }
            }
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        return nFeat;
    }
    */
    RSGISEliminateSmallClumps::~RSGISEliminateSmallClumps()
    {
        
    }
    
    
    
    
    
    
    
    /*
    
    RSGISEliminateFeature::RSGISEliminateFeature(unsigned int eliminatedFieldIdx, unsigned int mergedToFIDIdx, float specThreshold, unsigned int pxlCountIdx, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, std::vector<std::pair<unsigned long, unsigned long> > *eliminationPairs, bool bandStatsAvail, double *stretch2reflOffs, double *stretch2reflGains):RSGISProcessFeature()
    {
        this->eliminatedFieldIdx = eliminatedFieldIdx;
        this->mergedToFIDIdx = mergedToFIDIdx;
        this->specThreshold = specThreshold;
        this->pxlCountIdx = pxlCountIdx;
        this->bandStats = bandStats;
        this->eliminationPairs = eliminationPairs;
        this->bandStatsAvail = bandStatsAvail;
        this->stretch2reflOffs = stretch2reflOffs;
        this->stretch2reflGains = stretch2reflGains;
    }
    
    void RSGISEliminateFeature::processFeature(rsgis::rastergis::RSGISFeature *feat, rsgis::rastergis::RSGISAttributeTable *attTable)throw(RSGISAttributeTableException)
    {
        try 
        {
            unsigned long minFID = 0;
            double minDist = 0;
            double distance = 0;
            bool first = true;
            rsgis::rastergis::RSGISFeature *nFeat = NULL;
            for(std::vector<boost::uint_fast32_t>::iterator iterFeat = feat->neighbours->begin(); iterFeat != feat->neighbours->end(); ++iterFeat)
            {
                nFeat = attTable->getFeature(*iterFeat);
                attTable->holdFID(nFeat->fid);
                if(nFeat->boolFields->at(eliminatedFieldIdx))
                {
                    nFeat = getEliminatedNeighbour(nFeat, attTable);
                }
                attTable->removeHoldFID(nFeat->fid);
                if(nFeat->intFields->at(pxlCountIdx) > feat->intFields->at(pxlCountIdx))
                {
                    distance = this->calcDistance(feat, nFeat, bandStats);
                                        
                    if(first)
                    {
                        minDist = distance;
                        minFID = *iterFeat;
                        first = false;
                    }
                    else if(distance < minDist)
                    {
                        minDist = distance;
                        minFID = *iterFeat; 
                    }
                }
            }
            if(!first)
            {
                if(this->bandStatsAvail)
                {
                    //std::cout << minFID << " Min Dist (Before) = " << minDist << std::endl;
                    minDist = this->calcDistanceGainAndOff(feat, attTable->getFeature(minFID), bandStats);
                    //std::cout << minFID << " Min Dist (After) = " << minDist << std::endl;
                }
                
                if(minDist < specThreshold)
                {
                    eliminationPairs->push_back(std::pair<size_t, size_t>(minFID, ((unsigned long) feat->fid)));
                }
            }
        }
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    double RSGISEliminateFeature::calcDistance(rsgis::rastergis::RSGISFeature *feat1, rsgis::rastergis::RSGISFeature *feat2, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats)throw(RSGISAttributeTableException)
    {
        double dist = 0;
        double mean1 = 0;
        double mean2 = 0;
        try
        {
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                mean1 = feat1->floatFields->at((*iterBands)->sumIdx) / feat1->intFields->at(pxlCountIdx);
                mean2 = feat2->floatFields->at((*iterBands)->sumIdx) / feat2->intFields->at(pxlCountIdx);
                dist += pow((mean1 - mean2), 2);
                //dist += pow(feat1->floatFields->at((*iterBands)->meanIdx) - feat2->floatFields->at((*iterBands)->meanIdx), 2);
            }
            
            if(dist != 0)
            {
                dist = sqrt(dist);
            }
        }
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        return dist;
    }
    
    double RSGISEliminateFeature::calcDistanceGainAndOff(rsgis::rastergis::RSGISFeature *feat1, rsgis::rastergis::RSGISFeature *feat2, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats)throw(rsgis::RSGISAttributeTableException)
    {
        double dist = 0;
        double mean1 = 0;
        double mean2 = 0;
        try
        {
            unsigned int i = 0;
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                mean1 = feat1->floatFields->at((*iterBands)->sumIdx) / feat1->intFields->at(pxlCountIdx);
                mean2 = feat2->floatFields->at((*iterBands)->sumIdx) / feat2->intFields->at(pxlCountIdx);
                
                mean1 = this->stretch2reflOffs[i] + (mean1 * this->stretch2reflGains[i]);
                mean2 = this->stretch2reflOffs[i] + (mean2 * this->stretch2reflGains[i]);
                
                dist += pow((mean1 - mean2), 2);
                //dist += pow(feat1->floatFields->at((*iterBands)->meanIdx) - feat2->floatFields->at((*iterBands)->meanIdx), 2);
                ++i;
            }
            
            if(dist != 0)
            {
                dist = sqrt(dist);
            }
        }
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        return dist;
    }
    
    rsgis::rastergis::RSGISFeature* RSGISEliminateFeature::getEliminatedNeighbour(rsgis::rastergis::RSGISFeature *feat, rsgis::rastergis::RSGISAttributeTable *attTable)throw(RSGISAttributeTableException)
    {
        rsgis::rastergis::RSGISFeature *nFeat = NULL;
        try 
        {
            if(!feat->boolFields->at(eliminatedFieldIdx))
            {
                nFeat = feat;
            }
            else
            {
                nFeat = attTable->getFeature(feat->intFields->at(mergedToFIDIdx));
                attTable->holdFID(nFeat->fid);
                if(nFeat->boolFields->at(eliminatedFieldIdx))
                {
                    nFeat = this->getEliminatedNeighbour(nFeat, attTable);
                }
                attTable->removeHoldFID(nFeat->fid);
            }
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        return nFeat;
    }
    
    RSGISEliminateFeature::~RSGISEliminateFeature()
    {
        
    }
    
    
    
    

    RSGISApplyOutputFIDs::RSGISApplyOutputFIDs(rsgis::rastergis::RSGISAttributeTable *attTable, unsigned int outFIDIdx, unsigned int outFIDSetFieldIdx) : RSGISCalcImageValue(1)
    {
        this->attTable = attTable;
        this->outFIDIdx = outFIDIdx;
        this->outFIDSetFieldIdx = outFIDSetFieldIdx;
    }
    
    void RSGISApplyOutputFIDs::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = boost::lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {   
                rsgis::rastergis::RSGISFeature *feat = attTable->getFeature(clumpIdx);
                
                if(feat->boolFields->at(outFIDSetFieldIdx))
                {
                    output[0] = feat->intFields->at(outFIDIdx)+1;
                }
                else
                {
                    throw RSGISAttributeTableException("Features output FID has not been set.");
                }
            }
            catch(RSGISAttributeTableException &e)
            {
                std::cout << "clumpIdx = " << clumpIdx << std::endl;
                throw rsgis::img::RSGISImageCalcException(e.what());
            }
        }
        else
        {
            output[0] = 0;
        }
    }
        
    RSGISApplyOutputFIDs::~RSGISApplyOutputFIDs()
    {
        
    }
    */
    
    

    RSGISPopulateMeansPxlLocs::RSGISPopulateMeansPxlLocs(std::vector<rsgis::img::ImgClump*> *clumpTable, unsigned int numSpecBands):rsgis::img::RSGISCalcImageValue(0)
    {
        this->clumpTable = clumpTable;
        this->numSpecBands = numSpecBands;
    }
    
    void RSGISPopulateMeansPxlLocs::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {            
            if(bandValues[0] > 0)
            {
                size_t fid = boost::lexical_cast<size_t>(bandValues[0]);                
                
                rsgis::img::ImgClump *cClump = clumpTable->at(fid - 1);
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    cClump->sumVals[n] += bandValues[n+1];
                }
                
                unsigned int xPos = boost::lexical_cast<unsigned int>(extent.getMinX());
                unsigned int yPos = boost::lexical_cast<unsigned int>(extent.getMinY());
                
                cClump->pxls->push_back(rsgis::img::PxlLoc(xPos, yPos));
            }
        }
        catch(boost::numeric::negative_overflow& e)
        {
            //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(boost::numeric::positive_overflow& e)
        {
            //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(boost::numeric::bad_numeric_cast& e)
        {
            //std::cout << "bandValues[0] = " << bandValues[0] << std::endl;
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }

    RSGISPopulateMeansPxlLocs::~RSGISPopulateMeansPxlLocs()
    {
        
    }
    
    
    RSGISRemoveClumpsBelowThreshold::RSGISRemoveClumpsBelowThreshold(float threshold, int *clumpHisto, size_t numHistVals): rsgis::img::RSGISCalcImageValue(1)
    {
        this->threshold = threshold;
        this->clumpHisto = clumpHisto;
        this->numHistVals = numHistVals;
    }
    
    void RSGISRemoveClumpsBelowThreshold::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            if(intBandValues[0] > 0)
            {
                size_t fid = boost::lexical_cast<size_t>(intBandValues[0]);

                if(clumpHisto[fid] > this->threshold)
                {
                    output[0] = intBandValues[0];
                }
                else
                {
                    output[0] = 0.0;
                }
            }
            else
            {
                output[0] = 0.0;
            }
        }
        catch(boost::numeric::negative_overflow& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(boost::numeric::positive_overflow& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(boost::numeric::bad_numeric_cast& e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
       
    RSGISRemoveClumpsBelowThreshold::~RSGISRemoveClumpsBelowThreshold()
    {
        
    }
    
}}

