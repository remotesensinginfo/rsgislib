/*
 *  RSGISSelectClumps.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/09/2013.
 *  Copyright 2013 RSGISLib.
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

#include "RSGISSelectClumps.h"


namespace rsgis{namespace rastergis{
    
    RSGISSelectClumpsOnGrid::RSGISSelectClumpsOnGrid()
    {
        
    }
    
    void RSGISSelectClumpsOnGrid::selectClumpsOnGrid(GDALDataset *clumpsDataset, std::string inSelectField, std::string outSelectField, std::string eastingsField, std::string northingsField, std::string metricField, unsigned int rows, unsigned int cols, RSGISSelectMethods method)throw(rsgis::RSGISAttributeTableException)
    {
        try
        {
            std::cout << "Import attribute tables to memory.\n";
            const GDALRasterAttributeTable *gdalAttIn = clumpsDataset->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *gdalAttOut = NULL;
            
            if((gdalAttIn == NULL) || (gdalAttIn->GetRowCount() == 0))
            {
                throw RSGISAttributeTableException("Input image does not have an attribute table.");
            }
            else
            {
                gdalAttOut = new GDALRasterAttributeTable(*gdalAttIn);
            }
            
            unsigned int numRATRows = gdalAttOut->GetRowCount();
            //std::cout << "Number of RAT Rows: " << numRATRows << std::endl;
            
            RSGISRasterAttUtils attUtils;
            rsgis::img::RSGISImageUtils imgUtils;
            
            unsigned int inSelectIdx = attUtils.findColumnIndex(gdalAttOut, inSelectField);
            unsigned int eastingsIdx = attUtils.findColumnIndex(gdalAttOut, eastingsField);
            unsigned int northingsIdx = attUtils.findColumnIndex(gdalAttOut, northingsField);
            unsigned int metricIdx = attUtils.findColumnIndex(gdalAttOut, metricField);
            unsigned int outSelectIdx = attUtils.findColumnIndexOrCreate(gdalAttOut, outSelectField, GFT_Integer);
            
            unsigned int numTiles = rows * cols;
            //std::cout << "Number of Tiles: " << numTiles << std::endl;
            
            OGREnvelope *imgExtent = imgUtils.getSpatialExtent(clumpsDataset);
            double imgWidth = imgExtent->MaxX - imgExtent->MinX;
            double imgHeight = imgExtent->MaxY - imgExtent->MinY;
            
            //std::cout << "Image Width = " << imgWidth << std::endl;
            //std::cout << "Image Height = " << imgHeight << std::endl;
            
            double tileWidth = imgWidth / ((double)cols);
            double tileHeight = imgHeight / ((double)rows);
            
            //std::cout << "Tile Width = " << tileWidth << std::endl;
            //std::cout << "Tile Height = " << tileHeight << std::endl;
            
            double *selectVal = new double[numTiles];
            unsigned int *selectIdx = new unsigned int[numTiles];
            std::vector<unsigned int> **tileIdxs = new std::vector<unsigned int>*[numTiles];
            OGREnvelope **tilesEnvs = new OGREnvelope*[numTiles];
            unsigned int idx = 0;
            double tileMinX = 0;
            double tileMaxX = 0;
            double tileMaxY = imgExtent->MaxY;
            double tileMinY = tileMaxY - tileHeight;
            bool *first = new bool[numTiles];
            for(unsigned int r = 0; r < rows; ++r)
            {
                tileMinX = imgExtent->MinX;
                tileMaxX = tileMinX + tileWidth;
                for(unsigned int c = 0; c < cols; ++c)
                {
                    idx = c + (r * cols);
                    tileIdxs[idx] = new std::vector<unsigned int>();
                    tilesEnvs[idx] = new OGREnvelope();
                    first[idx] = true;
                    
                    tilesEnvs[idx]->MinX = tileMinX;
                    tilesEnvs[idx]->MaxX = tileMaxX;
                    tilesEnvs[idx]->MinY = tileMinY;
                    tilesEnvs[idx]->MaxY = tileMaxY;
                    
                    tileMinX = tileMinX + tileWidth;
                    tileMaxX = tileMaxX + tileWidth;
                    selectVal[idx] = 0;
                    selectIdx[idx] = 0;
                }
                tileMaxY = tileMaxY - tileHeight;
                tileMinY = tileMinY - tileHeight;
            }
            
            double eastings = 0.0;
            double northings = 0.0;
            double metricVal = 0.0;
            bool foundTile = false;
            unsigned int foundTileIdx = 0;
            for(unsigned int i = 0; i < numRATRows; ++i)
            {
                //std::cout << "Row " << i << std::endl;
                if(gdalAttOut->GetValueAsInt(i, inSelectIdx) == 1)
                {
                    idx = 0;
                    foundTile = false;
                    for(unsigned int r = 0; r < rows; ++r)
                    {
                        for(unsigned int c = 0; c < cols; ++c)
                        {
                            idx = c + (r * cols);
                            eastings = gdalAttOut->GetValueAsDouble(i, eastingsIdx);
                            northings = gdalAttOut->GetValueAsDouble(i, northingsIdx);
                            if( ((eastings >= tilesEnvs[idx]->MinX) & (eastings <= tilesEnvs[idx]->MaxX)) &
                                ((northings >= tilesEnvs[idx]->MinY) & (northings <= tilesEnvs[idx]->MaxY)))
                            {
                                tileIdxs[idx]->push_back(i);
                                foundTileIdx = idx;
                                foundTile = true;
                                break;
                            }
                        }
                        if(foundTile)
                        {
                            break;
                        }
                    }
                    
                    if(foundTile)
                    {
                        //gdalAttOut->SetValue(i, outSelectIdx, ((int)foundTileIdx+1));
                        metricVal = gdalAttOut->GetValueAsDouble(i, metricIdx);
                        if(first[foundTileIdx])
                        {
                            first[foundTileIdx] = false;
                            selectVal[foundTileIdx] = metricVal;
                            selectIdx[foundTileIdx] = i;
                        }
                        else
                        {
                            if(method == meanMethod)
                            {
                                selectVal[foundTileIdx] += metricVal;
                            }
                            else if((method == minMethod) & (metricVal < selectVal[foundTileIdx]))
                            {
                                //std::cout << "Tile " << foundTileIdx << ": \n\tMin: " << "Metric = " << metricVal << " Current Min Val = " << selectVal[foundTileIdx] << std::endl;
                                selectVal[foundTileIdx] = metricVal;
                                selectIdx[foundTileIdx] = i;
                            }
                            else if((method == minMethod) & (metricVal > selectVal[foundTileIdx]))
                            {
                                selectVal[foundTileIdx] = metricVal;
                                selectIdx[foundTileIdx] = i;
                            }
                        }
                    }
                    foundTile = false;
                }
                else
                {
                    gdalAttOut->SetValue(i, outSelectIdx, 0);
                }
            }
            
            /*
            for(unsigned int i = 0; i < numTiles; ++i)
            {
                std::cout << "Tile Size: " << tileIdxs[i]->size() << std::endl;
                std::cout << "\tTile " << i << " = " << selectVal[i] << " index " << selectIdx[i] << std::endl;
            }
            */
            
            if(method == meanMethod)
            {
                idx = 0;
                bool firstVal = true;
                unsigned int clumpIdx = 0;
                double clumpDistMean = 0.0;
                double tmpDist = 0.0;
                for(unsigned int r = 0; r < rows; ++r)
                {
                    for(unsigned int c = 0; c < cols; ++c)
                    {
                        idx = c + (r * cols);
                        if(tileIdxs[idx]->size() > 0)
                        {
                            selectVal[idx] = selectVal[idx] / tileIdxs[idx]->size();
                            firstVal = true;
                            for(std::vector<unsigned int>::iterator iterIdxs = tileIdxs[idx]->begin(); iterIdxs != tileIdxs[idx]->end(); ++iterIdxs)
                            {
                                metricVal = gdalAttOut->GetValueAsDouble(*iterIdxs, metricIdx);
                                tmpDist = sqrt((selectVal[idx]-metricVal)*(selectVal[idx]-metricVal));
                                if(firstVal)
                                {
                                    clumpDistMean = tmpDist;
                                    clumpIdx = *iterIdxs;
                                    firstVal = false;
                                }
                                else if(tmpDist < clumpDistMean)
                                {
                                    clumpDistMean = tmpDist;
                                    clumpIdx = *iterIdxs;
                                }
                            }
                            selectIdx[idx] = clumpIdx;
                        }
                        else
                        {
                            selectIdx[idx] = 0;
                        }
                    }
                }
            }
            
            idx = 0;
            for(unsigned int r = 0; r < rows; ++r)
            {
                for(unsigned int c = 0; c < cols; ++c)
                {
                    idx = c + (r * cols);
                    //std::cout << "Tile " << idx << ": " << tileIdxs[idx]->size() << std::endl;
                    //std::cout << "\t Selected " << idx << " = " << selectIdx[idx] << std::endl;
                    if(tileIdxs[idx]->size() > 0)
                    {
                        for(std::vector<unsigned int>::iterator iterIdxs = tileIdxs[idx]->begin(); iterIdxs != tileIdxs[idx]->end(); ++iterIdxs)
                        {
                            gdalAttOut->SetValue(*iterIdxs, outSelectIdx, 0);
                        }
                        gdalAttOut->SetValue(selectIdx[idx], outSelectIdx, 1);
                    }
                    
                    delete tileIdxs[idx];
                    delete tilesEnvs[idx];
                }
            }
            
            delete[] first;
            delete[] tileIdxs;
            delete[] tilesEnvs;
            delete[] selectVal;
            delete[] selectIdx;
            
            clumpsDataset->GetRasterBand(1)->SetDefaultRAT(gdalAttOut);
            delete gdalAttOut;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    RSGISSelectClumpsOnGrid::~RSGISSelectClumpsOnGrid()
    {
        
    }
    
    
}}



