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

    void RSGISFindInfoBetweenLayers::findClassMajority(GDALDataset *baseSegmentsDS, GDALDataset *infoSegmentsDS, std::string baseClassCol, std::string infoClassCol, bool ignoreZero, int baseRatBand, int infoRatBand)
    {
        RSGISRasterAttUtils attUtils;
        try
        {
            std::cout << "Reading RATs\n";
            // Get Attribute table
            GDALRasterAttributeTable *attTableTmpInfo = infoSegmentsDS->GetRasterBand(infoRatBand)->GetDefaultRAT();
            GDALRasterAttributeTable *infoATable = NULL;
            if(attTableTmpInfo != NULL)
            {
                infoATable = attTableTmpInfo;
            }
            else
            {
                throw RSGISAttributeTableException("The info dataset (band 1) did not have a RAT.");
            }

            GDALRasterAttributeTable *attTableTmpBase = baseSegmentsDS->GetRasterBand(baseRatBand)->GetDefaultRAT();
            GDALRasterAttributeTable *baseATable = NULL;
            if(attTableTmpBase != NULL)
            {
                baseATable = attTableTmpBase;
            }
            else
            {
                baseATable = new GDALDefaultRasterAttributeTable();
            }

            size_t numBaseRows = 0;
            size_t numInfoRows = 0;

            int baseClassIdx = 0;
            int infoClassIdx = 0;

            std::string colName = "";

            // Get column indices
            baseClassIdx = attUtils.findColumnIndexOrCreate(baseATable, baseClassCol, GFT_String);
            infoClassIdx = attUtils.findColumnIndex(infoATable,infoClassCol);

            // Load columnns to memory
            std::string *baseColData = NULL;
            std::string *infoColData = NULL;
            
            baseColData = attUtils.readStrColumnStdStr(baseATable, baseClassCol, &numBaseRows);
            infoColData = attUtils.readStrColumnStdStr(infoATable, infoClassCol, &numInfoRows);
            
            // Find all the classes in the info column.
            std::cout << "Finding all classes in the info dataset column\n";
            std::vector<std::string> classes;
            std::string rowClass = "";
            bool found = false;
            for(size_t i = 0; i < numInfoRows; ++i)
            {
                rowClass = infoColData[i];

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

            RSGISCalcClumpClassMajorities *calcImgClumpClassStats = new RSGISCalcClumpClassMajorities(baseColData, infoColData, classes, clumpCounter, ignoreZero);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgClumpClassStats);
            calcImageStats.calcImage(datasets, 2);
            delete calcImgClumpClassStats;
            delete[] datasets;
            
            std::cout << "Finding majority" << std::endl;

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

            std::cout << "Exporting" << std::endl;
            // Export to large objects.
            // Store in memory and then write all rows back.
            for(unsigned int i = 0; i < numBaseRows; ++i)
            {
                if(clumpMajorityIdx[i] != -1)
                {
                    baseColData[i] = classes.at(clumpMajorityIdx[i]);
                }
                else
                {
                    baseColData[i] = "NA";
                }
            }
            delete[] clumpMajorityIdx;
            
            // Write out
            attUtils.writeStrColumn(baseATable, baseClassCol, baseColData, numBaseRows);
            //baseATable->ValuesIO(GF_Write, baseClassIdx, 0, numBaseRows, );

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


    RSGISCalcClumpClassMajorities::RSGISCalcClumpClassMajorities(std::string *baseColData, std::string *infoColData, std::vector<std::string> classes, unsigned int **clumpCounter, bool ignoreZero) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->baseColData = baseColData;
        this->infoColData = infoColData;
        this->classes = classes;
        this->clumpCounter = clumpCounter;
        this->ignoreZero = ignoreZero; // Ignore values less than 0 in info layer
        
    }
    void RSGISCalcClumpClassMajorities::calcImageValue(float *bandValues, int numBands) 
    {
        if(numBands != 2)
        {
            throw rsgis::img::RSGISImageCalcException("There must be just 2 image bands.");
        }

        if( ((bandValues[0] > 0) & (!this->ignoreZero) ) |
        ((bandValues[0] > 0) & (bandValues[1] > 0)) )
        {
            // Band 1 is the base.
            // Band 2 is the info.
            size_t fidBase = boost::lexical_cast<size_t>(bandValues[0]);
            size_t fidInfo = boost::lexical_cast<size_t>(bandValues[1]);

            // Get class of the info unit.
            std::string classVal = infoColData[fidInfo];

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



