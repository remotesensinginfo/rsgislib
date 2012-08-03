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
    
    void RSGISFindClumpCatagoryStats::calcCatergoriesOverlaps(GDALDataset *clumpsDS, GDALDataset *catsDS, std::string outColsName, std::string majorityColName, bool copyClassName, std::string majClassNameField, std::string classNameField) throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            rsgis::utils::RSGISTextUtils txtUtils;
            
            // Get Attribute table
            const GDALRasterAttributeTable *attTableTmp = clumpsDS->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                 attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                attTable = new GDALRasterAttributeTable();
            }
            
            
            // Make sure it is long enough and extend if required.
            int numRows = attTable->GetRowCount();
            
            double maxVal = 0;
            clumpsDS->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            
            if(maxVal > numRows)
            {                
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
            // Create the pixel count column.
            int numColumns = attTable->GetColumnCount();
            bool foundPxlCount = false;
            int pxlCountIdx = 0;
            for(int i = 0; i < numColumns; ++i)
            {
                if(std::string(attTable->GetNameOfCol(i)) == "PxlCount")
                {
                    pxlCountIdx = i;
                    foundPxlCount = true;
                    break;
                }
            }
            
            if(!foundPxlCount)
            {
                attTable->CreateColumn("PxlCount", GFT_Integer, GFU_PixelCount);
                pxlCountIdx = numColumns++;
            }
            
            bool foundMajorityCol = false;
            int majorityColIdx = 0;
            for(int i = 0; i < numColumns; ++i)
            {
                if(std::string(attTable->GetNameOfCol(i)) == majorityColName)
                {
                    majorityColIdx = i;
                    foundMajorityCol = true;
                }
            }
            
            if(!foundMajorityCol)
            {
                attTable->CreateColumn(majorityColName.c_str(), GFT_Integer, GFU_Generic);
                majorityColIdx = numColumns++;
            }
            
            size_t majClassNameColIdx = 0;
            size_t inClassNameColIdx = 0;
            if(copyClassName)
            {
                bool foundCol = false;
                for(int i = 0; i < numColumns; ++i)
                {
                    if(std::string(attTable->GetNameOfCol(i)) == majClassNameField)
                    {
                        majClassNameColIdx = i;
                        foundCol = true;
                    }
                }
                if(!foundCol)
                {
                    attTable->CreateColumn(majClassNameField.c_str(), GFT_String, GFU_Name);
                    majClassNameColIdx = numColumns++;
                }
                
                
                attTableTmp = catsDS->GetRasterBand(1)->GetDefaultRAT();
                if(attTableTmp != NULL)
                {
                    for(int i = 0; i < attTableTmp->GetColumnCount(); ++i)
                    {
                        if(std::string(attTableTmp->GetNameOfCol(i)) == classNameField)
                        {
                            inClassNameColIdx = i;
                            foundCol = true;
                        }
                    }
                    
                    if(!foundCol)
                    {
                        throw rsgis::RSGISAttributeTableException("Could not find the class name column within the categories table.");
                    }
                }
                else
                {
                    throw rsgis::RSGISAttributeTableException("The categories input image does not have an attribute table.");
                }
                
            }
            
            // Find the range of category values. 
            std::cout << "Find the available categories\n";
            double minVal = 0;
            catsDS->GetRasterBand(1)->GetStatistics(false, true, &minVal, &maxVal, NULL, NULL);
            
            size_t minCat = boost::lexical_cast<size_t>(minVal);
            size_t maxCat = boost::lexical_cast<size_t>(maxVal);
            size_t numCatVals = (maxCat - minCat)+1;
            
            std::cout << "minCat = " << minCat << std::endl;
            std::cout << "maxCat = " << maxCat << std::endl;
            std::cout << "numCatVals = " << numCatVals << std::endl;
            
            size_t *catsCount = new size_t[numCatVals];
            for(size_t i = 0; i < numCatVals; ++i)
            {
                catsCount[i] = 0;
            }
            
            // Get Image Values
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpsDS;
            datasets[1] = catsDS;
            
            RSGISCountNumPxlsInCats *calcCatsCounts = new RSGISCountNumPxlsInCats(catsCount, minCat, numCatVals);
            rsgis::img::RSGISCalcImage calcImageCatCounts(calcCatsCounts);
            calcImageCatCounts.calcImage(datasets, 2);
            delete calcCatsCounts;
            
            bool foundColumn = false;
            
            std::map<size_t,CategoryField> *cats = new std::map<size_t,CategoryField>();
            for(size_t i = 0; i < numCatVals; ++i)
            {
                //std::cout << i << " = " << catsCount[i] << std::endl;
                if(catsCount[i] > 0)
                {
                    CategoryField catField;
                    catField.category = minCat+i;
                    catField.fieldName = outColsName + std::string("_") + txtUtils.sizettostring(catField.category);
                    
                    foundColumn = false;
                    for(int i = 0; i < numColumns; ++i)
                    {
                        if(std::string(attTable->GetNameOfCol(i)) == catField.fieldName)
                        {
                            catField.fieldIdx = i;
                            foundColumn = true;
                        }
                    }
                    
                    if(!foundColumn)
                    {
                        attTable->CreateColumn(catField.fieldName.c_str(), GFT_Real, GFU_Generic);
                        catField.fieldIdx = numColumns++;
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
            bool *first = new bool[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                first[i] = false;
                for(std::map<size_t,CategoryField>::iterator iterCats = cats->begin(); iterCats != cats->end(); ++iterCats)
                {
                    attTable->SetValue(i, (*iterCats).second.fieldIdx, 0.0);
                }
            }
            
            RSGISCountNumPxlsInCatsPerClump *calcCatClumpCounts = new RSGISCountNumPxlsInCatsPerClump(attTable, first, cats, pxlCountIdx);
            rsgis::img::RSGISCalcImage calcImageCatClumpCounts(calcCatClumpCounts);
            calcImageCatClumpCounts.calcImage(datasets, 2);
            delete calcCatClumpCounts;
            delete[] first;
            
            double ratioVal = 0;
            int majorityCat = 0;
            double maxRatio = 0;
            bool firstCat = true;
            
            for(size_t i = 0; i < numRows; ++i)
            {
                maxRatio = 0;
                majorityCat = 0;
                firstCat = true;
                for(std::map<size_t,CategoryField>::iterator iterCats = cats->begin(); iterCats != cats->end(); ++iterCats)
                {
                    if(attTable->GetValueAsDouble(i, pxlCountIdx) > 0)
                    {
                        ratioVal = attTable->GetValueAsDouble(i, (*iterCats).second.fieldIdx)/ attTable->GetValueAsDouble(i, pxlCountIdx);
                    }
                    else
                    {
                        ratioVal = 0;
                    }
                    attTable->SetValue(i, (*iterCats).second.fieldIdx, ratioVal);
                    
                    if(firstCat & (ratioVal > 0))
                    {
                        majorityCat = (*iterCats).second.category;
                        maxRatio = ratioVal;
                        firstCat = false;
                    }
                    else if((ratioVal > 0) & (ratioVal > maxRatio))
                    {
                        majorityCat = (*iterCats).second.category;
                        maxRatio = ratioVal;
                    }
                }
                attTable->SetValue(i, majorityColIdx, majorityCat);
                
                if(copyClassName)
                {
                    attTable->SetValue(i, majClassNameColIdx, attTableTmp->GetValueAsString(majorityCat, inClassNameColIdx));
                }
            }
            
            clumpsDS->GetRasterBand(1)->SetDefaultRAT(attTable);
            
            delete cats;
            delete[] datasets;
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
    
    
    
    
    
    
    
    
    RSGISCountNumPxlsInCats::RSGISCountNumPxlsInCats(size_t *catsCount, size_t minCat, size_t numCatVals) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->catsCount = catsCount;
        this->minCat = minCat;
        this->numCatVals = numCatVals;
    }
    
    void RSGISCountNumPxlsInCats::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        size_t cat = boost::lexical_cast<size_t>(bandValues[1]);
        ++catsCount[cat-minCat];
    }
    
    RSGISCountNumPxlsInCats::~RSGISCountNumPxlsInCats()
    {
        
	}
    
    
    
    
    
    
    
    RSGISCountNumPxlsInCatsPerClump::RSGISCountNumPxlsInCatsPerClump(GDALRasterAttributeTable *attTable, bool *first, std::map<size_t,CategoryField> *cats, size_t pxlCountIdx) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->first = first;
        this->cats = cats;
        this->pxlCountIdx = pxlCountIdx;
    }
		
    void RSGISCountNumPxlsInCatsPerClump::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            size_t cat = boost::lexical_cast<size_t>(bandValues[1]);
            size_t fieldIdx = 0;
            
            std::map<size_t,CategoryField>::iterator iterCats = cats->find(cat);
            if(iterCats == cats->end())
            {
                std::cout << "FID: " << fid << std::endl;
                std::cout << "Cat: " << cat << std::endl;
                throw rsgis::img::RSGISImageCalcException("Could not find the catergory.");
            }
            fieldIdx = (*iterCats).second.fieldIdx;
            
            attTable->SetValue(fid, fieldIdx, attTable->GetValueAsDouble(fid, fieldIdx)+1);
            
            if(first[fid])
            {
                attTable->SetValue(fid, pxlCountIdx, 1);
                first[fid] = false;
            }
            else
            {
                attTable->SetValue(fid, pxlCountIdx, attTable->GetValueAsInt(fid, pxlCountIdx)+1);
            }
        }
    }
    
    RSGISCountNumPxlsInCatsPerClump::~RSGISCountNumPxlsInCatsPerClump()
    {
        
    }
	
}}




