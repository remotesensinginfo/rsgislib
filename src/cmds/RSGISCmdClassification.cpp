/*
 *  RSGISCmdClassification.cpp
 *
 *
 *  Created by Pete Bunting on 18/11/2013.
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

#include "RSGISCmdClassification.h"
#include "RSGISCmdParent.h"

#include "common/RSGISException.h"
#include "common/RSGISImageException.h"

#include "img/RSGISImageUtils.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISVectorOutputException.h"

#include "classifier/RSGISRATClassificationUtils.h"
#include "classifier/RSGISGenAccuracyPoints.h"

#include "utils/RSGISFileUtils.h"

#include <boost/filesystem.hpp>

namespace rsgis{ namespace cmds {
    
    void executeCollapseRAT2Class(std::string clumpsImage, std::string outputImage, std::string outImageFormat, std::string classColumn, std::string classIntCol, bool useIntCol)
    {
        try
        {
            rsgis::img::RSGISImageUtils imgUtils;
            
            std::cout << "Opening an image\n";
            GDALAllRegister();
            GDALDataset *imageDataset = NULL;
            imageDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_ReadOnly);
            if(imageDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::classifier::RSGISCollapseSegmentsClassification collapseSegments;
            collapseSegments.collapseClassification(imageDataset, classColumn, classIntCol, useIntCol, outputImage, outImageFormat);
            
            // Tidy up
            GDALClose(imageDataset);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeGenerate3BandFromColourTable(std::string clumpsImage, std::string outputImage, std::string outImageFormat)
    {
        try
        {
            std::cout << "Openning input image.\n";
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALAllRegister();
            GDALDataset **imageDataset = new GDALDataset*[1];
            imageDataset[0] = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_ReadOnly);
            if(imageDataset[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::cout << "Reading colour table\n";
            GDALColorTable *clrTab = imageDataset[0]->GetRasterBand(1)->GetColorTable();

            std::string *bandNames = new std::string[3];
            bandNames[0] = "Red";
            bandNames[1] = "Green";
            bandNames[2] = "Blue";
            
            std::cout << "Applying to the image\n";
            rsgis::classifier::RSGISColourImageFromClassRAT *clrAsRGB = new rsgis::classifier::RSGISColourImageFromClassRAT(clrTab);
            rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(clrAsRGB, "", true);
            calcImg.calcImage(imageDataset, 1, outputImage, true, bandNames, outImageFormat, GDT_Byte);
            delete[] bandNames;
            
            // Tidy up
            GDALClose(imageDataset[0]);
            delete[] imageDataset;
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    
    void executeGenerateRandomAccuracyPts(std::string classImage, std::string outputVecFile, std::string outputVecLyr, std::string outVecFormat, std::string classImgCol, std::string classImgVecCol, std::string classRefVecCol, unsigned int numPts, unsigned int seed, bool force)
    {
        try
        {
            GDALAllRegister();
            OGRRegisterAll();
            
            GDALDataset *imgDataset = (GDALDataset *) GDALOpenShared(classImage.c_str(), GA_ReadOnly);
            if(imgDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + classImage;
                throw RSGISCmdException(message.c_str());
            }
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;

            outputVecFile = boost::filesystem::absolute(outputVecFile).string();

            GDALDriver *vecDriver = NULL;
            GDALDataset *outVecDS = NULL;
            OGRLayer *outVecLyrObj = NULL;
            OGRSpatialReference* ogrSpatialRef = NULL;

            if(outVecFormat == "ESRI Shapefile")
            {
                std::string outputDIR = fileUtils.getFileDirectoryPath(outputVecFile);
                if (vecUtils.checkDIR4SHP(outputDIR, outputVecLyr))
                {
                    if (force)
                    {
                        vecUtils.deleteSHP(outputDIR, outputVecLyr);
                    }
                    else
                    {
                        throw RSGISException("Vector file already exists, either delete or select force.");
                    }
                }
            }
            else
            {
                if(fileUtils.checkFilePresent(outputVecFile) & force)
                {
                    fileUtils.removeFileIfPresent(outputVecFile);
                }
                else
                {
                    throw RSGISException("Vector file already exists, either delete or select force.");
                }
            }
            ogrSpatialRef = new OGRSpatialReference(imgDataset->GetProjectionRef());
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            vecDriver = GetGDALDriverManager()->GetDriverByName(outVecFormat.c_str());
            if( vecDriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("Vector driver not available: " + outVecFormat);
            }
            outVecDS = vecDriver->Create(outputVecFile.c_str(), 0, 0, 0, GDT_Unknown, NULL);
            if( outVecDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVecFile;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outVecLyrObj = outVecDS->CreateLayer(outputVecLyr.c_str(), ogrSpatialRef, wkbPoint, NULL );
            if( outVecLyrObj == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + outputVecLyr;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            rsgis::classifier::RSGISGenAccuracyPoints genAccPts;
            genAccPts.generateRandomPointsVecOut(imgDataset, outVecLyrObj, classImgCol, classImgVecCol, classRefVecCol, numPts, seed);

            GDALClose(imgDataset);
            GDALClose(outVecDS);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    
    void executeGenerateStratifiedRandomAccuracyPts(std::string classImage, std::string outputVecFile, std::string outputVecLyr, std::string outVecFormat, std::string classImgCol, std::string classImgVecCol, std::string classRefVecCol, unsigned int numPtsPerClass, unsigned int seed, bool force, bool usePxlLst)
    {
        try
        {
            GDALAllRegister();
            OGRRegisterAll();
            
            GDALDataset *imgDataset = (GDALDataset *) GDALOpenShared(classImage.c_str(), GA_ReadOnly);
            if(imgDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + classImage;
                throw RSGISCmdException(message.c_str());
            }
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;

            outputVecFile = boost::filesystem::absolute(outputVecFile).string();

            GDALDriver *vecDriver = NULL;
            GDALDataset *outputVecDS = NULL;
            OGRLayer *outputVecLyrObj = NULL;
            OGRSpatialReference* ogrSpatialRef = NULL;

            if(outVecFormat == "ESRI Shapefile")
            {
                std::string outputDIR = fileUtils.getFileDirectoryPath(outputVecFile);
                if (vecUtils.checkDIR4SHP(outputDIR, outputVecLyr))
                {
                    if (force)
                    {
                        vecUtils.deleteSHP(outputDIR, outputVecLyr);
                    }
                    else
                    {
                        throw RSGISException("Vector file already exists, either delete or select force.");
                    }
                }
            }
            else
            {
                if(fileUtils.checkFilePresent(outputVecFile) & force)
                {
                    fileUtils.removeFileIfPresent(outputVecFile);
                }
                else
                {
                    throw RSGISException("Vector file already exists, either delete or select force.");
                }
            }
            ogrSpatialRef = new OGRSpatialReference(imgDataset->GetProjectionRef());
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            vecDriver = GetGDALDriverManager()->GetDriverByName(outVecFormat.c_str());
            if( vecDriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("Vector driver not available: " + outVecFormat);
            }
            outputVecDS = vecDriver->Create(outputVecFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputVecDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVecFile;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputVecLyrObj = outputVecDS->CreateLayer(outputVecLyr.c_str(), ogrSpatialRef, wkbPoint, NULL );
            if( outputVecLyrObj == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + outputVecLyr;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            rsgis::classifier::RSGISGenAccuracyPoints genAccPts;
            if(!usePxlLst)
            {
                genAccPts.generateStratifiedRandomPointsVecOut(imgDataset, outputVecLyrObj, classImgCol, classImgVecCol, classRefVecCol, numPtsPerClass, seed);
            }
            else
            {
                genAccPts.generateStratifiedRandomPointsVecOutUsePxlLst(imgDataset, outputVecLyrObj, classImgCol, classImgVecCol, classRefVecCol, numPtsPerClass, seed);
            }
            
            GDALClose(imgDataset);
            GDALClose(outputVecDS);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executePopClassInfoAccuracyPts(std::string classImage, std::string vecFile, std::string vecLyr, std::string classImgCol, std::string classImgVecCol, std::string classRefVecCol, bool addRefCol)
    {
        try
        {
            GDALAllRegister();
            OGRRegisterAll();
            
            GDALDataset *imgDataset = (GDALDataset *) GDALOpenShared(classImage.c_str(), GA_ReadOnly);
            if(imgDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + classImage;
                throw RSGISCmdException(message.c_str());
            }
            
            rsgis::utils::RSGISFileUtils fileUtils;

            vecFile = boost::filesystem::absolute(vecFile).string();

            /////////////////////////////////////
            //
            // Open Input Vector File.
            //
            /////////////////////////////////////
            GDALDataset *inputVecDS = NULL;
            inputVecDS = (GDALDataset*) GDALOpenEx(vecFile.c_str(), GDAL_OF_UPDATE, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + vecFile;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inVecLyrObj = inputVecDS->GetLayerByName(vecLyr.c_str());
            if(inVecLyrObj == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + vecLyr;
                throw RSGISFileException(message.c_str());
            }
            
            rsgis::classifier::RSGISGenAccuracyPoints genAccPts;
            genAccPts.popClassInfo2Vec(imgDataset, inVecLyrObj, classImgCol, classImgVecCol, classRefVecCol, addRefCol);
            
            GDALClose(imgDataset);
            GDALClose(inputVecDS);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

}}

