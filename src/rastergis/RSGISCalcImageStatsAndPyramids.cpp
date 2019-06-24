/*
 *  RSGISCalcImageStatsAndPyramids.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/05/2014.
 *  Copyright 2014 RSGISLib.
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

#include "RSGISCalcImageStatsAndPyramids.h"

namespace rsgis{namespace rastergis{
    
    
    RSGISPopulateWithImageStats::RSGISPopulateWithImageStats()
    {
        
    }
    
    void RSGISPopulateWithImageStats::populateImageWithRasterGISStats(GDALDataset *clumpsDataset, bool addColourTable, bool calcImagePyramids, bool ignoreZero, unsigned int ratBand)
    {
        try
        {
            this->populateImageWithRasterGISStats(clumpsDataset, addColourTable, ignoreZero, ratBand);
            if(calcImagePyramids)
            {
                this->calcPyramids(clumpsDataset);
            }
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    void RSGISPopulateWithImageStats::populateImageWithRasterGISStats(GDALDataset *clumpsDataset, bool addColourTable, bool ignoreZero, unsigned int ratBand)
    {
        try
        {
            if(ratBand == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT Band must be greater than zero.");
            }
            if(ratBand > clumpsDataset->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("RAT Band is larger than the number of bands within the image.");
            }
            
            rsgis::utils::RSGISTextUtils txtUtils;
            RSGISRasterAttUtils attUtils;
                        
            GDALRasterBand *band = clumpsDataset->GetRasterBand(ratBand);
            
            band->SetMetadataItem("LAYER_TYPE", "thematic");
            
            if(ignoreZero)
            {
                band->SetNoDataValue(0.0);
            }
            
            long max = 0;
            long min = 0;
            std::cout << "Get Image Min and Max.\n";
            RSGISCalcImgMinMax calcMinMac = RSGISCalcImgMinMax(&min, &max, ratBand-1);
            rsgis::img::RSGISCalcImage calcImageMinMax(&calcMinMac);
            calcImageMinMax.calcImage(&clumpsDataset, 1, 0);
            
            
            if((min == 0) & (max == 0))
            {
                band->SetMetadataItem("STATISTICS_HISTOBINFUNCTION", "direct");
                band->SetMetadataItem("STATISTICS_HISTOMIN", "0");
                band->SetMetadataItem("STATISTICS_HISTOMAX", "0");
                band->SetMetadataItem("STATISTICS_HISTONUMBINS", "1");
                
                GDALRasterAttributeTable *attTable = band->GetDefaultRAT();
                attTable->SetRowCount(1);
                unsigned int histoColIdx = attUtils.findColumnIndexOrCreate(attTable, "Histogram", GFT_Real, GFU_PixelCount);
                
                unsigned int redColIdx = 0;
                unsigned int greenColIdx = 0;
                unsigned int blueColIdx = 0;
                unsigned int alphaColIdx = 0;
                if(addColourTable)
                {
                    redColIdx = attUtils.findColumnIndexOrCreate(attTable, "Red", GFT_Integer, GFU_Red);
                    greenColIdx = attUtils.findColumnIndexOrCreate(attTable, "Green", GFT_Integer, GFU_Green);
                    blueColIdx = attUtils.findColumnIndexOrCreate(attTable, "Blue", GFT_Integer, GFU_Blue);
                    alphaColIdx = attUtils.findColumnIndexOrCreate(attTable, "Alpha", GFT_Integer, GFU_Alpha);
                }
                
                double *dataBlock = new double[1];
                dataBlock[0] = 0;
                int *redBlock = NULL;
                int *greenBlock = NULL;
                int *blueBlock = NULL;
                int *alphaBlock = NULL;
                if(addColourTable)
                {
                    redBlock = new int[1];
                    redBlock[0] = 0;
                    greenBlock = new int[1];
                    greenBlock[0] = 0;
                    blueBlock = new int[1];
                    blueBlock[0] = 0;
                    alphaBlock = new int[1];
                    alphaBlock[0] = 0;
                }
                
                attTable->ValuesIO(GF_Write, histoColIdx, 0, 1, dataBlock);
                if(addColourTable)
                {
                    attTable->ValuesIO(GF_Write, redColIdx, 0, 1, redBlock);
                    attTable->ValuesIO(GF_Write, greenColIdx, 0, 1, greenBlock);
                    attTable->ValuesIO(GF_Write, blueColIdx, 0, 1, blueBlock);
                    attTable->ValuesIO(GF_Write, alphaColIdx, 0, 1, alphaBlock);
                }
            }
            else
            {
                if(min < 0)
                {
                    throw rsgis::RSGISImageException("The minimum value is less than zero.");
                }
                
                size_t maxHistVal = max+1;
                size_t *histo = new size_t[maxHistVal];
                
                for(size_t i = 0; i < maxHistVal; ++i)
                {
                    histo[i] = 0;
                }
                
                std::cout << "Get Image Histogram.\n";
                RSGISGetClumpsHistogram calcImgHisto = RSGISGetClumpsHistogram(histo, maxHistVal, (ratBand-1));
                rsgis::img::RSGISCalcImage calcImageStats(&calcImgHisto);
                calcImageStats.calcImage(&clumpsDataset, 1, 0);
                
                if(ignoreZero)
                {
                    histo[0] = 0.0;
                }
                
                if(addColourTable)
                {
                    std::cout << "Adding Histogram and Colour Table to image file\n";
                }
                else
                {
                    std::cout << "Adding Histogram to image file\n";
                }
                
                GDALRasterAttributeTable *attTable = band->GetDefaultRAT();
                attTable->SetRowCount(maxHistVal);
                
                band->SetMetadataItem("STATISTICS_HISTOBINFUNCTION", "direct");
                band->SetMetadataItem("STATISTICS_HISTOMIN", "0");
                band->SetMetadataItem("STATISTICS_HISTOMAX", txtUtils.int64bittostring(maxHistVal).c_str());
                band->SetMetadataItem("STATISTICS_HISTONUMBINS", txtUtils.int64bittostring(maxHistVal).c_str());
                
                unsigned int histoColIdx = attUtils.findColumnIndexOrCreate(attTable, "Histogram", GFT_Real, GFU_PixelCount);
                
                unsigned int redColIdx = 0;
                unsigned int greenColIdx = 0;
                unsigned int blueColIdx = 0;
                unsigned int alphaColIdx = 0;
                if(addColourTable)
                {
                    redColIdx = attUtils.findColumnIndexOrCreate(attTable, "Red", GFT_Integer, GFU_Red);
                    greenColIdx = attUtils.findColumnIndexOrCreate(attTable, "Green", GFT_Integer, GFU_Green);
                    blueColIdx = attUtils.findColumnIndexOrCreate(attTable, "Blue", GFT_Integer, GFU_Blue);
                    alphaColIdx = attUtils.findColumnIndexOrCreate(attTable, "Alpha", GFT_Integer, GFU_Alpha);
                }
                
                double *dataBlock = new double[RAT_BLOCK_LENGTH];
                int *redBlock = NULL;
                int *greenBlock = NULL;
                int *blueBlock = NULL;
                int *alphaBlock = NULL;
                if(addColourTable)
                {
                    redBlock = new int[RAT_BLOCK_LENGTH];
                    greenBlock = new int[RAT_BLOCK_LENGTH];
                    blueBlock = new int[RAT_BLOCK_LENGTH];
                    alphaBlock = new int[RAT_BLOCK_LENGTH];
                }
                size_t numBlocks = floor((double)maxHistVal/(double)RAT_BLOCK_LENGTH);
                size_t rowsRemain = maxHistVal - (numBlocks * RAT_BLOCK_LENGTH);
                size_t startRow = 0;
                size_t rowID = 0;
                srand(time(NULL));
                for(size_t i = 0; i < numBlocks; ++i)
                {
                    for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        if(addColourTable)
                        {
                            if((rowID == 0) & ignoreZero)
                            {
                                redBlock[j] = 0;
                                greenBlock[j] = 0;
                                blueBlock[j] = 0;
                                alphaBlock[j] = 255;
                            }
                            else
                            {
                                redBlock[j] = rand() % 255 + 1;
                                greenBlock[j] = rand() % 255 + 1;
                                blueBlock[j] = rand() % 255 + 1;
                                alphaBlock[j] = 255;
                            }
                        }
                        dataBlock[j] = histo[rowID];
                        ++rowID;
                    }
                    attTable->ValuesIO(GF_Write, histoColIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                    if(addColourTable)
                    {
                        attTable->ValuesIO(GF_Write, redColIdx, startRow, RAT_BLOCK_LENGTH, redBlock);
                        attTable->ValuesIO(GF_Write, greenColIdx, startRow, RAT_BLOCK_LENGTH, greenBlock);
                        attTable->ValuesIO(GF_Write, blueColIdx, startRow, RAT_BLOCK_LENGTH, blueBlock);
                        attTable->ValuesIO(GF_Write, alphaColIdx, startRow, RAT_BLOCK_LENGTH, alphaBlock);
                    }
                    
                    startRow += RAT_BLOCK_LENGTH;
                }
                if(rowsRemain > 0)
                {
                    for(size_t j = 0; j < rowsRemain; ++j)
                    {
                        if(addColourTable)
                        {
                            if((rowID == 0) & ignoreZero)
                            {
                                redBlock[j] = 0;
                                greenBlock[j] = 0;
                                blueBlock[j] = 0;
                                alphaBlock[j] = 255;
                            }
                            else
                            {
                                redBlock[j] = rand() % 255 + 1;
                                greenBlock[j] = rand() % 255 + 1;
                                blueBlock[j] = rand() % 255 + 1;
                                alphaBlock[j] = 255;
                            }
                        }
                        dataBlock[j] = histo[rowID];
                        ++rowID;
                    }
                    attTable->ValuesIO(GF_Write, histoColIdx, startRow, rowsRemain, dataBlock);
                    if(addColourTable)
                    {
                        attTable->ValuesIO(GF_Write, redColIdx, startRow, rowsRemain, redBlock);
                        attTable->ValuesIO(GF_Write, greenColIdx, startRow, rowsRemain, greenBlock);
                        attTable->ValuesIO(GF_Write, blueColIdx, startRow, rowsRemain, blueBlock);
                        attTable->ValuesIO(GF_Write, alphaColIdx, startRow, rowsRemain, alphaBlock);
                    }
                }
                delete[] histo;
            }

            
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    void RSGISPopulateWithImageStats::calcPyramids(GDALDataset *clumpsDataset)
    {
        try
        {
            std::cout << "Calculating Image Pyramids.\n";
            int nLevels[] = { 4, 8, 16, 32, 64, 128, 256, 512 };
            int nOverviews = 0;
            int mindim = 0;
            const char *pszType = "NEAREST";
            
            /* first we work out how many overviews to build based on the size */
            if(clumpsDataset->GetRasterXSize() < clumpsDataset->GetRasterYSize())
            {
                mindim = clumpsDataset->GetRasterXSize();
            }
            else
            {
                mindim = clumpsDataset->GetRasterYSize();
            }
            
            nOverviews = 0;
            for(int i = 0; i < 8; i++)
            {
                if( (mindim/nLevels[i]) > 33 )
                {
                    ++nOverviews;
                }
            }
            
            int nLastProgress = -1;
            clumpsDataset->BuildOverviews(pszType, nOverviews, nLevels, 0, NULL,  (GDALProgressFunc)RSGISRATStatsTextProgress, &nLastProgress);
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    RSGISPopulateWithImageStats::~RSGISPopulateWithImageStats()
    {
        
    }
    
    
    RSGISGetClumpsHistogram::RSGISGetClumpsHistogram(size_t *histogram, size_t maxVal, unsigned int band):rsgis::img::RSGISCalcImageValue(0)
    {
        this->histogram = histogram;
        this->maxVal = maxVal;
        this->band = band;
    }

    void RSGISGetClumpsHistogram::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) 
    {
        if(numIntVals <= band)
        {
            throw rsgis::img::RSGISImageCalcException("Band is not in the input image...");
        }
        
        if((intBandValues[band] >= 0) & (intBandValues[band] < maxVal))
        {
            ++histogram[intBandValues[band]];
        }
    }
    
    RSGISGetClumpsHistogram::~RSGISGetClumpsHistogram()
    {
        
    }
    
    
    
    
}}

