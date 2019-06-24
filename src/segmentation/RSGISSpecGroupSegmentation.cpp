/*
 *  RSGISSpecGroupSegmentation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/01/2012.
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

#include "RSGISSpecGroupSegmentation.h"


namespace rsgis{namespace segment{
    
   
    RSGISSpecGroupSegmentation::RSGISSpecGroupSegmentation()
    {
        
    }
    
    void RSGISSpecGroupSegmentation::performSimpleClump(GDALDataset *spectral, GDALDataset *clumps, float specThreshold, bool noDataValProvided, float noDataVal) 
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        rsgis::img::RSGISImageUtils imgUtils;
        imgUtils.zerosUIntGDALDataset(clumps);
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned long clumpIdx = 1;
        std::vector<rsgis::img::PxlLoc> clumpPxls;
        std::queue<rsgis::img::PxlLoc> clumpSearchPxls;
        float *specPxlVals = new float[numSpecBands];
        float *specCPxlVals = new float[numSpecBands];
        
        unsigned int uiPxlVal = 0;
        float dist = 0;
        bool noDataValFound = false;
        
        int feedback = height/10;
        int feedbackCounter = 0;
        std::cout << "Started" << std::flush;
        for(unsigned int i = 0; i < height; ++i)
        {
            if((i % feedback) == 0)
            {
                std::cout << ".." << feedbackCounter << ".." << std::flush;
                feedbackCounter = feedbackCounter + 10;
            }
            
            for(unsigned int j = 0; j < width; ++j)
            {
                // Get pixel value from clump image for (j,i)
                clumpBand->RasterIO(GF_Read, j, i, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                
                // if value is zero create new clump
                if(uiPxlVal == 0)
                {
                    noDataValFound = true;
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        specPxlVals[n] = 0;
                        spectralBands[n]->RasterIO(GF_Read, j, i, 1, 1, &specPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        if(specPxlVals[n] != noDataVal)
                        {
                            noDataValFound = false;
                        }
                    }
                    
                    if(!noDataValProvided | !noDataValFound)
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
                            
                            // Above
                            if(((long)pxl.yPos)-1 >= 0)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    noDataValFound = true;
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                        if(specPxlVals[n] != noDataVal)
                                        {
                                            noDataValFound = false;
                                        }
                                    }
                                    if(!noDataValProvided | !noDataValFound)
                                    {
                                        dist = this->eucDistance(specPxlVals, specCPxlVals, numSpecBands);
                                        
                                        if(dist <= specThreshold)
                                        {
                                            clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                            clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                            clumpBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos-1, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                        }
                                    }
                                }
                            }
                            // Below
                            if((pxl.yPos+1) < height)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    noDataValFound = true;
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                        if(specPxlVals[n] != noDataVal)
                                        {
                                            noDataValFound = false;
                                        }
                                    }
                                    
                                    if(!noDataValProvided | !noDataValFound)
                                    {
                                        dist = this->eucDistance(specPxlVals, specCPxlVals, numSpecBands);
                                        
                                        if(dist <= specThreshold)
                                        {
                                            clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                            clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                            clumpBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos+1, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                        }
                                    }
                                }

                            }
                            // Left
                            if(((long)pxl.xPos)-1 >= 0)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    noDataValFound = true;
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                        if(specPxlVals[n] != noDataVal)
                                        {
                                            noDataValFound = false;
                                        }
                                    }
                                    
                                    if(!noDataValProvided | !noDataValFound)
                                    {
                                        dist = this->eucDistance(specPxlVals, specCPxlVals, numSpecBands);
                                        
                                        if(dist <= specThreshold)
                                        {
                                            clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                            clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                            clumpBand->RasterIO(GF_Write, pxl.xPos-1, pxl.yPos, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                        }
                                    }
                                }

                            }
                            // Right
                            if((pxl.xPos+1) < width)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    noDataValFound = true;
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                        if(specPxlVals[n] != noDataVal)
                                        {
                                            noDataValFound = false;
                                        }
                                    }
                                    
                                    if(!noDataValProvided | !noDataValFound)
                                    {
                                        dist = this->eucDistance(specPxlVals, specCPxlVals, numSpecBands);
                                        
                                        if(dist <= specThreshold)
                                        {
                                            clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                            clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                            clumpBand->RasterIO(GF_Write, pxl.xPos+1, pxl.yPos, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                        }
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
        
        delete[] specPxlVals;
        delete[] specCPxlVals;
    }
    
    void RSGISSpecGroupSegmentation::performSimpleClumpKeepPxlVals(GDALDataset *spectral, GDALDataset *clumps, float specThreshold) 
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        rsgis::img::RSGISImageUtils imgUtils;
        imgUtils.zerosUIntGDALDataset(clumps);
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned long clumpIdx = 1;
        std::vector<rsgis::img::PxlLoc> clumpPxls;
        std::queue<rsgis::img::PxlLoc> clumpSearchPxls;
        std::vector<float> *clumpSpecPxlVals = new  std::vector<float>[numSpecBands];
        float *specPxlVals = new float[numSpecBands];
        float *specCPxlVals = new float[numSpecBands];
        
        unsigned int uiPxlVal = 0;
        float dist = 0;
        
        int feedback = height/10;
        int feedbackCounter = 0;
        std::cout << "Started" << std::flush;
        for(unsigned int i = 0; i < height; ++i)
        {
            if((i % feedback) == 0)
            {
                std::cout << ".." << feedbackCounter << ".." << std::flush;
                feedbackCounter = feedbackCounter + 10;
            }
            
            for(unsigned int j = 0; j < width; ++j)
            {
                // Get pixel value from clump image for (j,i)
                clumpBand->RasterIO(GF_Read, j, i, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                
                // if value is zero create new clump
                if(uiPxlVal == 0)
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
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        clumpSpecPxlVals[n].clear();
                        spectralBands[n]->RasterIO(GF_Read, j, i, 1, 1, &specPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        clumpSpecPxlVals[n].push_back(specPxlVals[n]);
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

                        // Above
                        if(((long)pxl.yPos)-1 >= 0)
                        {
                            clumpBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                            if(uiPxlVal == 0)
                            {
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                }
                                
                                dist = this->eucDistance(specPxlVals, specCPxlVals, numSpecBands);
                                
                                if(dist <= specThreshold)
                                {
                                    clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                    clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        clumpSpecPxlVals[n].push_back(specCPxlVals[n]);
                                    }
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
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                }
                                
                                dist = this->eucDistance(specPxlVals, specCPxlVals, numSpecBands);
                                
                                if(dist <= specThreshold)
                                {
                                    clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                    clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        clumpSpecPxlVals[n].push_back(specCPxlVals[n]);
                                    }
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
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    spectralBands[n]->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                }
                                
                                dist = this->eucDistance(specPxlVals, specCPxlVals, numSpecBands);
                                
                                if(dist <= specThreshold)
                                {
                                    clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                    clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        clumpSpecPxlVals[n].push_back(specCPxlVals[n]);
                                    }
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
                                for(unsigned int n = 0; n < numSpecBands; ++n)
                                {
                                    spectralBands[n]->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                }
                                
                                dist = this->eucDistance(specPxlVals, specCPxlVals, numSpecBands);
                                
                                if(dist <= specThreshold)
                                {
                                    clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                    clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        clumpSpecPxlVals[n].push_back(specCPxlVals[n]);
                                    }
                                    clumpBand->RasterIO(GF_Write, pxl.xPos+1, pxl.yPos, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                }
                            }
                            
                        }
                    }
                    
                    clumpIdx++;
                }
            }
        }
        std::cout << " Complete (Generated " << clumpIdx-1 << " clumps).\n";
        
        delete[] clumpSpecPxlVals;
        delete[] specPxlVals;
        delete[] specCPxlVals;
    }

    void RSGISSpecGroupSegmentation::performSimpleClumpStdDevWeights(GDALDataset *spectral, GDALDataset *clumps, float specThreshold, bool noDataValProvided, float noDataVal) 
    {
        if(spectral->GetRasterXSize() != clumps->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(spectral->GetRasterYSize() != clumps->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        
        rsgis::img::RSGISImageUtils imgUtils;
        imgUtils.zerosUIntGDALDataset(clumps);
        
        unsigned int width = spectral->GetRasterXSize();
        unsigned int height = spectral->GetRasterYSize();
        unsigned int numSpecBands = spectral->GetRasterCount();
        
        float **spectralVals = new float*[numSpecBands];
        GDALRasterBand **spectralBands = new GDALRasterBand*[numSpecBands];
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            spectralBands[n] = spectral->GetRasterBand(n+1);
            spectralVals[n] = new float[width];
        }
        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        
        unsigned long clumpIdx = 1;
        std::vector<rsgis::img::PxlLoc> clumpPxls;
        std::queue<rsgis::img::PxlLoc> clumpSearchPxls;
        float *specPxlVals = new float[numSpecBands];
        float *specCPxlVals = new float[numSpecBands];
        
        float *meanSpecVals = new float[numSpecBands];
        float *stdDevSpecVals = new float[numSpecBands];
        unsigned int *numVals = new unsigned int[numSpecBands];
        
        for(unsigned int i = 0; i < height; ++i)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                spectralBands[n]->RasterIO(GF_Read, 0, i, width, 1, spectralVals[n], width, 1, GDT_Float32, 0, 0);
            }
            for(unsigned int j = 0; j < width; ++j)
            {
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    if(spectralVals[n][j] != 0)
                    {
                        meanSpecVals[n] += spectralVals[n][j];
                        ++numVals[n];
                    }
                }
            }
        }
        
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            meanSpecVals[n] = meanSpecVals[n] / numVals[n];
        }
        
        for(unsigned int i = 0; i < height; ++i)
        {
            for(unsigned int n = 0; n < numSpecBands; ++n)
            {
                spectralBands[n]->RasterIO(GF_Read, 0, i, width, 1, spectralVals[n], width, 1, GDT_Float32, 0, 0);
            }
            for(unsigned int j = 0; j < width; ++j)
            {
                for(unsigned int n = 0; n < numSpecBands; ++n)
                {
                    if(spectralVals[n][j] != 0)
                    {
                        stdDevSpecVals[n] += ((spectralVals[n][j]-meanSpecVals[n])*(spectralVals[n][j]-meanSpecVals[n]));
                    }
                }
            }
        }
        
        for(unsigned int n = 0; n < numSpecBands; ++n)
        {
            stdDevSpecVals[n] = sqrt(stdDevSpecVals[n] / numVals[n]);
        }
        
        unsigned int uiPxlVal = 0;
        float dist = 0;
        bool noDataValFound = false;
        
        int feedback = height/10;
        int feedbackCounter = 0;
        std::cout << "Started" << std::flush;
        for(unsigned int i = 0; i < height; ++i)
        {
            if((i % feedback) == 0)
            {
                std::cout << ".." << feedbackCounter << ".." << std::flush;
                feedbackCounter = feedbackCounter + 10;
            }
            
            for(unsigned int j = 0; j < width; ++j)
            {
                // Get pixel value from clump image for (j,i)
                clumpBand->RasterIO(GF_Read, j, i, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                
                // if value is zero create new clump
                if(uiPxlVal == 0)
                {
                    noDataValFound = true;
                    for(unsigned int n = 0; n < numSpecBands; ++n)
                    {
                        specPxlVals[n] = 0;
                        spectralBands[n]->RasterIO(GF_Read, j, i, 1, 1, &specPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                        if(specPxlVals[n] != noDataVal)
                        {
                            noDataValFound = false;
                        }
                    }
                    
                    if(!noDataValProvided | !noDataValFound)
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
                            
                            // Above
                            if(((long)pxl.yPos)-1 >= 0)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    noDataValFound = true;
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos-1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                        if(specPxlVals[n] != noDataVal)
                                        {
                                            noDataValFound = false;
                                        }
                                    }
                                    if(!noDataValProvided | !noDataValFound)
                                    {
                                        dist = this->weightedEucDistance(specPxlVals, specCPxlVals, numSpecBands, stdDevSpecVals);
                                        
                                        if(dist <= specThreshold)
                                        {
                                            clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                            clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos-1));
                                            clumpBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos-1, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                        }
                                    }
                                }
                            }
                            // Below
                            if((pxl.yPos+1) < height)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    noDataValFound = true;
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos, pxl.yPos+1, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                        if(specPxlVals[n] != noDataVal)
                                        {
                                            noDataValFound = false;
                                        }
                                    }
                                    
                                    if(!noDataValProvided | !noDataValFound)
                                    {
                                        dist = this->weightedEucDistance(specPxlVals, specCPxlVals, numSpecBands, stdDevSpecVals);
                                        
                                        if(dist <= specThreshold)
                                        {
                                            clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                            clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos, pxl.yPos+1));
                                            clumpBand->RasterIO(GF_Write, pxl.xPos, pxl.yPos+1, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                        }
                                    }
                                }
                                
                            }
                            // Left
                            if(((long)pxl.xPos)-1 >= 0)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    noDataValFound = true;
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos-1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                        if(specPxlVals[n] != noDataVal)
                                        {
                                            noDataValFound = false;
                                        }
                                    }
                                    
                                    if(!noDataValProvided | !noDataValFound)
                                    {
                                        dist = this->weightedEucDistance(specPxlVals, specCPxlVals, numSpecBands, stdDevSpecVals);
                                        
                                        if(dist <= specThreshold)
                                        {
                                            clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                            clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos-1, pxl.yPos));
                                            clumpBand->RasterIO(GF_Write, pxl.xPos-1, pxl.yPos, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                        }
                                    }
                                }
                                
                            }
                            // Right
                            if((pxl.xPos+1) < width)
                            {
                                clumpBand->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                                if(uiPxlVal == 0)
                                {
                                    noDataValFound = true;
                                    for(unsigned int n = 0; n < numSpecBands; ++n)
                                    {
                                        spectralBands[n]->RasterIO(GF_Read, pxl.xPos+1, pxl.yPos, 1, 1, &specCPxlVals[n], 1, 1, GDT_Float32, 0, 0);
                                        if(specPxlVals[n] != noDataVal)
                                        {
                                            noDataValFound = false;
                                        }
                                    }
                                    
                                    if(!noDataValProvided | !noDataValFound)
                                    {
                                        dist = this->weightedEucDistance(specPxlVals, specCPxlVals, numSpecBands, stdDevSpecVals);
                                        
                                        if(dist <= specThreshold)
                                        {
                                            clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                            clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                            clumpBand->RasterIO(GF_Write, pxl.xPos+1, pxl.yPos, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                        }
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
        
        delete[] specPxlVals;
        delete[] specCPxlVals;
        delete[] meanSpecVals;
        delete[] stdDevSpecVals;
        delete[] numVals;
    }
    
    float RSGISSpecGroupSegmentation::eucDistance(float *vals1, float *vals2, unsigned int numVals)
    {
        float sqSum = 0;
        for(unsigned int i = 0; i < numVals; ++i)
        {
            sqSum += (vals1[i] - vals2[i])*(vals1[i] - vals2[i]);
        }
        
        return sqrt(sqSum)/numVals;
    }
    
    float RSGISSpecGroupSegmentation::weightedEucDistance(float *vals1, float *vals2, unsigned int numVals, float *stddev)
    {
        float sqSum = 0;
        for(unsigned int i = 0; i < numVals; ++i)
        {
            sqSum += (((vals1[i] - vals2[i])*(vals1[i] - vals2[i]))/stddev[i]);
        }
        return sqrt(sqSum)/numVals;
    }
    
    RSGISSpecGroupSegmentation::~RSGISSpecGroupSegmentation()
    {
        
    }
    
    
}}



