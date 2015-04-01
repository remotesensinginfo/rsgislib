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
        
    void RSGISCalcClusterLocation::populateAttWithClumpLocation(GDALDataset *dataset, unsigned int ratBand, std::string eastColumn, std::string northColumn)throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            if(ratBand == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT Band must be greater than zero.");
            }
            if(ratBand > dataset->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("RAT Band is larger than the number of bands within the image.");
            }
            RSGISRasterAttUtils attUtils;
            
            GDALRasterAttributeTable *attTable = dataset->GetRasterBand(ratBand)->GetDefaultRAT();
            
            size_t numRows = attTable->GetRowCount();
            
            double maxVal = 0;
            int nLastProgress = -1;
            dataset->GetRasterBand(ratBand)->ComputeStatistics(false, NULL, &maxVal, NULL, NULL,  (GDALProgressFunc)RSGISRATStatsTextProgress, &nLastProgress);
            
            if(maxVal > numRows)
            {                
                attTable->SetRowCount(maxVal+1);
                numRows = maxVal+1;
            }
            
            int histColIdx = attUtils.findColumnIndex(attTable, "Histogram");
            int eastColIdx = attUtils.findColumnIndexOrCreate(attTable, eastColumn, GFT_Real);
            int northColIdx = attUtils.findColumnIndexOrCreate(attTable, northColumn, GFT_Real);
            
            double **spatialLoc = new double*[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                spatialLoc[i] = new double[2];
                spatialLoc[i][0] = 0.0;
                spatialLoc[i][1] = 0.0;
            }
            
            RSGISCalcClusterLocationCalcValue *calcLoc = new RSGISCalcClusterLocationCalcValue(spatialLoc, ratBand);
            rsgis::img::RSGISCalcImage calcImage(calcLoc);
            calcImage.calcImageExtent(&dataset, 1, 0);
            delete calcLoc;
            
            std::cout << "Writing data to output RAT\n";
            size_t numBlocks = floor((double)numRows/(double)RAT_BLOCK_LENGTH);
            size_t rowsRemain = numRows - (numBlocks * RAT_BLOCK_LENGTH);
            
            rsgis::math::RSGISMathsUtils mathUtils;
            double *dataEastBlock = new double[RAT_BLOCK_LENGTH];
            double *dataNorthBlock = new double[RAT_BLOCK_LENGTH];
            double *histDataBlock = new double[RAT_BLOCK_LENGTH];
            size_t startRow = 0;
            size_t rowID = 0;
            for(size_t i = 0; i < numBlocks; ++i)
            {
                attTable->ValuesIO(GF_Read, histColIdx, startRow, RAT_BLOCK_LENGTH, histDataBlock);
                rowID = startRow;
                for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                {
                    if(histDataBlock[j] > 0)
                    {
                        dataEastBlock[j] = spatialLoc[rowID][0] / histDataBlock[j];
                        dataNorthBlock[j] = spatialLoc[rowID][1] / histDataBlock[j];
                    }
                    else
                    {
                        dataEastBlock[j] = 0.0;
                        dataNorthBlock[j] = 0.0;
                    }
                    ++rowID;
                }
                attTable->ValuesIO(GF_Write, eastColIdx, startRow, RAT_BLOCK_LENGTH, dataEastBlock);
                attTable->ValuesIO(GF_Write, northColIdx, startRow, RAT_BLOCK_LENGTH, dataNorthBlock);
                
                
                startRow += RAT_BLOCK_LENGTH;
            }
            if(rowsRemain > 0)
            {
                attTable->ValuesIO(GF_Read, histColIdx, startRow, rowsRemain, histDataBlock);
                rowID = startRow;
                for(size_t j = 0; j < rowsRemain; ++j)
                {
                    if(histDataBlock[j] > 0)
                    {
                        dataEastBlock[j] = spatialLoc[rowID][0] / histDataBlock[j];
                        dataNorthBlock[j] = spatialLoc[rowID][1] / histDataBlock[j];
                    }
                    else
                    {
                        dataEastBlock[j] = 0.0;
                        dataNorthBlock[j] = 0.0;
                    }
                    ++rowID;
                }
                attTable->ValuesIO(GF_Write, eastColIdx, startRow, rowsRemain, dataEastBlock);
                attTable->ValuesIO(GF_Write, northColIdx, startRow, rowsRemain, dataNorthBlock);
            }
            
            delete[] histDataBlock;
            delete[] dataEastBlock;
            delete[] dataNorthBlock;
            
            for(size_t i = 0; i < numRows; ++i)
            {
                delete[] spatialLoc[i];
            }
            delete[] spatialLoc;
            
            dataset->GetRasterBand(ratBand)->SetMetadataItem("LAYER_TYPE", "thematic");
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISCalcClusterLocation::~RSGISCalcClusterLocation()
    {
        
    }
    
    
    
    RSGISCalcClusterLocationCalcValue::RSGISCalcClusterLocationCalcValue(double **spatialLoc, unsigned int ratBand): rsgis::img::RSGISCalcImageValue(0)
    {
        this->spatialLoc = spatialLoc;
        this->ratBand = ratBand;
    }
    
    void RSGISCalcClusterLocationCalcValue::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
    {
        if(intBandValues[ratBand-1] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(intBandValues[ratBand-1]);
            
            double eastings = extent.getMinX() + extent.getWidth()/2;
            double northings = extent.getMinY() + extent.getHeight()/2;
            
            spatialLoc[fid][0] += eastings;
            spatialLoc[fid][1] += northings;
        }
    }
    
    RSGISCalcClusterLocationCalcValue::~RSGISCalcClusterLocationCalcValue()
    {
        
    }
	
}}




