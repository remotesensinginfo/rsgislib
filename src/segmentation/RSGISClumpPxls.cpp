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
        
    void RSGISClumpPxls::performClump(GDALDataset *catagories, GDALDataset *clumps, bool noDataValProvided, unsigned int noDataVal, std::vector<unsigned int> *clumpPxlVals) 
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
            if((feedback > 0) && (i % feedback) == 0)
            {
                std::cout << "." << feedbackCounter << "." << std::flush;
                feedbackCounter = feedbackCounter + 10;
            }
            
            for(unsigned int j = 0; j < width; ++j)
            {
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
                        
                        if(clumpPxlVals != NULL)
                        {
                            clumpPxlVals->push_back(catPxlVal);
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
                        
                        ++clumpIdx;
                    }
                }
            }
        }
        std::cout << " Complete (Generated " << clumpIdx-1 << " clumps).\n";
        if(clumpPxlVals != NULL)
        {
            if(clumpPxlVals->size() != (clumpIdx-1))
            {
                std::cout << "Number of clump pixel values: " << clumpPxlVals->size() << std::endl;
                throw rsgis::img::RSGISImageCalcException("Number of clump pixel values in list is not equal to the number of clumps.");
            }
        }
        
    }
    
    void RSGISClumpPxls::performClumpPosVals(GDALDataset *catagories, GDALDataset *clumps) 
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
                // Get pixel value from clump image for (j,i)
                clumpBand->RasterIO(GF_Read, j, i, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                
                // if value is zero create new clump
                if(uiPxlVal == 0)
                {
                    catagoryBand->RasterIO(GF_Read, j, i, 1, 1, &catPxlVal, 1, 1, GDT_UInt32, 0, 0);
                    if(catPxlVal > 0)
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
                        
                        ++clumpIdx;
                    }
                }
            }
        }
        std::cout << " Complete (Generated " << clumpIdx-1 << " clumps).\n";
    }
    
    void RSGISClumpPxls::performMultiBandClump(std::vector<GDALDataset*> *catagories, std::string clumpsOutputPath, std::string outFormat, bool noDataValProvided, unsigned int noDataVal, bool addRatPxlVals) 
    {
        try
        {
            rsgis::img::RSGISImageUtils imgUtils;
            
            unsigned int numDS = catagories->size();
            int **dsOffsets = new int*[numDS];
            for(unsigned int i = 0; i < numDS; ++i)
            {
                dsOffsets[i] = new int[2];
            }
            int width = 0;
            int height = 0;
            double *gdalTransform = new double[6];
            
            imgUtils.getImageOverlap(catagories, dsOffsets, &width, &height, gdalTransform);
            
            GDALDriver *gdalDriver = GetGDALDriverManager()->GetDriverByName(outFormat.c_str());
			if(gdalDriver == NULL)
			{                
                delete[] gdalTransform;
                for(unsigned int i = 0; i < numDS; ++i)
                {
                    delete[] dsOffsets[i];
                }
                delete[] dsOffsets;
                
                throw rsgis::img::RSGISImageBandException("Requested GDAL driver does not exists..");
			}
            char **papszOptions = imgUtils.getGDALCreationOptionsForFormat(outFormat);
			std::cout << "New image width = " << width << " height = " << height << std::endl;
            
            GDALDataset *clumpsDS = gdalDriver->Create(clumpsOutputPath.c_str(), width, height, 1, GDT_UInt32, papszOptions);
			
			if(clumpsDS == NULL)
			{
                delete[] gdalTransform;
                for(unsigned int i = 0; i < numDS; ++i)
                {
                    delete[] dsOffsets[i];
                }
                delete[] dsOffsets;
                
                throw rsgis::img::RSGISImageBandException("Output image could not be created. Check filepath.");
			}
			clumpsDS->SetGeoTransform(gdalTransform);
			clumpsDS->SetProjection(catagories->at(0)->GetProjectionRef());
            imgUtils.zerosUIntGDALDataset(clumpsDS);
            
            // Count number of image bands
			unsigned int numInBands = 0;
            for(int i = 0; i < numDS; i++)
			{
				numInBands += catagories->at(i)->GetRasterCount();
			}
            
            // Get Image Input Bands
			int **bandOffsets = new int*[numInBands];
			GDALRasterBand **catBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < catagories->at(i)->GetRasterCount(); j++)
				{
					catBands[counter] = catagories->at(i)->GetRasterBand(j+1);
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					counter++;
				}
			}
            
            //Get Image Output Band
			GDALRasterBand *clumpBand = clumpsDS->GetRasterBand(1);
            clumpBand->SetDescription("Clumps");
                        
            unsigned long clumpIdx = 1;
            std::vector<rsgis::img::PxlLoc> clumpPxls;
            std::queue<rsgis::img::PxlLoc> clumpSearchPxls;
            unsigned int *catPxlVals = new unsigned int[numInBands];
            unsigned int *catCPxlVals = new unsigned int[numInBands];
            
            unsigned int uiPxlVal = 0;
            std::vector<int*> outRATVals;
            
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
                    // Get pixel value from clump image for (j,i)
                    clumpBand->RasterIO(GF_Read, j, i, 1, 1, &uiPxlVal, 1, 1, GDT_UInt32, 0, 0);
                    
                    // if value is zero create new clump
                    if(uiPxlVal == 0)
                    {
                        for(unsigned int n = 0; n < numInBands; ++n)
                        {
                            catBands[n]->RasterIO(GF_Read, bandOffsets[n][0]+j, bandOffsets[n][1]+i, 1, 1, &catPxlVals[n], 1, 1, GDT_UInt32, 0, 0);
                        }
                        
                        if((!noDataValProvided) | (noDataValProvided & (!this->allValueEqual(catPxlVals, numInBands, noDataVal))))
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
                            
                            if(addRatPxlVals)
                            {
                                int *vals = new int[numInBands];
                                for(unsigned int n = 0; n < numInBands; ++n)
                                {
                                    vals[n] = catPxlVals[n];
                                }
                                outRATVals.push_back(vals);
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
                                        for(unsigned int n = 0; n < numInBands; ++n)
                                        {
                                            catBands[n]->RasterIO(GF_Read, bandOffsets[n][0]+pxl.xPos, bandOffsets[n][1]+(pxl.yPos-1), 1, 1, &catCPxlVals[n], 1, 1, GDT_UInt32, 0, 0);
                                        }
                                        
                                        if(this->allValueEqual(catPxlVals, catCPxlVals, numInBands))
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
                                        for(unsigned int n = 0; n < numInBands; ++n)
                                        {
                                            catBands[n]->RasterIO(GF_Read, bandOffsets[n][0]+pxl.xPos, bandOffsets[n][1]+(pxl.yPos+1), 1, 1, &catCPxlVals[n], 1, 1, GDT_UInt32, 0, 0);
                                        }
                                        
                                        if(this->allValueEqual(catPxlVals, catCPxlVals, numInBands))
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
                                        for(unsigned int n = 0; n < numInBands; ++n)
                                        {
                                            catBands[n]->RasterIO(GF_Read, bandOffsets[n][0]+(pxl.xPos-1), bandOffsets[n][1]+pxl.yPos, 1, 1, &catCPxlVals[n], 1, 1, GDT_UInt32, 0, 0);
                                        }
                                        
                                        if(this->allValueEqual(catPxlVals, catCPxlVals, numInBands))
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
                                        for(unsigned int n = 0; n < numInBands; ++n)
                                        {
                                            catBands[n]->RasterIO(GF_Read, bandOffsets[n][0]+(pxl.xPos+1), bandOffsets[n][1]+pxl.yPos, 1, 1, &catCPxlVals[n], 1, 1, GDT_UInt32, 0, 0);
                                        }
                                        
                                        if(this->allValueEqual(catPxlVals, catCPxlVals, numInBands))
                                        {
                                            clumpPxls.push_back(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                            clumpSearchPxls.push(rsgis::img::PxlLoc(pxl.xPos+1, pxl.yPos));
                                            clumpBand->RasterIO(GF_Write, pxl.xPos+1, pxl.yPos, 1, 1, &clumpIdx, 1, 1, GDT_UInt32, 0, 0);
                                        }
                                    }
                                }
                            }
                            
                            ++clumpIdx;
                        }
                    }
                }
            }
            std::cout << " Complete (Generated " << clumpIdx-1 << " clumps).\n";
            
            clumpBand->SetMetadataItem("LAYER_TYPE", "thematic");
            if(addRatPxlVals)
            {
                GDALRasterAttributeTable *rat = clumpBand->GetDefaultRAT();
                size_t numRows = rat->GetRowCount();
                if((outRATVals.size()+1) > numRows)
                {
                    numRows = outRATVals.size()+1;
                    rat->SetRowCount(numRows);
                }
                rastergis::RSGISRasterAttUtils attUtils;
                utils::RSGISTextUtils txtUtils;
                int *ratColVals = new int[numRows];
                for(size_t i = 0; i < numInBands; ++i)
                {
                    for(size_t j = 0; j < numRows; ++j)
                    {
                        if(j == 0)
                        {
                            ratColVals[j] = 0;
                        }
                        else
                        {
                            ratColVals[j] = outRATVals.at(j-1)[i];
                        }
                    }
                    attUtils.writeIntColumn(rat, "ClumpVal_"+txtUtils.sizettostring(i+1), ratColVals, numRows);
                }
                delete[] ratColVals;
                
                for(size_t i = 0; i < outRATVals.size(); ++i)
                {
                    delete[] outRATVals.at(i);
                }
            }
            
            GDALClose(clumpsDS);
            
            delete[] catPxlVals;
            delete[] catCPxlVals;
            
            delete[] catBands;
            for(unsigned int i = 0; i < numInBands; ++i)
            {
                delete[] bandOffsets[i];
            }
            
            delete[] gdalTransform;
            for(unsigned int i = 0; i < numDS; ++i)
            {
                delete[] dsOffsets[i];
            }
            delete[] dsOffsets;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    bool RSGISClumpPxls::allValueEqual(unsigned int *vals, unsigned int numVals, unsigned int equalVal)
    {
        for(unsigned int i = 0; i < numVals; ++i)
        {
            if(vals[i] != equalVal)
            {
                return false;
            }
        }
        
        return true;
    }
    
    
    bool RSGISClumpPxls::allValueEqual(unsigned int *vals1, unsigned int *vals2, unsigned int numVals)
    {
        for(unsigned int i = 0; i < numVals; ++i)
        {
            if(vals1[i] != vals2[i])
            {
                return false;
            }
        }
        
        return true;
    }
    
    
    RSGISClumpPxls::~RSGISClumpPxls()
    {
        
    }
    
    
    RSGISRelabelClumps::RSGISRelabelClumps()
    {
        
    }
    
    void RSGISRelabelClumps::relabelClumps(GDALDataset *catagories, GDALDataset *clumps) 
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
    
    void RSGISRelabelClumps::relabelClumpsCalcImg(GDALDataset *catagories, GDALDataset *clumps) 
    {
        try
        {
            if(catagories->GetRasterXSize() != clumps->GetRasterXSize())
            {
                throw rsgis::img::RSGISImageCalcException("Widths are not the same");
            }
            if(catagories->GetRasterYSize() != clumps->GetRasterYSize())
            {
                throw rsgis::img::RSGISImageCalcException("Heights are not the same");
            }
            
            std::cout << "Finding maximum image value\n";
            rsgis::rastergis::RSGISRasterAttUtils ratUtils;
            long minVal = 0;
            long maxVal = 0;
            ratUtils.getImageBandMinMax(catagories, 1, &minVal, &maxVal);
            
            unsigned long maxClumpIdx = boost::lexical_cast<unsigned long>(maxVal)+1;
            
            unsigned long *clumpIdxLookUp = new unsigned long[maxClumpIdx];
            for(unsigned int i = 0; i < maxClumpIdx; ++i)
            {
                clumpIdxLookUp[i] = 0;
            }
            
            std::cout << "Creating Look up table.\n";
            RSGISCreateRelabelLookupTable *createLookUp = new RSGISCreateRelabelLookupTable(clumpIdxLookUp, maxClumpIdx);
            rsgis::img::RSGISCalcImage calcImgCreateLoopUp = rsgis::img::RSGISCalcImage(createLookUp);
            calcImgCreateLoopUp.calcImage(&catagories, 1, 0);
            delete createLookUp;
            
            
            std::cout << "Applying Look up table.\n";
            RSGISApplyRelabelLookupTable *applyLookUp = new RSGISApplyRelabelLookupTable(clumpIdxLookUp, maxClumpIdx);
            rsgis::img::RSGISCalcImage calcImgApplyLookUp = rsgis::img::RSGISCalcImage(applyLookUp);
            calcImgApplyLookUp.calcImage(&catagories, 1, 0, clumps);
            delete applyLookUp;
            
            delete[] clumpIdxLookUp;
            
        }
        catch(rsgis::img::RSGISImageCalcException &e)
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
    
    RSGISRelabelClumps::~RSGISRelabelClumps()
    {
        
    }
    
    
    

    RSGISCreateRelabelLookupTable::RSGISCreateRelabelLookupTable(unsigned long *clumpIdxLookUp, unsigned long numVals):rsgis::img::RSGISCalcImageValue(0)
    {
        this->clumpIdxLookUp = clumpIdxLookUp;
        this->numVals = numVals;
        this->nextVal = 1;
    }

    void RSGISCreateRelabelLookupTable::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) 
    {
        try
        {
            if((intBandValues[0] > 0) & (intBandValues[0] < numVals))
            {
                size_t fid = boost::lexical_cast<size_t>(intBandValues[0]);
                
                if(clumpIdxLookUp[fid] == 0)
                {
                    clumpIdxLookUp[fid] = this->nextVal++;
                }
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
    
    RSGISCreateRelabelLookupTable::~RSGISCreateRelabelLookupTable()
    {
        
    }

    
    
    RSGISApplyRelabelLookupTable::RSGISApplyRelabelLookupTable(unsigned long *clumpIdxLookUp, unsigned long numVals): rsgis::img::RSGISCalcImageValue(1)
    {
        this->clumpIdxLookUp = clumpIdxLookUp;
        this->numVals = numVals;
    }
		
    void RSGISApplyRelabelLookupTable::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
    {
        try
        {
            if((intBandValues[0] > 0) & (intBandValues[0] < numVals))
            {
                unsigned long fid = boost::lexical_cast<unsigned long>(intBandValues[0]);
                
                output[0] = clumpIdxLookUp[fid];
            }
            else
            {
                output[0] = 0;
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
    
    RSGISApplyRelabelLookupTable::~RSGISApplyRelabelLookupTable()
    {
        
    }
    
    
}}

