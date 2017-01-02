/*
 *  RSGISRegionGrowAttributeTable.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/02/2012.
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

#include "RSGISRegionGrowAttributeTable.h"


namespace rsgis{namespace segment{
  /*
    RSGISRegionGrowAttributeTable::RSGISRegionGrowAttributeTable()
    {
        
    }
    
    void RSGISRegionGrowAttributeTable::growClassRegionsUsingThresholds(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, std::vector<rsgis::rastergis::RSGISIfStatement*> *statements, std::string classAttName, int classAttVal)throw(rsgis::RSGISAttributeTableException,rsgis::RSGISImageException)
    {
        try
        {
            rsgis::rastergis::RSGISAttributeDataType classFieldDT = attTable->getDataType(classAttName);
            if(classFieldDT != rsgis::rastergis::rsgis_int)
            {
                throw RSGISAttributeTableException("The class field needs to be of type integer.");
            }
            
            unsigned int classFieldIdx  = attTable->getFieldIndex(classAttName);
            
            unsigned int width = 0;
            unsigned int height = 0;
            
            bool **mask = this->createMask(attTable, clumps, &width, &height, classFieldIdx, classAttVal);
            
            std::list<unsigned int> *connectClumps = new std::list<unsigned int>();
            
            rsgis::rastergis::RSGISFeature *feat = NULL;
            bool change = true;
            while(change)
            {
                change = false;
                this->getConnectedClumps(connectClumps, attTable, clumps, mask, width, height);
                            
                if(connectClumps->size() > 0)
                {
                    for(std::list<unsigned int>::iterator iterClumpIdxs = connectClumps->begin(); iterClumpIdxs != connectClumps->end(); ++iterClumpIdxs)
                    {
                        feat = attTable->getFeature((*iterClumpIdxs));
                        if(feat->intFields->at(classFieldIdx) != classAttVal)
                        {
                            if(attTable->applyIfStatementsToFeature(feat, statements))
                            {
                                change = true;
                            }
                        }
                    }
                }
                connectClumps->clear();
                
                if(change)
                {
                    this->updateMask(mask, attTable, clumps, width, height, classFieldIdx, classAttVal);
                }
            }
            
            delete connectClumps;
            
            for(unsigned int i = 0; i < height; ++i)
            {
                delete[] mask[i];                
            }
            delete[] mask;            
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    bool** RSGISRegionGrowAttributeTable::createMask(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, unsigned int *width, unsigned int *height, unsigned int classFieldIdx, int classVal)
    {
        *width = clumps->GetRasterXSize();
        *height = clumps->GetRasterYSize();
        
        GDALRasterBand *clumpsBand = clumps->GetRasterBand(1);
        unsigned int *clumpIdxs = new unsigned int[*width];
        
        rsgis::rastergis::RSGISFeature *feat = NULL;
        
        bool **mask = new bool*[*height];
        for(unsigned int i = 0; i < *height; ++i)
        {
            mask[i] = new bool[*width];
            
            clumpsBand->RasterIO(GF_Read, 0, i, *width, 1, clumpIdxs, *width, 1, GDT_UInt32, 0, 0);
            
            for(unsigned int j = 0; j < *width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    feat = attTable->getFeature(clumpIdxs[j]-1);
                    if(feat->intFields->at(classFieldIdx) == classVal)
                    {
                        mask[i][j] = true;
                    }
                    else
                    {
                        mask[i][j] = false;
                    }
                }
                else
                {
                    mask[i][j] = false;
                }
            }
        }
        
        delete[] clumpIdxs;
        
        return mask;
    }
    
    void RSGISRegionGrowAttributeTable::updateMask(bool **mask, rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, unsigned int width, unsigned int height, unsigned int classFieldIdx, int classVal)
    {
        GDALRasterBand *clumpsBand = clumps->GetRasterBand(1);
        unsigned int *clumpIdxs = new unsigned int[width];
        
        rsgis::rastergis::RSGISFeature *feat = NULL;
        
        for(unsigned int i = 0; i < height; ++i)
        {            
            clumpsBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            
            for(unsigned int j = 0; j < width; ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    feat = attTable->getFeature(clumpIdxs[j]-1);
                    if(feat->intFields->at(classFieldIdx) == classVal)
                    {
                        mask[i][j] = true;
                    }
                    else
                    {
                        mask[i][j] = false;
                    }
                }
                else
                {
                    mask[i][j] = false;
                }
            }
        }
        
        delete[] clumpIdxs;        
    }
    
    void RSGISRegionGrowAttributeTable::getConnectedClumps(std::list<unsigned int> *connectedClumps, rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, bool **mask, unsigned int width, unsigned int height)
    {        
        GDALRasterBand *clumpsBand = clumps->GetRasterBand(1);
        unsigned int *clumpIdxs = new unsigned int[width];
        
        for(unsigned int i = 1; i < (height-1); ++i)
        {            
            clumpsBand->RasterIO(GF_Read, 0, i, width, 1, clumpIdxs, width, 1, GDT_UInt32, 0, 0);
            
            for(unsigned int j = 1; j < (width-1); ++j)
            {
                if(clumpIdxs[j] != 0)
                {
                    if((mask[i+1][j] || 
                        mask[i-1][j] || 
                        mask[i][j+1] ||
                        mask[i][j-1]) & 
                        !mask[i][j])
                    {
                        connectedClumps->push_back(clumpIdxs[j]-1);
                    }
                }
            }
        }
        
        delete[] clumpIdxs;
        
        connectedClumps->sort();
        connectedClumps->unique();
    }
    
    RSGISRegionGrowAttributeTable::~RSGISRegionGrowAttributeTable()
    {
        
    }
   */
    
}}


