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
        
    void RSGISCalcClusterLocation::populateAttWithClumpLocation(GDALDataset *dataset, unsigned int ratBand, std::string eastColumn, std::string northColumn)
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
            
            long minVal = 0;
            long maxVal = 0;
            attUtils.getImageBandMinMax(dataset, 1, &minVal, &maxVal);
            
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
    
    void RSGISCalcClusterLocation::populateAttWithClumpLocationExtent(GDALDataset *dataset, unsigned int ratBand, std::string minXColX, std::string minXColY, std::string maxXColX, std::string maxXColY, std::string minYColX, std::string minYColY, std::string maxYColX, std::string maxYColY)
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
            
            long minVal = 0;
            long maxVal = 0;
            attUtils.getImageBandMinMax(dataset, 1, &minVal, &maxVal);
            
            if(maxVal > numRows)
            {
                attTable->SetRowCount(maxVal+1);
                numRows = maxVal+1;
            }
            
            int minXColXIdx = attUtils.findColumnIndexOrCreate(attTable, minXColX, GFT_Real);
            int minXColYIdx = attUtils.findColumnIndexOrCreate(attTable, minXColY, GFT_Real);
            int maxXColXIdx = attUtils.findColumnIndexOrCreate(attTable, maxXColX, GFT_Real);
            int maxXColYIdx = attUtils.findColumnIndexOrCreate(attTable, maxXColY, GFT_Real);
            int minYColXIdx = attUtils.findColumnIndexOrCreate(attTable, minYColX, GFT_Real);
            int minYColYIdx = attUtils.findColumnIndexOrCreate(attTable, minYColY, GFT_Real);
            int maxYColXIdx = attUtils.findColumnIndexOrCreate(attTable, maxYColX, GFT_Real);
            int maxYColYIdx = attUtils.findColumnIndexOrCreate(attTable, maxYColY, GFT_Real);
            
            double **spatialLoc = new double*[numRows];
            bool *first = new bool[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                first[i] = true;
                spatialLoc[i] = new double[8];
                spatialLoc[i][0] = 0.0;
                spatialLoc[i][1] = 0.0;
                spatialLoc[i][2] = 0.0;
                spatialLoc[i][3] = 0.0;
                spatialLoc[i][4] = 0.0;
                spatialLoc[i][5] = 0.0;
                spatialLoc[i][6] = 0.0;
                spatialLoc[i][7] = 0.0;
            }
            
            RSGISCalcClusterExtentCalcValue *calcLoc = new RSGISCalcClusterExtentCalcValue(spatialLoc, first, ratBand);
            rsgis::img::RSGISCalcImage calcImage(calcLoc);
            calcImage.calcImageExtent(&dataset, 1, 0);
            delete calcLoc;
            
            std::cout << "Writing data to output RAT\n";
            size_t numBlocks = floor((double)numRows/(double)RAT_BLOCK_LENGTH);
            size_t rowsRemain = numRows - (numBlocks * RAT_BLOCK_LENGTH);
            
            rsgis::math::RSGISMathsUtils mathUtils;
            double *dataMinXXBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMinXYBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMaxXXBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMaxXYBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMinYXBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMinYYBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMaxYXBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMaxYYBlock = new double[RAT_BLOCK_LENGTH];
            size_t startRow = 0;
            size_t rowID = 0;
            for(size_t i = 0; i < numBlocks; ++i)
            {
                rowID = startRow;
                for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                {
                    dataMinXXBlock[j] = spatialLoc[rowID][0];
                    dataMinXYBlock[j] = spatialLoc[rowID][1];
                    dataMaxXXBlock[j] = spatialLoc[rowID][2];
                    dataMaxXYBlock[j] = spatialLoc[rowID][3];
                    dataMinYXBlock[j] = spatialLoc[rowID][4];
                    dataMinYYBlock[j] = spatialLoc[rowID][5];
                    dataMaxYXBlock[j] = spatialLoc[rowID][6];
                    dataMaxYYBlock[j] = spatialLoc[rowID][7];
                    ++rowID;
                }
                attTable->ValuesIO(GF_Write, minXColXIdx, startRow, RAT_BLOCK_LENGTH, dataMinXXBlock);
                attTable->ValuesIO(GF_Write, minXColYIdx, startRow, RAT_BLOCK_LENGTH, dataMinXYBlock);
                attTable->ValuesIO(GF_Write, maxXColXIdx, startRow, RAT_BLOCK_LENGTH, dataMaxXXBlock);
                attTable->ValuesIO(GF_Write, maxXColYIdx, startRow, RAT_BLOCK_LENGTH, dataMaxXYBlock);
                attTable->ValuesIO(GF_Write, minYColXIdx, startRow, RAT_BLOCK_LENGTH, dataMinYXBlock);
                attTable->ValuesIO(GF_Write, minYColYIdx, startRow, RAT_BLOCK_LENGTH, dataMinYYBlock);
                attTable->ValuesIO(GF_Write, maxYColXIdx, startRow, RAT_BLOCK_LENGTH, dataMaxYXBlock);
                attTable->ValuesIO(GF_Write, maxYColYIdx, startRow, RAT_BLOCK_LENGTH, dataMaxYYBlock);
                
                startRow += RAT_BLOCK_LENGTH;
            }
            if(rowsRemain > 0)
            {
                rowID = startRow;
                for(size_t j = 0; j < rowsRemain; ++j)
                {
                    dataMinXXBlock[j] = spatialLoc[rowID][0];
                    dataMinXYBlock[j] = spatialLoc[rowID][1];
                    dataMaxXXBlock[j] = spatialLoc[rowID][2];
                    dataMaxXYBlock[j] = spatialLoc[rowID][3];
                    dataMinYXBlock[j] = spatialLoc[rowID][4];
                    dataMinYYBlock[j] = spatialLoc[rowID][5];
                    dataMaxYXBlock[j] = spatialLoc[rowID][6];
                    dataMaxYYBlock[j] = spatialLoc[rowID][7];
                    ++rowID;
                }
                attTable->ValuesIO(GF_Write, minXColXIdx, startRow, rowsRemain, dataMinXXBlock);
                attTable->ValuesIO(GF_Write, minXColYIdx, startRow, rowsRemain, dataMinXYBlock);
                attTable->ValuesIO(GF_Write, maxXColXIdx, startRow, rowsRemain, dataMaxXXBlock);
                attTable->ValuesIO(GF_Write, maxXColYIdx, startRow, rowsRemain, dataMaxXYBlock);
                attTable->ValuesIO(GF_Write, minYColXIdx, startRow, rowsRemain, dataMinYXBlock);
                attTable->ValuesIO(GF_Write, minYColYIdx, startRow, rowsRemain, dataMinYYBlock);
                attTable->ValuesIO(GF_Write, maxYColXIdx, startRow, rowsRemain, dataMaxYXBlock);
                attTable->ValuesIO(GF_Write, maxYColYIdx, startRow, rowsRemain, dataMaxYYBlock);
            }
            
            delete[] dataMinXXBlock;
            delete[] dataMinXYBlock;
            delete[] dataMaxXXBlock;
            delete[] dataMaxXYBlock;
            delete[] dataMinYXBlock;
            delete[] dataMinYYBlock;
            delete[] dataMaxYXBlock;
            delete[] dataMaxYYBlock;
            
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
    
    void RSGISCalcClusterLocation::populateAttWithClumpPxlLocation(GDALDataset *dataset, unsigned int ratBand, std::string minXCol, std::string maxXCol, std::string minYCol, std::string maxYCol)
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
            
            long minVal = 0;
            long maxVal = 0;
            attUtils.getImageBandMinMax(dataset, 1, &minVal, &maxVal);
            
            if(maxVal > numRows)
            {
                attTable->SetRowCount(maxVal+1);
                numRows = maxVal+1;
            }
            
            int minXIdx = attUtils.findColumnIndexOrCreate(attTable, minXCol, GFT_Real);
            int maxXIdx = attUtils.findColumnIndexOrCreate(attTable, maxXCol, GFT_Real);
            int minYIdx = attUtils.findColumnIndexOrCreate(attTable, minYCol, GFT_Real);
            int maxYIdx = attUtils.findColumnIndexOrCreate(attTable, maxYCol, GFT_Real);
            
            unsigned long **pxlLoc = new unsigned long*[numRows];
            bool *first = new bool[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                first[i] = true;
                pxlLoc[i] = new unsigned long[4];
                pxlLoc[i][0] = 0;
                pxlLoc[i][1] = 0;
                pxlLoc[i][2] = 0;
                pxlLoc[i][3] = 0;
            }
            
            RSGISCalcClusterPxlExtentCalcValue *calcLoc = new RSGISCalcClusterPxlExtentCalcValue(pxlLoc, first, ratBand);
            rsgis::img::RSGISCalcImage calcImage(calcLoc);
            calcImage.calcImagePosPxl(&dataset, 1, 0);
            delete calcLoc;
            
            std::cout << "Writing data to output RAT\n";
            size_t numBlocks = floor((double)numRows/(double)RAT_BLOCK_LENGTH);
            size_t rowsRemain = numRows - (numBlocks * RAT_BLOCK_LENGTH);
            
            rsgis::math::RSGISMathsUtils mathUtils;
            double *dataMinXBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMaxXBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMinYBlock = new double[RAT_BLOCK_LENGTH];
            double *dataMaxYBlock = new double[RAT_BLOCK_LENGTH];
            size_t startRow = 0;
            size_t rowID = 0;
            for(size_t i = 0; i < numBlocks; ++i)
            {
                rowID = startRow;
                for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                {
                    dataMinXBlock[j] = pxlLoc[rowID][0];
                    dataMinYBlock[j] = pxlLoc[rowID][1];
                    dataMaxXBlock[j] = pxlLoc[rowID][2];
                    dataMaxYBlock[j] = pxlLoc[rowID][3];
                    ++rowID;
                }
                attTable->ValuesIO(GF_Write, minXIdx, startRow, RAT_BLOCK_LENGTH, dataMinXBlock);
                attTable->ValuesIO(GF_Write, minYIdx, startRow, RAT_BLOCK_LENGTH, dataMinYBlock);
                attTable->ValuesIO(GF_Write, maxXIdx, startRow, RAT_BLOCK_LENGTH, dataMaxXBlock);
                attTable->ValuesIO(GF_Write, maxYIdx, startRow, RAT_BLOCK_LENGTH, dataMaxYBlock);
                
                startRow += RAT_BLOCK_LENGTH;
            }
            if(rowsRemain > 0)
            {
                rowID = startRow;
                for(size_t j = 0; j < rowsRemain; ++j)
                {
                    dataMinXBlock[j] = pxlLoc[rowID][0];
                    dataMinYBlock[j] = pxlLoc[rowID][1];
                    dataMaxXBlock[j] = pxlLoc[rowID][2];
                    dataMaxYBlock[j] = pxlLoc[rowID][3];
                    ++rowID;
                }
                attTable->ValuesIO(GF_Write, minXIdx, startRow, rowsRemain, dataMinXBlock);
                attTable->ValuesIO(GF_Write, minYIdx, startRow, rowsRemain, dataMinYBlock);
                attTable->ValuesIO(GF_Write, maxXIdx, startRow, rowsRemain, dataMaxXBlock);
                attTable->ValuesIO(GF_Write, maxYIdx, startRow, rowsRemain, dataMaxYBlock);
            }
            
            delete[] dataMinXBlock;
            delete[] dataMinYBlock;
            delete[] dataMaxXBlock;
            delete[] dataMaxYBlock;
            
            for(size_t i = 0; i < numRows; ++i)
            {
                delete[] pxlLoc[i];
            }
            delete[] pxlLoc;
            
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
    
    void RSGISCalcClusterLocationCalcValue::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent) 
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
    
    
    
    RSGISCalcClusterExtentCalcValue::RSGISCalcClusterExtentCalcValue(double **spatialLoc, bool *first, unsigned int ratBand): rsgis::img::RSGISCalcImageValue(0)
    {
        this->spatialLoc = spatialLoc;
        this->first = first;
        this->ratBand = ratBand;
    }
    
    void RSGISCalcClusterExtentCalcValue::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent) 
    {
        if(intBandValues[ratBand-1] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(intBandValues[ratBand-1]);
            
            if(first[fid])
            {
                spatialLoc[fid][0] = extent.getMinX(); // minX_X TLX
                spatialLoc[fid][1] = extent.getMaxY(); // minX_Y TLY
                spatialLoc[fid][2] = extent.getMaxX(); // maxX_X BRX
                spatialLoc[fid][3] = extent.getMinY(); // maxX_Y BRX
                spatialLoc[fid][4] = extent.getMinX(); // minY_X BLX
                spatialLoc[fid][5] = extent.getMinY(); // minY_Y BLY
                spatialLoc[fid][6] = extent.getMaxX(); // maxY_X TRX
                spatialLoc[fid][7] = extent.getMaxY(); // maxY_Y TRY
                first[fid] = false;
            }
            else
            {
                if(extent.getMinX() < spatialLoc[fid][0])
                {
                    spatialLoc[fid][0] = extent.getMinX(); // minX_X TLX
                    spatialLoc[fid][1] = extent.getMaxY(); // minX_Y TLY
                }
                
                if(extent.getMaxX() > spatialLoc[fid][2])
                {
                    spatialLoc[fid][2] = extent.getMaxX(); // maxX_X BRX
                    spatialLoc[fid][3] = extent.getMinY(); // maxX_Y BRX
                }
                
                if(extent.getMinY() < spatialLoc[fid][5])
                {
                    spatialLoc[fid][4] = extent.getMinX(); // minY_X BLX
                    spatialLoc[fid][5] = extent.getMinY(); // minY_Y BLY
                }
                
                if(extent.getMaxY() > spatialLoc[fid][7])
                {
                    spatialLoc[fid][6] = extent.getMaxX(); // maxY_X TRX
                    spatialLoc[fid][7] = extent.getMaxY(); // maxY_Y TRY
                }
            }
        }
    }
    
    RSGISCalcClusterExtentCalcValue::~RSGISCalcClusterExtentCalcValue()
    {
        
    }
    
    
    RSGISCalcClusterPxlExtentCalcValue::RSGISCalcClusterPxlExtentCalcValue(unsigned long **pxlLoc, bool *first, unsigned int ratBand): rsgis::img::RSGISCalcImageValue(0)
    {
        this->pxlLoc = pxlLoc;
        this->first = first;
        this->ratBand = ratBand;
    }
    
    void RSGISCalcClusterPxlExtentCalcValue::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent) 
    {
        if(intBandValues[ratBand-1] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(intBandValues[ratBand-1]);
            
            if(first[fid])
            {
                pxlLoc[fid][0] = extent.getMinX(); // minX
                pxlLoc[fid][1] = extent.getMinY(); // minY
                pxlLoc[fid][2] = extent.getMaxX(); // maxX
                pxlLoc[fid][3] = extent.getMaxY(); // maxY
                first[fid] = false;
            }
            else
            {
                if(extent.getMinX() < pxlLoc[fid][0])
                {
                    pxlLoc[fid][0] = extent.getMinX(); // minX
                }
                
                if(extent.getMaxX() > pxlLoc[fid][2])
                {
                    pxlLoc[fid][2] = extent.getMaxX(); // maxX
                }
                
                if(extent.getMinY() < pxlLoc[fid][1])
                {
                    pxlLoc[fid][1] = extent.getMinY(); // minY
                }
                
                if(extent.getMaxY() > pxlLoc[fid][3])
                {
                    pxlLoc[fid][3] = extent.getMaxY(); // maxY
                }
            }
        }
    }
    
    RSGISCalcClusterPxlExtentCalcValue::~RSGISCalcClusterPxlExtentCalcValue()
    {
        
    }
	
}}




