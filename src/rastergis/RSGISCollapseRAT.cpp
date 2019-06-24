/*
 *  RSGISCollapseRAT.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/10/2014.
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


#include "RSGISCollapseRAT.h"

namespace rsgis{namespace rastergis{
    
    RSGISCollapseRAT::RSGISCollapseRAT()
    {
        
    }
    
    void RSGISCollapseRAT::classifyClumps(GDALDataset *inputClumps, unsigned int ratBand, std::string selectColumn, std::string outImage, std::string gdalFormat)
    {
        try
        {
            RSGISRasterAttUtils ratUtils;
            
            GDALRasterBand *ratInImgBand = inputClumps->GetRasterBand(ratBand);
            GDALRasterAttributeTable *attInTable = ratInImgBand->GetDefaultRAT();
            
            size_t numRows = 0;
            int *selColVals = ratUtils.readIntColumn(attInTable, selectColumn, &numRows);
            
            size_t *collapsedIDs = new size_t[numRows];
            
            size_t fidCount = 1;
            collapsedIDs[0] = 0;
            for(size_t i = 1; i < numRows; ++i)
            {
                if(selColVals[i] == 1)
                {
                    collapsedIDs[i] = fidCount++;
                }
                else
                {
                    collapsedIDs[i] = 0;
                }
            }
            
            size_t outAttRowCount = fidCount;
            
            rsgis::img::RSGISCalcImageValue *collapseGenNewImg = new RSGISCollapseRATPopNewImage(collapsedIDs, numRows, ratBand);
            rsgis::img::RSGISCalcImage imgCalc = rsgis::img::RSGISCalcImage(collapseGenNewImg);
            imgCalc.calcImage(&inputClumps, 1, 0, outImage, false, NULL, gdalFormat, GDT_UInt32);
            delete collapseGenNewImg;
            delete[] collapsedIDs;
            
            GDALDataset *outClumpsDataset = (GDALDataset *) GDALOpenShared(outImage.c_str(), GA_Update);
            if(outClumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + outImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            GDALRasterBand *ratOutImgBand = outClumpsDataset->GetRasterBand(1);
            GDALRasterAttributeTable *attOutTable = new GDALDefaultRasterAttributeTable();
            attOutTable->SetRowCount(outAttRowCount);
            
            std::vector<RSGISRATCol> *ratInCols = ratUtils.getRatColumnsList(attInTable);
            unsigned int colIdx = 0;
            size_t numInRows = 0;
            for(std::vector<RSGISRATCol>::iterator iterCols = ratInCols->begin(); iterCols != ratInCols->end(); ++iterCols)
            {
                std::cout << "Column: " << (*iterCols).name << std::endl;

                colIdx = ratUtils.findColumnIndexOrCreate(attOutTable, (*iterCols).name, (*iterCols).type, (*iterCols).usage);
                
                if((*iterCols).type == GFT_Integer)
                {
                    numInRows = 0;
                    int *colInVals = ratUtils.readIntColumn(attInTable, (*iterCols).name, &numInRows);
                    if(numInRows != numRows)
                    {
                        throw RSGISAttributeTableException("Number of inputs rows has been messed up.");
                    }
                    int *colOutVal = new int[outAttRowCount];
                    colOutVal[0] = 0;
                    size_t fidCount = 1;
                    for(size_t i = 1; i < numRows; ++i)
                    {
                        if(selColVals[i] == 1)
                        {
                            if(fidCount == outAttRowCount)
                            {
                                std::cout << "fidCount = " << fidCount << std::endl;
                                std::cout << "outAttRowCount = " << outAttRowCount << std::endl;
                                
                                throw RSGISAttributeTableException("Number of rows in the output RAT has been messed up.");
                            }
                            
                            colOutVal[fidCount++] = colInVals[i];
                        }
                    }
                    attOutTable->ValuesIO(GF_Write, colIdx, 0, outAttRowCount, colOutVal);
                    delete[] colInVals;
                    delete[] colOutVal;
                }
                else if((*iterCols).type == GFT_Real)
                {
                    numInRows = 0;
                    double *colInVals = ratUtils.readDoubleColumn(attInTable, (*iterCols).name, &numInRows);
                    if(numInRows != numRows)
                    {
                        throw RSGISAttributeTableException("Number of inputs rows has been messed up.");
                    }
                    double *colOutVal = new double[outAttRowCount];
                    colOutVal[0] = 0.0;
                    size_t fidCount = 1;
                    for(size_t i = 1; i < numRows; ++i)
                    {
                        if(selColVals[i] == 1)
                        {
                            if(fidCount == outAttRowCount)
                            {
                                std::cout << "fidCount = " << fidCount << std::endl;
                                std::cout << "outAttRowCount = " << outAttRowCount << std::endl;
                                
                                throw RSGISAttributeTableException("Number of rows in the output RAT has been messed up.");
                            }
                            
                            colOutVal[fidCount++] = colInVals[i];
                        }
                    }
                    attOutTable->ValuesIO(GF_Write, colIdx, 0, outAttRowCount, colOutVal);
                    delete[] colInVals;
                    delete[] colOutVal;
                }
                else if((*iterCols).type == GFT_String)
                {
                    numInRows = 0;
                    std::string *colInVals = ratUtils.readStrColumnStdStr(attInTable, (*iterCols).name, &numInRows);
                    if(numInRows != numRows)
                    {
                        throw RSGISAttributeTableException("Number of inputs rows has been messed up.");
                    }
                    std::string *colOutVal = new std::string[outAttRowCount];
                    colOutVal[0] = "";
                    size_t fidCount = 1;
                    for(size_t i = 1; i < numRows; ++i)
                    {
                        if(selColVals[i] == 1)
                        {
                            if(fidCount == outAttRowCount)
                            {
                                std::cout << "fidCount = " << fidCount << std::endl;
                                std::cout << "outAttRowCount = " << outAttRowCount << std::endl;
                                
                                throw RSGISAttributeTableException("Number of rows in the output RAT has been messed up.");
                            }
                            
                            colOutVal[fidCount++] = colInVals[i];
                        }
                    }
                    ratUtils.writeStrColumn(attOutTable, (*iterCols).name, colOutVal, outAttRowCount);
                    delete[] colInVals;
                    delete[] colOutVal;
                }
                else
                {
                    throw RSGISAttributeTableException("RAT column data type is not known.");
                }
            
            }
            
            
            delete[] selColVals;
            delete ratInCols;
            
            ratOutImgBand->SetDefaultRAT(attOutTable);
            ratOutImgBand->SetMetadataItem("LAYER_TYPE", "thematic");
            
            RSGISPopulateWithImageStats popImageStats;
            popImageStats.populateImageWithRasterGISStats(outClumpsDataset, false, true, 1);
            popImageStats.calcPyramids(outClumpsDataset);
            
            GDALClose(outClumpsDataset);
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
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
    
    RSGISCollapseRAT::~RSGISCollapseRAT()
    {
        
    }
    
    
    
    
    RSGISCollapseRATPopNewImage::RSGISCollapseRATPopNewImage(size_t *collapsedIDs, size_t numVals, unsigned int ratBand): rsgis::img::RSGISCalcImageValue(1)
    {
        this->collapsedIDs = collapsedIDs;
        this->numVals = numVals;
        this->ratArrIdx = ratBand-1;
    }
    
    void RSGISCollapseRATPopNewImage::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
    {
        if(intBandValues[0] < numVals)
        {
            output[0] = collapsedIDs[intBandValues[ratArrIdx]];
        }
        else
        {
            throw rsgis::img::RSGISImageCalcException("Image pixel value was not within the attribute table.");
        }
    }
    
    RSGISCollapseRATPopNewImage::~RSGISCollapseRATPopNewImage()
    {
        
    }
    
    
    
    
}}
