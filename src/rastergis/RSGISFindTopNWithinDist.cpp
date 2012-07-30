/*
 *  RSGISFindTopNWithinDist.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 28/07/2012.
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

#include "RSGISFindTopNWithinDist.h"

namespace rsgis{namespace rastergis{
	
    RSGISFindTopNWithinDist::RSGISFindTopNWithinDist()
    {
        
    }
    
    void RSGISFindTopNWithinDist::calcMinDistTopN(GDALDataset *dataset, std::string spatDistCol, std::string distCol, std::string outColumn, unsigned int n, float distThreshold)throw(rsgis::RSGISAttributeTableException)
    {
        try 
        {
            GDALRasterAttributeTable *attTable = new GDALRasterAttributeTable(*dataset->GetRasterBand(1)->GetDefaultRAT());
            
            int numRows = attTable->GetRowCount();
            
            if(numRows == 0)
            {
                rsgis::RSGISAttributeTableException("There is no attribute table present.");
            }
            
            int numColumns = attTable->GetColumnCount();
            int outColIdx = 0;
            int spatDistColIdx = 0;
            int distColIdx = 0;
            
            if(numColumns == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns on the attribute table.");
            }
            else
            {
                bool foundOutCol = false;
                bool foundSpatDistCol = false;
                bool foundDistCol = false;
                for(int i = 0; i < numColumns; ++i)
                {
                    if(!foundOutCol && (std::string(attTable->GetNameOfCol(i)) == outColumn))
                    {
                        foundOutCol = true;
                        outColIdx = i;
                    }
                    else if(!foundSpatDistCol && (std::string(attTable->GetNameOfCol(i)) == spatDistCol))
                    {
                        foundSpatDistCol = true;
                        spatDistColIdx = i;
                    }
                    else if(!foundDistCol && (std::string(attTable->GetNameOfCol(i)) == distCol))
                    {
                        foundDistCol = true;
                        distColIdx = i;
                    }
                }
                
                if(!foundSpatDistCol)
                {
                    throw rsgis::RSGISAttributeTableException("Could not find the spatial distance column within the attribute table.");
                }
                
                if(!foundDistCol)
                {
                    throw rsgis::RSGISAttributeTableException("Could not find the distance column within the attribute table.");
                }
                
                if(!foundOutCol)
                {
                    attTable->CreateColumn(outColumn.c_str(), GFT_Integer, GFU_Generic);
                    outColIdx = numColumns++;
                }
            }
            
            std::list<DistItem> topNItems;
            
            double dist = 0;
            bool foundPos = false;
            for(int i = 1; i < numRows; ++i)
            {
                attTable->SetValue(i, outColIdx, 0);
                if(attTable->GetValueAsDouble(i, spatDistColIdx) < distThreshold)
                {
                    dist = attTable->GetValueAsDouble(i, distColIdx);
                    foundPos = false;
                    for(std::list<DistItem>::iterator iterItems = topNItems.begin(); iterItems != topNItems.end(); ++iterItems)
                    {
                        if(dist < (*iterItems).dist)
                        {
                            topNItems.insert(iterItems, DistItem(i, dist));
                            foundPos = true;
                            break;
                        }
                    }
                    
                    if(!foundPos & (topNItems.size() < n))
                    {
                        topNItems.push_back(DistItem(i, dist));
                    }
                    
                    if(topNItems.size() > n)
                    {
                        topNItems.pop_back();
                    }
                }
            }
            
            int counter = 1;
            for(std::list<DistItem>::iterator iterItems = topNItems.begin(); iterItems != topNItems.end(); ++iterItems)
            {
                 attTable->SetValue((*iterItems).fid, outColIdx, counter++);
            }
            
            dataset->GetRasterBand(1)->SetDefaultRAT(attTable);
            dataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
        }
        catch (rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    RSGISFindTopNWithinDist::~RSGISFindTopNWithinDist()
    {
        
    }
	
}}


