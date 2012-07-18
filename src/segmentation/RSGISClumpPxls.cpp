/*
 *  RSGISClumpPxls.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/01/2012.
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

#include "RSGISClumpPxls.h"


namespace rsgis{namespace segment{
    
    RSGISClumpPxls::RSGISClumpPxls()
    {
        
    }
        
    void RSGISClumpPxls::performClump(GDALDataset *catagories, GDALDataset *clumps, bool noDataValProvided, unsigned int noDataVal) throw(rsgis::img::RSGISImageCalcException)
    {
        if(catagories->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(catagories->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        rsgis::img::RSGISImageUtils imgUtils;
        imgUtils.zerosUIntGDALDataset(clumps);
        
        unsigned int width = catagories->GetRasterXSize();
        unsigned int height = catagories->GetRasterYSize();
        
        GDALRasterBand *catagoryBand = catagories->GetRasterBand(1);
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned long clumpIdx = 1;
        std::vector<rsgis::img::PxlLoc> clumpPxls;
        std::queue<rsgis::img::PxlLoc> clumpSearchPxls;
        unsigned int catPxlVal = 0;
        unsigned int catCPxlVal = 0;
        
        unsigned int uiPxlVal = 0;
        
        int feedback = height/10;
        int feedbackCounter = 0;
        std::cout << "Started" << std::flush;
        for(unsigned int i = 0; i < height; ++i)
        {
            if((i % feedback) == 0)
            {
                std::cout << "." << feedbackCounter << "." << std::flush;
                feedbackCounter = feedbackCounter + 10;
            }
            
            for(unsigned int j = 0; j < width; ++j)
            {
                //std::cout << "Processing Pixel [" << j << "," << i << "]\n";
                // Get pixel value from clump image for (j,i)
                clumpBand->RasterIO(GF_Read, j, i, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                
                // if value is zero create new clump
                if(uiPxlVal == 0) 
                {
                    catagoryBand->RasterIO(GF_Read, j, i, 1, 1, &catPxlVal, 1, 1, GDT_UInt32, 0, 0);
                    if((!noDataValProvided) | (noDataValProvided & (catPxlVal != noDataVal)))
                    {     
                        // Make sure all lists are empty.
                        clumpPxls.clear();
                        if(!clumpSearchPxls.empty())
                        {
                            while(clumpSearchPxls.size() > 0)
                            {
                                clumpSearchPxls.pop();
                            }
                        }
                                                
                        clumpPxls.push_back(rsgis::img::PxlLoc(j, i));
                        clumpSearchPxls.push(rsgis::img::PxlLoc(j, i));
                        clumpBand->RasterIO(GF_Write, j, i, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                        
                        // Add neigbouring pixels to clump.
                        // If no more pixels to add then stop.
                        while(clumpSearchPxls.size() > 0)
                        {
                            rsgis::img::PxlLoc pxl = clumpSearchPxls.front();
                            clumpSearchPxls.pop();
                            
                            //std::cout << "\tSearch Size = " << clumpSearchPxls.size() << std::endl;
                            //std::cout << "\t\tProcessing [" << pxl.xPos << "," << pxl.yPos << "]\n";
                            
                            // Above
                            if(((long)pxl.yPos)-1 >= 0)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    catagoryBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &catCPxlVal, 1, 1, GDT_UInt32, 0, 0);

                                    if(catPxlVal == catCPxlVal)
                                    {
                                        clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                        clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                        clumpBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos-1, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                    }
                                }
                            }
                            // Below
                            if((pxl.yPos+1) < height)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    catagoryBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &catCPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                    
                                    if(catPxlVal == catCPxlVal)
                                    {
                                        clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                        clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                        clumpBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos+1, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                    }
                                }
                                
                            }
                            // Left
                            if(((long)pxl.xPos)-1 >= 0)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    catagoryBand->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &catCPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                    
                                    if(catPxlVal == catCPxlVal)
                                    {
                                        clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                        clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                        clumpBand->RasterIO(GF_Write, pxl.xPos-1, pxl.yPos, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                    }
                                }
                            }
                            // Right
                            if((pxl.xPos+1) < width)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    catagoryBand->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &catCPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                    
                                    if(catPxlVal == catCPxlVal)
                                    {
                                        clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                        clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                        clumpBand->RasterIO(GF_Write, pxl.xPos+1, pxl.yPos, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                    }
                                }
                            }
                        }
                        
                        clumpIdx++;
                    }
                }
            }
        }
        std::cout << " Complete (Generated " << clumpIdx-1 << " clumps).\n";
    }
        
    RSGISClumpPxls::~RSGISClumpPxls()
    {
        
    }
    
    
    RSGISRelabelClumps::RSGISRelabelClumps()
    {
        
    }
    
    void RSGISRelabelClumps::relabelClumps(GDALDataset *catagories, GDALDataset *clumps) throw(rsgis::img::RSGISImageCalcException)
    {
        if(catagories->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(catagories->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        rsgis::img::RSGISImageUtils imgUtils;
        imgUtils.zerosUIntGDALDataset(clumps);
        
        unsigned int width = catagories->GetRasterXSize();
        unsigned int height = catagories->GetRasterYSize();
        
        GDALRasterBand *catagoryBand = catagories->GetRasterBand(1);
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned int *clumpIdxs = new unsigned int[width];
        
        unsigned long maxClumpIdx = 0;
        
        unsigned int feedback = height/10;
        unsigned int feedbackCounter = 0;
        std::cout << "Started ." << std::flush;
        for(unsigned int i = 0; i < height; ++i)
        {
            if((i % feedback) == 0)
            {
                std::cout << "." << feedbackCounter << "." << std::flush;
                feedbackCounter += 10;
            }
            catagoryBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
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
        std::cout << ". Complete\n";
        std::vector<unsigned int> clumpTable;
        clumpTable.reserve(maxClumpIdx);
        
        for(size_t i = 0; i < maxClumpIdx; ++i)
        {
            clumpTable.push_back(0);
        }
        
         feedbackCounter = 0;
        std::cout << "Started ." << std::flush;
        for(unsigned int i = 0; i < height; ++i)
        {
            if((i % feedback) == 0)
            {
                std::cout << "." << feedbackCounter << "." << std::flush;
                feedbackCounter += 10;
            }
            catagoryBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    clumpTable.at(clumpIdxs[j]-1) = 1;
                }
            }
        }
        std::cout << ". Complete\n";
        
        unsigned int idx = 1;
        for(size_t i = 0; i < maxClumpIdx; ++i)
        {
            if(clumpTable.at(i) == 1)
            {
                clumpTable.at(i) = idx++;
            }
        }
        
        feedbackCounter = 0;
        std::cout << "Started ." << std::flush;
        for(unsigned int i = 0; i < height; ++i)
        {
            if((i % feedback) == 0)
            {
                std::cout << "." << feedbackCounter << "." << std::flush;
                feedbackCounter += 10;
            }
            catagoryBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    clumpIdxs[j] = clumpTable.at(clumpIdxs[j]-1);
                }
            }
            clumpBand->RasterIO(GF_Write, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
        }
        std::cout << ". Complete\n";
                 
        delete[] clumpIdxs;
    }
    
    RSGISRelabelClumps::~RSGISRelabelClumps()
    {
        
    }
    
    
}}

