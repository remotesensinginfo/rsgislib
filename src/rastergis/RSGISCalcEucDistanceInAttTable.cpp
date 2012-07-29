/*
 *  RSGISCalcEucDistanceInAttTable.cpp
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

#include "RSGISCalcEucDistanceInAttTable.h"


namespace rsgis{namespace rastergis{
	

    RSGISCalcEucDistanceInAttTable::RSGISCalcEucDistanceInAttTable()
    {
        
    }
    
    void RSGISCalcEucDistanceInAttTable::calcEucDist(GDALDataset *dataset, size_t fid, std::string outColumn, std::vector<std::string> inColumns)throw(rsgis::RSGISAttributeTableException)
    {
        GDALRasterAttributeTable *attTable = new GDALRasterAttributeTable(*dataset->GetRasterBand(1)->GetDefaultRAT());
        
        int numRows = attTable->GetRowCount();
        
        if(numRows == 0)
        {
            rsgis::RSGISAttributeTableException("There is no attribute table present.");
        }
        
        int numColumns = attTable->GetColumnCount();
        int outColIdx = 0;
        bool *foundIdx = new bool[inColumns.size()];
        int *colIdxs = new int[inColumns.size()];
        for(size_t i = 0; i < inColumns.size(); ++i)
        {
            foundIdx[i] = false;
            colIdxs[i] = 0;
        }
        if(numColumns == 0)
        {
            rsgis::RSGISAttributeTableException("There are no columns on the attribute table.");
        }
        else
        {
            bool foundOutCol = false;
            for(int i = 0; i < numColumns; ++i)
            {
                if(!foundOutCol && (std::string(attTable->GetNameOfCol(i)) == outColumn))
                {
                    std::cout << "Out col found = " << i << std::endl;
                    foundOutCol = true;
                    outColIdx = i;
                }
                else
                {
                    for(size_t j = 0; j < inColumns.size(); ++j)
                    {
                        if(!foundIdx[j] && (std::string(attTable->GetNameOfCol(i)) == inColumns.at(j)))
                        {
                            colIdxs[j] = i;
                            foundIdx[j] = true;
                        }
                    }
                }
            }
            
            if(!foundOutCol)
            {
                attTable->CreateColumn(outColumn.c_str(), GFT_Real, GFU_Generic);
                outColIdx = numColumns++;
            }
                        
            for(size_t j = 0; j < inColumns.size(); ++j)
            {
                if(!foundIdx[j])
                {
                    std::string message = std::string("Column ") + inColumns.at(j) + std::string(" is not within the attribute table.");
                    throw rsgis::RSGISAttributeTableException(message);
                }
            }
        }
        
        std::vector<double> valsRef;
        for(size_t j = 0; j < inColumns.size(); ++j)
        {
            valsRef.push_back(attTable->GetValueAsDouble(fid, colIdxs[j]));
        }
        
        std::vector<double> vals;
        for(int i = 0; i < numRows; ++i)
        {
            //std::cout << "i = " << i;
            for(size_t j = 0; j < inColumns.size(); ++j)
            {
                vals.push_back(attTable->GetValueAsDouble(i, colIdxs[j]));
            }
            
            //std::cout << "\t" << this->getEuclideanDistance(valsRef, vals);
            
            attTable->SetValue(i, outColIdx, this->getEuclideanDistance(valsRef, vals));
            vals.clear();
            //std::cout << std::endl;
        }
        
        dataset->GetRasterBand(1)->SetDefaultRAT(attTable);
    }
    
    double RSGISCalcEucDistanceInAttTable::getEuclideanDistance(std::vector<double> vals1, std::vector<double> vals2)throw(rsgis::math::RSGISMathException)
    {
        double dist = 0;
        
        if(vals1.size() != vals2.size())
        {
            throw rsgis::math::RSGISMathException("Value vectors are different sizes.");
        }
        
        size_t numVals = vals1.size();
        for(size_t i = 0; i < numVals; ++i)
        {
            dist += pow((vals1[i]-vals2[i]), 2);
        }
        
        return sqrt(dist/((double)numVals));
    }
    
    RSGISCalcEucDistanceInAttTable::~RSGISCalcEucDistanceInAttTable()
    {
        
    }
	
}}