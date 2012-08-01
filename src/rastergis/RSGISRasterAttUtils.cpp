/*
 *  RSGISRasterAttUtils.cpp
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

#include "RSGISRasterAttUtils.h"

namespace rsgis{namespace rastergis{
	
    RSGISRasterAttUtils::RSGISRasterAttUtils()
    {
        
    }
    
    void RSGISRasterAttUtils::copyAttColumns(GDALDataset *inImage, GDALDataset *outImage, std::vector<std::string> fields) throw(RSGISAttributeTableException)
    {
        
        try 
        {
            std::cout << "Import attribute tables to memory.\n";
            const GDALRasterAttributeTable *gdalAttIn = inImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *gdalAttOut = NULL;//new GDALRasterAttributeTable(*outImage->GetRasterBand(1)->GetDefaultRAT());
            const GDALRasterAttributeTable *gdalAttOutTmp = outImage->GetRasterBand(1)->GetDefaultRAT();
            
            if((gdalAttOutTmp == NULL) || (gdalAttOutTmp->GetRowCount() == 0))
            {
                gdalAttOut = new GDALRasterAttributeTable();
            }
            else
            {
                gdalAttOut = new GDALRasterAttributeTable(*gdalAttOutTmp);
            }
            
            if(gdalAttIn->GetRowCount() > gdalAttOut->GetRowCount())
            {
                gdalAttOut->SetRowCount(gdalAttIn->GetRowCount());
            }
            
            std::cout << "Find field column indexes and created columns were required.\n";
            bool *foundInIdx = new bool[fields.size()];
            int *colInIdxs = new int[fields.size()];
            bool *foundOutIdx = new bool[fields.size()];
            int *colOutIdxs = new int[fields.size()];
            for(size_t i = 0; i < fields.size(); ++i)
            {
                foundInIdx[i] = false;
                colInIdxs[i] = 0;
                foundOutIdx[i] = false;
                colOutIdxs[i] = 0;
            }
            
            if(gdalAttIn->GetRowCount() == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
            }
            else
            {
                for(int i = 0; i < gdalAttIn->GetColumnCount(); ++i)
                {
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(!foundInIdx[j] && (std::string(gdalAttIn->GetNameOfCol(i)) == fields.at(j)))
                        {
                            colInIdxs[j] = i;
                            foundInIdx[j] = true;
                        }
                    }
                }
                
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(!foundInIdx[j])
                    {
                        std::string message = std::string("Column ") + fields.at(j) + std::string(" is not within the input attribute table.");
                        throw rsgis::RSGISAttributeTableException(message);
                    }
                }
                
                
                for(int i = 0; i < gdalAttOut->GetColumnCount(); ++i)
                {
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(!foundOutIdx[j] && (std::string(gdalAttOut->GetNameOfCol(i)) == fields.at(j)))
                        {
                            colOutIdxs[j] = i;
                            foundOutIdx[j] = true;
                        }
                    }
                }
                
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(!foundOutIdx[j])
                    {
                        if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                        {
                            gdalAttOut->CreateColumn(fields.at(j).c_str(), GFT_Integer, GFU_Generic);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                        {
                            gdalAttOut->CreateColumn(fields.at(j).c_str(), GFT_Real, GFU_Generic);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                        {
                            gdalAttOut->CreateColumn(fields.at(j).c_str(), GFT_String, GFU_Generic);
                        }
                        else
                        {
                            throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                        }
                        colOutIdxs[j] = gdalAttOut->GetColumnCount()-1;
                        foundOutIdx[j] = true;
                    }
                }
            }
            
            std::cout << "Copying columns to the new attribute table\n";
            for(int i = 0; i < gdalAttIn->GetRowCount(); ++i)
            {
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                    {
                        gdalAttOut->SetValue(i, colOutIdxs[j], gdalAttIn->GetValueAsInt(i, colInIdxs[j]));
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                    {
                        gdalAttOut->SetValue(i, colOutIdxs[j], gdalAttIn->GetValueAsDouble(i, colInIdxs[j]));
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                    {
                        gdalAttOut->SetValue(i, colOutIdxs[j], gdalAttIn->GetValueAsString(i, colInIdxs[j]));
                    }
                    else
                    {
                        throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                    }
                }
            }
            
            std::cout << "Adding RAT to output file.\n";
            outImage->GetRasterBand(1)->SetDefaultRAT(gdalAttOut);
            
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        
        
    }
    
    
    RSGISRasterAttUtils::~RSGISRasterAttUtils()
    {
        
    }
	
}}



