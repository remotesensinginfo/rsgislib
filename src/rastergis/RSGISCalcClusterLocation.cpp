/*
 *  RSGISCalcClusterLocation.cpp
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

#include "RSGISCalcClusterLocation.h"

namespace rsgis{namespace rastergis{
	
    RSGISCalcClusterLocation::RSGISCalcClusterLocation()
    {
        
    }
        
    void RSGISCalcClusterLocation::populateAttWithClumpLocation(GDALDataset *dataset, std::string eastColumn, std::string northColumn)throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            GDALRasterAttributeTable *attTable = new GDALRasterAttributeTable(*dataset->GetRasterBand(1)->GetDefaultRAT());
            
            int numRows = attTable->GetRowCount();
            
            if(numRows == 0)
            {
                double minVal = 0;
                double maxVal = 0;
                dataset->GetRasterBand(1)->GetStatistics(false, true, &minVal, &maxVal, NULL, NULL);
                
                attTable->SetRowCount(maxVal+1);
            }
            
            int numColumns = attTable->GetColumnCount();
            int countColIdx = 0;
            int eastColIdx = 0;
            int northColIdx = 0;
            if(numColumns == 0)
            {
                attTable->CreateColumn ("Count", GFT_Integer, GFU_PixelCount);
                countColIdx = 0;
                attTable->CreateColumn(eastColumn.c_str(), GFT_Real, GFU_Generic);
                eastColIdx = 1;
                attTable->CreateColumn(northColumn.c_str(), GFT_Real, GFU_Generic);
                northColIdx = 2;
            }
            else
            {
                bool foundCount = 0;
                bool foundEastings = 0;
                bool foundNorthings = 0;
                for(int i = 0; i < numColumns; ++i)
                {
                    if(!foundCount && (std::string(attTable->GetNameOfCol(i)) == std::string("Count")))
                    {
                        foundCount = true;
                        countColIdx = i;
                    }
                    else if(!foundEastings && (std::string(attTable->GetNameOfCol(i)) == eastColumn))
                    {
                        foundEastings = true;
                        eastColIdx = i;
                    }
                    else if(!foundNorthings && (std::string(attTable->GetNameOfCol(i)) == northColumn))
                    {
                        foundNorthings = true;
                        northColIdx = i;
                    }
                }
                
                if(!foundCount)
                {
                     attTable->CreateColumn ("Count", GFT_Integer, GFU_PixelCount);
                    countColIdx = numColumns++;
                }
                
                if(!foundEastings)
                {
                    attTable->CreateColumn(eastColumn.c_str(), GFT_Real, GFU_Generic);
                    eastColIdx = numColumns++;
                }
                
                if(!foundNorthings)
                {
                    attTable->CreateColumn(northColumn.c_str(), GFT_Real, GFU_Generic);
                    northColIdx = numColumns++;
                }
            }
            
            for(int i = 0; i < numRows; ++i)
            {
                attTable->SetValue(i, countColIdx, 0);
                attTable->SetValue(i, eastColIdx, 0.0);
                attTable->SetValue(i, northColIdx, 0.0);
            }
            
            RSGISCalcClusterLocationCalcValue *calcLoc = new RSGISCalcClusterLocationCalcValue(attTable, countColIdx, eastColIdx, northColIdx);
            rsgis::img::RSGISCalcImage calcImage(calcLoc);
            
            calcImage.calcImageExtent(&dataset, 1);
            
            delete calcLoc;
            
            for(int i = 0; i < numRows; ++i)
            {
                attTable->SetValue(i, eastColIdx, attTable->GetValueAsDouble(i, eastColIdx)/attTable->GetValueAsDouble(i, countColIdx));
                attTable->SetValue(i, northColIdx, attTable->GetValueAsDouble(i, northColIdx)/attTable->GetValueAsDouble(i, countColIdx));
            }
            
            dataset->GetRasterBand(1)->SetDefaultRAT(attTable);
            delete attTable;
        }
        catch(RSGISException &e)
        {
            throw rsgis::RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISCalcClusterLocation::~RSGISCalcClusterLocation()
    {
        
    }
    
    
    
    RSGISCalcClusterLocationCalcValue::RSGISCalcClusterLocationCalcValue(GDALRasterAttributeTable *attTable, int countColIdx, int eastColIdx, int northColIdx): rsgis::img::RSGISCalcImageValue(0)
    {
        this->attTable = attTable;
        this->countColIdx = countColIdx;
        this->eastColIdx = eastColIdx;
        this->northColIdx = northColIdx;
    }
    
    void RSGISCalcClusterLocationCalcValue::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            unsigned long clumpID = boost::lexical_cast<unsigned long>(bandValues[0]);
            
            attTable->SetValue(clumpID, countColIdx, attTable->GetValueAsInt(clumpID, countColIdx)+1);
                        
            double eastings = extent.getMinX() + extent.getWidth()/2;
            double northings = extent.getMaxY() - extent.getHeight()/2;
            
            attTable->SetValue(clumpID, eastColIdx, attTable->GetValueAsDouble(clumpID, eastColIdx)+eastings);
            attTable->SetValue(clumpID, northColIdx, attTable->GetValueAsDouble(clumpID, northColIdx)+northings);
        }
    }
    
    RSGISCalcClusterLocationCalcValue::~RSGISCalcClusterLocationCalcValue()
    {
        
    }
	
}}




