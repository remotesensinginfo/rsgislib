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
    
    RSGISAttributeTable* RSGISCreateNewAttributeTable::createAndPopPixelCount(GDALDataset *clumpsDataset)throw(RSGISImageCalcException, RSGISAttributeTableException)
    {
        RSGISMathsUtils mathUtils;
        
        RSGISAttributeTable *attTable = NULL;
        
        try 
        {
            // Calc max clump value = number of clumps.
            unsigned long long numClumps = this->calcMaxValue(clumpsDataset);
            cout << "There are " << numClumps << " in the input dataset\n";
            
            // Generate Attribute table
            cout << "Creating blank attribute table\n";
            attTable = new RSGISAttributeTableMem(numClumps);
            attTable->addAttIntField("pxlcount", 0);
            unsigned int pxlCountIdx = attTable->getFieldIndex(string("pxlcount"));

            
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = clumpsDataset;
            
            // Populate the attribute table.
            cout << "Populating the attribute table with sum and count values\n";
            RSGISPopAttributeTablePxlCountCalcImg *popTabPxlCount = new RSGISPopAttributeTablePxlCountCalcImg(0, attTable, pxlCountIdx);
            RSGISCalcImage calcImage(popTabPxlCount);
            calcImage.calcImage(datasets, 1);
            delete popTabPxlCount;
            delete[] datasets;
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        catch(RSGISImageBandException &e)
        {
            throw RSGISImageCalcException(e.what());
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
    
    unsigned long long RSGISCreateNewAttributeTable::calcMaxValue(GDALDataset *dataset)throw(RSGISImageCalcException)
    {
        unsigned int width = dataset->GetRasterXSize();
        unsigned int height = dataset->GetRasterYSize();
        GDALRasterBand *imgBand = dataset->GetRasterBand(1);
        
        unsigned int *clumpIdxs = new unsigned int[width];
        unsigned long long maxClumpIdx = 0;
        
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
    
    
    RSGISPopAttributeTablePxlCountCalcImg::RSGISPopAttributeTablePxlCountCalcImg(int numberOutBands, RSGISAttributeTable *attTable, unsigned int pxlCountIdx):RSGISCalcImageValue(numberOutBands)
    {
        this->attTable = attTable;
        this->pxlCountIdx = pxlCountIdx;
    }
    
    void RSGISPopAttributeTablePxlCountCalcImg::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        unsigned long clumpIdx = 0;
        
        try
        {
            clumpIdx = lexical_cast<unsigned long>(bandValues[0]);
        }
        catch(bad_lexical_cast &e)
        {
            throw RSGISImageCalcException(e.what());
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
                cout << "clumpIdx = " << clumpIdx << endl;
                throw RSGISImageCalcException(e.what());
            }
        }        
    }
    
    RSGISPopAttributeTablePxlCountCalcImg::~RSGISPopAttributeTablePxlCountCalcImg()
    {
        
    }
    
    
    
}}