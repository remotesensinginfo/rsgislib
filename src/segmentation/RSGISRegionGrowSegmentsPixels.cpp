/*
 *  RSGISRegionGrowSegmentsPixels.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/03/2012.
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

#include "RSGISRegionGrowSegmentsPixels.h"

namespace rsgis{namespace segment{
    

/*
    RSGISRegionGrowSegmentsPixels::RSGISRegionGrowSegmentsPixels(GDALDataset *inRefl, GDALDataset *outDataset)throw(RSGISImageException)
    {
        this->inRefl = inRefl;
        this->outDataset = outDataset;
        
        if(inRefl->GetRasterXSize() != outDataset->GetRasterXSize())
        {
            throw RSGISImageException("Widths are not the same");
        }
        if(inRefl->GetRasterYSize() != outDataset->GetRasterYSize())
        {
            throw RSGISImageException("Heights are not the same");
        }
        
        width = inRefl->GetRasterXSize();
        height = inRefl->GetRasterYSize();
        numSpecBands = inRefl->GetRasterCount();
        
        spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = inRefl->GetRasterBand(n+1);
        }
        outBand = outDataset->GetRasterBand(1);
        
        numPxls = 0;
        specPxlSumVals = new float[numSpecBands];
        specPxlMeanVals = new float[numSpecBands];
        specCPxlVals = new float[numSpecBands];
        
        mask = new bool*[height];
        for(unsigned int i = 0; i < height; ++i)
        {
            mask[i] = new bool[width];
        }
    }
    
    
    void RSGISRegionGrowSegmentsPixels::performRegionGrowing(std::vector<RSGISRegionGrowPxlSeeds> *seeds, float threshold)throw(rsgis::img::RSGISImageCalcException)
    {
        std::cout << "There are " << seeds->size() << " seeds to process\n";
        unsigned int seedCounter = 1;
        for(std::vector<RSGISRegionGrowPxlSeeds>::iterator iterSeed = seeds->begin(); iterSeed != seeds->end(); ++iterSeed)
        {
            std::cout << "Processing seed " << seedCounter++ << std::endl;
            try 
            {
                for(unsigned int i = 0; i < height; ++i)
                {
                    for(unsigned int j = 0; j < width; ++j)
                    {
                        mask[i][j] = false;
                    }
                }
                this->growSeed(&(*iterSeed), threshold);
            } 
            catch (rsgis::img::RSGISImageCalcException &e) 
            {
                throw e;
            }
        }
    }
    
    void RSGISRegionGrowSegmentsPixels::performRegionGrowing(std::vector<RSGISRegionGrowPxlSeeds> *seeds, float initThreshold, float thresholdIncrements, float maxThreshold, unsigned int maxIterations)throw(rsgis::img::RSGISImageCalcException)
    {
        std::cout << "There are " << seeds->size() << " seeds to process\n";
        unsigned int seedCounter = 1;
        for(std::vector<RSGISRegionGrowPxlSeeds>::iterator iterSeed = seeds->begin(); iterSeed != seeds->end(); ++iterSeed)
        {
            std::cout << "Processing seed " << seedCounter++ << std::endl;
            try 
            {
                this->growSeed(&(*iterSeed), initThreshold, thresholdIncrements, maxThreshold, maxIterations);
            } 
            catch (rsgis::img::RSGISImageCalcException &e) 
            {
                throw e;
            }
        }
    }
    
    
    void RSGISRegionGrowSegmentsPixels::growSeed(RSGISRegionGrowPxlSeeds *seed, float threshold)throw(rsgis::img::RSGISImageCalcException)
    {
        if(seed->xPxl >= width)
        {
            throw rsgis::img::RSGISImageCalcException("Seed is not within image: Width.");
        }
        
        if(seed->yPxl >= height)
        {
            throw rsgis::img::RSGISImageCalcException("Seed is not within image: Height.");
        }
        
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n]->RasterIO(GF_Read, seed->xPxl, seed->yPxl, 1, 1, &specPxlSumVals[n], 1, 1, GDT_Float32, 0, 0);
            specPxlMeanVals[n] = specPxlSumVals[n];
        }
        numPxls = 1;
        mask[seed->yPxl][seed->xPxl] = true;
        outBand->RasterIO(GF_Write, seed->xPxl, seed->yPxl, 1, 1, &seed->fid, 1, 1, GDT_UInt32, 0, 0);
        
        std::deque<rsgis::img::PxlLoc> searchPxls;
        searchPxls.push_back(rsgis::img::PxlLoc(seed->xPxl, seed->yPxl));
        
        double distance = 0;
        rsgis::img::PxlLoc pxl;
        while(searchPxls.size() > 0)
        {
            //std::cout << "searchPxls.size() = " << searchPxls.size() << std::endl;
            pxl = searchPxls.front();
            searchPxls.pop_front();
            
            // RIGHT
            if(pxl.xPos+1 < width)
            {
                //std::cout << "RIGHT\n";
                if(!mask[pxl.yPos][pxl.xPos+1])
                {
                    mask[pxl.yPos][pxl.xPos+1] = true;
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                    }
                    distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                    //std::cout << "\tDistance = " << distance << std::endl;
                    if(distance < threshold)
                    {
                        //std::cout << "\t\tWithin region\n";
                        outBand->RasterIO(GF_Write, pxl.xPos+1, pxl.yPos, 1, 1, &seed->fid, 1, 1, GDT_UInt32, 0, 0);
                        searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                    }
                }
            }
            //LEFT
            if(pxl.xPos > 0)
            {
                //std::cout << "LEFT\n";
                if(!mask[pxl.yPos][pxl.xPos-1])
                {
                    mask[pxl.yPos][pxl.xPos-1] = true;
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                    }
                    distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                    //std::cout << "\tDistance = " << distance << std::endl;
                    if(distance < threshold)
                    {
                        //std::cout << "\t\tWithin region\n";
                        outBand->RasterIO(GF_Write, pxl.xPos-1, pxl.yPos, 1, 1, &seed->fid, 1, 1, GDT_UInt32, 0, 0);
                        searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                    }
                }
            }
            // UP
            if(pxl.yPos+1 < height)
            {
                //std::cout << "UP\n";
                if(!mask[pxl.yPos+1][pxl.xPos])
                {
                    mask[pxl.yPos+1][pxl.xPos] = true;
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                    }
                    distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                    //std::cout << "\tDistance = " << distance << std::endl;
                    if(distance < threshold)
                    {
                        //std::cout << "\t\tWithin region\n";
                        outBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos+1, 1, 1, &seed->fid, 1, 1, GDT_UInt32, 0, 0);
                        searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                    }
                }
            }
            // DOWN
            if(pxl.yPos > 0)
            {
                //std::cout << "DOWN\n";
                if(!mask[pxl.yPos-1][pxl.xPos])
                {
                    mask[pxl.yPos-1][pxl.xPos] = true;
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                    }
                    distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                    //std::cout << "\tDistance = " << distance << std::endl;
                    if(distance < threshold)
                    {
                        //std::cout << "\t\tWithin region\n";
                        outBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos-1, 1, 1, &seed->fid, 1, 1, GDT_UInt32, 0, 0);
                        searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                    }
                }
            }            
        }
        
    }
    
    void RSGISRegionGrowSegmentsPixels::growSeed(RSGISRegionGrowPxlSeeds *seed, float initThreshold, float thresholdIncrements, float maxThreshold, unsigned int maxNumPixels)throw(rsgis::img::RSGISImageCalcException)
    {
        if(seed->xPxl >= width)
        {
            throw rsgis::img::RSGISImageCalcException("Seed is not within image: Width.");
        }
        
        if(seed->yPxl >= height)
        {
            throw rsgis::img::RSGISImageCalcException("Seed is not within image: Height.");
        }
        
        std::deque<rsgis::img::PxlLoc> searchPxls;
        
        double distance = 0;
        rsgis::img::PxlLoc pxl;
        
        unsigned int numThresholdSteps = ceil((maxThreshold - initThreshold)/thresholdIncrements);
        
        unsigned int prevNumPxlsInClump = 0;
        unsigned int numPxlsInClump = 0;
        
        float threshold = initThreshold;
        bool thresholdFound = false;
        float sizeChangeRatio = 0;
        
        for(unsigned int m = 0 ; m < numThresholdSteps; ++m)
        {
            threshold = initThreshold + (m * thresholdIncrements);
            
            for(unsigned int i = 0; i < height; ++i)
            {
                for(unsigned int j = 0; j < width; ++j)
                {
                    mask[i][j] = false;
                }
            }
        
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                spectralBands[n]->RasterIO(GF_Read, seed->xPxl, seed->yPxl, 1, 1, &specPxlSumVals[n], 1, 1, GDT_Float32, 0, 0);
                specPxlMeanVals[n] = specPxlSumVals[n];
            }
            
            searchPxls.clear();
            mask[seed->yPxl][seed->xPxl] = true;
            searchPxls.push_back(rsgis::img::PxlLoc(seed->xPxl, seed->yPxl));
            numPxlsInClump = 1;

            
            while(searchPxls.size() > 0)
            {
                //std::cout << "searchPxls.size() = " << searchPxls.size() << std::endl;
                pxl = searchPxls.front();
                searchPxls.pop_front();
                
                // RIGHT
                if(pxl.xPos+1 < width)
                {
                    //std::cout << "RIGHT\n";
                    if(!mask[pxl.yPos][pxl.xPos+1])
                    {
                        mask[pxl.yPos][pxl.xPos+1] = true;
                        for(unsigned int n = 0; n < numSpecBands; ++n)
                        {
                            spectralBands[n]->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        }
                        distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                        //std::cout << "\tDistance = " << distance << std::endl;
                        if(distance < threshold)
                        {
                            //std::cout << "\t\tWithin region\n";
                            searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                            ++numPxlsInClump;
                            for(unsigned int n = 0; n < numSpecBands; ++n)
                            {
                                specPxlSumVals[n] += specCPxlVals[n];
                                specPxlMeanVals[n] = specPxlSumVals[n]/numPxlsInClump;
                            }
                        }
                    }
                }
                //LEFT
                if(pxl.xPos > 0)
                {
                    //std::cout << "LEFT\n";
                    if(!mask[pxl.yPos][pxl.xPos-1])
                    {
                        mask[pxl.yPos][pxl.xPos-1] = true;
                        for(unsigned int n = 0; n < numSpecBands; ++n)
                        {
                            spectralBands[n]->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        }
                        distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                        //std::cout << "\tDistance = " << distance << std::endl;
                        if(distance < threshold)
                        {
                            //std::cout << "\t\tWithin region\n";
                            searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                            ++numPxlsInClump;
                            for(unsigned int n = 0; n < numSpecBands; ++n)
                            {
                                specPxlSumVals[n] += specCPxlVals[n];
                                specPxlMeanVals[n] = specPxlSumVals[n]/numPxlsInClump;
                            }
                        }
                    }
                }
                // UP
                if(pxl.yPos+1 < height)
                {
                    //std::cout << "UP\n";
                    if(!mask[pxl.yPos+1][pxl.xPos])
                    {
                        mask[pxl.yPos+1][pxl.xPos] = true;
                        for(unsigned int n = 0; n < numSpecBands; ++n)
                        {
                            spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        }
                        distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                        //std::cout << "\tDistance = " << distance << std::endl;
                        if(distance < threshold)
                        {
                            //std::cout << "\t\tWithin region\n";
                            searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                            ++numPxlsInClump;
                            for(unsigned int n = 0; n < numSpecBands; ++n)
                            {
                                specPxlSumVals[n] += specCPxlVals[n];
                                specPxlMeanVals[n] = specPxlSumVals[n]/numPxlsInClump;
                            }
                        }
                    }
                }
                // DOWN
                if(pxl.yPos > 0)
                {
                    //std::cout << "DOWN\n";
                    if(!mask[pxl.yPos-1][pxl.xPos])
                    {
                        mask[pxl.yPos-1][pxl.xPos] = true;
                        for(unsigned int n = 0; n < numSpecBands; ++n)
                        {
                            spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        }
                        distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                        //std::cout << "\tDistance = " << distance << std::endl;
                        if(distance < threshold)
                        {
                            //std::cout << "\t\tWithin region\n";
                            searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                            ++numPxlsInClump;
                            for(unsigned int n = 0; n < numSpecBands; ++n)
                            {
                                specPxlSumVals[n] += specCPxlVals[n];
                                specPxlMeanVals[n] = specPxlSumVals[n]/numPxlsInClump;
                            }
                        }
                    }
                }
                
                if(numPxlsInClump > maxNumPixels)
                {
                    thresholdFound = true;
                    break;
                }
            }
            
            //std::cout << "Iteration " << m << " has threshold " << threshold << " resulting in clump of size " << numPxlsInClump << std::endl;
            
            if(m == 0)
            {
                prevNumPxlsInClump = numPxlsInClump;
            }
            else
            {
                sizeChangeRatio = ((float)(numPxlsInClump - prevNumPxlsInClump))/numPxlsInClump;
                //std::cout << "Ratio Size increase = " << sizeChangeRatio << std::endl;
                if(sizeChangeRatio > 0.85)
                {
                    thresholdFound = true;
                    break;
                }
                prevNumPxlsInClump = numPxlsInClump;
            }
            
            if(thresholdFound)
            {
                break;
            }
            
        }
        
        if(thresholdFound)
        {
            threshold = threshold - thresholdIncrements;
            
            for(unsigned int i = 0; i < height; ++i)
            {
                for(unsigned int j = 0; j < width; ++j)
                {
                    mask[i][j] = false;
                }
            }
            
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                spectralBands[n]->RasterIO(GF_Read, seed->xPxl, seed->yPxl, 1, 1, &specPxlSumVals[n], 1, 1, GDT_Float32, 0, 0);
                specPxlMeanVals[n] = specPxlSumVals[n];
            }
            
            searchPxls.clear();
            mask[seed->yPxl][seed->xPxl] = true;
            searchPxls.push_back(rsgis::img::PxlLoc(seed->xPxl, seed->yPxl));
            numPxlsInClump = 1;
            
            while(searchPxls.size() > 0)
            {
                //std::cout << "searchPxls.size() = " << searchPxls.size() << std::endl;
                pxl = searchPxls.front();
                searchPxls.pop_front();
                
                // RIGHT
                if(pxl.xPos+1 < width)
                {
                    //std::cout << "RIGHT\n";
                    if(!mask[pxl.yPos][pxl.xPos+1])
                    {
                        mask[pxl.yPos][pxl.xPos+1] = true;
                        for(unsigned int n = 0; n < numSpecBands; ++n)
                        {
                            spectralBands[n]->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        }
                        distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                        //std::cout << "\tDistance = " << distance << std::endl;
                        if(distance < threshold)
                        {
                            //std::cout << "\t\tWithin region\n";
                            outBand->RasterIO(GF_Write, pxl.xPos+1, pxl.yPos, 1, 1, &seed->fid, 1, 1, GDT_UInt32, 0, 0);
                            searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                            ++numPxlsInClump;
                            for(unsigned int n = 0; n < numSpecBands; ++n)
                            {
                                specPxlSumVals[n] += specCPxlVals[n];
                                specPxlMeanVals[n] = specPxlSumVals[n]/numPxlsInClump;
                            }
                        }
                    }
                }
                //LEFT
                if(pxl.xPos > 0)
                {
                    //std::cout << "LEFT\n";
                    if(!mask[pxl.yPos][pxl.xPos-1])
                    {
                        mask[pxl.yPos][pxl.xPos-1] = true;
                        for(unsigned int n = 0; n < numSpecBands; ++n)
                        {
                            spectralBands[n]->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        }
                        distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                        //std::cout << "\tDistance = " << distance << std::endl;
                        if(distance < threshold)
                        {
                            //std::cout << "\t\tWithin region\n";
                            outBand->RasterIO(GF_Write, pxl.xPos-1, pxl.yPos, 1, 1, &seed->fid, 1, 1, GDT_UInt32, 0, 0);
                            searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                            ++numPxlsInClump;
                            for(unsigned int n = 0; n < numSpecBands; ++n)
                            {
                                specPxlSumVals[n] += specCPxlVals[n];
                                specPxlMeanVals[n] = specPxlSumVals[n]/numPxlsInClump;
                            }
                        }
                    }
                }
                // UP
                if(pxl.yPos+1 < height)
                {
                    //std::cout << "UP\n";
                    if(!mask[pxl.yPos+1][pxl.xPos])
                    {
                        mask[pxl.yPos+1][pxl.xPos] = true;
                        for(unsigned int n = 0; n < numSpecBands; ++n)
                        {
                            spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        }
                        distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                        //std::cout << "\tDistance = " << distance << std::endl;
                        if(distance < threshold)
                        {
                            //std::cout << "\t\tWithin region\n";
                            outBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos+1, 1, 1, &seed->fid, 1, 1, GDT_UInt32, 0, 0);
                            searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                            ++numPxlsInClump;
                            for(unsigned int n = 0; n < numSpecBands; ++n)
                            {
                                specPxlSumVals[n] += specCPxlVals[n];
                                specPxlMeanVals[n] = specPxlSumVals[n]/numPxlsInClump;
                            }
                        }
                    }
                }
                // DOWN
                if(pxl.yPos > 0)
                {
                    //std::cout << "DOWN\n";
                    if(!mask[pxl.yPos-1][pxl.xPos])
                    {
                        mask[pxl.yPos-1][pxl.xPos] = true;
                        for(unsigned int n = 0; n < numSpecBands; ++n)
                        {
                            spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        }
                        distance = this->eucDistance(specPxlMeanVals, specCPxlVals, numSpecBands);
                        //std::cout << "\tDistance = " << distance << std::endl;
                        if(distance < threshold)
                        {
                            //std::cout << "\t\tWithin region\n";
                            outBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos-1, 1, 1, &seed->fid, 1, 1, GDT_UInt32, 0, 0);
                            searchPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                            ++numPxlsInClump;
                            for(unsigned int n = 0; n < numSpecBands; ++n)
                            {
                                specPxlSumVals[n] += specCPxlVals[n];
                                specPxlMeanVals[n] = specPxlSumVals[n]/numPxlsInClump;
                            }
                        }
                    }
                }            
            }
        }
        
        
    }
    
    std::vector<RSGISRegionGrowPxlSeeds>* RSGISRegionGrowSegmentsPixels::parseSeedsText(std::string inFile) throw(rsgis::utils::RSGISTextException)
    {
        std::vector<RSGISRegionGrowPxlSeeds> *seeds = new std::vector<RSGISRegionGrowPxlSeeds>();
        
        try 
        {
            rsgis::utils::RSGISTextUtils txtUtils;
            rsgis::utils::RSGISTextFileLineReader txtReader;
            txtReader.openFile(inFile);
            
            std::vector<std::string> *tokens = new std::vector<std::string>();
            std::string line = "";
            unsigned long fid = 0;
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            while(!txtReader.endOfFile())
            {
                line = txtReader.readLine();

                if(!txtUtils.blankline(line) & !txtUtils.lineStart(line, '#'))
                {
                    txtUtils.tokenizeString(line, ',', tokens, true);
                    
                    if(tokens->size() == 3)
                    {
                        fid = txtUtils.strto64bitUInt(tokens->at(0));
                        xPxl = txtUtils.strto32bitUInt(tokens->at(1));
                        yPxl = txtUtils.strto32bitUInt(tokens->at(2));
                        
                        seeds->push_back(RSGISRegionGrowPxlSeeds(fid, xPxl, yPxl));
                    }
                    else
                    {
                        throw rsgis::utils::RSGISTextException("Seeds file requires format FID,xPixel,yPixel.");
                    }
                    tokens->clear();
                }
                
            }
            
            txtReader.closeFile();
            
        } 
        catch (rsgis::utils::RSGISTextException &e) 
        {
            throw e;
        }
        
        return seeds;
    }
        
    RSGISRegionGrowSegmentsPixels::~RSGISRegionGrowSegmentsPixels()
    {
        if(spectralBands != NULL)
        {
            delete[] spectralBands;
        }
        
        if(specPxlSumVals != NULL)
        {
            delete[] specPxlSumVals;
        }
        
        if(specPxlMeanVals != NULL)
        {
            delete[] specPxlMeanVals;
        }
        
        if(specCPxlVals != NULL)
        {
            delete[] specCPxlVals;
        }
        
        for(unsigned int i = 0; i < height; ++i)
        {
            delete[] mask[i];
        }
        delete[] mask;
        
    }
    
    
    
    
    RSGISFindRegionGrowingSeeds::RSGISFindRegionGrowingSeeds()
    {
        
    }
    
    std::vector<RSGISRegionGrowPxlSeeds>* RSGISFindRegionGrowingSeeds::findSeeds(GDALDataset *inRefl, GDALDataset *clumps,  std::vector<rsgis::rastergis::RSGISSubClumps*> *regions)throw(rsgis::img::RSGISImageCalcException)
    {
        std::vector<RSGISRegionGrowPxlSeeds> *seeds = new std::vector<RSGISRegionGrowPxlSeeds>();
        
        if(inRefl->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(inRefl->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        unsigned int width = inRefl->GetRasterXSize();
        unsigned int height = inRefl->GetRasterYSize();
        unsigned int numSpecBands = inRefl->GetRasterCount();
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        float **spectralValues = new float*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = inRefl->GetRasterBand(n+1);
        }
        GDALRasterBand *clumpsBand = clumps->GetRasterBand(1);
        unsigned int *clumpIdxs = NULL;
        unsigned int subClumpWidth = 0;
        unsigned int subClumpHeight = 0;
        float brightnessVal = 0;
        float brightnessSeedVal = 0;
        std::list<float> brightnessVals;
        unsigned int seedIdx = 0;
        unsigned int counter = 0;
        
        RSGISRegionGrowPxlSeeds cSeed;
        float brightDist = 0;
        float closeBrightDist = 0;
        bool first = true;
        unsigned int seedCount = 0;
        bool foundSeed = false;
        
        try
        {
            rsgis::rastergis::RSGISSubClumps *subClump = NULL;
            
            for(std::vector<rsgis::rastergis::RSGISSubClumps*>::iterator iterSubClumps = regions->begin(); iterSubClumps != regions->end(); ++iterSubClumps)
            {
                subClump = (*iterSubClumps);
                foundSeed = false;
                
                if(subClump->maxPxlY >= height)
                {
                    throw rsgis::img::RSGISImageCalcException("Subclump is not within dataset: Height");
                }
                
                if(subClump->maxPxlX >= width)
                {
                    throw rsgis::img::RSGISImageCalcException("Subclump is not within dataset: Width");
                }
                
                cSeed.xPxl = 0;
                cSeed.yPxl = 0;
                
                subClumpWidth = subClump->maxPxlX - subClump->minPxlX;
                subClumpHeight = subClump->maxPxlY - subClump->minPxlY;

                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    spectralValues[n] = new float[subClumpWidth];
                }
                clumpIdxs = new unsigned int[subClumpWidth];
                
                brightnessVals.clear();
                for(unsigned int y = subClump->minPxlY; y < subClump->maxPxlY; ++y)
                {
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        spectralBands[n]->RasterIO(GF_Read, subClump->minPxlX, y, subClumpWidth, 1, spectralValues[n], subClumpWidth, 1, GDT_Float32, 0, 0);
                    }
                    clumpsBand->RasterIO(GF_Read, subClump->minPxlX, y, subClumpWidth, 1, clumpIdxs, subClumpWidth, 1, GDT_UInt32, 0, 0);
                    
                    for(unsigned int x = 0; x < subClumpWidth; ++x)
                    {
                        //std::cout << clumpIdxs[x] << std::endl;
                        for(std::list<unsigned int>::iterator iterIdx = subClump->subclumps.begin(); iterIdx != subClump->subclumps.end(); ++iterIdx)
                        {
                            //std::cout << "\t" << (*iterIdx)+1 << std::endl;
                            if(((*iterIdx)+1) == clumpIdxs[x])
                            {
                                brightnessVal = 0;
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    brightnessVal += spectralValues[n][x];
                                }
                                //std::cout << "brightnessVal = " << brightnessVal << std::endl;
                                brightnessVals.push_back(brightnessVal);
                                break;
                            }
                        }
                    }
                    //std::cout << std::endl;
                }
                
                if(brightnessVals.size() == 0)
                {
                    throw rsgis::img::RSGISImageCalcException("Did not find any of the sub clumps.");
                }
                brightnessVals.sort();
                
                seedIdx = floor(brightnessVals.size() * 0.95);
                
                //std::cout << "Seed Idx: " << seedIdx << std::endl;
                
                counter = 0;
                for(std::list<float>::iterator iterBright = brightnessVals.begin(); iterBright != brightnessVals.end(); ++iterBright)
                {
                    //std::cout << "(*iterBright) = " << *iterBright << std::endl;
                    if(counter == seedIdx)
                    {
                        brightnessSeedVal = *iterBright;
                        foundSeed = true;
                        break;
                    }
                    ++counter;
                }
                
                if(!foundSeed)
                {
                    throw rsgis::img::RSGISImageCalcException("Did not find a seed.");
                }
                
                //std::cout << "brightnessSeedVal = " << brightnessSeedVal << std::endl;
                
                first = true;
                for(unsigned int y = subClump->minPxlY; y < subClump->maxPxlY; ++y)
                {
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        spectralBands[n]->RasterIO(GF_Read, subClump->minPxlX, y, subClumpWidth, 1, spectralValues[n], subClumpWidth, 1, GDT_Float32, 0, 0);
                    }
                    clumpsBand->RasterIO(GF_Read, subClump->minPxlX, y, subClumpWidth, 1, clumpIdxs, subClumpWidth, 1, GDT_UInt32, 0, 0);
                    
                    for(unsigned int x = 0; x < subClumpWidth; ++x)
                    {
                        for(std::list<unsigned int>::iterator iterIdx = subClump->subclumps.begin(); iterIdx != subClump->subclumps.end(); ++iterIdx)
                        {
                            if(((*iterIdx)+1) == clumpIdxs[x])
                            {
                                brightnessVal = 0;
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    brightnessVal += spectralValues[n][x];
                                }
                                
                                brightDist = sqrt((brightnessSeedVal - brightnessVal) * (brightnessSeedVal - brightnessVal));
                                
                                if(first)
                                {
                                    cSeed.xPxl = subClump->minPxlX + x;
                                    cSeed.yPxl = y;
                                    closeBrightDist = brightDist;
                                    first = false;
                                }
                                else if(brightDist < closeBrightDist)
                                {
                                    cSeed.xPxl = subClump->minPxlX + x;
                                    cSeed.yPxl = y;
                                    closeBrightDist = brightDist;
                                }                               
                            }
                        }
                    }                                        
                }
                
                //std::cout << "Pushing back: [" << cSeed.xPxl << "," << cSeed.yPxl << "]\n";
                
                seeds->push_back(RSGISRegionGrowPxlSeeds(seedCount++, cSeed.xPxl, cSeed.yPxl));
                
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    delete[] spectralValues[n];
                }
                delete[] clumpIdxs;
            }
            
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        
        delete[] spectralBands;
        delete[] spectralValues;
        
        return seeds;
    }
    
    RSGISFindRegionGrowingSeeds::~RSGISFindRegionGrowingSeeds()
    {
        
    }
    */
    
    
}}




