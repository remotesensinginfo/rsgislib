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
    
    void RSGISRasterAttUtils::copyAttColumns(GDALDataset *inImage, GDALDataset *outImage, std::vector<std::string> fields) throw(RSGISAttributeTableException)
    {
        try 
        {
            std::cout << "Import attribute tables to memory.\n";
            const GDALRasterAttributeTable *gdalAttIn = inImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *gdalAttOut = NULL;//new GDALRasterAttributeTable(*outImage->GetRasterBand(1)->GetDefaultRAT());
            const GDALRasterAttributeTable *gdalAttOutTmp = outImage->GetRasterBand(1)->GetDefaultRAT();
            
            if((gdalAttOutTmp == NULL) || (gdalAttOutTmp->GetRowCount() == 0))
            {
                gdalAttOut = new GDALDefaultRasterAttributeTable();
            }
            else
            {
                gdalAttOut = new GDALDefaultRasterAttributeTable(*((GDALDefaultRasterAttributeTable*)gdalAttOutTmp));
            }
            
            if(gdalAttIn == NULL)
            {
                rsgis::RSGISAttributeTableException("The input image does not have an attribute table.");
            }
            
            if(gdalAttIn->GetRowCount() > gdalAttOut->GetRowCount())
            {
                gdalAttOut->SetRowCount(gdalAttIn->GetRowCount());
            }
            
            std::cout << "Find field column indexes and created columns were required.\n";
            bool *foundInIdx = new bool[fields.size()];
            int *colInIdxs = new int[fields.size()];
            bool *foundOutIdx = new bool[fields.size()];
            int *colOutIdxs = new int[fields.size()];
            for(size_t i = 0; i < fields.size(); ++i)
            {
                foundInIdx[i] = false;
                colInIdxs[i] = 0;
                foundOutIdx[i] = false;
                colOutIdxs[i] = 0;
            }
            
            if(gdalAttIn->GetRowCount() == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
            }
            else
            {
                for(int i = 0; i < gdalAttIn->GetColumnCount(); ++i)
                {
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(!foundInIdx[j] && (std::string(gdalAttIn->GetNameOfCol(i)) == fields.at(j)))
                        {
                            colInIdxs[j] = i;
                            foundInIdx[j] = true;
                        }
                    }
                }
                
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(!foundInIdx[j])
                    {
                        std::string message = std::string("Column ") + fields.at(j) + std::string(" is not within the input attribute table.");
                        throw rsgis::RSGISAttributeTableException(message);
                    }
                }
                
                
                for(int i = 0; i < gdalAttOut->GetColumnCount(); ++i)
                {
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(!foundOutIdx[j] && (std::string(gdalAttOut->GetNameOfCol(i)) == fields.at(j)))
                        {
                            colOutIdxs[j] = i;
                            foundOutIdx[j] = true;
                        }
                    }
                }
                
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(!foundOutIdx[j])
                    {
                        if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                        {
                            gdalAttOut->CreateColumn(fields.at(j).c_str(), GFT_Integer, GFU_Generic);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                        {
                            gdalAttOut->CreateColumn(fields.at(j).c_str(), GFT_Real, GFU_Generic);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                        {
                            gdalAttOut->CreateColumn(fields.at(j).c_str(), GFT_String, GFU_Generic);
                        }
                        else
                        {
                            throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                        }
                        colOutIdxs[j] = gdalAttOut->GetColumnCount()-1;
                        foundOutIdx[j] = true;
                    }
                }
            }
            
            std::cout << "Copying columns to the new attribute table\n";
            for(int i = 0; i < gdalAttIn->GetRowCount(); ++i)
            {
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                    {
                        gdalAttOut->SetValue(i, colOutIdxs[j], gdalAttIn->GetValueAsInt(i, colInIdxs[j]));
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                    {
                        gdalAttOut->SetValue(i, colOutIdxs[j], gdalAttIn->GetValueAsDouble(i, colInIdxs[j]));
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                    {
                        gdalAttOut->SetValue(i, colOutIdxs[j], gdalAttIn->GetValueAsString(i, colInIdxs[j]));
                    }
                    else
                    {
                        throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                    }
                }
            }
            
            std::cout << "Adding RAT to output file.\n";
            outImage->GetRasterBand(1)->SetDefaultRAT(gdalAttOut);
            
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }        
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
        
            // Find required attributes in RAT
            bool *foundInIdx = new bool[fields.size()];
            int *colInIdxs = new int[fields.size()];     // Index in RAT
            int *colBlockIndxs = new int[fields.size()]; // Index in array to store block (array dependent on type)
            
            for(size_t i = 0; i < fields.size(); ++i)
            {
                foundInIdx[i] = false;
                colInIdxs[i] = 0;
                colBlockIndxs[i] = 0;
            }
            
            unsigned int nIntCol = 0;
            unsigned int nRealCol = 0;
            unsigned int nStringCol = 0;
            
            for(int i = 0; i < gdalAttIn->GetColumnCount(); ++i)
            {
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(!foundInIdx[j] && (std::string(gdalAttIn->GetNameOfCol(i)) == fields.at(j)))
                    {
                        colInIdxs[j] = i;
                        foundInIdx[j] = true;
                    
                        if(gdalAttIn->GetTypeOfCol(i) == GFT_Integer)
                        {
                            colBlockIndxs[j] = nIntCol;
                            ++nIntCol;
                        }
                        else if(gdalAttIn->GetTypeOfCol(i) == GFT_Real)
                        {
                            colBlockIndxs[j] = nRealCol;
                            ++nRealCol;
                        }
                        else if(gdalAttIn->GetTypeOfCol(i) == GFT_String)
                        {
                            colBlockIndxs[j] = nStringCol;
                            ++nStringCol;
                        }
                    }
                }
            }
            
            for(size_t j = 0; j < fields.size(); ++j)
            {
                if(!foundInIdx[j])
                {
                    std::string message = std::string("Column ") + fields.at(j) + std::string(" is not within the input attribute table.");
                    throw rsgis::RSGISAttributeTableException(message);
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
            if(outFile.is_open())
            {
                outFile.precision(12);
                
                // Write column headings
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(j != 0)
                    {
                        outFile << ",";
                    }
                    outFile << gdalAttIn->GetNameOfCol(colInIdxs[j]);
                }
                outFile << std::endl;
                
                // Itterate through blocks
                int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
                int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
                
                int feedback = nRows/10.0;
                int feedbackCounter = 0;
                
                std::cout << "Started " << std::flush;
                
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
                        
                        for(size_t j = 0; j < fields.size(); ++j)
                        {
                            if(j != 0)
                            {
                                outFile << ",";
                            }
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
                        
                        for(size_t j = 0; j < fields.size(); ++j)
                        {
                            if(j != 0)
                            {
                                outFile << ",";
                            }
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
                    }
                }
                std::cout << ".Completed\n";
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
            delete blockDataReal;
            
            for(int i = 0; i < nStringCol; ++i)
            {
                delete[] blockDataStr[i];
            }
            delete[] blockDataStr;
            
            delete[] foundInIdx;
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
    
    void RSGISRasterAttUtils::applyClassColours(GDALDataset *inImage, std::string classInField, std::map<size_t, rsgis::utils::RSGISColourInt> classColoursPairs) throw(RSGISAttributeTableException)
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
            for(int i = 0; i < gdalAttIn->GetColumnCount(); ++i)
            {
                if(!foundInClassIdx && (std::string(gdalAttIn->GetNameOfCol(i)) == classInField))
                {
                    colInClassIdx = i;
                    foundInClassIdx = true;
                }
                else if(!outRedFound && (std::string(gdalAttIn->GetNameOfCol(i)) == "Red"))
                {
                    outRedIdx = i;
                    outRedFound = true;
                }
                else if(!outGreenFound && (std::string(gdalAttIn->GetNameOfCol(i)) == "Green"))
                {
                    outGreenIdx = i;
                    outGreenFound = true;
                }
                else if(!outBlueFound && (std::string(gdalAttIn->GetNameOfCol(i)) == "Blue"))
                {
                    outBlueIdx = i;
                    outBlueFound = true;
                }
                else if(!outAlphaFound && (std::string(gdalAttIn->GetNameOfCol(i)) == "Alpha"))
                {
                    outAlphaIdx = i;
                    outAlphaFound = true;
                }
            }
            
            if(!foundInClassIdx)
            {
                std::string message = std::string("Column ") + classInField + std::string(" is not within the input attribute table.");
                throw rsgis::RSGISAttributeTableException(message);
            }
            
            if(!outRedFound)
            {
                gdalAttIn->CreateColumn("Red", GFT_Integer, GFU_Red);
                outRedFound = true;
                outRedIdx = gdalAttIn->GetColumnCount()-1;
            }
            
            if(!outGreenFound)
            {
                gdalAttIn->CreateColumn("Green", GFT_Integer, GFU_Green);
                outGreenFound = true;
                outGreenIdx = gdalAttIn->GetColumnCount()-1;
            }
            
            if(!outBlueFound)
            {
                gdalAttIn->CreateColumn("Blue", GFT_Integer, GFU_Blue);
                outBlueFound = true;
                outBlueIdx = gdalAttIn->GetColumnCount()-1;
            }
            
            if(!outAlphaFound)
            {
                gdalAttIn->CreateColumn("Alpha", GFT_Integer, GFU_Alpha);
                outAlphaFound = true;
                outAlphaIdx = gdalAttIn->GetColumnCount()-1;
            }
        }
        
        std::cout << "Applying colour to class IDs.\n";
        size_t numRows = gdalAttIn->GetRowCount();
        size_t inClassID = 0;
        int red = 0;
        int green = 0;
        int blue = 0;
        int alpha = 0;
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
            
            std::map<size_t, rsgis::utils::RSGISColourInt>::iterator iterClass = classColoursPairs.find(inClassID);
            if(iterClass == classColoursPairs.end())
            {
                red = 0;
                green = 0;
                blue = 0;
                alpha = 0;
            }
            else
            {
                red = (*iterClass).second.getRed();
                green = (*iterClass).second.getGreen();
                blue = (*iterClass).second.getBlue();
                alpha = (*iterClass).second.getAlpha();
            }
            
            gdalAttIn->SetValue(i, outRedIdx, red);
            gdalAttIn->SetValue(i, outGreenIdx, green);
            gdalAttIn->SetValue(i, outBlueIdx, blue);
            gdalAttIn->SetValue(i, outAlphaIdx, alpha);
        }
        std::cout << ".Completed\n";
        
        std::cout << "Adding RAT to output file.\n";
        inImage->GetRasterBand(1)->SetDefaultRAT(gdalAttIn);
        
    }
    
    void RSGISRasterAttUtils::applyClassStrColours(GDALDataset *inImage, std::string classInField, std::map<std::string, rsgis::utils::RSGISColourInt> classColoursPairs) throw(RSGISAttributeTableException)
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
            for(int i = 0; i < gdalAttIn->GetColumnCount(); ++i)
            {
                if(!foundInClassIdx && (std::string(gdalAttIn->GetNameOfCol(i)) == classInField))
                {
                    colInClassIdx = i;
                    foundInClassIdx = true;
                }
                else if(!outRedFound && (std::string(gdalAttIn->GetNameOfCol(i)) == "Red"))
                {
                    outRedIdx = i;
                    outRedFound = true;
                }
                else if(!outGreenFound && (std::string(gdalAttIn->GetNameOfCol(i)) == "Green"))
                {
                    outGreenIdx = i;
                    outGreenFound = true;
                }
                else if(!outBlueFound && (std::string(gdalAttIn->GetNameOfCol(i)) == "Blue"))
                {
                    outBlueIdx = i;
                    outBlueFound = true;
                }
                else if(!outAlphaFound && (std::string(gdalAttIn->GetNameOfCol(i)) == "Alpha"))
                {
                    outAlphaIdx = i;
                    outAlphaFound = true;
                }
            }
            
            if(!foundInClassIdx)
            {
                std::string message = std::string("Column ") + classInField + std::string(" is not within the input attribute table.");
                throw rsgis::RSGISAttributeTableException(message);
            }
            
            if(!outRedFound)
            {
                gdalAttIn->CreateColumn("Red", GFT_Integer, GFU_Red);
                outRedFound = true;
                outRedIdx = gdalAttIn->GetColumnCount()-1;
            }
            
            if(!outGreenFound)
            {
                gdalAttIn->CreateColumn("Green", GFT_Integer, GFU_Green);
                outGreenFound = true;
                outGreenIdx = gdalAttIn->GetColumnCount()-1;
            }
            
            if(!outBlueFound)
            {
                gdalAttIn->CreateColumn("Blue", GFT_Integer, GFU_Blue);
                outBlueFound = true;
                outBlueIdx = gdalAttIn->GetColumnCount()-1;
            }
            
            if(!outAlphaFound)
            {
                gdalAttIn->CreateColumn("Alpha", GFT_Integer, GFU_Alpha);
                outAlphaFound = true;
                outAlphaIdx = gdalAttIn->GetColumnCount()-1;
            }
        }
        
        std::cout << "Applying colour to class IDs.\n";
        size_t numRows = gdalAttIn->GetRowCount();
        std::string inClassName = 0;
        int red = 0;
        int green = 0;
        int blue = 0;
        int alpha = 0;
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
            
            inClassName = std::string(gdalAttIn->GetValueAsString(i, colInClassIdx));
            
            std::map<std::string, rsgis::utils::RSGISColourInt>::iterator iterClass = classColoursPairs.find(inClassName);
            if(iterClass == classColoursPairs.end())
            {
                red = 0;
                green = 0;
                blue = 0;
                alpha = 0;
            }
            else
            {
                red = (*iterClass).second.getRed();
                green = (*iterClass).second.getGreen();
                blue = (*iterClass).second.getBlue();
                alpha = (*iterClass).second.getAlpha();
            }
            
            gdalAttIn->SetValue(i, outRedIdx, red);
            gdalAttIn->SetValue(i, outGreenIdx, green);
            gdalAttIn->SetValue(i, outBlueIdx, blue);
            gdalAttIn->SetValue(i, outAlphaIdx, alpha);
        }
        std::cout << ".Completed\n";
        
        std::cout << "Adding RAT to output file.\n";
        inImage->GetRasterBand(1)->SetDefaultRAT(gdalAttIn);
        
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
    
    unsigned int RSGISRasterAttUtils::findColumnIndexOrCreate(GDALRasterAttributeTable *gdalATT, std::string colName, GDALRATFieldType dType) throw(RSGISAttributeTableException)
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
            gdalATT->CreateColumn(colName.c_str(), dType, GFU_Generic);
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
    
    RSGISRasterAttUtils::~RSGISRasterAttUtils()
    {
        
    }
	
}}



