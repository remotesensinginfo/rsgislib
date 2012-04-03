/*
 *  RSGISHierarchicalClumps.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/02/2012.
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

#include "RSGISHierarchicalClumps.h"

namespace rsgis{namespace rastergis{
    

    RSGISHierarchicalClumps::RSGISHierarchicalClumps()
    {
        
    }
    vector<RSGISSubClumps*>* RSGISHierarchicalClumps::findSubClumps(GDALDataset *clumps, GDALDataset *subClumps, bool noDataValProvided, unsigned int noDataVal) throw(RSGISImageException)
    {
        vector<RSGISSubClumps*> *clumpStats = new vector<RSGISSubClumps*>();
        
        try 
        {
            if(clumps->GetRasterXSize() != subClumps->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(clumps->GetRasterYSize() != subClumps->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            
            unsigned int width = clumps->GetRasterXSize();
            unsigned int height = clumps->GetRasterYSize();
            
            GDALRasterBand *clumpsBand = clumps->GetRasterBand(1);
            GDALRasterBand *subClumpBand = subClumps->GetRasterBand(1);
            
            unsigned int *clumpIdxs = new unsigned int[width];
            unsigned int *subClumpIdxs = new unsigned int[width];
            
            double *transform = new double[6];
            clumps->GetGeoTransform(transform);
            double tlX = transform[0];
            double tlY = transform[3];
            double pxlSize = transform[1];
            double pxlArea = pxlSize * pxlSize;
            delete[] transform;
            
            unsigned long maxClumpIdx = 0;
            for(unsigned int i = 0; i < height; ++i)
            {
                clumpsBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
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
            clumpStats->reserve(maxClumpIdx);
            
            bool *firstPxl = new bool[maxClumpIdx];
            RSGISSubClumps *tmpClump = NULL;
            for(unsigned long i = 0; i < maxClumpIdx; ++i)
            {
                tmpClump = new RSGISSubClumps();
                tmpClump->area = 0;
                tmpClump->tlX = 0;
                tmpClump->tlY = 0;
                tmpClump->brX = 0;
                tmpClump->brY = 0;
                tmpClump->minPxlX = 0;
                tmpClump->minPxlY = 0;
                tmpClump->maxPxlX = 0;
                tmpClump->maxPxlY = 0;
                clumpStats->push_back(tmpClump);
                firstPxl[i] = true;
            }
            
            unsigned long cIdx = 0;
            double tmpX = 0;
            double tmpY = 0;
            for(unsigned int i = 0; i < height; ++i)
            {
                clumpsBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
                subClumpBand->RasterIO(GF_Read, 0, i, width, 1, subClumpIdxs, width, 1, GDT_UInt32, 0, 0);
                
                for(unsigned int j = 0; j < width; ++j)
                {
                    if(!noDataValProvided | (clumpIdxs[j] != noDataVal))
                    {
                        cIdx = clumpIdxs[j] - 1;
                        tmpClump = clumpStats->at(cIdx);
                        
                        if(firstPxl[cIdx])
                        {
                            tmpClump->area = pxlArea;
                            tmpClump->tlX = tlX + (j*pxlSize);
                            tmpClump->tlY = tlY - (i*pxlSize);
                            tmpClump->brX = tlX + (j*pxlSize);
                            tmpClump->brY = tlY - (i*pxlSize);
                            tmpClump->minPxlX = j;
                            tmpClump->minPxlY = i;
                            tmpClump->maxPxlX = j;
                            tmpClump->maxPxlY = i;
                            firstPxl[cIdx] = false; 
                        }
                        else
                        {
                            tmpX = tlX + (j*pxlSize);
                            tmpY = tlY - (i*pxlSize);
                            
                            if(tmpX < tmpClump->tlX)
                            {
                                tmpClump->tlX = tmpX;
                                tmpClump->minPxlX = j;
                            }
                            else if(tmpX > tmpClump->brX)
                            {
                                tmpClump->brX = tmpX;
                                tmpClump->maxPxlX = j;
                                
                            }
                            
                            if(tmpY < tmpClump->brY)
                            {
                                tmpClump->brY = tmpY;
                                tmpClump->maxPxlY = i;
                            }
                            else if(tmpY > tmpClump->tlY)
                            {
                                tmpClump->tlY = tmpY;
                                tmpClump->minPxlY = i;
                            }
                            
                            tmpClump->area += pxlArea;
                        }
                        
                        if(!noDataValProvided | (subClumpIdxs[j] != noDataVal))
                        {
                            tmpClump->subclumps.push_back(subClumpIdxs[j]-1);
                        }
                    }
                }
            }
            
            for(unsigned long i = 0; i < maxClumpIdx; ++i)
            {
                clumpStats->at(i)->subclumps.sort();
                clumpStats->at(i)->subclumps.unique();
            }
        }
        catch (RSGISImageException &e) 
        {
            throw e;
        }
        
        return clumpStats;
    }
    
    RSGISHierarchicalClumps::~RSGISHierarchicalClumps()
    {
        
    }
    
}}





