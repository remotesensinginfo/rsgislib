/*
 *  RSGISDefineClumpsInTiles.cpp
 *  RSGIS_LIB
 *
 *  Created by John Armston on 19/10/2014.
 *  Copyright 2014 RSGISLib.
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
 
#include "RSGISDefineClumpsInTiles.h"

namespace rsgis{namespace rastergis{
    
    RSGISDefineClumpsInTiles::RSGISDefineClumpsInTiles()
    {
        
    }
    
    void RSGISDefineClumpsInTiles::defineSegmentTilePos(GDALDataset *clumpsDataset, GDALDataset *tileDataset, std::string outColName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody) throw(RSGISImageException, RSGISAttributeTableException)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            
            // Get Attribute table
            GDALRasterAttributeTable *attTable = clumpsDataset->GetRasterBand(1)->GetDefaultRAT();
            
            size_t numRows = attTable->GetRowCount();
            if(numRows == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT has no rows, i.e., it is empty!");
            }

            // Make sure it is long enough and extend if required.
            long maxVal = 0;
            long minVal = 0;
            attUtils.getImageBandMinMax(clumpsDataset, 1, &minVal, &maxVal);
            
            if(maxVal > numRows)
            {
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
            RSGISSegTilePos *clumpTilePos = new RSGISSegTilePos[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                clumpTilePos[i].overlap = false;
                clumpTilePos[i].boundary = false;
                clumpTilePos[i].body = false;
            }
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpsDataset;
            datasets[1] = tileDataset;
            
            RSGISFindClumpPositionsInTile *calcImgValStats = new RSGISFindClumpPositionsInTile(numRows, clumpTilePos, tileOverlap, tileBoundary, tileBody);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgValStats);
            calcImageStats.calcImage(datasets, 2);
            delete calcImgValStats;         

            delete[] datasets;
            
            int *colVals = new int[numRows];
            for(size_t i = 1; i < numRows; ++i)
            {
                if(clumpTilePos[i].boundary)
                {
                    colVals[i] = (int)tileBoundary;
                }
                else if(clumpTilePos[i].overlap & clumpTilePos[i].body)
                {
                    colVals[i] = (int)tileBoundary; // Just in case the boundary is not defined.
                }
                else if(clumpTilePos[i].overlap)
                {
                    colVals[i] = (int)tileOverlap;                  
                }
                else if(clumpTilePos[i].body)
                {
                    colVals[i] = (int)tileBody;
                }
            }
            
            attUtils.writeIntColumn(attTable, outColName, colVals, numRows);
            
            delete[] clumpTilePos;
            delete[] colVals;
            
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }

    void RSGISDefineClumpsInTiles::defineBorderSegmentsUsingMask(GDALDataset *clumpsDataset, GDALDataset *maskDataset, std::string outColName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody) throw(RSGISImageException, RSGISAttributeTableException)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            
            // Get Attribute table
            GDALRasterAttributeTable *attTable = clumpsDataset->GetRasterBand(1)->GetDefaultRAT();
            
            size_t numRows = attTable->GetRowCount();
            if(numRows == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT has no rows, i.e., it is empty!");
            }

            // Make sure it is long enough and extend if required.
            long maxVal = 0;
            long minVal = 0;
            attUtils.getImageBandMinMax(clumpsDataset, 1, &minVal, &maxVal);
            
            if(maxVal > numRows)
            {
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
            RSGISSegTilePos *clumpTilePos = new RSGISSegTilePos[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                clumpTilePos[i].overlap = false;
                clumpTilePos[i].boundary = false;
                clumpTilePos[i].body = false;
            }
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpsDataset;
            datasets[1] = maskDataset;
            
            RSGISFindClumpPositionsInTile *calcImgValStats = new RSGISFindClumpPositionsInTile(numRows, clumpTilePos, 100, 1, 200);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgValStats);
            calcImageStats.calcImage(datasets, 2);
            delete calcImgValStats;
            
            delete[] datasets;
            
            // read existing column values
            size_t currentNumRows = 0;
            int *currentColVals = attUtils.readIntColumn(attTable, outColName, &currentNumRows);
            if(numRows != currentNumRows)
            {
                throw rsgis::RSGISAttributeTableException("Current and new RAT have different number of records. Bad programming is here.");
            }            
                     
            int *colVals = new int[numRows];
            for(size_t i = 1; i < numRows; ++i)
            {
                if((clumpTilePos[i].boundary) && (currentColVals[i] != tileOverlap))
                {
                    colVals[i] = (int)tileBoundary;
                }
            }
            
            attUtils.writeIntColumn(attTable, outColName, colVals, numRows);
            
            delete[] clumpTilePos;
            delete[] colVals;
            delete[] currentColVals;
            
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }

    void RSGISDefineClumpsInTiles::defineBorderSegments(GDALDataset *clumpsDataset, std::string outColName) throw(RSGISImageException, RSGISAttributeTableException)
    {
        RSGISRasterAttUtils attUtils;
        
        // Get Attribute table
        GDALRasterAttributeTable *attTable = clumpsDataset->GetRasterBand(1)->GetDefaultRAT();
        
        size_t numRows = attTable->GetRowCount();
        if(numRows == 0)
        {
            throw rsgis::RSGISAttributeTableException("RAT has no rows, i.e., it is empty!");
        }
        
        // Make sure it is long enough and extend if required.
        long maxVal = 0;
        long minVal = 0;
        attUtils.getImageBandMinMax(clumpsDataset, 1, &minVal, &maxVal);
        
        if(maxVal > numRows)
        {
            attTable->SetRowCount(maxVal+1);
        }
        numRows = attTable->GetRowCount();
        
        int *borderMask = new int[numRows];
        for(size_t i = 0; i < numRows; ++i)
        {
            borderMask[i] = 0;
        }
        
        RSGISFindImageBorder *findImageBorders = new RSGISFindImageBorder(numRows, borderMask);
        rsgis::img::RSGISCalcImage calcImageStats(findImageBorders);
        calcImageStats.calcImageBorderPixels(clumpsDataset, true);
        delete findImageBorders;
        
        
        attUtils.writeIntColumn(attTable, outColName, borderMask, numRows);
        
        delete[] borderMask;
    }
    
    RSGISDefineClumpsInTiles::~RSGISDefineClumpsInTiles()
    {
        
    }
    
    
    
    
    RSGISFindClumpPositionsInTile::RSGISFindClumpPositionsInTile(size_t numRows, RSGISSegTilePos *clumpPos, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody): rsgis::img::RSGISCalcImageValue(0)
    {
        this->numRows = numRows;
        this->clumpPos = clumpPos;
        this->tileOverlap = tileOverlap;
        this->tileBoundary = tileBoundary;
        this->tileBody = tileBody;
    }
		
    void RSGISFindClumpPositionsInTile::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if((bandValues[0] > 0) && (bandValues[0] < numRows))
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            
            if(bandValues[1] == this->tileOverlap)
            {
                this->clumpPos[fid].overlap = true;
            }
            else if(bandValues[1] == this->tileBoundary)
            {
                this->clumpPos[fid].boundary = true;
            }
            else if(bandValues[1] == this->tileBody)
            {
                this->clumpPos[fid].body = true;
            }
        }
    }
    
    RSGISFindClumpPositionsInTile::~RSGISFindClumpPositionsInTile()
    {
        
    }
    
    
    RSGISFindImageBorder::RSGISFindImageBorder(size_t numRows, int *borderMask): rsgis::img::RSGISCalcImageValue(0)
    {
        this->numRows = numRows;
        this->borderMask = borderMask;
    }
    
    void RSGISFindImageBorder::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numIntVals == 1)
        {
            //std::cout << "FID = " << intBandValues[0] << std::endl;
            if((intBandValues[0] > 0) & (intBandValues[0] < numRows))
            {
                this->borderMask[intBandValues[0]] = 1;
            }
        }
        else
        {
            throw rsgis::img::RSGISImageCalcException("RSGISFindImageBorder must have a single int inputted.");
        }
    }

    RSGISFindImageBorder::~RSGISFindImageBorder()
    {
        
    }
    
    
    
    
}}


