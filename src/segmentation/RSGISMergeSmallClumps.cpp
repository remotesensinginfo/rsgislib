/*
 *  RSGISMergeSmallClumps.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 30/01/2012.
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

#include "RSGISMergeSmallClumps.h"


namespace rsgis{namespace segment{
    
    RSGISMergeSmallClumps::RSGISMergeSmallClumps()
    {
        
    }
    
    void RSGISMergeSmallClumps::mergeSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize) throw(rsgis::img::RSGISImageCalcException)
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
        
        unsigned long clumpIdx = 0;
        unsigned int uiPxlVal = 0;
        
        unsigned long maxClumpIdx = 0;
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
                // Get list of neighbours.
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
                    if(firstNeighbourTested)
                    {
                        closestNeighbour = *iterClumps;
                        closestNeighbourDist = clumpTable->at((*iterClumps)-1)->pxls->size();
                        firstNeighbourTested = false;
                    }
                    else if(clumpTable->at((*iterClumps)-1)->pxls->size() < closestNeighbourDist)
                    {
                        closestNeighbour = *iterClumps;
                        closestNeighbourDist = clumpTable->at((*iterClumps)-1)->pxls->size();
                    }
                }
                
                // Perform Merge
                if(!firstNeighbourTested)
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
    
    RSGISMergeSmallClumps::~RSGISMergeSmallClumps()
    {
        
    }
    
}}



