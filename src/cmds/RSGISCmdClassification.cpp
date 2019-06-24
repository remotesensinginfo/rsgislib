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
    
    
    void executeGenerateRandomAccuracyPts(std::string classImage, std::string outputShp, std::string classImgCol, std::string classImgVecCol, std::string classRefVecCol, unsigned int numPts, unsigned int seed, bool force)
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
            
            outputShp = boost::filesystem::absolute(outputShp).string();
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputShp);
            
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* ogrSpatialRef = NULL;
            
            std::string outputDIR = "";
            
            outputDIR = fileUtils.getFileDirectoryPath(outputShp);
            
            if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
            {
                if(force)
                {
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    throw RSGISException("Shapefile already exists, either delete or select force.");
                }
            }
            ogrSpatialRef = new OGRSpatialReference(imgDataset->GetProjectionRef());
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputShp.c_str(), 0, 0, 0, GDT_Unknown, NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputShp;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), ogrSpatialRef, wkbPoint, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            rsgis::classifier::RSGISGenAccuracyPoints genAccPts;
            genAccPts.generateRandomPointsVecOut(imgDataset, outputSHPLayer, classImgCol, classImgVecCol, classRefVecCol, numPts, seed);

            GDALClose(imgDataset);
            GDALClose(outputSHPDS);
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

    
    void executeGenerateStratifiedRandomAccuracyPts(std::string classImage, std::string outputShp, std::string classImgCol, std::string classImgVecCol, std::string classRefVecCol, unsigned int numPtsPerClass, unsigned int seed, bool force, bool usePxlLst)
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
            
            outputShp = boost::filesystem::absolute(outputShp).string();
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputShp);
            
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* ogrSpatialRef = NULL;
            
            std::string outputDIR = "";
            
            outputDIR = fileUtils.getFileDirectoryPath(outputShp);
            
            if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
            {
                if(force)
                {
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    throw RSGISException("Shapefile already exists, either delete or select force.");
                }
            }
            ogrSpatialRef = new OGRSpatialReference(imgDataset->GetProjectionRef());
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputShp.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputShp;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), ogrSpatialRef, wkbPoint, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            rsgis::classifier::RSGISGenAccuracyPoints genAccPts;
            if(!usePxlLst)
            {
                genAccPts.generateStratifiedRandomPointsVecOut(imgDataset, outputSHPLayer, classImgCol, classImgVecCol, classRefVecCol, numPtsPerClass, seed);
            }
            else
            {
                genAccPts.generateStratifiedRandomPointsVecOutUsePxlLst(imgDataset, outputSHPLayer, classImgCol, classImgVecCol, classRefVecCol, numPtsPerClass, seed);
            }
            
            GDALClose(imgDataset);
            GDALClose(outputSHPDS);
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
    
    void executePopClassInfoAccuracyPts(std::string classImage, std::string shpFile, std::string classImgCol, std::string classImgVecCol, std::string classRefVecCol, bool addRefCol)
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
            
            shpFile = boost::filesystem::absolute(shpFile).string();
            std::string SHPFileInLayer = vecUtils.getLayerName(shpFile);
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            GDALDataset *inputSHPDS = NULL;
            inputSHPDS = (GDALDataset*) GDALOpenEx(shpFile.c_str(), GDAL_OF_UPDATE, NULL, NULL, NULL);  
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + shpFile;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            
            rsgis::classifier::RSGISGenAccuracyPoints genAccPts;
            genAccPts.popClassInfo2Vec(imgDataset, inputSHPLayer, classImgCol, classImgVecCol, classRefVecCol, addRefCol);
            
            GDALClose(imgDataset);
            GDALClose(inputSHPDS);
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

