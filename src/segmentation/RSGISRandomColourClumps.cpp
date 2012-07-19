/*
 *  RSGISRandomColourClumps.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/01/2012.
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

#include "RSGISRandomColourClumps.h"

namespace rsgis{namespace segment{
    
    RSGISRandomColourClumps::RSGISRandomColourClumps()
    {
        
    }
        
    void RSGISRandomColourClumps::generateRandomColouredClump(GDALDataset *clumps, GDALDataset *colourImg, std::string inputLUTFile, bool importLUT, std::string exportLUTFile, bool exportLUT) throw(rsgis::img::RSGISImageCalcException)
    {
        if(clumps->GetRasterXSize() != colourImg->GetRasterXSize())
        {
            throw rsgis::img::RSGISImageCalcException("Widths are not the same");
        }
        if(clumps->GetRasterYSize() != colourImg->GetRasterYSize())
        {
            throw rsgis::img::RSGISImageCalcException("Heights are not the same");
        }
        if(colourImg->GetRasterCount() != 3)
        {
            throw rsgis::img::RSGISImageCalcException("Colour image needs to have 3 image bands.");
        }
        
        rsgis::img::RSGISImageUtils imgUtils;
        imgUtils.zerosUIntGDALDataset(colourImg);
        
        unsigned int width = clumps->GetRasterXSize();
        unsigned int height = clumps->GetRasterYSize();
        
        unsigned int *clumpIdxs = new unsigned int[width];
        int *clrRVals = new int[width];
        int *clrGVals = new int[width];
        int *clrBVals = new int[width];

        GDALRasterBand *clumpBand = clumps->GetRasterBand(1);
        GDALRasterBand *rClrBand = colourImg->GetRasterBand(1);
        GDALRasterBand *gClrBand = colourImg->GetRasterBand(2);
        GDALRasterBand *bClrBand = colourImg->GetRasterBand(3);
        
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
        
        ImgClumpRGB *cClump;
        std::vector<ImgClumpRGB*> *clumpTab = NULL;
        if(importLUT)
        {
            std::cout << "Importing Colours LUT\n";
            clumpTab = this->importLUTFromFile(inputLUTFile);
        }
        else
        {
            clumpTab = new std::vector<ImgClumpRGB*>();
            clumpTab->reserve(maxClumpIdx);
            srand ( time(NULL) );
            for(unsigned int i = 0; i < maxClumpIdx; ++i)
            {
                cClump = new ImgClumpRGB(i+1);
                cClump->red = rand() % 255 + 1;
                cClump->green = rand() % 255 + 1;
                cClump->blue = rand() % 255 + 1;
                clumpTab->push_back(cClump);
            }
        }
        
        for(unsigned int i = 0; i < height; ++i)
        {
            clumpBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    cClump = clumpTab->at(clumpIdxs[j] - 1);
                    
                    clrRVals[j] = cClump->red;
                    clrGVals[j] = cClump->green;
                    clrBVals[j] = cClump->blue;
                }
                else
                {
                    clrRVals[j] = 0;
                    clrGVals[j] = 0;
                    clrBVals[j] = 0;
                }
            }
            
            rClrBand->RasterIO(GF_Write, 0, i, width, 1, clrRVals, width, 1, GDT_UInt32, 0, 0);
            gClrBand->RasterIO(GF_Write, 0, i, width, 1, clrGVals, width, 1, GDT_UInt32, 0, 0);
            bClrBand->RasterIO(GF_Write, 0, i, width, 1, clrBVals, width, 1, GDT_UInt32, 0, 0);
        }
        
        if(exportLUT)
        {
            this->exportLUT2File(exportLUTFile, clumpTab);
        }
        
        delete[] clumpIdxs;
        delete[] clrRVals;
        delete[] clrGVals;
        delete[] clrBVals;
        
        for(std::vector<ImgClumpRGB*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
        {
            delete *iterClumps;
        }
        delete clumpTab;
    }
    
    std::vector<ImgClumpRGB*>* RSGISRandomColourClumps::importLUTFromFile(std::string inFile) throw(rsgis::utils::RSGISTextException)
    {
        std::vector<ImgClumpRGB*> *clumpTab = new std::vector<ImgClumpRGB*>();
        try 
        {
            rsgis::utils::RSGISTextUtils txtUtils;
            size_t numLines = txtUtils.countLines(inFile);
            clumpTab->reserve(numLines);
            std::vector<std::string> *tokens = new std::vector<std::string>();
            std::string line = "";
            size_t count = 1;
            ImgClumpRGB *cRGB = NULL;
            rsgis::utils::RSGISTextFileLineReader reader;
            reader.openFile(inFile);
            while(!reader.endOfFile())
            {
                line = reader.readLine();
                if(!txtUtils.blankline(line))
                {
                    txtUtils.tokenizeString(line, ',', tokens, true, true);
                    if(tokens->size() != 3)
                    {
                        std::cout << "Line: " << line << std::endl;
                        throw rsgis::utils::RSGISTextException("Line must has 3 tokens");
                    }
                    cRGB = new ImgClumpRGB(count++);
                    cRGB->red = txtUtils.strto32bitInt(tokens->at(0));
                    cRGB->green = txtUtils.strto32bitInt(tokens->at(1));
                    cRGB->blue = txtUtils.strto32bitInt(tokens->at(2));
                    clumpTab->push_back(cRGB);
                    tokens->clear();
                }
            }
            
            delete tokens;
        }
        catch (rsgis::utils::RSGISTextException &e) 
        {
            throw e;
        }
        return clumpTab;
    }
    
    void RSGISRandomColourClumps::exportLUT2File(std::string outFile, std::vector<ImgClumpRGB*> *clumpTab) throw(rsgis::utils::RSGISTextException)
    {
        try 
        {
            std::ofstream outTxtFile;
            outTxtFile.open(outFile.c_str(), std::ios::out | std::ios::trunc);
            
            if(outTxtFile.is_open())
            {
                for(std::vector<ImgClumpRGB*>::iterator iterClumps = clumpTab->begin(); iterClumps != clumpTab->end(); ++iterClumps)
                {
                    outTxtFile << (*iterClumps)->red << "," << (*iterClumps)->green << "," << (*iterClumps)->blue << std::endl;
                }
                outTxtFile.flush();
                outTxtFile.close();
            }
        }
        catch (rsgis::utils::RSGISTextException &e) 
        {
            throw e;
        }
    }
    
    RSGISRandomColourClumps::~RSGISRandomColourClumps()
    {
        
    }
    
}}

