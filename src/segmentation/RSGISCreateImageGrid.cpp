/*
 *  RSGISCreateImageGrid.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/04/2015.
 *  Copyright 2015 RSGISLib.
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

#include "RSGISCreateImageGrid.h"

namespace rsgis{namespace segment{
    

    RSGISCreateImageGrid::RSGISCreateImageGrid()
    {
        
    }
    
    void RSGISCreateImageGrid::createClumpsGrid(GDALDataset *clumpsImage, unsigned int numXPxls, unsigned int numYPxls)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            unsigned long width = clumpsImage->GetRasterXSize();
            unsigned long height = clumpsImage->GetRasterYSize();
            unsigned int numBands = clumpsImage->GetRasterCount();
            if(numBands != 1)
            {
                throw RSGISImageException("Data must only have 1 image band.");
            }
            
            unsigned int numFullCols = (width/numXPxls);
            unsigned int extraColsPxls = width - (numFullCols * numXPxls);
            unsigned int numCols = numFullCols;
            if(extraColsPxls > 0)
            {
                ++numCols;
            }
            
            unsigned int numFullRows = (height/numYPxls);
            unsigned int extraRowsPxls = height - (numFullRows * numYPxls);
            unsigned int numRows = numFullRows;
            if(extraRowsPxls > 0)
            {
                ++numRows;
            }
            
            unsigned int numTiles = numRows * numCols;
            
            std::cout << "Num Tiles [" << numCols << ", " << numRows << "] = " << numTiles << std::endl;
            
            unsigned int startPxlXIdx = 0;
            unsigned int startPxlYIdx = 0;
            unsigned int endPxlXIdx = 0;
            unsigned int endPxlYIdx = 0;
            unsigned int tileID = 1;
            for(unsigned int i = 0; i < numFullRows; ++i)
            {
                startPxlYIdx = i * numYPxls;
                endPxlYIdx = startPxlYIdx + numYPxls;
                for(unsigned int j = 0; j < numFullCols; ++j)
                {
                    startPxlXIdx = j * numXPxls;
                    endPxlXIdx = startPxlXIdx + numXPxls;
                    this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                }
                
                if(extraColsPxls > 0)
                {
                    startPxlXIdx = numFullCols * numXPxls;
                    endPxlXIdx = startPxlXIdx + extraColsPxls;
                    this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                }
            }
            
            if(extraRowsPxls > 0)
            {
                startPxlYIdx = numFullRows * numYPxls;
                endPxlYIdx = startPxlYIdx + extraRowsPxls;
                for(unsigned int j = 0; j < numFullCols; ++j)
                {
                    startPxlXIdx = j * numXPxls;
                    endPxlXIdx = startPxlXIdx + numXPxls;
                    this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                }
                
                if(extraColsPxls > 0)
                {
                    startPxlXIdx = numFullCols * numXPxls;
                    endPxlXIdx = startPxlXIdx + extraColsPxls;
                    this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                }
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    void RSGISCreateImageGrid::createClumpsOffsetGrid(GDALDataset *clumpsImage, unsigned int numXPxls, unsigned int numYPxls)throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            unsigned long width = clumpsImage->GetRasterXSize();
            unsigned long height = clumpsImage->GetRasterYSize();
            
            if((width < (numXPxls/2)) | (height < (numYPxls/2)))
            {
                throw rsgis::img::RSGISImageCalcException("Image must be larger that half tile size.");
            }
            
            unsigned int numBands = clumpsImage->GetRasterCount();
            if(numBands != 1)
            {
                throw RSGISImageException("Data must only have 1 image band.");
            }
            
            unsigned int hNumXPxls = numXPxls/2;
            unsigned int hNumYPxls = numYPxls/2;
            
            unsigned long widthOff = width - hNumXPxls;
            unsigned long heightOff = height - hNumYPxls;
            
            unsigned int numFullCols = (widthOff/numXPxls);
            unsigned int extraColsPxls = widthOff - (numFullCols * numXPxls);
            unsigned int numCols = numFullCols + 1; // extra column for the first 'Half Tile'.
            if(extraColsPxls > 0)
            {
                numCols += 1;
            }
            
            unsigned int numFullRows = (heightOff/numYPxls);
            unsigned int extraRowsPxls = heightOff - (numFullRows * numYPxls);
            unsigned int numRows = numFullRows + 1; // extra row for the first 'Half Tile'
            if(extraRowsPxls > 0)
            {
                numRows += 1;
            }
            
            unsigned int numTiles = numRows * numCols;
            
            std::cout << "Num Tiles [" << numCols << ", " << numRows << "] = " << numTiles << std::endl;
            
            unsigned int startPxlXIdx = 0;
            unsigned int startPxlYIdx = 0;
            unsigned int endPxlXIdx = 0;
            unsigned int endPxlYIdx = 0;
            unsigned int tileID = 1;
            
            // First row of half tiles
            startPxlYIdx = 0;
            endPxlYIdx = hNumYPxls;
            
            // First tile in row.
            startPxlXIdx = 0;
            endPxlXIdx = hNumXPxls;
            
            this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
            
            for(unsigned int j = 0; j < numFullCols; ++j)
            {
                startPxlXIdx = (j * numXPxls) + hNumXPxls;
                endPxlXIdx = startPxlXIdx + numXPxls;
                this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
            }
            
            if(extraColsPxls > 0)
            {
                startPxlXIdx = hNumXPxls + (numFullCols * numXPxls);
                endPxlXIdx = startPxlXIdx + extraColsPxls;
                this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
            }
            
            
            for(unsigned int i = 0; i < numFullRows; ++i)
            {
                startPxlYIdx = (i * numYPxls) + hNumYPxls;
                endPxlYIdx = startPxlYIdx + numYPxls;
                
                // First tile in row.
                startPxlXIdx = 0;
                endPxlXIdx = hNumXPxls;
                this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                
                for(unsigned int j = 0; j < numFullCols; ++j)
                {
                    startPxlXIdx = (j * numXPxls) + hNumXPxls;
                    endPxlXIdx = startPxlXIdx + numXPxls;
                    this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                }
                
                if(extraColsPxls > 0)
                {
                    startPxlXIdx = hNumXPxls + (numFullCols * numXPxls);
                    endPxlXIdx = startPxlXIdx + extraColsPxls;
                    this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                }
            }
            
            if(extraRowsPxls > 0)
            {
                startPxlYIdx = hNumYPxls + (numFullRows * numYPxls);
                endPxlYIdx = startPxlYIdx + extraRowsPxls;
                
                // First tile in row.
                startPxlXIdx = 0;
                endPxlXIdx = hNumXPxls;
                this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                
                for(unsigned int j = 0; j < numFullCols; ++j)
                {
                    startPxlXIdx = (j * numXPxls) + hNumXPxls;
                    endPxlXIdx = startPxlXIdx + numXPxls;
                    this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                }
                
                if(extraColsPxls > 0)
                {
                    startPxlXIdx = hNumXPxls + (numFullCols * numXPxls);
                    endPxlXIdx = startPxlXIdx + extraColsPxls;
                    this->writeImagePxlVal2Region(clumpsImage, tileID++, startPxlXIdx, endPxlXIdx, startPxlYIdx, endPxlYIdx);
                }
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }
    
    void RSGISCreateImageGrid::writeImagePxlVal2Region(GDALDataset *clumpsImage, unsigned int pxlVal, unsigned int startX, unsigned int endX, unsigned int startY, unsigned int endY) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            unsigned int blockWidth = endX - startX;
            unsigned int blockHeight = endY - startY;
            unsigned int blockSize = blockWidth * blockHeight;
            unsigned int *block = new unsigned int[blockSize];
            for(unsigned int i = 0; i < blockSize; ++i)
            {
                block[i] = pxlVal;
            }
            clumpsImage->GetRasterBand(1)->RasterIO(GF_Write, startX, startY, blockWidth, blockHeight, block, blockWidth, blockHeight, GDT_UInt32, 0, 0);
            delete[] block;
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
    }

    RSGISCreateImageGrid::~RSGISCreateImageGrid()
    {
        
    }
    
}}
