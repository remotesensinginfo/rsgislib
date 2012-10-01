/*
 *  RSGISFindInfoBetweenLayers.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 30/09/2012.
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

#include "RSGISFindInfoBetweenLayers.h"

namespace rsgis{namespace rastergis{
    
    RSGISFindInfoBetweenLayers::RSGISFindInfoBetweenLayers()
    {
        
    }
    
    void RSGISFindInfoBetweenLayers::findClassMajority(GDALDataset *baseSegmentsDS, GDALDataset *infoSegmentsDS, std::string baseClassCol, std::string infoClassCol) throw(RSGISAttributeTableException)
    {
        try
        {
            std::cout << "Reading RATs\n";
            // Get Attribute table
            const GDALRasterAttributeTable *attTableTmpInfo = infoSegmentsDS->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *infoATable = NULL;
            if(attTableTmpInfo != NULL)
            {
                infoATable = new GDALRasterAttributeTable(*attTableTmpInfo);
            }
            else
            {
                throw RSGISAttributeTableException("The info dataset (band 1) did not have a RAT.");
            }
            
            const GDALRasterAttributeTable *attTableTmpBase = baseSegmentsDS->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *baseATable = NULL;
            if(attTableTmpBase != NULL)
            {
                baseATable = new GDALRasterAttributeTable(*attTableTmpBase);
            }
            else
            {
                baseATable = new GDALRasterAttributeTable();
            }
            
            int numBaseRows = baseATable->GetRowCount();
            int numInfoRows = infoATable->GetRowCount();
            
            int baseClassIdx = 0;
            int infoClassIdx = 0;
            
            int numBaseCols = baseATable->GetColumnCount();
            int numInfoCols = infoATable->GetColumnCount();
            std::string colName = "";
            bool foundCol = false;
            
            for(int i = 0; i < numBaseCols; ++i)
            {
                colName = std::string(baseATable->GetNameOfCol(i));
                if(colName == baseClassCol)
                {
                    baseClassIdx = i;
                    foundCol = true;
                }
            }
            
            if(!foundCol)
            {
                baseATable->CreateColumn(baseClassCol.c_str(), GFT_String, GFU_Generic);
                baseClassIdx = numBaseCols++;
            }

            colName = "";
            foundCol = false;
            
            for(int i = 0; i < numInfoCols; ++i)
            {
                colName = std::string(infoATable->GetNameOfCol(i));
                if(colName == infoClassCol)
                {
                    infoClassIdx = i;
                    foundCol = true;
                }
            }
            
            if(!foundCol)
            {
                std::string message = std::string("Info layer column \'") + infoClassCol + std::string("\' was not found.");
                throw RSGISAttributeTableException(message);
            }
            
            // Find all the classes in the info column.
            std::cout << "Finding all classes in the info dataset column\n";
            std::vector<std::string> classes;
            std::string rowClass = "";
            bool found = false;
            for(size_t i = 0; i < numInfoRows; ++i)
            {
                rowClass = std::string(infoATable->GetValueAsString(i, infoClassIdx));
                
                found = false;
                for(std::vector<std::string>::iterator iterClass = classes.begin(); iterClass != classes.end(); ++iterClass)
                {
                    if(rowClass == *iterClass)
                    {
                        found = true;
                        break;
                    }
                }
                
                if(!found)
                {
                    classes.push_back(rowClass);
                }
            }
            
            std::cout << "Available Classes:\n";
            unsigned int idx = 0;
            for(std::vector<std::string>::iterator iterClass = classes.begin(); iterClass != classes.end(); ++iterClass)
            {
                std::cout << idx++ << ") Class: \'" << *iterClass << "\'" << std::endl;
            }
            unsigned int numClasses = classes.size();
            
            
            // Create a data structure to whole the data for each clump.
            unsigned int **clumpCounter = new unsigned int*[numBaseRows];
            for(unsigned int i = 0; i < numBaseRows; ++i)
            {
                clumpCounter[i] = new unsigned int[numClasses];
                for(unsigned int j = 0; j < numClasses; ++j)
                {
                    clumpCounter[i][j] = 0;
                }
            }
            
            // Iterate through the images to populate the data structure.
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = baseSegmentsDS;
            datasets[1] = infoSegmentsDS;
            
            RSGISCalcClumpClassMajorities *calcImgClumpClassStats = new RSGISCalcClumpClassMajorities(baseATable, infoATable, infoClassIdx, classes, clumpCounter);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgClumpClassStats);
            calcImageStats.calcImage(datasets, 2);
            delete calcImgClumpClassStats;
            delete[] datasets;
            
            
            
            // Find the majority column
            unsigned int maxIdx = 0;
            unsigned int maxVal = 0;
            int *clumpMajorityIdx = new int[numBaseRows];
            for(unsigned int i = 0; i < numBaseRows; ++i)
            {
                for(unsigned int j = 0; j < numClasses; ++j)
                {                    
                    if(j == 0)
                    {
                        maxIdx = 0;
                        maxVal = clumpCounter[i][j];
                    }
                    else if(clumpCounter[i][j] > maxVal)
                    {
                        maxIdx = j;
                        maxVal = clumpCounter[i][j];
                    }
                }
                
                if(maxVal == 0)
                {
                    clumpMajorityIdx[i] = -1;
                }
                else
                {
                    clumpMajorityIdx[i] = maxIdx;
                }
                delete[] clumpCounter[i];
            }
            delete[] clumpCounter;
            
            // Export to large objects.
            for(unsigned int i = 0; i < numBaseRows; ++i)
            {
                if(clumpMajorityIdx[i] != -1)
                {
                    baseATable->SetValue(i, baseClassIdx, classes.at(clumpMajorityIdx[i]).c_str());
                }
                else
                {
                    baseATable->SetValue(i, baseClassIdx, "NA");
                }
            }
            delete[] clumpMajorityIdx;
            
            baseSegmentsDS->GetRasterBand(1)->SetDefaultRAT(baseATable);
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    RSGISFindInfoBetweenLayers::~RSGISFindInfoBetweenLayers()
    {
        
    }
    
    	
    RSGISCalcClumpClassMajorities::RSGISCalcClumpClassMajorities(GDALRasterAttributeTable *baseATable, GDALRasterAttributeTable *infoATable, unsigned int infoClassIdx, std::vector<std::string> classes, unsigned int **clumpCounter) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->baseATable = baseATable;
        this->infoATable = infoATable;
        this->infoClassIdx = infoClassIdx;
        this->classes = classes;
        this->clumpCounter = clumpCounter;
    }
    
    void RSGISCalcClumpClassMajorities::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numBands != 2)
        {
            throw rsgis::img::RSGISImageCalcException("There must be just 2 image bands.");
        }
        
        if(bandValues[0] > 0 & bandValues[1] > 0)
        {
            // Band 1 is the base.
            // Band 2 is the info.
            size_t fidBase = boost::lexical_cast<size_t>(bandValues[0]);
            size_t fidInfo = boost::lexical_cast<size_t>(bandValues[1]);
            
            // Get class of the info unit.
            std::string classVal = infoATable->GetValueAsString(fidInfo, infoClassIdx);
            
            unsigned int idx = 0;
            bool foundClass = false;
            for(std::vector<std::string>::iterator iterClass = classes.begin(); iterClass != classes.end(); ++iterClass)
            {
                if(classVal == *iterClass)
                {
                    foundClass = true;
                    break;
                }
                else
                {
                    ++idx;
                }
            }
            
            if(!foundClass)
            {
                std::string message = std::string("Could not find class \'") + classVal + std::string("\'");
                throw rsgis::img::RSGISImageCalcException(message);
            }
            
            // Increment the class count for the base unit.
            ++this->clumpCounter[fidBase][idx];
        }
        
    }
		
    RSGISCalcClumpClassMajorities::~RSGISCalcClumpClassMajorities()
    {
        
	}
	
}}



