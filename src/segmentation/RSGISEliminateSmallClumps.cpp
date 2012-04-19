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
    
    void RSGISEliminateSmallClumps::eliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold) throw(RSGISImageCalcException)
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw RSGISImageCalcException("Heights are not the same");
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
        vector<ImgClump*> *clumpTable = new vector<ImgClump*>();
        clumpTable->reserve(maxClumpIdx);
        deque<ImgClump*> smallClumps;
        
        ImgClump *cClump = NULL;
        ImgClump *tClump = NULL;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new ImgClump(i+1);
            cClump->sumVals = new float[numSpecBands];
            cClump->meanVals = new float[numSpecBands];
            cClump->pxls = new vector<PxlLoc>();
            cClump->active = true;
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                cClump->sumVals[n] = 0;
                cClump->meanVals[n] = 0;
            }
            clumpTable->push_back(cClump);
        }
        
        cout << "Calculate Initial Clump Means\n";
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
                    cClump->pxls->push_back(PxlLoc(j, i));
                }
            }
        }
        
        for(vector<ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
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
        
        cout << "There are " << clumpTable->size() << " clumps. " << smallClumps.size() << " are too small\n";
        
        PxlLoc tLoc;
        list<unsigned long> neighbours;
        unsigned long closestNeighbour = 0;
        bool firstNeighbourTested = true;
        float closestNeighbourDist = 0;
        float distance = 0;
        
        cout << "Eliminating Small Clumps." << endl;
        long smallClumpsCounter = 0;
        while(smallClumps.size() > 0)
        {
            if((smallClumpsCounter % 10000) == 0)
            {
                cout << "Eliminated " << smallClumpsCounter << " > " << smallClumps.size() << " to go...\r";
            }
            
            cClump = smallClumps.front();
            smallClumps.pop_front();
            // Check that the clump was not selected for a merging already and therefore over minimum size...
            if((cClump->active) & (cClump->pxls->size() < minClumpSize))
            {
                // Get list of neighbours.
                neighbours.clear();
                for(vector<PxlLoc>::iterator iterPxls = cClump->pxls->begin(); iterPxls != cClump->pxls->end(); ++iterPxls)
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
                for(list<unsigned long>::iterator iterClumps = neighbours.begin(); iterClumps != neighbours.end(); ++iterClumps)
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
                
                //cout << "For " << cClump->clumpID << "(size = " << cClump->pxls->size() << ") the closest neighbour is " << closestNeighbour << " with distance " << closestNeighbourDist << endl;
                
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
        cout << "Eliminated " << smallClumpsCounter << " small clumps\n";
        
        
        
        for(vector<ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
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
    
    void RSGISEliminateSmallClumps::stepwiseEliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold) throw(RSGISImageCalcException)
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw RSGISImageCalcException("Heights are not the same");
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
        
        cout << "Calc Number of clumps\n";
        unsigned long maxClumpIdx = 0;
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
        vector<ImgClump*> *clumpTable = new vector<ImgClump*>();
        clumpTable->reserve(maxClumpIdx);
        
        cout << "Build clump table\n";
        ImgClump *cClump = NULL;
        ImgClump *tClump = NULL;
        for(unsigned int i = 0; i < maxClumpIdx; ++i)
        {
            cClump = new ImgClump(i+1);
            cClump->sumVals = new float[numSpecBands];
            cClump->meanVals = new float[numSpecBands];
            cClump->pxls = new vector<PxlLoc>();
            cClump->active = true;
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                cClump->sumVals[n] = 0;
                cClump->meanVals[n] = 0;
            }
            clumpTable->push_back(cClump);
        }
        
        deque<ImgClump*> smallClumps;
        vector< pair<ImgClump*, ImgClump*> > mergeLookupTab;
        
        cout << "Calculate Initial Clump Means\n";
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
                    cClump->pxls->push_back(PxlLoc(j, i));
                }
            }
        }
        
        for(vector<ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                (*iterClumps)->meanVals[n] = (*iterClumps)->sumVals[n] / (*iterClumps)->pxls->size();
            }
        }
        cout << "There are " << clumpTable->size() << " clumps.\n";
        
        PxlLoc tLoc;
        list<unsigned long> neighbours;
        unsigned long closestNeighbour = 0;
        bool firstNeighbourTested = true;
        float closestNeighbourDist = 0;
        float distance = 0;
        
        cout << "Eliminating Small Clumps." << endl;
        long smallClumpsCounter = 0;
        for(unsigned int clumpArea = 1; clumpArea <= minClumpSize; ++clumpArea)
        {
            cout << "Eliminating clumps of size " << clumpArea << endl;
            for(vector<ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
            {
                if(((*iterClumps) != NULL) && ((*iterClumps)->active))
                {
                    if((*iterClumps)->pxls->size() <= clumpArea)
                    {
                        smallClumps.push_back(*iterClumps);
                    }
                }
            }
            
            cout << "Found " << smallClumps.size() << " small clumps to be eliminated." << endl;
            
            mergeLookupTab.clear();
            while(smallClumps.size() > 0)
            {
                cClump = smallClumps.front();
                smallClumps.pop_front();
                // Check that the clump was not selected for a merging already and therefore over minimum size...
                if((cClump->active) & (cClump->pxls->size() < minClumpSize))
                {
                    // Get list of neighbours.
                    neighbours.clear();
                    for(vector<PxlLoc>::iterator iterPxls = cClump->pxls->begin(); iterPxls != cClump->pxls->end(); ++iterPxls)
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
                    for(list<unsigned long>::iterator iterClumps = neighbours.begin(); iterClumps != neighbours.end(); ++iterClumps)
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
                    
                    //cout << "For " << cClump->clumpID << "(size = " << cClump->pxls->size() << ") the closest neighbour is " << closestNeighbour << " with distance " << closestNeighbourDist << endl;
                    
                    // Perform Merge
                    if(!firstNeighbourTested)
                    {
                        if(closestNeighbourDist < specThreshold)
                        {
                            // PUT INTO LOOK UP TABLE TO BE APPLIED AFTERWARDS.
                            tClump = clumpTable->at(closestNeighbour-1);
                            pair<ImgClump*, ImgClump*> pair2Merge = pair<ImgClump*, ImgClump*>(cClump, tClump);
                            mergeLookupTab.push_back(pair2Merge);
                        }
                    }
                        
                }
                
                ++smallClumpsCounter;
            }
            
            // Update Clump Table
            pair<ImgClump*, ImgClump*> pair2Merge;
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

                delete pair2Merge.first->sumVals;
                delete pair2Merge.first->meanVals;
                delete pair2Merge.first->pxls;
                pair2Merge.first->active = false;
            }
            cout << "Eliminated " << smallClumpsCounter << " small clumps\n";
        }
        cout << "Finshed Elimination. " << smallClumpsCounter << " small clumps eliminated\n";
        
        
        
        for(vector<ImgClump*>::iterator iterClumps = clumpTable->begin(); iterClumps != clumpTable->end(); ++iterClumps)
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
    
    void RSGISEliminateSmallClumps::stepwiseEliminateSmallClumpsWithAtt(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *output, RSGISAttributeTable *attTable, unsigned int minClumpSize, float specThreshold) throw(RSGISImageCalcException)
    {
        try
        {
            RSGISTextUtils textUtils;
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumps;
            datasets[1] = spectral;
            
            unsigned int numBands = spectral->GetRasterCount();
            vector<RSGISBandAttStats*> *bandStats = new vector<RSGISBandAttStats*>();
            bandStats->reserve(numBands);
            RSGISBandAttStats *tmpBand = NULL;
            for(unsigned int i = 0; i < numBands; ++i)
            {
                tmpBand = new RSGISBandAttStats();
                tmpBand->band = i + 1;
                tmpBand->calcMean = true;
                tmpBand->meanField = string("b") + textUtils.uInt32bittostring(i) + string("_Mean");
                tmpBand->calcSum = true;
                tmpBand->sumField = string("b") + textUtils.uInt32bittostring(i) + string("_Sum");
                tmpBand->calcMax = false;
                tmpBand->calcMedian = false;
                tmpBand->calcMin = false;
                tmpBand->calcStdDev = false;
                bandStats->push_back(tmpBand);
            }
            
            cout << "Populate Image Stats:\n";
            RSGISPopulateAttributeTableBandStats popAttStats;
            popAttStats.populateWithBandStatisticsWithinAtt(attTable, datasets, 2, bandStats);
            
            cout << "Create extra attribute tables columns\n";
            if(!attTable->hasAttribute("Eliminated"))
            {
                attTable->addAttBoolField("Eliminated", false);
            }
            else if(attTable->getDataType("Eliminated") != rsgis_bool)
            {
                throw RSGISImageCalcException("Cannot proceed as \'Eliminated\' field is not of type boolean.");
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
            else if(attTable->getDataType("OutputFIDSet") != rsgis_bool)
            {
                throw RSGISImageCalcException("Cannot proceed as \'OutputFIDSet\' field is not of type boolean.");
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
            else if(attTable->getDataType("OutputFID") != rsgis_int)
            {
                throw RSGISImageCalcException("Cannot proceed as \'OutputFID\' field is not of type integer.");
            }
            unsigned int outFIDIdx = attTable->getFieldIndex("OutputFID");
            
            if(!attTable->hasAttribute("MergedToFID"))
            {
                attTable->addAttIntField("MergedToFID", 0);
            }
            else if(attTable->getDataType("MergedToFID") != rsgis_int)
            {
                throw RSGISImageCalcException("Cannot proceed as \'MergedToFID\' field is not of type integer.");
            }
            unsigned int mergedToFIDIdx = attTable->getFieldIndex("MergedToFID");
            
            unsigned int pxlCountIdx = attTable->getFieldIndex("pxlcount");
            
            cout << "Find neighbours:\n";
            RSGISFindClumpNeighbours findNeighbours;
            findNeighbours.findNeighbours(clumps, attTable);
                        
            RSGISAttExpressionLessThanConstEq *areaThreshold = new RSGISAttExpressionLessThanConstEq("pxlcount", attTable->getFieldIndex("pxlcount"), rsgis_int, 1);
            RSGISAttExpressionBoolField *boolEliminatedExp = new RSGISAttExpressionBoolField("Eliminated", eliminatedFieldIdx, rsgis_bool);
            
            vector<RSGISAttExpression*> *exps = new vector<RSGISAttExpression*>();
            exps->push_back(areaThreshold);
            exps->push_back(boolEliminatedExp);
            
            RSGISIfStatement *ifStat = new RSGISIfStatement();
            ifStat->exp = new RSGISAttExpressionAND(exps);
            ifStat->field = "";
            ifStat->dataType = rsgis_na;
            ifStat->fldIdx = 0;
            ifStat->value = 0;
            ifStat->noExp = false;
            ifStat->ignore = false;
            
            RSGISProcessFeature *processFeature = new RSGISEliminateFeature(eliminatedFieldIdx, mergedToFIDIdx, specThreshold, pxlCountIdx, bandStats);
            
            cout << "Eliminating features of size " << flush;
            for(unsigned int i = 1; i <= minClumpSize; ++i)
            {
                cout << i << ", " << flush;
                areaThreshold->setValue(i);
                attTable->processIfStatements(ifStat, processFeature, NULL);
            }
            cout << "Completed\n";
            
            delete ifStat->exp;
            delete ifStat;
            delete processFeature;
            
            // Find output FIDs for relabelling. 
            
            // Relabel output FIDs to remove gaps.
            
            // Generate output image from original and output FIDs.
            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        catch(RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    RSGISEliminateSmallClumps::~RSGISEliminateSmallClumps()
    {
        
    }
    
    
    
    
    
    
    
    
    
    RSGISEliminateFeature::RSGISEliminateFeature(unsigned int eliminatedFieldIdx, unsigned int mergedToFIDIdx, float specThreshold, unsigned int pxlCountIdx, vector<RSGISBandAttStats*> *bandStats):RSGISProcessFeature()
    {
        this->eliminatedFieldIdx = eliminatedFieldIdx;
        this->mergedToFIDIdx = mergedToFIDIdx;
        this->specThreshold = specThreshold;
        this->pxlCountIdx = pxlCountIdx;
        this->bandStats = bandStats;
    }
    
    void RSGISEliminateFeature::processFeature(RSGISFeature *feat, RSGISAttributeTable *attTable)throw(RSGISAttributeTableException)
    {
        try 
        {
            unsigned long minFID = 0;
            double minDist = 0;
            double distance = 0;
            bool first = true;
            RSGISFeature *nFeat = NULL;
            for(vector<unsigned long long>::iterator iterFeat = feat->neighbours->begin(); iterFeat != feat->neighbours->end(); ++iterFeat)
            {
                nFeat = attTable->getFeature(*iterFeat);
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
                if(minDist < specThreshold)
                {
                    nFeat = attTable->getFeature(minFID);
                    nFeat->intFields->at(pxlCountIdx) += feat->intFields->at(pxlCountIdx);
                    for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {
                        nFeat->floatFields->at((*iterBands)->sumIdx) += feat->floatFields->at((*iterBands)->sumIdx);
                        nFeat->floatFields->at((*iterBands)->meanIdx) = nFeat->floatFields->at((*iterBands)->sumIdx) / nFeat->intFields->at(pxlCountIdx);
                    }
                    feat->intFields->at(mergedToFIDIdx) = minFID;
                    feat->boolFields->at(eliminatedFieldIdx) = true;
                }
            }
        }
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
    }
    
    double RSGISEliminateFeature::calcDistance(RSGISFeature *feat1, RSGISFeature *feat2, vector<RSGISBandAttStats*> *bandStats)throw(RSGISAttributeTableException)
    {
        double dist = 0;
        try
        {
            for(vector<RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                dist += pow(feat1->floatFields->at((*iterBands)->meanIdx) - feat2->floatFields->at((*iterBands)->meanIdx), 2);
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
    
    RSGISEliminateFeature::~RSGISEliminateFeature()
    {
        
    }
    
}}

