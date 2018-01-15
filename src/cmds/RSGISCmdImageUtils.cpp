/*
 *  RSGISCmdImageUtils.cpp
 *
 *
 *  Created by Pete Bunting on 03/05/2013.
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

#include <boost/filesystem.hpp>

#include "RSGISCmdImageUtils.h"
#include "RSGISCmdParent.h"

#include "geos/geom/Envelope.h"

#include "common/RSGISImageException.h"

#include "img/RSGISBandMath.h"
#include "img/RSGISImageMaths.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCopyImage.h"
#include "img/RSGISStretchImage.h"
#include "img/RSGISMaskImage.h"
#include "img/RSGISImageMosaic.h"
#include "img/RSGISPopWithStats.h"
#include "img/RSGISAddBands.h"
#include "img/RSGISExtractImageValues.h"
#include "img/RSGISImageComposite.h"
#include "img/RSGISAddBands.h"
#include "img/RSGISSampleImage.h"
#include "img/RSGISPanSharpen.h"
#include "img/RSGISSharpenLowResImagery.h"

#include "vec/RSGISImageTileVector.h"
#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorIO.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISPolygonData.h"

#include "utils/RSGISTextUtils.h"

namespace rsgis{ namespace cmds {

    void executeStretchImage(std::string inputImage, std::string outputImage, bool saveOutStats, std::string outStatsFile, bool ignoreZeros, bool onePassSD, std::string gdalFormat, RSGISLibDataType outDataType, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();

            GDALDataset *inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            bool useNoData = false;
            double inNoData = 0.0;
            double outNoData = 0.0;
            if(ignoreZeros)
            {
                useNoData = true;
                inNoData = 0.0;
                outNoData = 0.0;
            }

            rsgis::img::RSGISStretchImage stretchImg = rsgis::img::RSGISStretchImage(inDataset, outputImage, saveOutStats, outStatsFile, onePassSD, gdalFormat, RSGIS_to_GDAL_Type(outDataType), 0, 255, useNoData, inNoData, outNoData);
            if(stretchType == linearMinMax)
            {
                stretchImg.executeLinearMinMaxStretch();
            }
            else if(stretchType == linearPercent)
            {
                stretchImg.executeLinearPercentStretch(stretchParam);
            }
            else if(stretchType == linearStdDev)
            {
                stretchImg.executeLinearStdDevStretch(stretchParam);
            }
            else if(stretchType == histogram)
            {
                stretchImg.executeHistogramStretch();
            }
            else if(stretchType == exponential)
            {
                stretchImg.executeExponentialStretch();
            }
            else if(stretchType == logarithmic)
            {
                stretchImg.executeLogrithmicStretch();
            }
            else if(stretchType == powerLaw)
            {
                stretchImg.executePowerLawStretch(stretchParam);
            }
            else
            {
                throw RSGISException("Stretch is not recognised.");
            }

            GDALClose(inDataset);
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeStretchImageWithStats(std::string inputImage, std::string outputImage, std::string inStatsFile, std::string gdalFormat, RSGISLibDataType outDataType, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();

            GDALDataset *inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISStretchImageWithStats stretchImg = rsgis::img::RSGISStretchImageWithStats(inDataset, outputImage, inStatsFile, gdalFormat, RSGIS_to_GDAL_Type(outDataType), 0, 255, false, 0.0, 0.0);
            if(stretchType == linearMinMax)
            {
                stretchImg.executeLinearMinMaxStretch();
            }
            else if(stretchType == histogram)
            {
                stretchImg.executeHistogramStretch();
            }
            else if(stretchType == exponential)
            {
                stretchImg.executeExponentialStretch();
            }
            else if(stretchType == logarithmic)
            {
                stretchImg.executeLogrithmicStretch();
            }
            else if(stretchType == powerLaw)
            {
                stretchImg.executePowerLawStretch(stretchParam);
            }
            else
            {
                throw RSGISException("Stretch is not recognised - when the stats are specified in a text file on all stretches are available (or make sense) as the stats file contains all the info.");
            }

            GDALClose(inDataset);
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeNormaliseImgPxlVals(std::string inputImage, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, float inNoDataVal, float outNoDataVal, float outMinVal, float outMaxVal, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISStretchImage stretchImg = rsgis::img::RSGISStretchImage(inDataset, outputImage, false, "", false, gdalFormat, RSGIS_to_GDAL_Type(outDataType), outMinVal, outMaxVal, true, inNoDataVal, outNoDataVal);
            if(stretchType == linearMinMax)
            {
                stretchImg.executeLinearMinMaxStretch();
            }
            else if(stretchType == linearPercent)
            {
                stretchImg.executeLinearPercentStretch(stretchParam);
            }
            else if(stretchType == linearStdDev)
            {
                stretchImg.executeLinearStdDevStretch(stretchParam);
            }
            else if(stretchType == histogram)
            {
                stretchImg.executeHistogramStretch();
            }
            else if(stretchType == exponential)
            {
                stretchImg.executeExponentialStretch();
            }
            else if(stretchType == logarithmic)
            {
                stretchImg.executeLogrithmicStretch();
            }
            else if(stretchType == powerLaw)
            {
                stretchImg.executePowerLawStretch(stretchParam);
            }
            else
            {
                throw RSGISException("Stretch is not recognised.");
            }
            
            GDALClose(inDataset);
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeMaskImage(std::string inputImage, std::string imageMask, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, float outValue, std::vector<float> maskValues)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            GDALDataset *mask = (GDALDataset *) GDALOpenShared(imageMask.c_str(), GA_ReadOnly);
            if(mask == NULL)
            {
                std::string message = std::string("Could not open image ") + imageMask;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISMaskImage maskImage =  rsgis::img::RSGISMaskImage();
            maskImage.maskImage(dataset, mask, outputImage, gdalFormat, RSGIS_to_GDAL_Type(outDataType), outValue, maskValues);

            GDALClose(dataset);
            GDALClose(mask);
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeCreateTiles(std::string inputImage, std::string outputImageBase, unsigned int width, unsigned int height, unsigned int tileOverlap, bool offsetTiling, std::string gdalFormat, RSGISLibDataType outDataType, std::string outFileExtension, std::vector<std::string> *outFileNames)throw(RSGISCmdException)
    {
        std::cout.precision(12);
        GDALAllRegister();
        
        try
        {
            rsgis::img::RSGISImageUtils imgUtils;
            // Open Image
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISCmdException(message.c_str());
            }
            
            // Set up envlopes for image tiles
            std::vector<geos::geom::Envelope*> *tileEnvelopes = new std::vector<geos::geom::Envelope*>;
            
            int numImageBands = dataset->GetRasterCount();
            unsigned int imgSizeX = dataset->GetRasterXSize();
            unsigned int imgSizeY = dataset->GetRasterYSize();
            
            double *gdalTransform = new double[6];
            dataset->GetGeoTransform( gdalTransform );
            double pxlXRes = gdalTransform[1];
            double pxlYRes = gdalTransform[5];
            if(pxlYRes < 0)
            {
                pxlYRes = pxlYRes * (-1);
            }
            double imgTLX = gdalTransform[0];
            double imgTLY = gdalTransform[3];
            delete[] gdalTransform;
            
            if(offsetTiling)
            {
                unsigned int xOff = width/2;
                unsigned int yOff = height/2;
                float numXTilesFlt = float(imgSizeX-xOff) / float(width);
                float numYTilesFlt = float(imgSizeY-yOff) / float(height);
                
                unsigned int numXTiles = floor(numXTilesFlt);
                unsigned int numYTiles = floor(numYTilesFlt);
                unsigned int remainPxlX = imgSizeX - (numXTiles * width);
                unsigned int remainPxlY = imgSizeY - (numYTiles * height);
                
                unsigned int numTiles = 0;
                long cTileX = 0;
                long cTileY = 0;
                
                long tileXMin = 0;
                long tileXMax = 0;
                long tileYMin = 0;
                long tileYMax = 0;
                
                double tileTLX = 0.0;
                double tileTLY = 0.0;
                double tileBRX = 0.0;
                double tileBRY = 0.0;
                if(yOff > 0)
                {
                    cTileX = 0;
                    if(xOff > 0)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + xOff + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + yOff + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        cTileX += xOff;
                        ++numTiles;
                    }
                    for(unsigned int j = 0; j < numXTiles; ++j)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + width + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + yOff + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        cTileX += width;
                        ++numTiles;
                    }
                    if(remainPxlX > 0)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + remainPxlX + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + yOff + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        ++numTiles;
                    }
                    cTileY += yOff;
                }
                for(unsigned int i = 0; i < numYTiles; ++i)
                {
                    cTileX = 0;
                    if(xOff > 0)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + xOff + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + height + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        cTileX += xOff;
                        ++numTiles;
                    }
                    for(unsigned int j = 0; j < numXTiles; ++j)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + width + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + height + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        cTileX += width;
                        ++numTiles;
                    }
                    if(remainPxlX > 0)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + remainPxlX + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + height + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        ++numTiles;
                    }
                    cTileY += height;
                }
                if(remainPxlY > 0)
                {
                    cTileX = 0;
                    if(xOff > 0)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + xOff + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + remainPxlY + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        cTileX += xOff;
                        ++numTiles;
                    }
                    for(unsigned int j = 0; j < numXTiles; ++j)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + width + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + remainPxlY + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        cTileX += width;
                        ++numTiles;
                    }
                    if(remainPxlX > 0)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + remainPxlX + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + remainPxlY + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        ++numTiles;
                    }
                }
            }
            else
            {
                float numXTilesFlt = float(imgSizeX) / float(width);
                float numYTilesFlt = float(imgSizeY) / float(height);
                
                unsigned int numXTiles = floor(numXTilesFlt);
                unsigned int numYTiles = floor(numYTilesFlt);
                unsigned int remainPxlX = imgSizeX - (numXTiles * width);
                unsigned int remainPxlY = imgSizeY - (numYTiles * height);
                
                unsigned int numTiles = 0;
                long cTileX = 0;
                long cTileY = 0;
                
                long tileXMin = 0;
                long tileXMax = 0;
                long tileYMin = 0;
                long tileYMax = 0;
                
                double tileTLX = 0.0;
                double tileTLY = 0.0;
                double tileBRX = 0.0;
                double tileBRY = 0.0;
                
                for(unsigned int i = 0; i < numYTiles; ++i)
                {
                    cTileX = 0;
                    for(unsigned int j = 0; j < numXTiles; ++j)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + width + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + height + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        cTileX += width;
                        ++numTiles;
                    }
                    if(remainPxlX > 0)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + remainPxlX + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + height + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        ++numTiles;
                    }
                    cTileY += height;
                }
                if(remainPxlY > 0)
                {
                    cTileX = 0;
                    for(unsigned int j = 0; j < numXTiles; ++j)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + width + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + remainPxlY + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        cTileX += width;
                        ++numTiles;
                    }
                    if(remainPxlX > 0)
                    {
                        tileXMin = cTileX - tileOverlap;
                        tileXMax = cTileX + remainPxlX + tileOverlap;
                        tileYMin = cTileY - tileOverlap;
                        tileYMax = cTileY + remainPxlY + tileOverlap;
                        
                        if(tileXMin < 0)
                        {
                            tileXMin = 0;
                        }
                        if(tileXMax > imgSizeX)
                        {
                            tileXMax = imgSizeX;
                        }
                        if(tileYMin < 0)
                        {
                            tileYMin = 0;
                        }
                        if(tileYMax > imgSizeY)
                        {
                            tileYMax = imgSizeY;
                        }
                        
                        tileTLX = imgTLX + (tileXMin*pxlXRes);
                        tileTLY = imgTLY - (tileYMax*pxlYRes);
                        tileBRX = imgTLX + (tileXMax*pxlXRes);
                        tileBRY = imgTLY - (tileYMin*pxlYRes);
                        tileEnvelopes->push_back(new geos::geom::Envelope(tileTLX, tileBRX, tileTLY, tileBRY));
                        
                        ++numTiles;
                    }
                }
            }
            
            rsgis::img::RSGISCopyImage copyImage = rsgis::img::RSGISCopyImage(numImageBands);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&copyImage, "", true);
            
            std::string outputFilePath = "";
            for(unsigned int i = 0; i < tileEnvelopes->size(); ++i)
            {
                std::cout << "Tile " << i+1 << "/" << tileEnvelopes->size() << std::endl;
                outputFilePath = outputImageBase + "_tile" + boost::lexical_cast<std::string>(i) + "." + outFileExtension;
                try
                {
                    calcImage.calcImageInEnv(&dataset, 1, outputFilePath, tileEnvelopes->at(i), false, NULL, gdalFormat, RSGIS_to_GDAL_Type(outDataType));

                    if(outFileNames != NULL)
                    {
                        outFileNames->push_back(outputFilePath);
                    }
                }
                catch (rsgis::img::RSGISImageBandException e)
                {
                    throw RSGISCmdException(e.what());
                }
            }
            GDALClose(dataset);
        }
        catch(rsgis::RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executePopulateImgStats(std::string inputImage, bool useIgnoreVal, float nodataValue, bool calcImgPyramids, std::vector<int> pyraScaleVals)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISPopWithStats popWithStats;
            popWithStats.calcPopStats( inDataset, useIgnoreVal, nodataValue, calcImgPyramids, pyraScaleVals);


            GDALClose(inDataset);
        }
        catch(rsgis::RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeImageMosaic(std::string *inputImages, int numDS, std::string outputImage, float background, float skipVal, unsigned int skipBand, unsigned int overlapBehaviour, std::string format, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        GDALAllRegister();
        try
        {
            rsgis::img::RSGISImageMosaic mosaic;
            // Projection hardcoded to from image (to simplify interface)
            mosaic.mosaicSkipVals(inputImages, numDS, outputImage, background, skipVal, true, "", skipBand, overlapBehaviour, format, RSGIS_to_GDAL_Type(outDataType));
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    std::vector<std::string> executeOrderImageUsingValidDataProp(std::vector<std::string> images, float noDataValue) throw(RSGISCmdException)
    {
        GDALAllRegister();
        std::vector<std::string> orderedImages;
        try
        {
            rsgis::img::RSGISImageMosaic mosaic;
            mosaic.orderInImagesValidData(images, &orderedImages, noDataValue);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
        return orderedImages;
    }

    void executeImageInclude(std::string *inputImages, int numDS, std::string baseImage, bool bandsDefined, std::vector<int> bands, float skipVal, bool useSkipVal) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();

            GDALDataset *baseDS = (GDALDataset *) GDALOpenShared(baseImage.c_str(), GA_Update);
            if(baseDS == NULL)
            {
                std::string message = std::string("Could not open image ") + baseImage;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISImageMosaic mosaic;
            if(useSkipVal)
            {
                mosaic.includeDatasetsSkipVals(baseDS, inputImages, numDS, bands, bandsDefined, skipVal);
            }
            else
            {
                mosaic.includeDatasets(baseDS, inputImages, numDS, bands, bandsDefined);
            }

            GDALClose(baseDS);
            delete[] inputImages;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
                
    void executeImageIncludeOverlap(std::string *inputImages, int numDS, std::string baseImage, int numOverlapPxls) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *baseDS = (GDALDataset *) GDALOpenShared(baseImage.c_str(), GA_Update);
            if(baseDS == NULL)
            {
                std::string message = std::string("Could not open image ") + baseImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageMosaic mosaic;
            mosaic.includeDatasetsIgnoreOverlap(baseDS, inputImages, numDS, numOverlapPxls);
            
            GDALClose(baseDS);
            delete[] inputImages;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeImageIncludeIndImgIntersect(std::string *inputImages, int numDS, std::string baseImage) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *baseDS = (GDALDataset *) GDALOpenShared(baseImage.c_str(), GA_Update);
            if(baseDS == NULL)
            {
                std::string message = std::string("Could not open image ") + baseImage;
                throw RSGISImageException(message.c_str());
            }
            int numBaseBands = baseDS->GetRasterCount();
            
            int useNoDataValInt = false;
            double noDataVal = baseDS->GetRasterBand(1)->GetNoDataValue(&useNoDataValInt);
            bool useNoDataVal = (bool)useNoDataValInt;
            
            GDALDataset **datasets = new GDALDataset*[2];
            rsgis::img::RSGISIncludeSingleImgCalcImgVal *addImg2Base = new rsgis::img::RSGISIncludeSingleImgCalcImgVal(numBaseBands, useNoDataVal, noDataVal);
            rsgis::img::RSGISCalcImage *calcImg = new rsgis::img::RSGISCalcImage(addImg2Base);
            for(int i = 0; i < numDS; ++i)
            {
                GDALDataset *inImg = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                datasets[0] = baseDS;
                datasets[1] = inImg;
                
                useNoDataValInt = false;
                noDataVal = inImg->GetRasterBand(1)->GetNoDataValue(&useNoDataValInt);
                useNoDataVal = (bool)useNoDataValInt;
                
                addImg2Base->setNoDataValue(useNoDataVal, noDataVal);
                calcImg->calcImagePartialOutput(datasets, 2, baseDS);
                GDALClose(inImg);
            }
            delete addImg2Base;
            delete calcImg;
            delete[] datasets;
            
            GDALClose(baseDS);
            delete[] inputImages;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeImageIncludeOverviews(std::string baseImage, std::vector<std::string> inputImages, std::vector<int> pyraScaleVals) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *baseDS = (GDALDataset *) GDALOpenShared(baseImage.c_str(), GA_Update);
            if(baseDS == NULL)
            {
                std::string message = std::string("Could not open image ") + baseImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISCombineImgTileOverview combineOverviews;
            combineOverviews.combineKEAImgTileOverviews(baseDS, inputImages, pyraScaleVals);
            
            GDALClose(baseDS);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeAssignProj(std::string inputImage, std::string wktStr, bool readWKTFromFile, std::string wktFile)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            if(readWKTFromFile)
            {
                rsgis::utils::RSGISTextUtils textUtils;
                wktStr = textUtils.readFileToString(wktFile);
            }

            inDataset->SetProjection(wktStr.c_str());

            GDALClose(inDataset);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeAssignSpatialInfo(std::string inputImage, double xTL, double yTL, double xRes, double yRes, double xRot, double yRot, bool xTLDef, bool yTLDef, bool xResDef, bool yResDef, bool xRotDef, bool yRotDef)throw(RSGISCmdException)
    {
        try
        {
            std::cout.precision(12);
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            double *trans = new double[6];
            inDataset->GetGeoTransform(trans);

            if(xTLDef)
            {
                trans[0] = xTL;
            }
            if(yTLDef)
            {
                trans[3] = yTL;
            }
            if(xResDef)
            {
                trans[1] = xRes;
            }
            if(yResDef)
            {
                trans[5] = yRes;
            }
            if(xRotDef)
            {
                trans[2] = xRot;
            }
            if(yRotDef)
            {
                trans[4] = yRot;
            }

            inDataset->SetGeoTransform(trans);

            GDALClose(inDataset);
            delete[] trans;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeCopyProj(std::string inputImage, std::string refImageFile)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *refDataset = (GDALDataset *) GDALOpen(refImageFile.c_str(), GA_ReadOnly);
            if(refDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + refImageFile;
                throw RSGISImageException(message.c_str());
            }

            std::string wktString = std::string(refDataset->GetProjectionRef());

            GDALClose(refDataset);

            executeAssignProj(inputImage, wktString);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeCopyProjSpatial(std::string inputImage, std::string refImageFile)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *refDataset = (GDALDataset *) GDALOpen(refImageFile.c_str(), GA_ReadOnly);
            if(refDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + refImageFile;
                throw RSGISImageException(message.c_str());
            }

            std::string wktString = std::string(refDataset->GetProjectionRef());

            double *trans = new double[6];
            refDataset->GetGeoTransform(trans);

            GDALClose(refDataset);


            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            inDataset->SetGeoTransform(trans);
            inDataset->SetProjection(wktString.c_str());

            delete[] trans;
            GDALClose(inDataset);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }


    void executeStackImageBands(std::string *imageFiles, std::string *imageBandNames, int numImages, std::string outputImage, bool skipPixels, float skipValue, float noDataValue, std::string gdalFormat, RSGISLibDataType outDataType, bool replaceBandNames)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            std::cout << "There are " << numImages << " images to stack\n";
            GDALDataset **datasets = new GDALDataset*[numImages];
            for(int i = 0; i < numImages; i++)
            {
                std::cout << imageFiles[i] << std::endl;
                datasets[i] = (GDALDataset *) GDALOpenShared(imageFiles[i].c_str(), GA_ReadOnly);
                if(datasets[i] == NULL)
                {
                    std::string message = std::string("Could not open image ") + imageFiles[i];
                    throw RSGISCmdException(message.c_str());
                }
            }

            rsgis::img::RSGISAddBands stackbands;
            stackbands.stackImages(datasets, numImages, outputImage, imageBandNames, skipPixels, skipValue, noDataValue, gdalFormat, RSGIS_to_GDAL_Type(outDataType), replaceBandNames);

            if(datasets != NULL)
            {
                for(int i = 0; i < numImages; i++)
                {
                    if(datasets[i] != NULL)
                    {
                        GDALClose(datasets[i]);
                    }
                }
                delete[] datasets;
            }
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }


    void executeImageRasterZone2HDF(std::string imageFile, std::string maskImage, std::string outputHDF, float maskVal)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();

            GDALDataset *maskDS = (GDALDataset *) GDALOpen(maskImage.c_str(), GA_ReadOnly);
            if(maskDS == NULL)
            {
                std::string message = std::string("Could not open image ") + maskImage;
                throw RSGISImageException(message.c_str());
            }

            GDALDataset *imageDS = (GDALDataset *) GDALOpen(imageFile.c_str(), GA_ReadOnly);
            if(imageDS == NULL)
            {
                std::string message = std::string("Could not open image ") + imageFile;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISExtractImageValues extractVals;
            extractVals.extractDataWithinMask2HDF(maskDS, imageDS, outputHDF, maskVal);

            GDALClose(maskDS);
            GDALClose(imageDS);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
            
    void executeImageBandRasterZone2HDF(std::vector<std::pair<std::string, std::vector<unsigned int> > > imageFiles, std::string maskImage, std::string outputHDF, float maskVal)throw(RSGISCmdException)
    {
        try
        {
            rsgis::img::RSGISExtractImageValues extractVals;
            extractVals.extractImgBandDataWithinMask2HDF(imageFiles, maskImage, outputHDF, maskVal);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeSubsetImageBands(std::string inputImage, std::string outputImage, std::vector<unsigned int> bands, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();

            GDALDataset *imageDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(imageDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            unsigned int numBands = imageDS->GetRasterCount();

            for(std::vector<unsigned int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
            {
                if(((*iterBands) == 0) || ((*iterBands) > numBands))
                {
                    throw RSGISImageException("Not all the image bands are present within the input image file (Note. Bands are numbered from 1).");
                }
            }

            rsgis::img::RSGISCopyImageBandSelect *copyImageBands = new rsgis::img::RSGISCopyImageBandSelect(bands);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(copyImageBands, "", true);

            calcImage->calcImage(&imageDS, 1, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(outDataType));

            GDALClose(imageDS);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }


    void executeSubset(std::string inputImage, std::string inputVector, std::string outputImage, std::string imageFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            OGRRegisterAll();

            GDALDataset **dataset = NULL;
            GDALDataset *inputVecDS = NULL;
            OGRLayer *inputVecLayer = NULL;

            rsgis::img::RSGISCopyImage *copyImage = NULL;
            rsgis::img::RSGISCalcImage *calcImage = NULL;

            rsgis::vec::RSGISVectorUtils vecUtils;

            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).string();

            std::string vectorLayerName = vecUtils.getLayerName(inputVector);
            int numImageBands = 0;

            // Open Image
            dataset = new GDALDataset*[1];
            std::cout << inputImage << std::endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            numImageBands = dataset[0]->GetRasterCount();
            std::cout << "Raster Band Count = " << numImageBands << std::endl;

            // Open vector
            inputVecDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
            if(inputVecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + vectorLayerName;
                throw RSGISFileException(message.c_str());
            }
            OGREnvelope ogrExtent;
            inputVecLayer->GetExtent(&ogrExtent);
            geos::geom::Envelope extent = geos::geom::Envelope(ogrExtent.MinX, ogrExtent.MaxX, ogrExtent.MinY, ogrExtent.MaxY);

            copyImage = new rsgis::img::RSGISCopyImage(numImageBands);
            calcImage = new rsgis::img::RSGISCalcImage(copyImage, "", true);
            calcImage->calcImageInEnv(dataset, 1, outputImage, &extent, false, NULL, imageFormat, RSGIS_to_GDAL_Type(outDataType));

            GDALClose(dataset[0]);
            delete[] dataset;
            GDALClose(inputVecDS);
            delete calcImage;
            delete copyImage;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeSubsetBBox(std::string inputImage, std::string outputImage, std::string imageFormat, RSGISLibDataType outDataType, double xMin, double xMax, double yMin, double yMax) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            OGRRegisterAll();
            
            // Open Image
            std::cout << inputImage << std::endl;
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            int numImageBands = dataset->GetRasterCount();
            std::cout << "Raster Band Count = " << numImageBands << std::endl;
            
            geos::geom::Envelope extent = geos::geom::Envelope(xMin, xMax, yMin, yMax);
            
            rsgis::img::RSGISCopyImage *copyImage = new rsgis::img::RSGISCopyImage(numImageBands);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(copyImage, "", true);
            calcImage.calcImageInEnv(&dataset, 1, outputImage, &extent, false, NULL, imageFormat, RSGIS_to_GDAL_Type(outDataType));
            
            GDALClose(dataset);
            delete copyImage;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeSubset2Polys(std::string inputImage, std::string inputVector, std::string filenameAttribute, std::string outputImageBase,
                              std::string imageFormat, RSGISLibDataType outDataType, std::string outFileExtension, std::vector<std::string> *outFileNames) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            OGRRegisterAll();

            GDALDataset **dataset = NULL;
            GDALDataset *inputVecDS = NULL;
            OGRLayer *inputVecLayer = NULL;

            rsgis::img::RSGISCopyImage *copyImage = NULL;
            rsgis::img::RSGISCalcImage *calcImage = NULL;

            rsgis::vec::RSGISVectorIO vecIO;
            rsgis::vec::RSGISPolygonData **polyData = NULL;
            rsgis::vec::RSGISImageTileVector **data = NULL;
            rsgis::vec::RSGISVectorUtils vecUtils;

            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).string();

            std::string vectorLayerName = vecUtils.getLayerName(inputVector);
            int numImageBands = 0;
            int numFeatures = 0;
            std::string outputFilePath;

            // Open Image
            dataset = new GDALDataset*[1];
            std::cout << inputImage << std::endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            numImageBands = dataset[0]->GetRasterCount();
            std::cout << "Raster Band Count = " << numImageBands << std::endl;

            // Open vector
            inputVecDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
            if(inputVecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + vectorLayerName;
                throw rsgis::RSGISFileException(message.c_str());
            }

            // READ IN SHAPEFILE
            numFeatures = inputVecLayer->GetFeatureCount();
            polyData = new rsgis::vec::RSGISPolygonData*[numFeatures];
            for(int i = 0; i < numFeatures; i++)
            {
                polyData[i] = new rsgis::vec::RSGISImageTileVector(filenameAttribute);
            }
            std::cout << "Reading in " << numFeatures << " features\n";
            vecIO.readPolygons(inputVecLayer, polyData, numFeatures);

            //Convert to RSGISImageTileVector
            data = new rsgis::vec::RSGISImageTileVector*[numFeatures];
            for(int i = 0; i < numFeatures; i++)
            {
                data[i] = dynamic_cast<rsgis::vec::RSGISImageTileVector*>(polyData[i]);
            }
            delete[] polyData;

            copyImage = new rsgis::img::RSGISCopyImage(numImageBands);
            calcImage = new rsgis::img::RSGISCalcImage(copyImage, "", true);

            unsigned int failCount = 0;
            for(int i = 0; i < numFeatures; i++)
            {
                outputFilePath = outputImageBase + data[i]->getFileName() + "." + outFileExtension;
                std::cout << i << ": " << outputFilePath << std::endl;
                try
                {
                    calcImage->calcImageInEnv(dataset, 1, outputFilePath, data[i]->getBBox(), false, NULL, imageFormat, RSGIS_to_GDAL_Type(outDataType));
                    if(outFileNames != NULL){outFileNames->push_back(outputFilePath);}
                }
                catch (rsgis::img::RSGISImageBandException e)
                {
                    ++failCount;
                    if(failCount <= 100)
                    {
                        std::cerr << "RSGISException caught: " << e.what() << std::endl;
                        std::cerr << "Check output path exists and is writable and all polygons in shapefile:" << std::endl;
                        std::cerr << " " << inputVector << std::endl;
                        std::cerr << "Are completely within:" << std::endl;
                        std::cerr << " " << inputImage << std::endl;
                    }
                    else
                    {
                        std::cerr << "Over 100 exceptions have been caught, exiting" << std::endl;
                        throw e;
                    }
                }
            }

            GDALClose(dataset[0]);
            delete[] dataset;
            GDALClose(inputVecDS);
            delete calcImage;
            delete copyImage;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (rsgis::vec::RSGISVectorOutputException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeSubset2Img(std::string inputImage, std::string inputROIImage, std::string outputImage, std::string imageFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
			GDALAllRegister();
			OGRRegisterAll();

			GDALDataset **dataset = NULL;
            GDALDataset *roiDataset = NULL;

            rsgis::img::RSGISCopyImage *copyImage = NULL;
            rsgis::img::RSGISCalcImage *calcImage = NULL;

			int numImageBands = 0;

            // Open Image
            dataset = new GDALDataset*[1];
            std::cout << inputImage << std::endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            numImageBands = dataset[0]->GetRasterCount();
            std::cout << "Raster Band Count = " << numImageBands << std::endl;

            roiDataset = (GDALDataset *) GDALOpenShared(inputROIImage.c_str(), GA_ReadOnly);
            if(roiDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputROIImage;
                throw rsgis::RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISImageUtils imgUtils;

            OGREnvelope *ogrExtent = imgUtils.getSpatialExtent(roiDataset);
            geos::geom::Envelope extent = geos::geom::Envelope(ogrExtent->MinX, ogrExtent->MaxX, ogrExtent->MinY, ogrExtent->MaxY);

            std::cout.precision(12);
            std::cout << "BBOX [" << ogrExtent->MinX << "," << ogrExtent->MaxX << "][" << ogrExtent->MinY << "," << ogrExtent->MaxY << "]\n";

            copyImage = new rsgis::img::RSGISCopyImage(numImageBands);
            calcImage = new rsgis::img::RSGISCalcImage(copyImage, "", true);
            calcImage->calcImageInEnv(dataset, 1, outputImage, &extent, false, NULL, imageFormat, RSGIS_to_GDAL_Type(outDataType));

            GDALClose(dataset[0]);
            delete[] dataset;
            GDALClose(roiDataset);

            delete calcImage;
            delete copyImage;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeCreateBlankImage(std::string outputImage, unsigned int numBands, unsigned int width, unsigned int height, double tlX, double tlY, double resolution, float pxlVal, std::string wktFile, std::string wktStr, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
			GDALAllRegister();
            double *transformation = new double[6];
            transformation[0] = tlX;
            transformation[1] = resolution;
            transformation[2] = 0;
            transformation[3] = tlY;
            transformation[4] = 0;
            transformation[5] = resolution * (-1);

            std::string projection = "";
            if(wktFile != "")
            {
                rsgis::utils::RSGISTextUtils textUtils;
                projection = textUtils.readFileToString(wktFile);
            }
            else
            {
                projection = wktStr;
            }

            rsgis::img::RSGISImageUtils imgUtils;
            GDALDataset* outImage = imgUtils.createBlankImage(outputImage, transformation, width, height, numBands, projection, pxlVal, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            GDALClose(outImage);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }


    void executeCreateCopyBlankImage(std::string inputImage, std::string outputImage, unsigned int numBands, float pxlVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
			GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISImageUtils imgUtils;
            GDALDataset *outDataset = imgUtils.createCopy(inDataset, numBands, outputImage, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            imgUtils.assignValGDALDataset(outDataset, pxlVal);

            GDALClose(inDataset);
            GDALClose(outDataset);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
                
    void executeCreateCopyBlankImage(std::string inputImage, std::string outputImage, unsigned int numBands, double xMin, double xMax, double yMin, double yMax, double resX, double resY, float pxlVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            GDALDataset *outDataset = imgUtils.createCopy(inDataset, numBands, outputImage, gdalFormat, RSGIS_to_GDAL_Type(outDataType), xMin, xMax, yMin, yMax, resX, resY);
            imgUtils.assignValGDALDataset(outDataset, pxlVal);
            
            GDALClose(inDataset);
            GDALClose(outDataset);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeCreateCopyBlankImageVecExtent(std::string inputImage, std::string inputVector, std::string outputImage, unsigned int numBands, float pxlVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            OGRRegisterAll();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            GDALDataset *inputVecDS = NULL;
            OGRLayer *inputVecLayer = NULL;
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).string();
            rsgis::vec::RSGISVectorUtils vecUtils;
            std::string vectorLayerName = vecUtils.getLayerName(inputVector);
            
            // Open vector
            inputVecDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
            if(inputVecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + vectorLayerName;
                throw RSGISFileException(message.c_str());
            }
            OGREnvelope ogrExtent;
            inputVecLayer->GetExtent(&ogrExtent);
            geos::geom::Envelope extent = geos::geom::Envelope(ogrExtent.MinX, ogrExtent.MaxX, ogrExtent.MinY, ogrExtent.MaxY);
            
            rsgis::img::RSGISImageUtils imgUtils;
            GDALDataset *outDataset = imgUtils.createCopy(inDataset, numBands, outputImage, gdalFormat, RSGIS_to_GDAL_Type(outDataType), extent);
            imgUtils.assignValGDALDataset(outDataset, pxlVal);
            
            GDALClose(inDataset);
            GDALClose(outDataset);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeStackStats(std::string inputImage, std::string outputImage, std::string calcStat, bool allBands, unsigned int numBands, std::string imageFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = NULL;
            rsgis::img::RSGISCalcImage *calcImage = NULL;

            datasets = new GDALDataset*[1];

            datasets[0] = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            int numRasterBands = datasets[0]->GetRasterCount();

            int numOutputBands = 1;

            // If calculating stats using all image bands
            if(allBands)
            {
                numBands = numRasterBands;
                std::cout << "Calculating " << calcStat << " over all bands of a " << numRasterBands << " band input image to create a " << numOutputBands << " band output image" << std::endl;
            }
            // If using ever n bands
            else
            {
                numOutputBands = numRasterBands / numBands;

                std::cout << "Calculating " << calcStat << " for every " << numBands << " bands of a " << numRasterBands << " band input image to create a " << numOutputBands << " band output image" << std::endl;
            }

            // Convert from string to enum
            rsgis::img::compositeStat outCompStat;
            if(calcStat == "mean"){outCompStat = rsgis::img::compositeMean;}
            else if(calcStat == "min"){outCompStat = rsgis::img::compositeMin;}
            else if(calcStat == "max"){outCompStat = rsgis::img::compositeMax;}
            else if(calcStat == "range"){outCompStat = rsgis::img::compositeRange;}
            else{throw RSGISCmdException("Statistic not recognized, options are: mean, min, max, range.");}

            rsgis::img::RSGISImageComposite *compositeImage = new rsgis::img::RSGISImageComposite(numOutputBands, numBands, outCompStat);
            calcImage = new rsgis::img::RSGISCalcImage(compositeImage, "", true);
            calcImage->calcImage(datasets, 1, outputImage, false, NULL, imageFormat, RSGIS_to_GDAL_Type(outDataType));

            // Tidy up
            GDALClose(datasets[0]);
            delete[] datasets;

            delete calcImage;
            delete compositeImage;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeProduceRegularGridImage(std::string inputImage, std::string outputImage, std::string gdalFormat, float pxlRes, int minVal, int maxVal, bool singleLine) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            GDALDataset *outDataset = NULL;
            
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            double *trans = new double[6];
            inDataset->GetGeoTransform(trans);
            
            double xRes = trans[1];
            if(xRes < 0)
            {
                xRes = xRes * -1;
            }
            double yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * -1;
            }
            
            trans[1] = pxlRes;
            trans[5] = pxlRes * (-1);
            
            
            std::cout << "Image Pixel Ratio: [" << (xRes/pxlRes) << "," << (yRes/pxlRes) << "]\n";
            
            int xSize = (inDataset->GetRasterXSize() * (xRes/pxlRes));
            int ySize = (inDataset->GetRasterYSize() * (yRes/pxlRes));
            
            std::cout << "Image Size: [" << xSize << "," << ySize << "]\n";
            
            std::string proj = "";
            
            rsgis::img::RSGISImageUtils imgUtils;
            outDataset = imgUtils.createBlankImage(outputImage, trans, xSize, ySize, 1, proj, 0.0, gdalFormat, GDT_Int32);
            outDataset->SetProjection(inDataset->GetProjectionRef());
            
            // Populate with pixel values.
            imgUtils.populateImagePixelsInRange(outDataset, minVal, maxVal, singleLine);
            
            outDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(outDataset);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    
    void executeFiniteImageMask(std::string inputImage, std::string outputImage, std::string gdalFormat) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = NULL;
            
            dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISMaskImage maskImg;
            maskImg.genFiniteImgMask(dataset, outputImage, gdalFormat);
            
            
            GDALDataset *outDataset = (GDALDataset *) GDALOpen(outputImage.c_str(), GA_Update);
            if(outDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + outputImage;
                throw RSGISImageException(message.c_str());
            }
            outDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(dataset);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeValidImageMask(std::vector<std::string> inputImages, std::string outputImage, std::string gdalFormat, float noDataVal) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            unsigned int numImages = inputImages.size();
            GDALDataset **datasets = new GDALDataset*[numImages];
            for(unsigned int i = 0; i < numImages; ++i)
            {
                std::cout << i << ") " << inputImages.at(i) << std::endl;
                datasets[i] = (GDALDataset *) GDALOpen(inputImages.at(i).c_str(), GA_ReadOnly);
                if(datasets[i] == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages.at(i);
                    throw RSGISImageException(message.c_str());
                }
            }
            
            rsgis::img::RSGISMaskImage maskImg;
            maskImg.genValidImgMask(datasets, numImages, outputImage, gdalFormat, noDataVal);
            
            GDALDataset *outDataset = (GDALDataset *) GDALOpen(outputImage.c_str(), GA_Update);
            if(outDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + outputImage;
                throw RSGISImageException(message.c_str());
            }
            outDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            for(unsigned int i = 0; i < numImages; ++i)
            {
                GDALClose(datasets[i]);
            }
            delete[] datasets;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeCombineImagesSingleBandIgnoreNoData(std::vector<std::string> inputImages, std::string outputImage, float noDataVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            unsigned int numImages = inputImages.size();
            GDALDataset **datasets = new GDALDataset*[numImages];
            
            for(unsigned int i = 0; i < numImages; ++i)
            {
                datasets[i] = (GDALDataset *) GDALOpen(inputImages.at(i).c_str(), GA_ReadOnly);
                if(datasets[i] == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages.at(i);
                    throw RSGISImageException(message.c_str());
                }
            }
            
            rsgis::img::RSGISCombineImagesIgnoreNoData *combineImagesCalc = new rsgis::img::RSGISCombineImagesIgnoreNoData(noDataVal);
            rsgis::img::RSGISCalcImage *calcImage = new rsgis::img::RSGISCalcImage(combineImagesCalc, "", true);
            
            calcImage->calcImage(datasets, numImages, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            delete calcImage;
            
            
            // Tidy up
            for(unsigned int i = 0; i < numImages; ++i)
            {
                GDALClose(datasets[i]);
            }
            delete[] datasets;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
            
            
    void executePerformRandomPxlSample(std::string inputImage, std::string outputImage, std::string gdalFormat, std::vector<int> maskVals, unsigned long numSamples) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();

            GDALDataset *inputImgDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputImgDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            if(inputImgDS->GetRasterCount() != 1)
            {
                throw RSGISImageException("The input image must only have one image band.");
            }
            
            GDALDataType outDSDataType = inputImgDS->GetRasterBand(1)->GetRasterDataType();
            
            rsgis::img::RSGISImageUtils imgUtils;
            GDALDataset *outImgDS =  imgUtils.createCopy(inputImgDS, 1, outputImage, gdalFormat, outDSDataType);
            
            std::cout << "Performing sampling - this can take some time if there are very few pixels of a given value.\n";
            std::cout << "\t If it is slow consider using rsgislib.imageutils.performRandomPxlSampleInMaskLowPxlCount.\n";
            rsgis::img::RSGISSampleImage sampleImg;
            sampleImg.randomSampleImageMask(inputImgDS, 1, outImgDS, maskVals, numSamples);
            std::cout << "Completed Sampling.\n";
            
            GDALClose(inputImgDS);
            GDALClose(outImgDS);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
                
    void executePerformRandomPxlSampleSmallPxlCount(std::string inputImage, std::string outputImage, std::string gdalFormat, std::vector<int> maskVals, unsigned long numSamples, int rndSeed) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *inputImgDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputImgDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            if(inputImgDS->GetRasterCount() != 1)
            {
                throw RSGISImageException("The input image must only have one image band.");
            }
            
            GDALDataType outDSDataType = inputImgDS->GetRasterBand(1)->GetRasterDataType();
            
            rsgis::img::RSGISImageUtils imgUtils;
            GDALDataset *outImgDS =  imgUtils.createCopy(inputImgDS, 1, outputImage, gdalFormat, outDSDataType);
            
            std::cout << "Performing sampling.\n";
            rsgis::img::RSGISSampleImage sampleImg;
            sampleImg.randomSampleImageMaskSmallPxlCount(inputImgDS, 1, outImgDS, maskVals, numSamples, rndSeed);
            std::cout << "Completed Sampling.\n";
            
            GDALClose(inputImgDS);
            GDALClose(outImgDS);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
                
                
                
    void executePerformHCSPanSharpen(std::string inputImage, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, unsigned int winSize, bool useNaiveMethod) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            int numRasterBands = dataset->GetRasterCount();
            
            // Calculate statistics
            float *imageStats = new float[4];  // Set up an array to hold image stats
            
            std::cout << "Calculating image mean.." << std::endl;
            rsgis::img::RSGISHCSPanSharpenCalcMeanStats panMean = rsgis::img::RSGISHCSPanSharpenCalcMeanStats(numRasterBands, imageStats);
            rsgis::img::RSGISCalcImage calcImageMean = rsgis::img::RSGISCalcImage(&panMean, "", true);
            calcImageMean.calcImage(&dataset, 1);
            panMean.returnStats();
            
            std::cout << "Calculating image standard deviation.." << std::endl;
            rsgis::img::RSGISHCSPanSharpenCalcSDStats panSD = rsgis::img::RSGISHCSPanSharpenCalcSDStats(numRasterBands, imageStats);
            rsgis::img::RSGISCalcImage calcImageSD = rsgis::img::RSGISCalcImage(&panSD, "", true);
            calcImageSD.calcImage(&dataset, 1);
            panSD.returnStats();

            
            std::cout << "Pan sharpening.." << std::endl;
            rsgis::img::RSGISHCSPanSharpen panSharpen = rsgis::img::RSGISHCSPanSharpen(numRasterBands - 1, imageStats);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&panSharpen, "", true);
            if(useNaiveMethod)
            {
                // naive mode
                calcImage.calcImage(&dataset, 1, outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            }
            else
            {
                // smart mode
                calcImage.calcImageWindowData(&dataset, 1, outputImage, winSize, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            }
            
            // Tidy up
            GDALClose(dataset);
            delete[] imageStats;
            
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
                
    void executeSharpenLowResImgBands(std::string inputImage, std::string outputImage, std::vector<RSGISInitSharpenBandInfo> bandInfo, unsigned int winSize, int noDataVal, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
            rsgis::utils::RSGISTextUtils textUtils;
            
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            int numRasterBands = dataset->GetRasterCount();
            
            rsgis::img::RSGISSharpenBandInfo *rsgisBandInfo = new rsgis::img::RSGISSharpenBandInfo[numRasterBands];
            bool *bandFound = new bool[numRasterBands];
            for(unsigned int i = 0; i < numRasterBands; ++i)
            {
                rsgisBandInfo[i].band = i+1;
                rsgisBandInfo[i].bandName = "";
                rsgisBandInfo[i].status = rsgis::img::rsgis_sharp_band_ignore;
                bandFound[i] = false;
            }
            
            unsigned int lowResCount = 0;
            unsigned int highResCount = 0;
            
            unsigned int idx = 0;
            for(std::vector<RSGISInitSharpenBandInfo>::iterator iterBands = bandInfo.begin(); iterBands != bandInfo.end(); ++iterBands)
            {
                if( ((*iterBands).band < 1) | ((*iterBands).band > numRasterBands))
                {
                    GDALClose(dataset);
                    delete[] rsgisBandInfo;
                    delete[] bandFound;
                    std::string message = std::string("The band specified (") + textUtils.uInttostring((*iterBands).band) + std::string(") is not within the image file; note band indexing starts at 1.");
                    throw RSGISImageException(message.c_str());
                }
                idx = ((*iterBands).band-1);
                if(!bandFound[idx])
                {
                    rsgisBandInfo[idx].band = (*iterBands).band;
                    rsgisBandInfo[idx].status = (rsgis::img::RSGISSharpenBandStatus) (*iterBands).status;
                    rsgisBandInfo[idx].bandName = (*iterBands).bandName;
                    bandFound[idx] = true;
                    
                    if(rsgisBandInfo[idx].status == rsgis::img::rsgis_sharp_band_lowres)
                    {
                        ++lowResCount;
                    }
                    else if(rsgisBandInfo[idx].status == rsgis::img::rsgis_sharp_band_highres)
                    {
                        ++highResCount;
                    }
                }
                else
                {
                    GDALClose(dataset);
                    delete[] rsgisBandInfo;
                    delete[] bandFound;
                    std::string message = std::string("The band specified (") + textUtils.uInttostring((*iterBands).band) + std::string(") is duplicated within the band info list.");
                    throw RSGISImageException(message.c_str());
                }
            }
            
            for(unsigned int i = 0; i < numRasterBands; ++i)
            {
                if(!bandFound[i])
                {
                    std::string message = std::string("Band info has not specified been specified for band ") + textUtils.uInttostring(rsgisBandInfo[i].band) + std::string(" - info must be specified for all image bands in the input image (note, bands can be ignored).");
                    GDALClose(dataset);
                    delete[] rsgisBandInfo;
                    delete[] bandFound;
                    throw RSGISImageException(message.c_str());
                }
            }
            
            if(lowResCount == 0)
            {
                GDALClose(dataset);
                delete[] rsgisBandInfo;
                delete[] bandFound;
                throw RSGISImageException("There must be at least 1 low resolution image band specified.");
            }
            else if(highResCount == 0)
            {
                GDALClose(dataset);
                delete[] rsgisBandInfo;
                delete[] bandFound;
                throw RSGISImageException("There must be at least 1 high resolution image band specified.");
            }
            
            std::cout << "Calculate the input image min and max image values per band.\n";
            // Calculate the image min and max values
            rsgis::img::ImageStats **imgStats = new rsgis::img::ImageStats*[numRasterBands];
            for(unsigned int i = 0; i < numRasterBands; ++i)
            {
                imgStats[i] = new rsgis::img::ImageStats();
                imgStats[i]->min = 0.0;
                imgStats[i]->max = 0.0;
                imgStats[i]->mean = 0.0;
                imgStats[i]->stddev = 0.0;
            }
            rsgis::img::RSGISImageStatistics imgStatsObj;
            imgStatsObj.calcImageStatistics(&dataset, 1, imgStats, numRasterBands, false, true, false);
            
            double *imgMinVal = new double[numRasterBands];
            double *imgMaxVal = new double[numRasterBands];
            for(unsigned int i = 0; i < numRasterBands; ++i)
            {
                imgMinVal[i] = imgStats[i]->min;
                imgMaxVal[i] = imgStats[i]->max;
                delete imgStats[i];
                std::cout << "Band " << (i+1) << " Min: " << imgMinVal[i] << " Max: " << imgMaxVal[i] << std::endl;
            }
            delete[] imgStats;
            
            // Perform image band sharpening.
            std::cout << "Perform image band sharpening.\n";
            rsgis::img::RSGISSharpenLowResBands sharpenImg = rsgis::img::RSGISSharpenLowResBands(numRasterBands, rsgisBandInfo, numRasterBands, lowResCount, highResCount, winSize, noDataVal, imgMinVal, imgMaxVal);
            rsgis::img::RSGISCalcImage calcImage = rsgis::img::RSGISCalcImage(&sharpenImg, "", true);
            calcImage.calcImageWindowData(&dataset, 1, outputImage, winSize, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            // Define the output image bands names.
            GDALDataset *outDataset = (GDALDataset *) GDALOpen(outputImage.c_str(), GA_Update);
            if(outDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + outputImage;
                throw RSGISImageException(message.c_str());
            }
            std::vector<std::string> bandNames;
            for(unsigned int i = 0; i < numRasterBands; ++i)
            {
                bandNames.push_back(rsgisBandInfo[i].bandName);
            }
            rsgis::img::RSGISImageUtils imgUtils;
            imgUtils.setImageBandNames(outDataset, bandNames, true);
            GDALClose(outDataset);
            
            // Tidy up
            GDALClose(dataset);
            delete[] rsgisBandInfo;
            delete[] bandFound;
            delete[] imgMinVal;
            delete[] imgMaxVal;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeCreateMaxNDVICompsiteImage(std::vector<std::string> inputImages, std::string outputImage, unsigned int redBand, unsigned int nirBand, std::string gdalFormat, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
            if(inputImages.size() < 2)
            {
                throw RSGISImageException("Input images list must have at least 2 images.");
            }
            
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[inputImages.size()];
            int imgIdx = 0;
            unsigned int numImgBands = 0;
            bool first = true;
            for(std::vector<std::string>::iterator iterImgs = inputImages.begin(); iterImgs != inputImages.end(); ++iterImgs)
            {
                std::cout << "Openning: " << (*iterImgs) << std::endl;
                datasets[imgIdx] = (GDALDataset *) GDALOpen((*iterImgs).c_str(), GA_ReadOnly);
                if(datasets[imgIdx] == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterImgs);
                    throw RSGISImageException(message.c_str());
                }
                if(first)
                {
                    numImgBands = datasets[imgIdx]->GetRasterCount();
                    first = false;
                }
                else if(numImgBands != datasets[imgIdx]->GetRasterCount())
                {
                    for(int i = 0; i <= imgIdx; ++i)
                    {
                        GDALClose(datasets[i]);
                    }
                    delete[] datasets;
                    throw RSGISImageException("Input images have different number of image bands.");
                }
                ++imgIdx;
            }
            
            rsgis::img::RSGISMaxNDVIImageComposite imgCompPxlCalc = rsgis::img::RSGISMaxNDVIImageComposite(numImgBands, redBand, nirBand, inputImages.size());
            
            rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(&imgCompPxlCalc, "", true);
            calcImg.calcImage(datasets, inputImages.size(), outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            // Tidy up
            for(int i = 0; i < inputImages.size(); ++i)
            {
                GDALClose(datasets[i]);
            }
            delete[] datasets;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
                
    void executeCreateRefImgCompsiteImage(std::vector<std::string> inputImages, std::string outputImage, std::string refImage, std::string gdalFormat, RSGISLibDataType outDataType, float outNoDataVal) throw(RSGISCmdException)
    {
        try
        {
            if(inputImages.size() < 2)
            {
                throw RSGISImageException("Input images list must have at least 2 images.");
            }
            
            GDALAllRegister();
            unsigned int totNumImgs = inputImages.size()+1;
            GDALDataset **datasets = new GDALDataset*[totNumImgs];
            int imgIdx = 0;
            
            std::cout << "Openning: " << (refImage) << std::endl;
            datasets[imgIdx] = (GDALDataset *) GDALOpen((refImage).c_str(), GA_ReadOnly);
            if(datasets[imgIdx] == NULL)
            {
                std::string message = std::string("Could not open image ") + (refImage);
                throw RSGISImageException(message.c_str());
            }
            if(datasets[imgIdx]->GetRasterCount() != 1)
            {
                GDALClose(datasets[imgIdx]);
                delete[] datasets;
                throw RSGISImageException("The reference image inputted has more than one image band.");
            }
            imgIdx = imgIdx + 1;
            
            unsigned int numImgBands = 0;
            bool first = true;
            for(std::vector<std::string>::iterator iterImgs = inputImages.begin(); iterImgs != inputImages.end(); ++iterImgs)
            {
                std::cout << "Openning: " << (*iterImgs) << std::endl;
                datasets[imgIdx] = (GDALDataset *) GDALOpen((*iterImgs).c_str(), GA_ReadOnly);
                if(datasets[imgIdx] == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterImgs);
                    throw RSGISImageException(message.c_str());
                }
                if(first)
                {
                    numImgBands = datasets[imgIdx]->GetRasterCount();
                    first = false;
                }
                else if(numImgBands != datasets[imgIdx]->GetRasterCount())
                {
                    for(int i = 0; i <= imgIdx; ++i)
                    {
                        GDALClose(datasets[i]);
                    }
                    delete[] datasets;
                    throw RSGISImageException("Input images have different number of image bands.");
                }
                ++imgIdx;
            }
            
            rsgis::img::RSGISRefImgImageComposite imgCompPxlCalc = rsgis::img::RSGISRefImgImageComposite(numImgBands, inputImages.size(), outNoDataVal);
            
            rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(&imgCompPxlCalc, "", true);
            calcImg.calcImage(datasets, 1, inputImages.size(), outputImage, false, NULL, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            
            // Tidy up
            for(int i = 0; i < totNumImgs; ++i)
            {
                GDALClose(datasets[i]);
            }
            delete[] datasets;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

}}

