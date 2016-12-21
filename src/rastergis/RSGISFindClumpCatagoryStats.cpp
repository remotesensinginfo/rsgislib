/*
 *  RSGISFindClumpCatagoryStats.cpp
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

#include "RSGISFindClumpCatagoryStats.h"

namespace rsgis{namespace rastergis{
	
    RSGISFindClumpCatagoryStats::RSGISFindClumpCatagoryStats()
    {
        
    }
    
    void RSGISFindClumpCatagoryStats::calcCatergoriesOverlaps(GDALDataset *clumpsDS, GDALDataset *catsDS, std::string outColsName, std::string majorityColName, bool copyClassName, std::string majClassNameField, std::string classNameField, unsigned int ratBandClumps, unsigned int ratBandCats) throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            if(ratBandClumps == 0)
            {
                throw rsgis::RSGISAttributeTableException("Clumps RAT Band must be greater than zero.");
            }
            if(ratBandClumps > clumpsDS->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("Clumps RAT Band is larger than the number of bands within the image.");
            }
            
            if(ratBandCats == 0)
            {
                throw rsgis::RSGISAttributeTableException("Catagories RAT Band must be greater than zero.");
            }
            if(ratBandCats > catsDS->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("Catagories RAT Band is larger than the number of bands within the image.");
            }
            RSGISRasterAttUtils attUtils;
            rsgis::utils::RSGISTextUtils txtUtils;
            
            // Get Attribute table
            GDALRasterAttributeTable *attTableClumps = clumpsDS->GetRasterBand(ratBandClumps)->GetDefaultRAT();
            GDALRasterAttributeTable *attTableCats = catsDS->GetRasterBand(ratBandCats)->GetDefaultRAT();
            
            // Make sure it is long enough and extend if required.
            int numRows = attTableClumps->GetRowCount();
            
            long minVal = 0;
            long maxVal = 0;
            attUtils.getImageBandMinMax(clumpsDS, ratBandClumps, &minVal, &maxVal);
            
            if(maxVal > numRows)
            {
                attTableClumps->SetRowCount(maxVal+1);
                numRows = maxVal+1;
            }
            numRows = attTableClumps->GetRowCount();
            
            unsigned int histoIdx = attUtils.findColumnIndex(attTableClumps, "Histogram");
            unsigned int majorityColIdx = attUtils.findColumnIndexOrCreate(attTableClumps, majorityColName, GFT_Integer);
            
            unsigned int majClassNameColIdx = 0;
            if(copyClassName)
            {
                majClassNameColIdx = attUtils.findColumnIndexOrCreate(attTableClumps, majClassNameField, GFT_String, GFU_Name);
            }
            
            // Find the range of category values. 
            std::cout << "Find the available categories\n";
            minVal = 0;
            maxVal = 0;
            attUtils.getImageBandMinMax(catsDS, ratBandCats, &minVal, &maxVal);
            
            if(minVal < 0)
            {
                throw rsgis::RSGISAttributeTableException("Minimum class value is 0, values less than zero are invalid.");
            }
            
            size_t minCat = 0;
            size_t maxCat = boost::lexical_cast<size_t>(maxVal);
            size_t numCatVals = (maxCat - minCat)+1;
            
            /*
            std::cout << "minCat = " << minCat << std::endl;
            std::cout << "maxCat = " << maxCat << std::endl;
            std::cout << "numCatVals = " << numCatVals << std::endl;
            */
            
            size_t *catsCount = new size_t[numCatVals];
            for(size_t i = 0; i < numCatVals; ++i)
            {
                catsCount[i] = 0;
            }
            
            // Get Image Values
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = catsDS;
            datasets[1] = clumpsDS;
            
            RSGISCountNumPxlsInCats *calcCatsCounts = new RSGISCountNumPxlsInCats(catsCount, minCat, numCatVals, ratBandCats);
            rsgis::img::RSGISCalcImage calcImageCatCounts(calcCatsCounts);
            calcImageCatCounts.calcImage(datasets, 2, 0);
            delete calcCatsCounts;
            
            std::map<size_t,CategoryField> *cats = new std::map<size_t,CategoryField>();
            for(size_t i = 0; i < numCatVals; ++i)
            {
                //std::cout << i << " = " << catsCount[i] << std::endl;
                if(catsCount[i] > 0)
                {
                    CategoryField catField;
                    catField.category = minCat+i;
                    catField.fieldName = outColsName + std::string("_") + txtUtils.sizettostring(catField.category);
                    catField.fieldIdx = attUtils.findColumnIndexOrCreate(attTableClumps, catField.fieldName, GFT_Real);
                    catField.localIdx = i;
                    
                    if(copyClassName)
                    {
                        catField.className = attUtils.readStringColumnVal(attTableCats, classNameField, catField.category);
                    }
                    
                    cats->insert(std::pair<size_t,CategoryField>(catField.category, catField));
                }
            }
                        
            std::cout << "Categories Found\n";
            for(std::map<size_t,CategoryField>::iterator iterCats = cats->begin(); iterCats != cats->end(); ++iterCats)
            {
                std::cout << "\t" << (*iterCats).second.category;
            }
            std::cout << std::endl;
            
            // Calculate the ratios.
            unsigned int **catStats = new unsigned int*[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                catStats[i] = new unsigned int[numCatVals];
                for(unsigned int j = 0; j < numCatVals; ++j)
                {
                    catStats[i][j] = 0;
                }
            }
            
            unsigned int ratClumpsBandIdx = ratBandClumps - 1;
            unsigned int ratCatsBandIdx = clumpsDS->GetRasterCount() + ratBandCats - 1;
            datasets[0] = clumpsDS;
            datasets[1] = catsDS;
            RSGISCountNumPxlsInCatsPerClump *calcCatClumpCounts = new RSGISCountNumPxlsInCatsPerClump(catStats, cats, ratClumpsBandIdx, ratCatsBandIdx);
            rsgis::img::RSGISCalcImage calcImageCatClumpCounts(calcCatClumpCounts);
            calcImageCatClumpCounts.calcImage(datasets, 2, 0);
            delete calcCatClumpCounts;
            delete[] datasets;
            
            /*
            for(size_t i = 0; i < numRows; ++i)
            {
                std::cout << i << ": ";
                for(unsigned int j = 0; j < numCatVals; ++j)
                {
                    if(j == 0)
                    {
                        std::cout << catStats[i][j];
                    }
                    else
                    {
                        std::cout << ", " << catStats[i][j];
                    }
                }
                std::cout << std::endl;
            }
            */
            
            std::cout << "Writing Majority Values to Output RAT\n";
            size_t numBlocks = floor((double)numRows/(double)RAT_BLOCK_LENGTH);
            size_t rowsRemain = numRows - (numBlocks * RAT_BLOCK_LENGTH);
            
            rsgis::math::RSGISMathsUtils mathUtils;
            double *dataBlock = new double[RAT_BLOCK_LENGTH];
            double *histDataBlock = new double[RAT_BLOCK_LENGTH];
            
            int *majBlock = new int[RAT_BLOCK_LENGTH];
            double *majBlockProp = new double[RAT_BLOCK_LENGTH];
            bool *majBlockFirst = new bool[RAT_BLOCK_LENGTH];
            for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
            {
                majBlock[j] = -1;
                majBlockProp[j] = 0;
                majBlockFirst[j] = true;
            }
            char **majClassNamesBlock = NULL;
            if(copyClassName)
            {
                majClassNamesBlock = new char*[RAT_BLOCK_LENGTH];
            }
            size_t startRow = 0;
            size_t rowID = 0;
            
            for(size_t i = 0; i < numBlocks; ++i)
            {
                attTableClumps->ValuesIO(GF_Read, histoIdx, startRow, RAT_BLOCK_LENGTH, histDataBlock);
                for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                {
                    majBlock[j] = -1;
                    majBlockProp[j] = 0;
                    majBlockFirst[j] = true;
                    if(copyClassName)
                    {
                        majClassNamesBlock[j] = "";
                    }
                }
                for(std::map<size_t,CategoryField>::iterator iterCats = cats->begin(); iterCats != cats->end(); ++iterCats)
                {
                    rowID = startRow;
                    for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        if(histDataBlock[j] > 0)
                        {
                            //std::cout << "catStats[" << rowID << "][" << (*iterCats).second.localIdx << "] = " << catStats[rowID][(*iterCats).second.localIdx] << std::endl;
                            dataBlock[j] = ((double)catStats[rowID][(*iterCats).second.localIdx]) / ((double)histDataBlock[j]);
                            //std::cout << j << " = " << ((double)catStats[rowID][(*iterCats).second.localIdx]) << "/" << ((double)histDataBlock[j]) << " = " << dataBlock[j] << std::endl;
                            if(majBlockFirst[j])
                            {
                                if(dataBlock[j] > 0)
                                {
                                    majBlock[j] = (*iterCats).first;
                                    majBlockProp[j] = dataBlock[j];
                                    majBlockFirst[j] = false;
                                    if(copyClassName)
                                    {
                                        majClassNamesBlock[j] = const_cast<char*>((*iterCats).second.className.c_str());
                                    }
                                }
                            }
                            else if(dataBlock[j] > majBlockProp[j])
                            {
                                majBlock[j] = (*iterCats).first;
                                majBlockProp[j] = dataBlock[j];
                                if(copyClassName)
                                {
                                    majClassNamesBlock[j] = const_cast<char*>((*iterCats).second.className.c_str());
                                }
                            }
                        }
                        else
                        {
                            dataBlock[j] = 0.0;
                        }
                        ++rowID;
                    }
                    attTableClumps->ValuesIO(GF_Write, (*iterCats).second.fieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                }
                attTableClumps->ValuesIO(GF_Write, majorityColIdx, startRow, RAT_BLOCK_LENGTH, majBlock);
                if(copyClassName)
                {
                    attTableClumps->ValuesIO(GF_Write, majClassNameColIdx, startRow, RAT_BLOCK_LENGTH, majClassNamesBlock);
                }
                
                startRow += RAT_BLOCK_LENGTH;
            }
            if(rowsRemain > 0)
            {
                attTableClumps->ValuesIO(GF_Read, histoIdx, startRow, rowsRemain, histDataBlock);
                for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                {
                    majBlock[j] = -1;
                    majBlockProp[j] = 0;
                    majBlockFirst[j] = true;
                    if(copyClassName)
                    {
                        majClassNamesBlock[j] = "";
                    }
                }
                for(std::map<size_t,CategoryField>::iterator iterCats = cats->begin(); iterCats != cats->end(); ++iterCats)
                {
                    rowID = startRow;
                    for(size_t j = 0; j < rowsRemain; ++j)
                    {
                        if(histDataBlock[j] > 0)
                        {
                            //std::cout << "catStats[" << rowID << "][" << (*iterCats).second.localIdx << "] = " << catStats[rowID][(*iterCats).second.localIdx] << std::endl;
                            dataBlock[j] = ((double)catStats[rowID][(*iterCats).second.localIdx]) / ((double)histDataBlock[j]);
                            //std::cout << j << " = " << ((double)catStats[rowID][(*iterCats).second.localIdx]) << "/" << ((double)histDataBlock[j]) << " = " << dataBlock[j] << std::endl;
                            if(majBlockFirst[j])
                            {
                                if(dataBlock[j] > 0)
                                {
                                    majBlock[j] = (*iterCats).first;
                                    majBlockProp[j] = dataBlock[j];
                                    majBlockFirst[j] = false;
                                    if(copyClassName)
                                    {
                                        majClassNamesBlock[j] = const_cast<char*>((*iterCats).second.className.c_str());
                                    }
                                }
                            }
                            else if(dataBlock[j] > majBlockProp[j])
                            {
                                majBlock[j] = (*iterCats).first;
                                majBlockProp[j] = dataBlock[j];
                                if(copyClassName)
                                {
                                    majClassNamesBlock[j] = const_cast<char*>((*iterCats).second.className.c_str());
                                }
                            }
                        }
                        else
                        {
                            dataBlock[j] = 0.0;
                        }
                        
                        ++rowID;
                    }
                    attTableClumps->ValuesIO(GF_Write, (*iterCats).second.fieldIdx, startRow, rowsRemain, dataBlock);
                }
                attTableClumps->ValuesIO(GF_Write, majorityColIdx, startRow, rowsRemain, majBlock);
                if(copyClassName)
                {
                    attTableClumps->ValuesIO(GF_Write, majClassNameColIdx, startRow, rowsRemain, majClassNamesBlock);
                }
            }

            delete[] dataBlock;
            delete[] histDataBlock;
            
            delete[] majBlock;
            delete[] majBlockProp;
            delete[] majBlockFirst;
            if(copyClassName)
            {
                delete[] majClassNamesBlock;
            }
            
            
            
            for(size_t i = 0; i < numRows; ++i)
            {
                delete[] catStats[i];
            }
            delete[] catStats;
        }
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
        
    RSGISFindClumpCatagoryStats::~RSGISFindClumpCatagoryStats()
    {
        
    }
    
    
    
    
    
    
    
    
    RSGISCountNumPxlsInCats::RSGISCountNumPxlsInCats(size_t *catsCount, size_t minCat, size_t numCatVals, unsigned int ratBandCats) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->catsCount = catsCount;
        this->minCat = minCat;
        this->numCatVals = numCatVals;
        this->ratBandCats = ratBandCats;
    }
    
    void RSGISCountNumPxlsInCats::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException)
    {
        size_t cat = boost::lexical_cast<size_t>(intBandValues[ratBandCats-1]);
        ++catsCount[cat-minCat];
    }
    
    RSGISCountNumPxlsInCats::~RSGISCountNumPxlsInCats()
    {
        
	}
    
    
    
    
    
    
    
    RSGISCountNumPxlsInCatsPerClump::RSGISCountNumPxlsInCatsPerClump(unsigned int **catStats, std::map<size_t,CategoryField> *cats, unsigned int ratClumpsBand, unsigned int ratCatsBand) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->catStats = catStats;
        this->cats = cats;
        this->ratClumpsBand = ratClumpsBand;
        this->ratCatsBand = ratCatsBand;
    }
		
    void RSGISCountNumPxlsInCatsPerClump::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException)
    {
        if(intBandValues[ratClumpsBand] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(intBandValues[ratClumpsBand]);
            size_t cat = boost::lexical_cast<size_t>(intBandValues[ratCatsBand]);
            size_t localIdx = 0;
            
            //std::cout << "CAT = " << cat << std::endl;
            //std::cout << "FID = " << fid << std::endl;
            
            std::map<size_t,CategoryField>::iterator iterCats = cats->find(cat);
            if(iterCats == cats->end())
            {
                std::cout << "FID: " << fid << std::endl;
                std::cout << "Cat: " << cat << std::endl;
                throw rsgis::img::RSGISImageCalcException("Could not find the catergory.");
            }
            
            //std::cout << "Local Cat = " << (*iterCats).first << " Cat = " << (*iterCats).second.category << std::endl;
            
            localIdx = (*iterCats).second.localIdx;
            
            ++this->catStats[fid][localIdx];
            
            //std::cout << "local Idx = " << localIdx << " = " << this->catStats[fid][localIdx] << std::endl;
        }
    }
    
    RSGISCountNumPxlsInCatsPerClump::~RSGISCountNumPxlsInCatsPerClump()
    {
        
    }
	
}}




