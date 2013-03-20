/*
 *  RSGISCreateImageTiles.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/03/2013.
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

#include "RSGISCreateImageTiles.h"


namespace rsgis{namespace rastergis{
    
    RSGISCreateImageTileMasks::RSGISCreateImageTileMasks()
    {
        
    }
    
    void RSGISCreateImageTileMasks::createTileMasks(GDALDataset *image, std::string outputBase, std::string format, std::string ext, bool useOverlap, float overlap, bool growOverlap) throw(rsgis::RSGISImageException)
    {
        try
        {
            std::cout.precision(12);
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            rsgis::utils::RSGISTextUtils txtUtils;
                        
            const GDALRasterAttributeTable *attTableConst = image->GetRasterBand(1)->GetDefaultRAT();
            size_t numTiles = attTableConst->GetRowCount();
            
            unsigned int xMinIdx = attUtils.findColumnIndex(attTableConst, "XMIN");
            unsigned int xMaxIdx = attUtils.findColumnIndex(attTableConst, "XMAX");
            unsigned int yMinIdx = attUtils.findColumnIndex(attTableConst, "YMIN");
            unsigned int yMaxIdx = attUtils.findColumnIndex(attTableConst, "YMAX");
            
            double xMin = 0;
            double xMax = 0;
            double yMin = 0;
            double yMax = 0;
            
            RSGISDefineTileMask *defTiles = new RSGISDefineTileMask();
            rsgis::img::RSGISCalcImage calcTile = rsgis::img::RSGISCalcImage(defTiles);
            
            RSGISGrowOverlapRingRegion *calcOverlap = NULL;
            rsgis::img::RSGISCalcEditImage *calcEditImage = NULL;
            
            unsigned int numPxlsOverlap = 0;
            
            if(useOverlap)
            {
                double *trans = new double[6];
                image->GetGeoTransform(trans);
                
                numPxlsOverlap = ceil(overlap/trans[1]);
                std::cout << "Overlap will be " << numPxlsOverlap << " pixels\n";
                
                calcOverlap = new RSGISGrowOverlapRingRegion(numPxlsOverlap);
                calcEditImage = new rsgis::img::RSGISCalcEditImage(calcOverlap);
                
                delete[] trans;
            }
            
            std::string outputFile = "";
            geos::geom::Envelope *env = new geos::geom::Envelope();
            
            size_t tileCount = 1;
            for(size_t i = 1; i < numTiles; ++i)
            {
                xMin = attTableConst->GetValueAsDouble(i, xMinIdx);
                xMax = attTableConst->GetValueAsDouble(i, xMaxIdx);
                yMin = attTableConst->GetValueAsDouble(i, yMinIdx);
                yMax = attTableConst->GetValueAsDouble(i, yMaxIdx);
                
                if((xMin != 0) & (xMax != 0) & (yMin != 0) & (yMax != 0))
                {
                    if(useOverlap)
                    {
                        xMin -= overlap;
                        xMax += overlap;
                        yMin -= overlap;
                        yMax += overlap;
                    }
                    
                    std::cout << "Create tile " << tileCount << ":\t[" << xMin << "," << xMax << "][" << yMin << "," << yMax << "]\n";
                    
                    outputFile = outputBase + txtUtils.int64bittostring(i) + std::string(".") + ext;

                    std::cout << "Creating output file: " << outputFile << std::endl;
                    
                    env->init(xMin, xMax, yMin, yMax);
                    
                    defTiles->setTileVal(i);
                    calcTile.calcImageInEnv(&image, 1, outputFile, env, false, NULL, format, GDT_UInt32);
                    
                    
                    GDALDataset *outDataset = (GDALDataset *) GDALOpen(outputFile.c_str(), GA_Update);
                    if(outDataset == NULL)
                    {
                        std::string message = std::string("Could not open image ") + outputFile;
                        throw RSGISImageException(message.c_str());
                    }
                    
                    if(useOverlap & growOverlap)
                    {
                        // Now grow the overlap region.
                        std::cout << "Grow the overlap region\n";
                        bool change = true;
                        unsigned int dist = 1;
                        calcOverlap->reset();
                        while(change)
                        {
                            std::cout << "Distance " << dist << ":\t" << std::flush;
                            calcOverlap->resetChange();
                            calcEditImage->calcImageWindowData(outDataset, 3, -2);
                            change = calcOverlap->changeOccurred();
                            calcOverlap->incrementCounter();
                            dist += 1;
                        }
                        
                        std::cout << "Define a single region for the overlap area\n";
                        calcEditImage->calcImageUseOut(outDataset);
                        
                    }
                    
                    outDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
                    
                    rsgis::img::RSGISPopWithStats popWithStats;
                    popWithStats.calcPopStats( outDataset, true, 0, true );
                    
                    GDALClose(outDataset);
                    
                    ++tileCount;
                }
            }
            
            delete defTiles;
            
        }
        catch (rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    RSGISCreateImageTileMasks::~RSGISCreateImageTileMasks()
    {
        
    }
    
    
    
    
    RSGISDefineTileMask::RSGISDefineTileMask() : rsgis::img::RSGISCalcImageValue(1)
    {
        
    }
    
    void RSGISDefineTileMask::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] == tileVal)
        {
            output[0] = 1;
        }
        else
        {
            output[0] = 0;
        }
    }
    
    void RSGISDefineTileMask::setTileVal(size_t tileVal)
    {
        this->tileVal = tileVal;
    }
    
    RSGISDefineTileMask::~RSGISDefineTileMask()
    {
        
    }
    
    
    
    
    
    
    RSGISGrowOverlapRingRegion::RSGISGrowOverlapRingRegion(unsigned int maxNumGrowthPixels):rsgis::img::RSGISCalcImageValue(1)
    {
        counter = 1;
        change = false;
        this->maxNumGrowthPixels = maxNumGrowthPixels;
    }
    
    void RSGISGrowOverlapRingRegion::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if((bandValues[0] == 0) | (bandValues[0] == 1))
        {
            output[0] = bandValues[0];
        }
        else
        {
            output[0] = 2;
        }
    }
    
    void RSGISGrowOverlapRingRegion::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        int midPoint = floor(((float)winSize)/2.0);
        if(counter == this->maxNumGrowthPixels)
        {
            output[0] = dataBlock[0][midPoint][midPoint];
        }
        else if(dataBlock[0][midPoint][midPoint] == 0)
        {
            if((dataBlock[0][midPoint-1][midPoint] == counter) |
               (dataBlock[0][midPoint][midPoint-1] == counter) |
               (dataBlock[0][midPoint][midPoint+1] == counter) |
               (dataBlock[0][midPoint+1][midPoint] == counter) |
               (dataBlock[0][midPoint-1][midPoint-1] == counter) |
               (dataBlock[0][midPoint-1][midPoint+1] == counter) |
               (dataBlock[0][midPoint+1][midPoint+1] == counter) |
               (dataBlock[0][midPoint+1][midPoint-1] == counter))
            {
                output[0] = counter+1;
                change = true;
            }
            else
            {
                output[0] = dataBlock[0][midPoint][midPoint];
            }
        }
        else
        {
            output[0] = dataBlock[0][midPoint][midPoint];
        }
    }
    
    bool RSGISGrowOverlapRingRegion::changeOccurred()
    {
        return change;
    }
    
    void RSGISGrowOverlapRingRegion::resetChange()
    {
        change = false;
    }
    
    void RSGISGrowOverlapRingRegion::incrementCounter()
    {
        counter += 1;
    }
    
    void RSGISGrowOverlapRingRegion::reset()
    {
        counter = 1;
        change = false;
    }
    
    RSGISGrowOverlapRingRegion::~RSGISGrowOverlapRingRegion()
    {
        
    }
    
    
    
    
    RSGISCutOutTile::RSGISCutOutTile(float noDataVal, unsigned int numOfBands):rsgis::img::RSGISCalcImageValue(numOfBands)
    {
        this->noDataVal = noDataVal;
    }
    
    void RSGISCutOutTile::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            for(unsigned int i = 0; i < numOutBands; ++i)
            {
                output[i] = bandValues[i+1];
            }
        }
        else
        {
            for(unsigned int i = 0; i < numOutBands; ++i)
            {
                output[i] = 0;
            }
        }
    }
    
    RSGISCutOutTile::~RSGISCutOutTile()
    {
        
    }
    
}}

