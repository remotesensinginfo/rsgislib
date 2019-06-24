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
    
    void RSGISEliminateSmallClumps::eliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold) 
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
        
        rsgis::rastergis::RSGISRasterAttUtils ratUtils;
        long minVal = 0;
        long maxVal = 0;
        ratUtils.getImageBandMinMax(clumps, 1, &minVal, &maxVal);
        
        unsigned long maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal);

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
    
    void RSGISEliminateSmallClumps::stepwiseEliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail) 
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
        
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned int uiPxlVal = 0;
        
        std::cout << "Calc Number of clumps\n";
        rsgis::rastergis::RSGISRasterAttUtils ratUtils;
        long minVal = 0;
        long maxVal = 0;
        ratUtils.getImageBandMinMax(clumps, 1, &minVal, &maxVal);
        unsigned long maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal);

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
    }
    
    void RSGISEliminateSmallClumps::stepwiseIterativeEliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail) 
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

        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned int uiPxlVal = 0;
        
        std::cout << "Calc Number of clumps\n";
        rsgis::rastergis::RSGISRasterAttUtils ratUtils;
        long minVal = 0;
        long maxVal = 0;
        ratUtils.getImageBandMinMax(clumps, 1, &minVal, &maxVal);
        unsigned long maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal);
        std::cout << "There are " << maxClumpIdx << " initial clumps." << std::endl;

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
    }
    
    void RSGISEliminateSmallClumps::stepwiseEliminateSmallClumpsNoMean(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail) 
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
        rsgis::rastergis::RSGISRasterAttUtils ratUtils;
        long minVal = 0;
        long maxVal = 0;
        ratUtils.getImageBandMinMax(clumps, 1, &minVal, &maxVal);
        unsigned long maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal);

        rsgis::img::ImgClumpSum **clumpTable = new rsgis::img::ImgClumpSum*[maxClumpIdx];
        
        std::cout << "Build clump table\n";
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
  
    RSGISEliminateSmallClumps::~RSGISEliminateSmallClumps()
    {
        
    }
    
    

    RSGISPopulateMeansPxlLocs::RSGISPopulateMeansPxlLocs(std::vector<rsgis::img::ImgClump*> *clumpTable, unsigned int numSpecBands):rsgis::img::RSGISCalcImageValue(0)
    {
        this->clumpTable = clumpTable;
        this->numSpecBands = numSpecBands;
    }
    
    void RSGISPopulateMeansPxlLocs::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) 
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
    
    void RSGISRemoveClumpsBelowThreshold::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
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

