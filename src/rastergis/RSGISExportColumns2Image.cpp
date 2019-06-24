/*
 *  RSGISExportColumns2Image.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/09/2012.
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

#include "RSGISExportColumns2Image.h"

namespace rsgis{namespace rastergis{
    

    RSGISExportColumns2ImageCalcImage::RSGISExportColumns2ImageCalcImage(int numberOutBands, GDALRasterAttributeTable *attTable, unsigned int columnIndex): rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->attTable = attTable;
        this->nRows = attTable->GetRowCount();
        if(this->nRows == 0)
        {
            rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
        }
        this->columnData = new double[this->nRows]; // Set up array to hold column from RAT
        
        // Load column to memory
        loadColumn(columnIndex);
    }
    
    void RSGISExportColumns2ImageCalcImage::loadColumn(int columnIndex)
    {
        if(this->attTable->GetTypeOfCol(columnIndex) == GFT_String)
        {
            throw rsgis::RSGISAttributeTableException("Can't export a column containing strings to an image");
        }
        
        // Itterate through blocks
        int nRows = this->attTable ->GetRowCount();
        int nBlocks = floor(((double) nRows) / ((double) RAT_BLOCK_LENGTH));
        int remainRows = nRows - (nBlocks * RAT_BLOCK_LENGTH );
        
        double *blockDataReal = new double[RAT_BLOCK_LENGTH];
        
        int feedback = nRows/10.0;
        int feedbackCounter = 0;
        
        std::cout << "Reading column to memory" << std::endl;
        std::cout << "Started " << std::flush;
        
        int rowOffset = 0;
        for(int i = 0; i < nBlocks; i++)
        {
            rowOffset =  RAT_BLOCK_LENGTH * i;
            
            // Read block
            this->attTable->ValuesIO(GF_Read, columnIndex, rowOffset, RAT_BLOCK_LENGTH, blockDataReal);
            
            // Loop through block
            
            for(int m = 0; m < RAT_BLOCK_LENGTH; ++m)
            {
                // Show progress
                if((feedback != 0) && (((i*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                this->columnData[(i*RAT_BLOCK_LENGTH)+m] = blockDataReal[m];
            }
            
        }
        if(remainRows > 0)
        {
            rowOffset =  RAT_BLOCK_LENGTH * nBlocks;
            
            // Read block
            this->attTable->ValuesIO(GF_Read, columnIndex, rowOffset, remainRows, blockDataReal);
            
            // Loop through block
            
            for(int m = 0; m < remainRows; ++m)
            {
                // Show progress
                if((feedback != 0) && (((nBlocks*RAT_BLOCK_LENGTH)+m) % feedback) == 0)
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                this->columnData[(nBlocks*RAT_BLOCK_LENGTH)+m] = blockDataReal[m];
            }
        }
        std::cout << ".Completed\n";
    }
    
    void RSGISExportColumns2ImageCalcImage::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
    {
        if(intBandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(intBandValues[0]);
            output[0] = columnData[fid];
        }
        else
        {
            output[0] = 0;
        }
    }
    RSGISExportColumns2ImageCalcImage::~RSGISExportColumns2ImageCalcImage()
    {
        delete[] this->columnData;
    }

}}





