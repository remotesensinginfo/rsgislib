/*
 *  RSGISRasterAttUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2012.
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

#include "RSGISRasterAttUtils.h"

namespace rsgis{namespace rastergis{

    RSGISRasterAttUtils::RSGISRasterAttUtils()
    {

    }

    void RSGISRasterAttUtils::copyAttColumns(GDALDataset *inImage, GDALDataset *outImage, std::vector<std::string> fields, bool copyColours, bool copyHist, int ratBand) throw(RSGISAttributeTableException)
    {
        try
        {
            std::cout << "Open attribute table\n";
            GDALRasterAttributeTable *gdalAttIn = inImage->GetRasterBand(ratBand)->GetDefaultRAT();
            GDALRasterAttributeTable *gdalAttOut = NULL;
            GDALRasterAttributeTable *gdalAttOutTmp = outImage->GetRasterBand(ratBand)->GetDefaultRAT();

            int inRedIdx = 0;
            int inGreenIdx = 0;
            int inBlueIdx = 0;
            int inAlphaIdx = 0;
            int inHistIndx = 0;

            int outRedIdx = 0;
            int outGreenIdx = 0;
            int outBlueIdx = 0;
            int outAlphaIdx = 0;
            int outHistIndx = 0;

            if((gdalAttOutTmp == NULL) || (gdalAttOutTmp->GetRowCount() == 0))
            {
                std::cout << "Creating new attribute table " << std::endl;

                gdalAttOut = new GDALDefaultRasterAttributeTable();
            }
            else
            {
                std::cout << "Using existing attribute table " << std::endl;
                gdalAttOut = gdalAttOutTmp;
            }

            if(gdalAttIn == NULL)
            {
                rsgis::RSGISAttributeTableException("The input image does not have an attribute table.");
            }

            if(gdalAttIn->GetRowCount() > gdalAttOut->GetRowCount())
            {
                gdalAttOut->SetRowCount(gdalAttIn->GetRowCount());
            }

            std::cout << "Find field column indexes and create required columns.\n";
            int *colInIdxs = new int[fields.size()];
            int *colOutIdxs = new int[fields.size()];
            for(size_t i = 0; i < fields.size(); ++i)
            {
                colInIdxs[i] = 0;
                colOutIdxs[i] = 0;
            }

            if(gdalAttIn->GetRowCount() == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
            }

            for(size_t j = 0; j < fields.size(); ++j)
            {
                colInIdxs[j] = findColumnIndex(gdalAttIn, fields.at(j));
            }

            for(size_t j = 0; j < fields.size(); ++j)
            {
                if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                {
                    colOutIdxs[j] = findColumnIndexOrCreate(gdalAttOut, fields.at(j), GFT_Integer, gdalAttIn->GetUsageOfCol(colInIdxs[j]));
                }
                else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                {
                    colOutIdxs[j] = findColumnIndexOrCreate(gdalAttOut, fields.at(j), GFT_Real, gdalAttIn->GetUsageOfCol(colInIdxs[j]));
                }
                else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                {
                    colOutIdxs[j] = findColumnIndexOrCreate(gdalAttOut, fields.at(j), GFT_String, gdalAttIn->GetUsageOfCol(colInIdxs[j]));
                }
            }

            if(copyColours)
            {
                try
                {
                    inRedIdx = findColumnIndex(gdalAttIn,"Red");
                    inGreenIdx = findColumnIndex(gdalAttIn,"Green");
                    inBlueIdx = findColumnIndex(gdalAttIn,"Blue");
                    inAlphaIdx = findColumnIndex(gdalAttIn,"Alpha");
                }
                catch (rsgis::RSGISAttributeTableException &e)
                {
                    copyColours = false;
                }

                // Get or create column indies for colour columns
                outRedIdx = findColumnIndexOrCreate(gdalAttOut, "Red", GFT_Integer, GFU_Red);
                outGreenIdx = findColumnIndexOrCreate(gdalAttOut, "Green", GFT_Integer, GFU_Green);
                outBlueIdx = findColumnIndexOrCreate(gdalAttOut, "Blue", GFT_Integer, GFU_Blue);
                outAlphaIdx = findColumnIndexOrCreate(gdalAttOut, "Alpha", GFT_Integer, GFU_Alpha);
            }

            if(copyHist)
            {
                try
                {
                    inHistIndx = findColumnIndex(gdalAttIn,"Histogram");
                }
                catch (rsgis::RSGISAttributeTableException &e)
                {
                    copyHist = false;
                }
                outHistIndx = findColumnIndexOrCreate(gdalAttOut, "Histogram", GFT_Integer, GFU_PixelCount);
            }

            std::cout << "Copying columns to the new attribute table\n";
            // Allocate arrays to store blocks of data
            int nRows = gdalAttIn->GetRowCount();

            int *blockDataInt = new int[RAT_BLOCK_LENGTH];
            double *blockDataReal = new double[RAT_BLOCK_LENGTH];
            char **blockDataStr = new char*[RAT_BLOCK_LENGTH];

            // Itterate through blocks
            int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );

            int feedback = nBlocks/10.0;
            int feedbackCounter = 0;

            if(feedback != 0){std::cout << "Started " << std::flush;}

            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;

                if((feedback != 0) && ((i % feedback) == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }

                for(size_t j = 0; j < fields.size(); ++j)
                {
                    // For each column read a block of data from the input RAT and write to the output RAT
                    if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                    {
                        gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                        gdalAttOut->ValuesIO(GF_Write, colOutIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                    {
                        gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataReal);
                        gdalAttOut->ValuesIO(GF_Write, colOutIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataReal);
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                    {
                        gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataStr);
                        gdalAttOut->ValuesIO(GF_Write, colOutIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataStr);
                    }
                    else
                    {
                        throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                    }
                }

                if(copyColours)
                {
                    // Red
                    gdalAttIn->ValuesIO(GF_Read, inRedIdx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                    gdalAttOut->ValuesIO(GF_Write, outRedIdx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);

                    // Green
                    gdalAttIn->ValuesIO(GF_Read, inGreenIdx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                    gdalAttOut->ValuesIO(GF_Write, outGreenIdx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);

                    // Blue
                    gdalAttIn->ValuesIO(GF_Read, inBlueIdx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                    gdalAttOut->ValuesIO(GF_Write, outBlueIdx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                }
                if(copyHist)
                {
                    gdalAttIn->ValuesIO(GF_Read, inHistIndx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                    gdalAttOut->ValuesIO(GF_Write, outHistIndx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                }

            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;

                for(size_t j = 0; j < fields.size(); ++j)
                {
                    // For each column read a block of data from the input RAT and write to the output RAT
                    if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                    {
                        gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, remainRows, blockDataInt);
                        gdalAttOut->ValuesIO(GF_Write, colOutIdxs[j], rowOffset, remainRows, blockDataInt);
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                    {
                        gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, remainRows, blockDataReal);
                        gdalAttOut->ValuesIO(GF_Write, colOutIdxs[j], rowOffset, remainRows, blockDataReal);
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                    {
                        gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, remainRows, blockDataStr);
                        gdalAttOut->ValuesIO(GF_Write, colOutIdxs[j], rowOffset, remainRows, blockDataStr);
                    }
                    else
                    {
                        throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                    }
                }
                if(copyColours)
                {
                    // Red
                    gdalAttIn->ValuesIO(GF_Read, inRedIdx, rowOffset, remainRows, blockDataInt);
                    gdalAttOut->ValuesIO(GF_Write, outRedIdx, rowOffset, remainRows, blockDataInt);

                    // Green
                    gdalAttIn->ValuesIO(GF_Read, inGreenIdx, rowOffset, remainRows, blockDataInt);
                    gdalAttOut->ValuesIO(GF_Write, outGreenIdx, rowOffset, remainRows, blockDataInt);

                    // Blue
                    gdalAttIn->ValuesIO(GF_Read, inBlueIdx, rowOffset, remainRows, blockDataInt);
                    gdalAttOut->ValuesIO(GF_Write, outBlueIdx, rowOffset, remainRows, blockDataInt);
                }
                if(copyHist)
                {
                    gdalAttIn->ValuesIO(GF_Read, inHistIndx, rowOffset, remainRows, blockDataInt);
                    gdalAttOut->ValuesIO(GF_Write, outHistIndx, rowOffset, remainRows, blockDataInt);
                }

            }
            if(feedback != 0){std::cout << ".Completed\n";}
            else{std::cout << "Completed\n";}

            // Tidy up
            delete[] blockDataInt;
            delete[] blockDataReal;
            delete[] blockDataStr;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
    }

    
    void RSGISRasterAttUtils::copyAttColumnsWithOff(GDALRasterAttributeTable *inRAT, GDALRasterAttributeTable *outRAT, std::vector<std::string> fields, unsigned int offset, bool ignoreFirstRow, bool copyColours, bool copyHist) throw(RSGISAttributeTableException)
    {
        if(inRAT == NULL)
        {
            rsgis::RSGISAttributeTableException("The input RAT is NULL.");
        }
        
        if(outRAT == NULL)
        {
            rsgis::RSGISAttributeTableException("The output RAT is NULL.");
        }
        
        std::cout << "Find field column indexes and create required columns.\n";
        int *colInIdxs = new int[fields.size()];
        int *colOutIdxs = new int[fields.size()];
        for(size_t i = 0; i < fields.size(); ++i)
        {
            colInIdxs[i] = 0;
            colOutIdxs[i] = 0;
        }
        
        if(inRAT->GetRowCount() == 0)
        {
            rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
        }
        
        for(size_t j = 0; j < fields.size(); ++j)
        {
            colInIdxs[j] = findColumnIndex(inRAT, fields.at(j));
        }
        
        for(size_t j = 0; j < fields.size(); ++j)
        {
            if(inRAT->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
            {
                colOutIdxs[j] = findColumnIndexOrCreate(outRAT, fields.at(j), GFT_Integer, inRAT->GetUsageOfCol(colInIdxs[j]));
            }
            else if(inRAT->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
            {
                colOutIdxs[j] = findColumnIndexOrCreate(outRAT, fields.at(j), GFT_Real, inRAT->GetUsageOfCol(colInIdxs[j]));
            }
            else if(inRAT->GetTypeOfCol(colInIdxs[j]) == GFT_String)
            {
                colOutIdxs[j] = findColumnIndexOrCreate(outRAT, fields.at(j), GFT_String, inRAT->GetUsageOfCol(colInIdxs[j]));
            }
        }
        
        int inRedIdx = 0;
        int inGreenIdx = 0;
        int inBlueIdx = 0;
        int inAlphaIdx = 0;
        int inHistIndx = 0;
        
        int outRedIdx = 0;
        int outGreenIdx = 0;
        int outBlueIdx = 0;
        int outAlphaIdx = 0;
        int outHistIndx = 0;
        
        if(copyColours)
        {
            try
            {
                inRedIdx = findColumnIndex(inRAT,"Red");
                inGreenIdx = findColumnIndex(inRAT,"Green");
                inBlueIdx = findColumnIndex(inRAT,"Blue");
                inAlphaIdx = findColumnIndex(inRAT,"Alpha");
            }
            catch (rsgis::RSGISAttributeTableException &e)
            {
                std::cerr << "No colour columns in input attribute table - skipping copying" << std::endl;
                copyColours = false;
            }
            
            // Get or create column indies for colour columns
            outRedIdx = findColumnIndexOrCreate(outRAT, "Red", GFT_Integer, GFU_Red);
            outGreenIdx = findColumnIndexOrCreate(outRAT, "Green", GFT_Integer, GFU_Green);
            outBlueIdx = findColumnIndexOrCreate(outRAT, "Blue", GFT_Integer, GFU_Blue);
            outAlphaIdx = findColumnIndexOrCreate(outRAT, "Alpha", GFT_Integer, GFU_Alpha);
        }
        
        if(copyHist)
        {
            try
            {
                inHistIndx = findColumnIndex(inRAT,"Histogram");
            }
            catch (rsgis::RSGISAttributeTableException &e)
            {
                std::cerr << "No histogram column in input attribute table - skipping copying" << std::endl;
                copyHist = false;
            }
            outHistIndx = findColumnIndexOrCreate(outRAT, "Histogram", GFT_Integer, GFU_PixelCount);
        }
        
        std::cout << "Copying columns to the new attribute table\n";
        // Allocate arrays to store blocks of data
        int nRows = inRAT->GetRowCount();
        if(ignoreFirstRow)
        {
            nRows = nRows - 1;
        }
        
        outRAT->SetRowCount(offset+nRows);
        
        int *blockDataInt = new int[RAT_BLOCK_LENGTH];
        double *blockDataReal = new double[RAT_BLOCK_LENGTH];
        char **blockDataStr = new char*[RAT_BLOCK_LENGTH];
        
        // Iterate through blocks
        int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
        int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
        
        int feedback = nBlocks/10.0;
        int feedbackCounter = 0;
        
        if(feedback != 0)
        {
            std::cout << "Started " << std::flush;
        }
        
        int rowOffset = 0;
        int rowOffsetOut = offset;
        for(int i = 0; i < nBlocks; i++)
        {
            rowOffset =  RAT_BLOCK_LENGTH * i;
            if(ignoreFirstRow)
            {
                rowOffset = rowOffset + 1;
            }
            rowOffsetOut = (RAT_BLOCK_LENGTH * i) + offset;
            
            if((feedback != 0) && ((i % feedback) == 0))
            {
                std::cout << "." << feedbackCounter << "." << std::flush;
                feedbackCounter = feedbackCounter + 10;
            }
            
            for(size_t j = 0; j < fields.size(); ++j)
            {
                // For each column read a block of data from the input RAT and write to the output RAT
                if(inRAT->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                {
                    inRAT->ValuesIO(GF_Read, colInIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                    outRAT->ValuesIO(GF_Write, colOutIdxs[j], rowOffsetOut, RAT_BLOCK_LENGTH, blockDataInt);
                }
                else if(inRAT->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                {
                    inRAT->ValuesIO(GF_Read, colInIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataReal);
                    outRAT->ValuesIO(GF_Write, colOutIdxs[j], rowOffsetOut, RAT_BLOCK_LENGTH, blockDataReal);
                }
                else if(inRAT->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                {
                    inRAT->ValuesIO(GF_Read, colInIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataStr);
                    outRAT->ValuesIO(GF_Write, colOutIdxs[j], rowOffsetOut, RAT_BLOCK_LENGTH, blockDataStr);
                }
                else
                {
                    throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                }
            }
            
            if(copyColours)
            {
                // Red
                inRAT->ValuesIO(GF_Read, inRedIdx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                outRAT->ValuesIO(GF_Write, outRedIdx, rowOffsetOut, RAT_BLOCK_LENGTH, blockDataInt);
                
                // Green
                inRAT->ValuesIO(GF_Read, inGreenIdx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                outRAT->ValuesIO(GF_Write, outGreenIdx, rowOffsetOut, RAT_BLOCK_LENGTH, blockDataInt);
                
                // Blue
                inRAT->ValuesIO(GF_Read, inBlueIdx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                outRAT->ValuesIO(GF_Write, outBlueIdx, rowOffsetOut, RAT_BLOCK_LENGTH, blockDataInt);
            }
            if(copyHist)
            {
                inRAT->ValuesIO(GF_Read, inHistIndx, rowOffset, RAT_BLOCK_LENGTH, blockDataInt);
                outRAT->ValuesIO(GF_Write, outHistIndx, rowOffsetOut, RAT_BLOCK_LENGTH, blockDataInt);
            }
            
        }
        if(remainRows > 0)
        {
            rowOffset =  RAT_BLOCK_LENGTH * nBlocks;
            if(ignoreFirstRow)
            {
                rowOffset = rowOffset + 1;
            }
            rowOffsetOut = (RAT_BLOCK_LENGTH * nBlocks) + offset;
            
            for(size_t j = 0; j < fields.size(); ++j)
            {
                // For each column read a block of data from the input RAT and write to the output RAT
                if(inRAT->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                {
                    inRAT->ValuesIO(GF_Read, colInIdxs[j], rowOffset, remainRows, blockDataInt);
                    outRAT->ValuesIO(GF_Write, colOutIdxs[j], rowOffsetOut, remainRows, blockDataInt);
                }
                else if(inRAT->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                {
                    inRAT->ValuesIO(GF_Read, colInIdxs[j], rowOffset, remainRows, blockDataReal);
                    outRAT->ValuesIO(GF_Write, colOutIdxs[j], rowOffsetOut, remainRows, blockDataReal);
                }
                else if(inRAT->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                {
                    inRAT->ValuesIO(GF_Read, colInIdxs[j], rowOffset, remainRows, blockDataStr);
                    outRAT->ValuesIO(GF_Write, colOutIdxs[j], rowOffsetOut, remainRows, blockDataStr);
                }
                else
                {
                    throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                }
            }
            
            if(copyColours)
            {
                // Red
                inRAT->ValuesIO(GF_Read, inRedIdx, rowOffset, remainRows, blockDataInt);
                outRAT->ValuesIO(GF_Write, outRedIdx, rowOffsetOut, remainRows, blockDataInt);
                
                // Green
                inRAT->ValuesIO(GF_Read, inGreenIdx, rowOffset, remainRows, blockDataInt);
                outRAT->ValuesIO(GF_Write, outGreenIdx, rowOffsetOut, remainRows, blockDataInt);
                
                // Blue
                inRAT->ValuesIO(GF_Read, inBlueIdx, rowOffset, remainRows, blockDataInt);
                outRAT->ValuesIO(GF_Write, outBlueIdx, rowOffsetOut, remainRows, blockDataInt);
            }
            
            if(copyHist)
            {
                inRAT->ValuesIO(GF_Read, inHistIndx, rowOffset, remainRows, blockDataInt);
                outRAT->ValuesIO(GF_Write, outHistIndx, rowOffsetOut, remainRows, blockDataInt);
            }
            
        }
        if(feedback != 0)
        {
            std::cout << ".Completed\n";
        }
        else
        {
            std::cout << "Completed\n";
        }

        
        // Tidy up
        delete[] blockDataInt;
        delete[] blockDataReal;
        delete[] blockDataStr;
    }
    
    void RSGISRasterAttUtils::copyColourForCats(GDALDataset *catsImage, GDALDataset *classImage, std::string classField) throw(RSGISAttributeTableException)
    {
        try
        {
            std::cout << "Import attribute tables to memory.\n";
            GDALRasterAttributeTable *gdalAttIn = catsImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *gdalAttClumps = NULL;//new GDALRasterAttributeTable(*outImage->GetRasterBand(1)->GetDefaultRAT());
            const GDALRasterAttributeTable *gdalAttClasses = classImage->GetRasterBand(1)->GetDefaultRAT();

            if((gdalAttIn == NULL) || (gdalAttIn->GetRowCount() == 0))
            {
                throw rsgis::RSGISAttributeTableException("The clumps image does not have an attribute table.");
            }
            else
            {
                gdalAttClumps = new GDALDefaultRasterAttributeTable(*((GDALDefaultRasterAttributeTable*)gdalAttIn));
            }

            if((gdalAttClasses == NULL) || (gdalAttClasses->GetRowCount() == 0))
            {
                throw rsgis::RSGISAttributeTableException("The classes image does not have an attribute table.");
            }

            std::cout << "Find field column indexes and created columns were required.\n";
            bool foundClassIdx = false;
            int classIdx = 0;
            bool inRedFound = false;
            int inRedIdx = 0;
            bool inGreenFound = false;
            int inGreenIdx = 0;
            bool inBlueFound = false;
            int inBlueIdx = 0;
            bool inAlphaFound = false;
            int inAlphaIdx = 0;

            bool outRedFound = false;
            int outRedIdx = 0;
            bool outGreenFound = false;
            int outGreenIdx = 0;
            bool outBlueFound = false;
            int outBlueIdx = 0;
            bool outAlphaFound = false;
            int outAlphaIdx = 0;


            if(gdalAttIn->GetRowCount() == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
            }
            else
            {
                for(int i = 0; i < gdalAttClasses->GetColumnCount(); ++i)
                {
                    if(!inRedFound && (std::string(gdalAttClasses->GetNameOfCol(i)) == "Red"))
                    {
                        inRedIdx = i;
                        inRedFound = true;
                    }
                    else if(!inGreenFound && (std::string(gdalAttClasses->GetNameOfCol(i)) == "Green"))
                    {
                        inGreenIdx = i;
                        inGreenFound = true;
                    }
                    else if(!inBlueFound && (std::string(gdalAttClasses->GetNameOfCol(i)) == "Blue"))
                    {
                        inBlueIdx = i;
                        inBlueFound = true;
                    }
                    else if(!inAlphaFound && (std::string(gdalAttClasses->GetNameOfCol(i)) == "Alpha"))
                    {
                        inAlphaIdx = i;
                        inAlphaFound = true;
                    }
                }

                if(!inRedFound)
                {
                    throw rsgis::RSGISAttributeTableException("A \'Red\' column was not within the input classes table.");
                }

                if(!inGreenFound)
                {
                    throw rsgis::RSGISAttributeTableException("A \'Green\' column was not within the input classes table.");
                }

                if(!inBlueFound)
                {
                    throw rsgis::RSGISAttributeTableException("A \'Blue\' column was not within the input classes table.");
                }

                if(!inAlphaFound)
                {
                    throw rsgis::RSGISAttributeTableException("A \'Alpha\' column was not within the input classes table.");
                }


                for(int i = 0; i < gdalAttClumps->GetColumnCount(); ++i)
                {
                    if(!foundClassIdx && (std::string(gdalAttClumps->GetNameOfCol(i)) == classField))
                    {
                        classIdx = i;
                        foundClassIdx = true;
                    }
                    else if(!outRedFound && (std::string(gdalAttClumps->GetNameOfCol(i)) == "Red"))
                    {
                        outRedIdx = i;
                        outRedFound = true;
                    }
                    else if(!outGreenFound && (std::string(gdalAttClumps->GetNameOfCol(i)) == "Green"))
                    {
                        outGreenIdx = i;
                        outGreenFound = true;
                    }
                    else if(!outBlueFound && (std::string(gdalAttClumps->GetNameOfCol(i)) == "Blue"))
                    {
                        outBlueIdx = i;
                        outBlueFound = true;
                    }
                    else if(!outAlphaFound && (std::string(gdalAttClumps->GetNameOfCol(i)) == "Alpha"))
                    {
                        outAlphaIdx = i;
                        outAlphaFound = true;
                    }
                }

                if(!foundClassIdx)
                {
                    throw rsgis::RSGISAttributeTableException("The class field column was not within the category table.");
                }

                if(!outRedFound)
                {
                    gdalAttClumps->CreateColumn("Red", GFT_Integer, GFU_Red);
                    outRedFound = true;
                    outRedIdx = gdalAttClumps->GetColumnCount()-1;
                }

                if(!outGreenFound)
                {
                    gdalAttClumps->CreateColumn("Green", GFT_Integer, GFU_Green);
                    outGreenFound = true;
                    outGreenIdx = gdalAttClumps->GetColumnCount()-1;
                }

                if(!outBlueFound)
                {
                    gdalAttClumps->CreateColumn("Blue", GFT_Integer, GFU_Blue);
                    outBlueFound = true;
                    outBlueIdx = gdalAttClumps->GetColumnCount()-1;
                }

                if(!outAlphaFound)
                {
                    gdalAttClumps->CreateColumn("Alpha", GFT_Integer, GFU_Alpha);
                    outAlphaFound = true;
                    outAlphaIdx = gdalAttClumps->GetColumnCount()-1;
                }



            }

            int classID = 0;
            int redVal = 0;
            int greenVal = 0;
            int blueVal = 0;
            int alphaVal = 0;
            std::cout << "Copying the colours across\n";
            for(int i = 0; i < gdalAttClumps->GetRowCount(); ++i)
            {
                classID = gdalAttClumps->GetValueAsInt(i, classIdx);
                if(classID >= 0)
                {
                    redVal = gdalAttClasses->GetValueAsInt(classID, inRedIdx);
                    greenVal = gdalAttClasses->GetValueAsInt(classID, inGreenIdx);
                    blueVal = gdalAttClasses->GetValueAsInt(classID, inBlueIdx);
                    alphaVal = gdalAttClasses->GetValueAsInt(classID, inAlphaIdx);

                    gdalAttClumps->SetValue(i, outRedIdx, redVal);
                    gdalAttClumps->SetValue(i, outGreenIdx, greenVal);
                    gdalAttClumps->SetValue(i, outBlueIdx, blueVal);
                    gdalAttClumps->SetValue(i, outAlphaIdx, alphaVal);
                }
            }

            std::cout << "Adding RAT to output file.\n";
            catsImage->GetRasterBand(1)->SetDefaultRAT(gdalAttClumps);


        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
    }

    void RSGISRasterAttUtils::exportColumns2ASCII(GDALDataset *inImage, std::string outputFile, std::vector<std::string> fields, int ratBand) throw(RSGISAttributeTableException)
    {
        try
        {
            GDALRasterAttributeTable *gdalAttIn = inImage->GetRasterBand(ratBand)->GetDefaultRAT();

            if(gdalAttIn == NULL)
            {
                rsgis::RSGISAttributeTableException("The input image does not have an attribute table.");
            }
            if(gdalAttIn->GetRowCount() == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
            }

            std::cout << "Finding Attributes in RAT" << std::endl;
            // Find required attributes in RAT
            int *colInIdxs = new int[fields.size()];     // Index in RAT
            int *colBlockIndxs = new int[fields.size()]; // Index in array to store block (array dependent on type)

            for(size_t i = 0; i < fields.size(); ++i)
            {
                colInIdxs[i] = 0;
                colBlockIndxs[i] = 0;
            }

            unsigned int nIntCol = 0;
            unsigned int nRealCol = 0;
            unsigned int nStringCol = 0;

            for(size_t j = 0; j < fields.size(); ++j)
            {
                colInIdxs[j] = findColumnIndex(gdalAttIn, fields.at(j));

                if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                {
                    colBlockIndxs[j] = nIntCol;
                    ++nIntCol;
                }
                else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                {
                    colBlockIndxs[j] = nRealCol;
                    ++nRealCol;
                }
                else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                {
                    colBlockIndxs[j] = nStringCol;
                    ++nStringCol;
                }
            }

            // Allocate arrays to store blocks of data
            int nRows = gdalAttIn->GetRowCount();

            int **blockDataInt = new int*[nIntCol];
            for(int i = 0; i < nIntCol; ++i)
            {
                blockDataInt[i] = new int[RAT_BLOCK_LENGTH];
            }

            double **blockDataReal = new double*[nRealCol];
            for(int i = 0; i < nRealCol; ++i)
            {
                blockDataReal[i] = new double[RAT_BLOCK_LENGTH];
            }

            char ***blockDataStr = new char**[nStringCol];
            for(int i = 0; i < nStringCol; ++i)
            {
                blockDataStr[i] = new char*[RAT_BLOCK_LENGTH];
            }

            std::cout << "Copying columns to the ASCII file.\n";
            std::ofstream outFile;
            outFile.open(outputFile.c_str());
            unsigned int outFID = 0;
            if(outFile.is_open())
            {
                outFile.precision(12);

                // Write column headings
                outFile << "FID";
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    outFile << ",";
                    outFile << gdalAttIn->GetNameOfCol(colInIdxs[j]);
                }
                outFile << std::endl;

                // Itterate through blocks
                int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
                int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );

                int feedback = nRows/10.0;
                int feedbackCounter = 0;

                if(feedback != 0){std::cout << "Started " << std::flush;}

                int rowOffset = 0;
                for(int i = 0; i < nBlocks; i++)
                {
                    rowOffset =  RAT_BLOCK_LENGTH * i;

                    // Get block of data from RAT
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                        {
                            gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataInt[colBlockIndxs[j]]);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                        {
                            gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataReal[colBlockIndxs[j]]);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                        {
                            gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, RAT_BLOCK_LENGTH, blockDataStr[colBlockIndxs[j]]);
                        }
                    }

                    // Loop through block

                    for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                    {
                        // Show progress
                        if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }
                        
                        outFile << outFID; // Write out FID
                        
                        for(size_t j = 0; j < fields.size(); ++j)
                        {
                            outFile << ",";
                                
                            if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                            {
                                outFile << blockDataInt[colBlockIndxs[j]][m];
                            }
                            else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                            {
                                outFile << floor((blockDataReal[colBlockIndxs[j]][m]*1000)+0.5) / 1000;
                            }
                            else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                            {
                                outFile << "\"" << blockDataStr[colBlockIndxs[j]][m] << "\"" ;
                            }
                            else
                            {
                                throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                            }
                        }
                        outFile << std::endl;
                        ++outFID;
                    }

                }
                if(remainRows > 0)
                {
                    rowOffset =  RAT_BLOCK_LENGTH * nBlocks;

                    // Get block of data from RAT
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                        {
                            gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, remainRows, blockDataInt[colBlockIndxs[j]]);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                        {
                            gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, remainRows, blockDataReal[colBlockIndxs[j]]);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                        {
                            gdalAttIn->ValuesIO(GF_Read, colInIdxs[j], rowOffset, remainRows, blockDataStr[colBlockIndxs[j]]);
                        }
                    }

                    // Loop through block

                    for(int m = 0; m < remainRows; ++m)
                    {
                        // Show progress
                        if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                        {
                            std::cout << "." << feedbackCounter << "." << std::flush;
                            feedbackCounter = feedbackCounter + 10;
                        }

                        outFile << outFID; // Write out FID
                        
                        for(size_t j = 0; j < fields.size(); ++j)
                        {

                            outFile << ",";

                            if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                            {
                                outFile << blockDataInt[colBlockIndxs[j]][m];
                            }
                            else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                            {
                                outFile << floor((blockDataReal[colBlockIndxs[j]][m]* 1000)+0.5) / 1000;
                            }
                            else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                            {
                                outFile << "\"" << blockDataStr[colBlockIndxs[j]][m] << "\"" ;
                            }
                            else
                            {
                                throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                            }
                        }
                        outFile << std::endl;
                        ++outFID;
                    }
                }
                if(feedback != 0){std::cout << ".Completed\n";}
                else{std::cout << "Completed\n";}

                outFile.flush();
                outFile.close();

            }
            else
            {
                rsgis::RSGISAttributeTableException("Could not open the specified output ASCII file.");
            }

            // Tidy up
            for(int i = 0; i < nIntCol; ++i)
            {
                delete[] blockDataInt[i];
            }
            delete[] blockDataInt;

            for(int i = 0; i < nRealCol; ++i)
            {
                delete[] blockDataReal[i];
            }
            delete[] blockDataReal;

            for(int i = 0; i < nStringCol; ++i)
            {
                delete[] blockDataStr[i];
            }
            delete[] blockDataStr;

            delete[] colInIdxs;
            delete[] colBlockIndxs;
        }
        catch (rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
    }

    void RSGISRasterAttUtils::translateClasses(GDALDataset *inImage, std::string classInField, std::string classOutField, std::map<size_t, size_t> classPairs) throw(RSGISAttributeTableException)
    {
        try
        {
            std::cout << "Import attribute table to memory.\n";
            const GDALRasterAttributeTable *gdalAttInTmp = inImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *gdalAttIn = NULL;

            if((gdalAttInTmp == NULL) || (gdalAttInTmp->GetRowCount() == 0))
            {
                throw rsgis::RSGISAttributeTableException("The clumps image does not have an attribute table.");
            }
            else
            {
                gdalAttIn = new GDALDefaultRasterAttributeTable(*((GDALDefaultRasterAttributeTable*)gdalAttInTmp));
            }



            std::cout << "Find field column indexes in RAT.\n";
            bool foundInClassIdx = false;
            int colInClassIdx = 0;
            bool foundOutClassIdx = false;
            int colOutClassIdx = 0;

            if(gdalAttIn->GetRowCount() == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
            }
            else
            {
                for(int i = 0; i < gdalAttIn->GetColumnCount(); ++i)
                {
                    if(!foundInClassIdx && (std::string(gdalAttIn->GetNameOfCol(i)) == classInField))
                    {
                        colInClassIdx = i;
                        foundInClassIdx = true;
                    }
                    else if(!foundOutClassIdx && (std::string(gdalAttIn->GetNameOfCol(i)) == classOutField))
                    {
                        colOutClassIdx = i;
                        foundOutClassIdx = true;
                    }
                }

                if(!foundInClassIdx)
                {
                    std::string message = std::string("Column ") + classInField + std::string(" is not within the input attribute table.");
                    throw rsgis::RSGISAttributeTableException(message);
                }

                if(!foundOutClassIdx)
                {
                    gdalAttIn->CreateColumn(classOutField.c_str(), GFT_Integer, GFU_Generic);
                    foundOutClassIdx = true;
                    colOutClassIdx = gdalAttIn->GetColumnCount()-1;
                }
            }

            std::cout << "Translating class IDs.\n";
            size_t numRows = gdalAttIn->GetRowCount();
            size_t inClassID = 0;
            int outClassID = 0;
            unsigned int feedbackStep = numRows/10;
            unsigned int feedback = 0;
            std::cout << "Started." << std::flush;
            for(int i = 0; i < numRows; ++i)
            {
                if((numRows > 20) && (i % feedbackStep == 0))
                {
                    std::cout << "." << feedback << "." << std::flush;
                    feedback += 10;
                }

                inClassID = gdalAttIn->GetValueAsInt(i, colInClassIdx);

                std::map<size_t, size_t>::iterator iterClass = classPairs.find(inClassID);
                if(iterClass == classPairs.end())
                {
                    outClassID = -1;
                }
                else
                {
                    outClassID = (int)(*iterClass).second;
                }

                gdalAttIn->SetValue(i, colOutClassIdx, outClassID);

            }
            std::cout << ".Completed\n";

            std::cout << "Adding RAT to output file.\n";
            inImage->GetRasterBand(1)->SetDefaultRAT(gdalAttIn);

        }
        catch (rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
    }

    void RSGISRasterAttUtils::applyClassColours(GDALDataset *inImage, std::string classInField, std::map<size_t, rsgis::utils::RSGISColourInt> classColoursPairs, int ratBand) throw(RSGISAttributeTableException)
    {
        std::cout << "Import attribute table to memory.\n";
        GDALRasterAttributeTable *gdalAttInTmp = inImage->GetRasterBand(ratBand)->GetDefaultRAT();
        GDALRasterAttributeTable *gdalAttIn = NULL;

        if((gdalAttInTmp == NULL) || (gdalAttInTmp->GetRowCount() == 0))
        {
            std::cout << "Creating new attribute table " << std::endl;
            gdalAttIn = new GDALDefaultRasterAttributeTable();
        }
        else
        {
            std::cout << "Using existing attribute table " << std::endl;
            gdalAttIn = gdalAttInTmp;
        }

        std::cout << "Find field column indexes in RAT.\n";
        int colInClassIdx = 0;
        int outRedIdx = 0;
        int outGreenIdx = 0;
        int outBlueIdx = 0;
        int outAlphaIdx = 0;

        if(gdalAttIn->GetRowCount() == 0)
        {
            rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
        }
        else
        {
            // Get column index for in class
            colInClassIdx = findColumnIndex(gdalAttIn, classInField);

            // Get or create column indies fr colour columns
            outRedIdx = findColumnIndexOrCreate(gdalAttIn, "Red", GFT_Integer, GFU_Red);
            outGreenIdx = findColumnIndexOrCreate(gdalAttIn, "Green", GFT_Integer, GFU_Green);
            outBlueIdx = findColumnIndexOrCreate(gdalAttIn, "Blue", GFT_Integer, GFU_Blue);
            outAlphaIdx = findColumnIndexOrCreate(gdalAttIn, "Alpha", GFT_Integer, GFU_Alpha);
        }

        std::cout << "Applying colour to class IDs.\n";
        size_t nRows = gdalAttIn->GetRowCount();
        size_t inClassID = 0;

        int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
        int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );

        int feedback = nRows/10.0;
        int feedbackCounter = 0;

        // Allocate memory for blocks
        int *inBlockData = new int[RAT_BLOCK_LENGTH];
        int *outBlockRed = new int[RAT_BLOCK_LENGTH];
        int *outBlockGreen = new int[RAT_BLOCK_LENGTH];
        int *outBlockBlue = new int[RAT_BLOCK_LENGTH];
        int *outBlockAlpha = new int[RAT_BLOCK_LENGTH];

        if(feedback != 0){std::cout << "Started " << std::flush;}

        int rowOffset = 0;
        for(int i = 0; i < nBlocks; i++)
        {
            rowOffset =  RAT_BLOCK_LENGTH * i;

            // Read in block
            gdalAttIn->ValuesIO(GF_Read, colInClassIdx, rowOffset, RAT_BLOCK_LENGTH, inBlockData);

            // Loop through block
            for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
            {
                // Show progress
                if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }

                inClassID = inBlockData[m];

                std::map<size_t, rsgis::utils::RSGISColourInt>::iterator iterClass = classColoursPairs.find(inClassID);
                if(iterClass == classColoursPairs.end())
                {
                    outBlockRed[m] = 0;
                    outBlockGreen[m] = 0;
                    outBlockBlue[m] = 0;
                    outBlockAlpha[m] = 0;
                }
                else
                {
                    outBlockRed[m] = (*iterClass).second.getRed();
                    outBlockGreen[m] = (*iterClass).second.getGreen();
                    outBlockBlue[m] = (*iterClass).second.getBlue();
                    outBlockAlpha[m] = (*iterClass).second.getAlpha();
                }
            }

            // Write out blocks
            gdalAttIn->ValuesIO(GF_Write, outRedIdx, rowOffset, RAT_BLOCK_LENGTH, outBlockRed);
            gdalAttIn->ValuesIO(GF_Write, outGreenIdx, rowOffset, RAT_BLOCK_LENGTH, outBlockGreen);
            gdalAttIn->ValuesIO(GF_Write, outBlueIdx, rowOffset, RAT_BLOCK_LENGTH, outBlockBlue);
            gdalAttIn->ValuesIO(GF_Write, outAlphaIdx, rowOffset, RAT_BLOCK_LENGTH, outBlockAlpha);

        }
        if(remainRows > 0)
        {
            rowOffset =  RAT_BLOCK_LENGTH * nBlocks;

            // Read in block
            gdalAttIn->ValuesIO(GF_Read, colInClassIdx, rowOffset, remainRows, inBlockData);

            // Loop through block
            for(int m = 0; m < remainRows; ++m)
            {
                // Show progress
                if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }

                inClassID = inBlockData[m];

                std::map<size_t, rsgis::utils::RSGISColourInt>::iterator iterClass = classColoursPairs.find(inClassID);
                if(iterClass == classColoursPairs.end())
                {
                    outBlockRed[m] = 0;
                    outBlockGreen[m] = 0;
                    outBlockBlue[m] = 0;
                    outBlockAlpha[m] = 0;
                }
                else
                {
                    outBlockRed[m] = (*iterClass).second.getRed();
                    outBlockGreen[m] = (*iterClass).second.getGreen();
                    outBlockBlue[m] = (*iterClass).second.getBlue();
                    outBlockAlpha[m] = (*iterClass).second.getAlpha();
                }
            }

            // Write out blocks
            gdalAttIn->ValuesIO(GF_Write, outRedIdx, rowOffset, remainRows, outBlockRed);
            gdalAttIn->ValuesIO(GF_Write, outGreenIdx, rowOffset, remainRows, outBlockGreen);
            gdalAttIn->ValuesIO(GF_Write, outBlueIdx, rowOffset, remainRows, outBlockBlue);
            gdalAttIn->ValuesIO(GF_Write, outAlphaIdx, rowOffset, remainRows, outBlockAlpha);
        }

        if(feedback != 0){std::cout << ".Completed\n";}
        else{std::cout << "Completed\n";}

        std::cout << "Adding RAT to output file.\n";
        inImage->GetRasterBand(ratBand)->SetDefaultRAT(gdalAttIn);

        // Tidy up
        delete[] inBlockData;
        delete[] outBlockRed;
        delete[] outBlockGreen;
        delete[] outBlockBlue;
        delete[] outBlockAlpha;

    }

    void RSGISRasterAttUtils::applyClassStrColours(GDALDataset *inImage, std::string classInField, std::map<std::string, rsgis::utils::RSGISColourInt> classColoursPairs, int ratBand) throw(RSGISAttributeTableException)
    {

        std::cout << "Import attribute table to memory.\n";
        GDALRasterAttributeTable *gdalAttInTmp = inImage->GetRasterBand(ratBand)->GetDefaultRAT();
        GDALRasterAttributeTable *gdalAttIn = NULL;

        if((gdalAttInTmp == NULL) || (gdalAttInTmp->GetRowCount() == 0))
        {
            std::cout << "Creating new attribute table " << std::endl;
            gdalAttIn = new GDALDefaultRasterAttributeTable();
        }
        else
        {
            std::cout << "Using existing attribute table " << std::endl;
            gdalAttIn = gdalAttInTmp;
        }

        std::cout << "Find field column indexes in RAT.\n";
        int colInClassIdx = 0;
        int outRedIdx = 0;
        int outGreenIdx = 0;
        int outBlueIdx = 0;
        int outAlphaIdx = 0;

        if(gdalAttIn->GetRowCount() == 0)
        {
            rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
        }
        else
        {
            // Get column index for in class
            colInClassIdx = findColumnIndex(gdalAttIn, classInField);

            // Get or create column indies fr colour columns
            outRedIdx = findColumnIndexOrCreate(gdalAttIn, "Red", GFT_Integer, GFU_Red);
            outGreenIdx = findColumnIndexOrCreate(gdalAttIn, "Green", GFT_Integer, GFU_Green);
            outBlueIdx = findColumnIndexOrCreate(gdalAttIn, "Blue", GFT_Integer, GFU_Blue);
            outAlphaIdx = findColumnIndexOrCreate(gdalAttIn, "Alpha", GFT_Integer, GFU_Alpha);
        }

        std::cout << "Applying colour to class names.\n";
        size_t nRows = gdalAttIn->GetRowCount();
        std::string inClassName = "";

        int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
        int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );

        int feedback = nRows/10.0;
        int feedbackCounter = 0;

        // Allocate memory for blocks
        char **inBlockData = new char*[RAT_BLOCK_LENGTH];
        int *outBlockRed = new int[RAT_BLOCK_LENGTH];
        int *outBlockGreen = new int[RAT_BLOCK_LENGTH];
        int *outBlockBlue = new int[RAT_BLOCK_LENGTH];
        int *outBlockAlpha = new int[RAT_BLOCK_LENGTH];

        if(feedback != 0){std::cout << "Started " << std::flush;}

        int rowOffset = 0;
        for(int i = 0; i < nBlocks; i++)
        {
            rowOffset =  RAT_BLOCK_LENGTH * i;

            // Read in block
            gdalAttIn->ValuesIO(GF_Read, colInClassIdx, rowOffset, RAT_BLOCK_LENGTH, inBlockData);

            // Loop through block
            for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
            {
                // Show progress
                if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }

                inClassName = inBlockData[m];

                std::map<std::string, rsgis::utils::RSGISColourInt>::iterator iterClass = classColoursPairs.find(inClassName);
                if(iterClass == classColoursPairs.end())
                {
                    outBlockRed[m] = 0;
                    outBlockGreen[m] = 0;
                    outBlockBlue[m] = 0;
                    outBlockAlpha[m] = 0;
                }
                else
                {
                    outBlockRed[m] = (*iterClass).second.getRed();
                    outBlockGreen[m] = (*iterClass).second.getGreen();
                    outBlockBlue[m] = (*iterClass).second.getBlue();
                    outBlockAlpha[m] = (*iterClass).second.getAlpha();
                }
            }

            // Write out blocks
            gdalAttIn->ValuesIO(GF_Write, outRedIdx, rowOffset, RAT_BLOCK_LENGTH, outBlockRed);
            gdalAttIn->ValuesIO(GF_Write, outGreenIdx, rowOffset, RAT_BLOCK_LENGTH, outBlockGreen);
            gdalAttIn->ValuesIO(GF_Write, outBlueIdx, rowOffset, RAT_BLOCK_LENGTH, outBlockBlue);
            gdalAttIn->ValuesIO(GF_Write, outAlphaIdx, rowOffset, RAT_BLOCK_LENGTH, outBlockAlpha);

        }
        if(remainRows > 0)
        {
            rowOffset =  RAT_BLOCK_LENGTH * nBlocks;

            // Read in block
            gdalAttIn->ValuesIO(GF_Read, colInClassIdx, rowOffset, remainRows, inBlockData);

            // Loop through block
            for(int m = 0; m < remainRows; ++m)
            {
                // Show progress
                if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }

                inClassName = inBlockData[m];

                std::map<std::string, rsgis::utils::RSGISColourInt>::iterator iterClass = classColoursPairs.find(inClassName);
                if(iterClass == classColoursPairs.end())
                {
                    outBlockRed[m] = 0;
                    outBlockGreen[m] = 0;
                    outBlockBlue[m] = 0;
                    outBlockAlpha[m] = 0;
                }
                else
                {
                    outBlockRed[m] = (*iterClass).second.getRed();
                    outBlockGreen[m] = (*iterClass).second.getGreen();
                    outBlockBlue[m] = (*iterClass).second.getBlue();
                    outBlockAlpha[m] = (*iterClass).second.getAlpha();
                }
            }

            // Write out blocks
            gdalAttIn->ValuesIO(GF_Write, outRedIdx, rowOffset, remainRows, outBlockRed);
            gdalAttIn->ValuesIO(GF_Write, outGreenIdx, rowOffset, remainRows, outBlockGreen);
            gdalAttIn->ValuesIO(GF_Write, outBlueIdx, rowOffset, remainRows, outBlockBlue);
            gdalAttIn->ValuesIO(GF_Write, outAlphaIdx, rowOffset, remainRows, outBlockAlpha);
        }

        if(feedback != 0){std::cout << ".Completed\n";}
        else{std::cout << "Completed\n";}

        std::cout << "Adding RAT to output file.\n";
        inImage->GetRasterBand(ratBand)->SetDefaultRAT(gdalAttIn);

        // Tidy up
        delete[] inBlockData;
        delete[] outBlockRed;
        delete[] outBlockGreen;
        delete[] outBlockBlue;
        delete[] outBlockAlpha;

    }

    unsigned int RSGISRasterAttUtils::findColumnIndex(const GDALRasterAttributeTable *gdalATT, std::string colName) throw(RSGISAttributeTableException)
    {
        int numColumns = gdalATT->GetColumnCount();
        bool foundCol = false;
        unsigned int colIdx = 0;
        for(int i = 0; i < numColumns; ++i)
        {
            if(std::string(gdalATT->GetNameOfCol(i)) == colName)
            {
                foundCol = true;
                colIdx = i;
                break;
            }
        }

        if(!foundCol)
        {
            std::string message = std::string("The column ") + colName + std::string(" could not be found.");
            throw RSGISAttributeTableException(message);
        }

        return colIdx;
    }

    unsigned int RSGISRasterAttUtils::findColumnIndexOrCreate(GDALRasterAttributeTable *gdalATT, std::string colName, GDALRATFieldType dType, GDALRATFieldUsage dUsage) throw(RSGISAttributeTableException)
    {
        int numColumns = gdalATT->GetColumnCount();
        bool foundCol = false;
        unsigned int colIdx = 0;
        for(int i = 0; i < numColumns; ++i)
        {
            if(std::string(gdalATT->GetNameOfCol(i)) == colName)
            {
                foundCol = true;
                colIdx = i;
                break;
            }
        }

        if(!foundCol)
        {
            gdalATT->CreateColumn(colName.c_str(), dType, dUsage);
            colIdx = numColumns;
        }

        return colIdx;
    }

    double RSGISRasterAttUtils::readDoubleColumnVal(const GDALRasterAttributeTable *gdalATT, std::string colName, unsigned int row) throw(RSGISAttributeTableException)
    {
        double val = 0.0;
        try
        {
            unsigned int colIdx = this->findColumnIndex(gdalATT, colName);

            if(row > gdalATT->GetRowCount())
            {
                throw RSGISAttributeTableException("Row is not within the RAT.");
            }

            val = gdalATT->GetValueAsDouble(row, colIdx);

        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }

        return val;
    }

    long RSGISRasterAttUtils::readIntColumnVal(const GDALRasterAttributeTable *gdalATT, std::string colName, unsigned int row) throw(RSGISAttributeTableException)
    {
        long val = 0;
        try
        {
            unsigned int colIdx = this->findColumnIndex(gdalATT, colName);

            if(row > gdalATT->GetRowCount())
            {
                throw RSGISAttributeTableException("Row is not within the RAT.");
            }

            val = gdalATT->GetValueAsInt(row, colIdx);

        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }

        return val;
    }

    std::string RSGISRasterAttUtils::readStringColumnVal(const GDALRasterAttributeTable *gdalATT, std::string colName, unsigned int row) throw(RSGISAttributeTableException)
    {
        std::string val = "";
        try
        {
            unsigned int colIdx = this->findColumnIndex(gdalATT, colName);

            if(row > gdalATT->GetRowCount())
            {
                throw RSGISAttributeTableException("Row is not within the RAT.");
            }

            val = std::string(gdalATT->GetValueAsString(row, colIdx));

        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }

        return val;
    }


    double* RSGISRasterAttUtils::readDoubleColumn(GDALRasterAttributeTable *attTable, std::string colName, size_t *colLen) throw(RSGISAttributeTableException)
    {
        double *outData = NULL;
        try
        {
            int columnIndex = this->findColumnIndex(attTable, colName);

            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            *colLen = nRows;
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );

            outData = new double[nRows];

            double *blockData = new double[RAT_BLOCK_LENGTH];

            int feedback = nRows/10.0;
            int feedbackCounter = 0;

            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }

            if(feedback != 0){std::cout << "Started " << std::flush;}

            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;

                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);

                // Loop through block

                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData[(i*RAT_BLOCK_LENGTH)+m] = blockData[m];
                }

            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;

                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, remainRows, blockData);

                // Loop through block

                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData[(nBlocks*RAT_BLOCK_LENGTH)+m] = blockData[m];
                }
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }

        return outData;
    }

    int* RSGISRasterAttUtils::readIntColumn(GDALRasterAttributeTable *attTable, std::string colName, size_t *colLen) throw(RSGISAttributeTableException)
    {
        int *outData = NULL;
        try
        {
            int columnIndex = this->findColumnIndex(attTable, colName);

            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            *colLen = nRows;
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );

            outData = new int[nRows];

            int *blockData = new int[RAT_BLOCK_LENGTH];

            int feedback = nRows/10.0;
            int feedbackCounter = 0;

            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }

            if(feedback != 0){std::cout << "Started " << std::flush;}

            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;

                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);

                // Loop through block

                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData[(i*RAT_BLOCK_LENGTH)+m] = blockData[m];
                }

            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;

                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, remainRows, blockData);

                // Loop through block

                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData[(nBlocks*RAT_BLOCK_LENGTH)+m] = blockData[m];
                }
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }

        return outData;
    }

    char** RSGISRasterAttUtils::readStrColumn(GDALRasterAttributeTable *attTable, std::string colName, size_t *colLen) throw(RSGISAttributeTableException)
    {
        char **outData = NULL;
        try
        {
            int columnIndex = this->findColumnIndex(attTable, colName);

            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            *colLen = nRows;
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );

            outData = new char*[nRows];

            char **blockData = new char*[RAT_BLOCK_LENGTH];

            int feedback = nRows/10.0;
            int feedbackCounter = 0;

            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }

            if(feedback != 0){std::cout << "Started " << std::flush;}

            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;

                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);

                // Loop through block

                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData[(i*RAT_BLOCK_LENGTH)+m] = blockData[m];
                }

            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;

                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, remainRows, blockData);

                // Loop through block

                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData[(nBlocks*RAT_BLOCK_LENGTH)+m] = blockData[m];
                }
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        return outData;
    }
    
    std::string* RSGISRasterAttUtils::readStrColumnStdStr(GDALRasterAttributeTable *attTable, std::string colName, size_t *colLen) throw(RSGISAttributeTableException)
    {
        std::string *outData = NULL;
        try
        {
            int columnIndex = this->findColumnIndex(attTable, colName);
            
            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            *colLen = nRows;
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
            
            outData = new std::string[nRows];
            
            char **blockData = new char*[RAT_BLOCK_LENGTH];
            
            int feedback = nRows/10.0;
            int feedbackCounter = 0;
            
            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }
            
            if(feedback != 0){std::cout << "Started " << std::flush;}
            
            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;
                
                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);
                
                // Loop through block
                
                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData[(i*RAT_BLOCK_LENGTH)+m] = std::string(blockData[m]);
                }
                
            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;
                
                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, remainRows, blockData);
                
                // Loop through block
                
                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData[(nBlocks*RAT_BLOCK_LENGTH)+m] = std::string(blockData[m]);
                }
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return outData;
    }
    
    
    
    
    std::vector<double>* RSGISRasterAttUtils::readDoubleColumnAsVec(GDALRasterAttributeTable *attTable, std::string colName) throw(RSGISAttributeTableException)
    {
        std::vector<double> *outData = new std::vector<double>();
        try
        {
            int columnIndex = this->findColumnIndex(attTable, colName);
            
            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
            
            outData->reserve(nRows);
            
            double *blockData = new double[RAT_BLOCK_LENGTH];
            
            int feedback = nRows/10.0;
            int feedbackCounter = 0;
            
            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }
            
            if(feedback != 0){std::cout << "Started " << std::flush;}
            
            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;
                
                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);
                
                // Loop through block
                
                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData->push_back(blockData[m]);
                }
                
            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;
                
                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, remainRows, blockData);
                
                // Loop through block
                
                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData->push_back(blockData[m]);
                }
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return outData;
    }
    
    std::vector<int>* RSGISRasterAttUtils::readIntColumnAsVec(GDALRasterAttributeTable *attTable, std::string colName) throw(RSGISAttributeTableException)
    {
        std::vector<int> *outData = new std::vector<int>();
        try
        {
            int columnIndex = this->findColumnIndex(attTable, colName);
            
            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
            
            outData->reserve(nRows);
            
            int *blockData = new int[RAT_BLOCK_LENGTH];
            
            int feedback = nRows/10.0;
            int feedbackCounter = 0;
            
            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }
            
            if(feedback != 0){std::cout << "Started " << std::flush;}
            
            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;
                
                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);
                
                // Loop through block
                
                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData->push_back(blockData[m]);
                }
                
            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;
                
                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, remainRows, blockData);
                
                // Loop through block
                
                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData->push_back(blockData[m]);
                }
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return outData;
    }
    
    std::vector<std::string>* RSGISRasterAttUtils::readStrColumnAsVec(GDALRasterAttributeTable *attTable, std::string colName) throw(RSGISAttributeTableException)
    {
        std::vector<std::string> *outData = new std::vector<std::string>();
        try
        {
            int columnIndex = this->findColumnIndex(attTable, colName);
            
            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
            
            outData->reserve(nRows);
            
            char **blockData = new char*[RAT_BLOCK_LENGTH];
            
            int feedback = nRows/10.0;
            int feedbackCounter = 0;
            
            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }
            
            if(feedback != 0){std::cout << "Started " << std::flush;}
            
            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;
                
                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);
                
                // Loop through block
                
                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData->push_back(std::string(blockData[m]));
                }
                
            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;
                
                // Read block
                attTable->ValuesIO(GF_Read, columnIndex, rowOffset, remainRows, blockData);
                
                // Loop through block
                
                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    outData->push_back(std::string(blockData[m]));
                }
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return outData;
    }
    
    
    
    
    
    std::vector<std::vector<size_t>* >* RSGISRasterAttUtils::getRATNeighbours(GDALDataset *clumpImage, unsigned int ratBand) throw(RSGISAttributeTableException)
    {
        std::vector<std::vector<size_t>* > *neighbours = new std::vector<std::vector<size_t>* >();
        
        try
        {
            kealib::KEAImageIO *keaImgIO;
            void *internalData = clumpImage->GetInternalHandle("");
            if(internalData != NULL)
            {
                try
                {
                    keaImgIO = static_cast<kealib::KEAImageIO*>(internalData);
                    
                    if((keaImgIO == NULL) | (keaImgIO == 0))
                    {
                        throw RSGISAttributeTableException("Could not get hold of the internal KEA Image IO Object - is input image a KEA file?");
                    }
                }
                catch(RSGISAttributeTableException& e)
                {
                    throw e;
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    throw RSGISAttributeTableException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    throw RSGISAttributeTableException(e.what());
                }
            }
            else
            {
                throw RSGISAttributeTableException("Internal data on GDAL Dataset was NULL - check input file is KEA.");
            }
            
            kealib::KEAAttributeTable *keaAtt = keaImgIO->getAttributeTable(kealib::kea_att_file, ratBand);
            size_t numRows = keaAtt->getSize();
            neighbours->reserve(numRows);
            
            keaAtt->getNeighbours(0, numRows, neighbours);
            
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return neighbours;
    }
    
    
    void RSGISRasterAttUtils::writeStrColumn(GDALRasterAttributeTable *attTable, std::string colName, std::string *strDataVal, size_t colLen) throw(RSGISAttributeTableException)
    {
        try
        {
            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            
            if(nRows != colLen)
            {
                throw RSGISAttributeTableException("The column length provided and the length of the RAT are not equal...");
            }
            
            unsigned int columnIndex = this->findColumnIndexOrCreate(attTable, colName, GFT_String);
            
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
            
            char **blockData = new char*[RAT_BLOCK_LENGTH];
            
            int feedback = nRows/10.0;
            int feedbackCounter = 0;
            
            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }
            
            if(feedback != 0){std::cout << "Started " << std::flush;}
            
            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;
                
                // Loop through block
                
                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    blockData[m] = const_cast<char*>(strDataVal[(i*RAT_BLOCK_LENGTH)+m].c_str());
                }
                
                // Write block
                attTable->ValuesIO(GF_Write, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);
                
            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;
                
                // Loop through block
                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    blockData[m] = const_cast<char*>(strDataVal[(nBlocks*RAT_BLOCK_LENGTH)+m].c_str());
                }
                
                // Read block
                attTable->ValuesIO(GF_Write, columnIndex, rowOffset, remainRows, blockData);
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    std::vector<RSGISRATCol>* RSGISRasterAttUtils::getRatColumnsList(GDALRasterAttributeTable *gdalATT) throw(RSGISAttributeTableException)
    {
        std::vector<RSGISRATCol> *ratCols = new std::vector<RSGISRATCol>();
        try
        {
            unsigned int numCols = gdalATT->GetColumnCount();
            ratCols->reserve(numCols);
            
            RSGISRATCol colDetails = RSGISRATCol();
            for(unsigned int i = 0; i < numCols; ++i)
            {
                colDetails = RSGISRATCol();
                
                colDetails.name = std::string(gdalATT->GetNameOfCol(i));
                colDetails.idx = i;
                colDetails.usage = gdalATT->GetUsageOfCol(i);
                colDetails.type = gdalATT->GetTypeOfCol(i);
                                
                ratCols->push_back(colDetails);
            }
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return ratCols;
    }
    
    std::vector<RSGISRATCol>* RSGISRasterAttUtils::getVectorColumns(OGRLayer *layer, bool ignoreErr) throw(RSGISAttributeTableException)
    {
        std::vector<RSGISRATCol> *colNames = new std::vector<RSGISRATCol>();
        try
        {
            OGRFeatureDefn *ogrFeatDef = layer->GetLayerDefn();
            int numFields = ogrFeatDef->GetFieldCount();
            for(int i = 0; i < numFields; ++i)
            {
                OGRFieldDefn *fieldDef = ogrFeatDef->GetFieldDefn(i);
                
                RSGISRATCol ratCol;
                ratCol.name = std::string(fieldDef->GetNameRef());
                ratCol.usage = GFU_Generic;
                
                if(fieldDef->GetType() == OFTInteger)
                {
                    ratCol.type = GFT_Integer;
                }
                else if(fieldDef->GetType() == OFTReal)
                {
                    ratCol.type = GFT_Real;
                }
                else if(fieldDef->GetType() == OFTString)
                {
                    ratCol.type = GFT_String;
                }
                else
                {
                    std::cerr << ratCol.name << " data type could not be translated to a RAT data type.\n";
                    if(ignoreErr)
                    {
                        continue;
                    }
                    else
                    {
                        throw RSGISAttributeTableException("Data type could not be transferred to RAT.");
                    }
                }
                
                colNames->push_back(ratCol);
            }
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        return colNames;
    }
    

    void RSGISRasterAttUtils::writeIntColumn(GDALRasterAttributeTable *attTable, std::string colName, int *intDataVal, size_t colLen) throw(RSGISAttributeTableException)
    {
        try
        {
            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            
            if(nRows != colLen)
            {
                throw RSGISAttributeTableException("The column length provided and the length of the RAT are not equal...");
            }
            
            unsigned int columnIndex = this->findColumnIndexOrCreate(attTable, colName, GFT_Integer);
            
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
            
            int *blockData = new int[RAT_BLOCK_LENGTH];
            
            int feedback = nRows/10.0;
            int feedbackCounter = 0;
            
            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }
            
            if(feedback != 0){std::cout << "Started " << std::flush;}
            
            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;
                
                // Loop through block
                
                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    blockData[m] = intDataVal[(i*RAT_BLOCK_LENGTH)+m];
                }
                
                // Write block
                attTable->ValuesIO(GF_Write, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);
                
            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;
                
                // Loop through block
                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    blockData[m] = intDataVal[(nBlocks*RAT_BLOCK_LENGTH)+m];
                }
                
                // Read block
                attTable->ValuesIO(GF_Write, columnIndex, rowOffset, remainRows, blockData);
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }


    void RSGISRasterAttUtils::writeRealColumn(GDALRasterAttributeTable *attTable, std::string colName, double *realDataVal, size_t colLen) throw(RSGISAttributeTableException)
    {
        try
        {
            // Iterate through blocks
            size_t nRows = attTable->GetRowCount();
            
            if(nRows != colLen)
            {
                throw RSGISAttributeTableException("The column length provided and the length of the RAT are not equal...");
            }
            
            unsigned int columnIndex = this->findColumnIndexOrCreate(attTable, colName, GFT_Real);
            
            unsigned int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
            unsigned int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
            
            double *blockData = new double[RAT_BLOCK_LENGTH];
            
            int feedback = nRows/10.0;
            int feedbackCounter = 0;
            
            // Don't show progress if less rows than block length
            if(nRows < RAT_BLOCK_LENGTH)
            {
                feedback = 0;
            }
            
            if(feedback != 0){std::cout << "Started " << std::flush;}
            
            int rowOffset = 0;
            for(int i = 0; i < nBlocks; i++)
            {
                rowOffset =  RAT_BLOCK_LENGTH * i;
                
                // Loop through block
                
                for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    blockData[m] = realDataVal[(i*RAT_BLOCK_LENGTH)+m];
                }
                
                // Write block
                attTable->ValuesIO(GF_Write, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockData);
                
            }
            if(remainRows > 0)
            {
                rowOffset =  RAT_BLOCK_LENGTH * nBlocks;
                
                // Loop through block
                for(int m = 0; m < remainRows; ++m)
                {
                    // Show progress
                    if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    blockData[m] = realDataVal[(nBlocks*RAT_BLOCK_LENGTH)+m];
                }
                
                // Read block
                attTable->ValuesIO(GF_Write, columnIndex, rowOffset, remainRows, blockData);
            }
            if(feedback != 0){std::cout << ".Completed\n";}
            delete[] blockData;
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISRasterAttUtils::getImageBandMinMax(GDALDataset *inImage, int band, long *minVal, long *maxVal) throw(RSGISAttributeTableException)
    {
        try
        {
            RSGISCalcImgMinMax calcMinMac = RSGISCalcImgMinMax(minVal, maxVal, band-1);
            rsgis::img::RSGISCalcImage calcImageMinMax(&calcMinMac);
            calcImageMinMax.calcImage(&inImage, 1, 0);
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }

    RSGISRasterAttUtils::~RSGISRasterAttUtils()
    {

    }
    
    
    
    
    
    RSGISCalcImgMinMax::RSGISCalcImgMinMax(long *minVal, long *maxVal, unsigned int band):rsgis::img::RSGISCalcImageValue(0)
    {
        this->minVal = minVal;
        this->maxVal = maxVal;
        this->band = band;
        first = true;
    }
    
    void RSGISCalcImgMinMax::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException)
    {
        if(first)
        {
            *minVal = intBandValues[band];
            *maxVal = intBandValues[band];
            first = false;
        }
        else
        {
            if(intBandValues[band] < *minVal)
            {
                *minVal = intBandValues[band];
            }
            else if(intBandValues[band] > *maxVal)
            {
                *maxVal = intBandValues[band];
            }
        }
    }
    
    RSGISCalcImgMinMax::~RSGISCalcImgMinMax()
    {
        
    }
    
    

}}



