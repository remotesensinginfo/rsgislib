/*
 *  RSGISCreateNewAttributeTable.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/03/2012.
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

#include "RSGISCreateNewAttributeTable.h"

namespace rsgis{namespace rastergis{
    
    
    RSGISCreateNewAttributeTable::RSGISCreateNewAttributeTable()
    {
        
    }
    
    RSGISAttributeTable* RSGISCreateNewAttributeTable::createAndPopPixelCount(GDALDataset *clumpsDataset, bool useMemory, std::string outFilePath, unsigned long cacheSize)throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException)
    {
        rsgis::math::RSGISMathsUtils mathUtils;
        
        RSGISAttributeTable *attTable = NULL;
        
        try 
        {
            // Calc max clump value = number of clumps.
            size_t numClumps = this->calcMaxValue(clumpsDataset);
            std::cout << "There are " << numClumps << " in the input dataset\n";
            
            // Generate Attribute table
            std::cout << "Creating blank attribute table\n";
            if(useMemory)
            {
                attTable = new RSGISAttributeTableMem(numClumps);
            }
            else
            {
                attTable = new RSGISAttributeTableHDF(numClumps, outFilePath, false, cacheSize);
            }
            attTable->addAttIntField("pxlcount", 0);
            
            unsigned int pxlCountIdx = attTable->getFieldIndex(std::string("pxlcount"));
            
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = clumpsDataset;
            
            // Populate the attribute table.
            std::cout << "Populating the attribute table with sum and count values\n";
            RSGISPopAttributeTablePxlCountCalcImg *popTabPxlCount = new RSGISPopAttributeTablePxlCountCalcImg(0, attTable, pxlCountIdx);
            rsgis::img::RSGISCalcImage calcImage(popTabPxlCount);
            calcImage.calcImage(datasets, 1);
            delete popTabPxlCount;
            delete[] datasets;
        } 
        catch (rsgis::img::RSGISImageCalcException &e) 
        {
            throw e;
        }
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return attTable;
    }
    
    RSGISAttributeTable* RSGISCreateNewAttributeTable::createAndPopPixelCountOffLine(GDALDataset *clumpsDataset, bool useMemory, std::string outFilePath, unsigned long cacheSize)throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException)
    {
        RSGISAttributeTable *attTable = NULL;
        
        try 
        {
            // Calc max clump value = number of clumps.
            size_t numClumps = this->calcMaxValue(clumpsDataset);
            std::cout << "There are " << numClumps << " in the input dataset\n";
            
            // Generate Attribute table
            std::cout << "Creating blank attribute table\n";
            if(useMemory)
            {
                attTable = new RSGISAttributeTableMem(numClumps);
            }
            else
            {
                attTable = new RSGISAttributeTableHDF(numClumps, outFilePath, false, cacheSize);
            }
            attTable->addAttIntField("pxlcount", 0);
            
            unsigned int pxlCountIdx = attTable->getFieldIndex(std::string("pxlcount"));
            
            
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = clumpsDataset;
            
            size_t *counts = new size_t[numClumps];
            for(size_t i = 0; i < numClumps; ++i)
            {
                counts[i] = 0;
            }
            
            // Populate the attribute table.
            std::cout << "Populating the attribute table with sum and count values\n";
            RSGISPopAttributeTablePxlCountArrCalcImg *popTabPxlCount = new RSGISPopAttributeTablePxlCountArrCalcImg(0, counts, numClumps);
            rsgis::img::RSGISCalcImage calcImage(popTabPxlCount);
            calcImage.calcImage(datasets, 1);
            delete popTabPxlCount;
            delete[] datasets;
            
            size_t idx = 0;
            for(attTable->start(); attTable->end(); ++(*attTable))
            {
                (*(*attTable))->intFields->at(pxlCountIdx) = counts[idx++];
            }
            
            delete[] counts;
             
        } 
        catch (rsgis::img::RSGISImageCalcException &e) 
        {
            throw e;
        }
        catch(rsgis::img::RSGISImageBandException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (RSGISAttributeTableException &e) 
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        
        return attTable;
    }

    
    size_t RSGISCreateNewAttributeTable::calcMaxValue(GDALDataset *dataset)throw(rsgis::img::RSGISImageCalcException)
    {
        unsigned int width = dataset->GetRasterXSize();
        unsigned int height = dataset->GetRasterYSize();
        GDALRasterBand *imgBand = dataset->GetRasterBand(1);
        
        unsigned int *clumpIdxs = new unsigned int[width];
        size_t maxClumpIdx = 0;
        
        for(unsigned int i = 0; i < height; ++i)
        {
            imgBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
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
        
        delete clumpIdxs;
        
        return maxClumpIdx;
    }
    
    
    RSGISCreateNewAttributeTable::~RSGISCreateNewAttributeTable()
    {
        
    }
    
    
    RSGISPopAttributeTablePxlCountCalcImg::RSGISPopAttributeTablePxlCountCalcImg(int numberOutBands, RSGISAttributeTable *attTable, unsigned int pxlCountIdx):rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->attTable = attTable;
        this->pxlCountIdx = pxlCountIdx;
    }
    
    void RSGISPopAttributeTablePxlCountCalcImg::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = boost::lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            try
            {
                RSGISFeature *feat = attTable->getFeature(clumpIdx);
                ++feat->intFields->at(this->pxlCountIdx);
            }
            catch(RSGISAttributeTableException &e)
            {
                std::cout << "clumpIdx = " << clumpIdx << std::endl;
                throw rsgis::img::RSGISImageCalcException(e.what());
            }
        }        
    }
    
    RSGISPopAttributeTablePxlCountCalcImg::~RSGISPopAttributeTablePxlCountCalcImg()
    {
        
    }
    
    
    RSGISPopAttributeTablePxlCountArrCalcImg::RSGISPopAttributeTablePxlCountArrCalcImg(int numberOutBands, size_t *pxlCounts, size_t len):rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->len = len;
        this->pxlCounts = pxlCounts;
    }
    
    void RSGISPopAttributeTablePxlCountArrCalcImg::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = boost::lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(boost::bad_lexical_cast &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        
        if(clumpIdx > 0)
        {
            --clumpIdx;
            
            if(clumpIdx >= len)
            {
                throw rsgis::img::RSGISImageCalcException("Clump index is only within table.");
            }
            
            ++this->pxlCounts[clumpIdx];
        }        
    }
    
    RSGISPopAttributeTablePxlCountArrCalcImg::~RSGISPopAttributeTablePxlCountArrCalcImg()
    {
        
    }
    
    
    
}}