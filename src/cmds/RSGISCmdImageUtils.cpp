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

#include "vec/RSGISImageTileVector.h"
#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorIO.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISPolygonData.h"

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

            rsgis::img::RSGISStretchImage stretchImg = rsgis::img::RSGISStretchImage(inDataset, outputImage, saveOutStats, outStatsFile, ignoreZeros, onePassSD, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
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
            //GDALDestroyDriverManager();
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

            rsgis::img::RSGISStretchImageWithStats stretchImg = rsgis::img::RSGISStretchImageWithStats(inDataset, outputImage, inStatsFile, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
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
                throw RSGISException("Stretch is not recognised.");
            }

            GDALClose(inDataset);
            //GDALDestroyDriverManager();
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
            //GDALDestroyDriverManager();
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

    void executeCreateTiles(std::string inputImage, std::string outputImageBase, float width, float height, float tileOverlap, bool offsetTiling, std::string gdalFormat, RSGISLibDataType outDataType, std::string outFileExtension, std::vector<std::string> *outFileNames)throw(RSGISCmdException)
    {
        GDALAllRegister();
        OGRRegisterAll();

        GDALDataset **dataset = NULL;

        rsgis::img::RSGISImageUtils imgUtils;
        rsgis::img::RSGISCopyImage *copyImage = NULL;
        rsgis::img::RSGISCalcImage *calcImage = NULL;

        int numImageBands = 0;
        std::string outputFilePath;

        try
        {
            // Open Image
            dataset = new GDALDataset*[1];
            //cout << inputImage << endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISCmdException(message.c_str());
            }
            numImageBands = dataset[0]->GetRasterCount();
            std::cout << "Raster Band Count = " << numImageBands << std::endl;

            // Set up envlopes for image tiles
            std::vector<geos::geom::Envelope*> *tileEnvelopes = new std::vector<geos::geom::Envelope*>;

            int numDS = 1;
            double *gdalTransform = new double[6];
            int **dsOffsets = new int*[numDS];
            for(int i = 0; i < numDS; i++)
            {
                dsOffsets[i] = new int[2];
            }
            int imgHeight = 0;
            int imgWidth = 0;

            imgUtils.getImageOverlap(dataset, numDS, dsOffsets, &imgWidth, &imgHeight, gdalTransform);

            double pixelXRes = gdalTransform[1];
            double pixelYRes = gdalTransform[5];
            double absPixelYRes = pixelYRes;
            if(absPixelYRes < 0){absPixelYRes = -1.0*absPixelYRes;}

            // Get absolute minimum and maximum values from image
            double imageMinX = gdalTransform[0];
            double imageMaxY = gdalTransform[3];
            double imageMaxX = imageMinX + (imgWidth * pixelXRes);
            double imageMinY = imageMaxY + (imgHeight * pixelYRes);

            // Get minimum and maximum images to use for tile grid
            double minX = imageMinX;
            double maxX = imageMaxX;
            double minY = imageMinY;
            double maxY = imageMaxY;

            if(offsetTiling)
            {
                minX -= (width * pixelXRes)/2;
                maxX += (width * pixelXRes)/2;
                if((height * pixelYRes) > 0)
                {
                    minY -= (height * pixelYRes)/2;
                    maxY += (height * pixelYRes)/2;
                }
                else
                {
                    minY += (height * pixelYRes)/2;
                    maxY -= (height * pixelYRes)/2;
                }

            }

            double tileWidthMapUnits = width * pixelXRes;
            double tileHeightMapUnits = height*pixelYRes;
            if(tileHeightMapUnits < 0){tileHeightMapUnits = tileHeightMapUnits*-1;}  // Make tile height positive (makes things simpler)
            double tileXOverlapMapUnits = tileOverlap * pixelXRes;
            double tileYOverlapMapUnits = tileOverlap * absPixelYRes;

            double xStart = 0;
            double yStart = 0;
            double yEnd = 0;
            double xEnd = 0;
            double xStartOverlap = 0;
            double yStartOverlap = 0;
            double xEndOverlap = 0;
            double yEndOverlap = 0;

            std::cout << "Tile Width: " << width << " pixels (" <<  tileWidthMapUnits << " map units)" << std::endl;
            std::cout << "Tile Height: " << height << " pixels (" <<  tileHeightMapUnits << " map units)" << std::endl;
            std::cout << "Tile Overlap: " << tileOverlap << " pixels (" <<  tileXOverlapMapUnits << " map units)" << std::endl;
            if(offsetTiling)
            {
                std::cout << "Tiling is offset by half a tile.\n";
            }

            // Start at top left corner and work down (minX, maxY)
            for(xStart = minX; xStart < maxX; xStart+=tileWidthMapUnits)
            {
                xEnd = xStart + tileWidthMapUnits;
                xStartOverlap = xStart - tileXOverlapMapUnits;
                xEndOverlap = xEnd + tileXOverlapMapUnits;

                if(xStartOverlap < imageMinX) // Check tile will fit within image
                {
                    xStartOverlap = imageMinX;
                }
                if(xEndOverlap > imageMaxX) // Check tile will fit within image
                {
                    xEndOverlap = imageMaxX;
                }

                if((xEndOverlap > imageMinX) && (xStartOverlap < xEndOverlap)) // Check x extent is within image (min and max), don't run if not
                {
                    for(yStart = maxY; yStart > minY; yStart-=tileHeightMapUnits)
                    {
                        yEnd = yStart - tileHeightMapUnits;

                        yStartOverlap = yStart + tileYOverlapMapUnits;
                        yEndOverlap = yEnd - tileYOverlapMapUnits;

                        if(yStartOverlap > imageMaxY) // Check tile will fit within image
                        {
                            yStartOverlap = imageMaxY+(0.5*absPixelYRes);
                        }
                        if(yEndOverlap < imageMinY) // Check tile will fit within image
                        {
                            yEndOverlap = imageMinY+(0.5*absPixelYRes);
                        }
                        if((yStartOverlap > imageMinY) && (yStartOverlap > yEndOverlap)) // Check y extent is within image (min and max), don't run if not
                        {
                            tileEnvelopes->push_back(new geos::geom::Envelope(xStartOverlap, xEndOverlap, yStartOverlap, yEndOverlap));
                        }
                    }
                }
            }

            copyImage = new rsgis::img::RSGISCopyImage(numImageBands);
            calcImage = new rsgis::img::RSGISCalcImage(copyImage, "", true);

            for(unsigned int i = 0; i < tileEnvelopes->size(); ++i)
            {
                std::cout << "Tile " << i+1 << "/" << tileEnvelopes->size() << std::endl;
                outputFilePath = outputImageBase + "_tile" + boost::lexical_cast<std::string>(i) + "." + outFileExtension;
                try
                {
                    calcImage->calcImageInEnv(dataset, 1, outputFilePath, tileEnvelopes->at(i), false, NULL, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
                    // Save out file name to vector
                    if(outFileNames != NULL){outFileNames->push_back(outputFilePath);}
                }
                catch (rsgis::img::RSGISImageBandException e)
                {
                    throw RSGISCmdException(e.what());
                }
            }

            GDALClose(dataset[0]);
            delete[] dataset;
            //GDALDestroyDriverManager();
            delete calcImage;
            delete copyImage;
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
            //GDALDestroyDriverManager();
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

            //std::cout << "TL: [" << trans[0] << "," << trans[3] << "]" << std::endl;
            //std::cout << "RES: [" << trans[1] << "," << trans[5] << "]" << std::endl;
            //std::cout << "ROT: [" << trans[2] << "," << trans[4] << "]" << std::endl;

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
            OGRCleanupAll();
            //GDALDestroyDriverManager();
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
            OGRCleanupAll();
            //GDALDestroyDriverManager();
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

            //GDALDestroyDriverManager();
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
            
            rsgis::img::RSGISSampleImage sampleImg;
            sampleImg.randomSampleImageMask(inputImgDS, 1, outImgDS, maskVals, numSamples);
            
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
            
    
}}

