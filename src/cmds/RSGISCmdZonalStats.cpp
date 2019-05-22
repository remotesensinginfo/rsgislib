/*
 *  RSGISCmdZonalStats.cpp
 *
 *
 *  Created by Dan Clewley on 08/08/2013.
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

#include "RSGISCmdZonalStats.h"
#include "RSGISCmdParent.h"

#include "common/RSGISVectorException.h"
#include "common/RSGISException.h"

#include "math/RSGISMatrices.h"

#include "img/RSGISPixelInPoly.h"
#include "img/RSGISPixelInPoly.cpp"

#include "vec/RSGISVectorZonalStats.h"
#include "vec/RSGISZonalStats2Matrix.h"
#include "vec/RSGISProcessVector.h"
#include "vec/RSGISZonalStats.h"
#include "vec/RSGISZonalImage2HDF.h"
#include "vec/RSGISExtractEndMembers2Matrix.h"

#include <boost/algorithm/string.hpp>

namespace rsgis{ namespace cmds {

    void executePointValue(std::string inputImage, std::string inputVecPolys, std::string outputStatsFile, bool outputToText,
                           bool force, bool useBandNames, bool shortenBandNames)throw(RSGISCmdException)
    {
        // Convert to absolute path
        inputVecPolys = boost::filesystem::absolute(inputVecPolys).string();

        GDALAllRegister();
        OGRRegisterAll();

        rsgis::utils::RSGISFileUtils fileUtils;
        rsgis::vec::RSGISVectorUtils vecUtils;

        rsgis::vec::RSGISProcessVector *processVector = NULL;
        rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;

        std::string SHPFileInLayer = vecUtils.getLayerName(inputVecPolys);
        std::string SHPFileOutLayer = "";

        GDALDataset *inputImageDS = NULL;
        GDALDataset *inputSHPDS = NULL;
        OGRLayer *inputSHPLayer = NULL;
        GDALDriver *shpFiledriver = NULL;
        GDALDataset *outputSHPDS = NULL;
        OGRLayer *outputSHPLayer = NULL;
        OGRSpatialReference* inputSpatialRef = NULL;

        std::string outputDIR = "";

        try
        {
            if (!outputToText)
            {
                SHPFileOutLayer = vecUtils.getLayerName(outputStatsFile);
                outputDIR = fileUtils.getFileDirectoryPath(outputStatsFile);

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
            }

            /////////////////////////////////////
            //
            // Open Input Image.
            //
            /////////////////////////////////////
            inputImageDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputImageDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISException(message.c_str());
            }

            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVecPolys;
                throw RSGISException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISException(message.c_str());
            }

            inputSpatialRef = inputSHPLayer->GetSpatialRef();

            OGRFeature *feature = inputSHPLayer->GetFeature(1);
            OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
            OGRFeature::DestroyFeature(feature);

            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            if (!outputToText)
            {
                const char *pszDriverName = "ESRI Shapefile";
                shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
                if( shpFiledriver == NULL )
                {
                    throw RSGISException("SHP driver not available.");
                }
                outputSHPDS = shpFiledriver->Create(outputStatsFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
                if( outputSHPDS == NULL )
                {
                    std::string message = std::string("Could not create vector file ") + outputStatsFile;
                    throw RSGISException(message.c_str());
                }
                outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
                if( outputSHPLayer == NULL )
                {
                    std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                    throw RSGISException(message.c_str());
                }

                processFeature = new rsgis::vec::RSGISVectorZonalStats(inputImageDS,"",useBandNames);
                processVector = new rsgis::vec::RSGISProcessVector(processFeature);

                processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);
                GDALClose(outputSHPDS);

                delete processVector;
                delete processFeature;
            }

            else
            {

                processFeature = new rsgis::vec::RSGISVectorZonalStats(inputImageDS, outputStatsFile, useBandNames, shortenBandNames);
                processVector = new rsgis::vec::RSGISProcessVector(processFeature);

                processVector->processVectorsNoOutput(inputSHPLayer, true);

                delete processVector;
                delete processFeature;

            }

            GDALClose(inputImageDS);
            GDALClose(inputSHPDS);
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

    void executePixelStats(std::string inputImage, std::string inputVecPolys, std::string outputStatsFile, rsgis::cmds::RSGISBandAttZonalStatsCmds *calcStats,
                           std::string inputRasPolys, bool outputToText, bool force, bool useBandNames,
                           bool ignoreProjection, int pixelInPolyMethodInt, bool shortenBandNames) throw(RSGISCmdException)
    {
        // Convert to absolute path
        inputVecPolys = boost::filesystem::absolute(inputVecPolys).string();

        // Convert from int to enum
        rsgis::img::pixelInPolyOption pixelInPolyMethod = rsgis::img::pixelInPolyInt2Enum(pixelInPolyMethodInt);

        // Check for raster version of polygon
        bool useRasPoly = false;
        if(inputRasPolys != ""){useRasPoly = false;}

        unsigned int numAttributes = 0; // Number of attributes (bands)

        // Calculate the same statistics for all bands
		bool minAll = calcStats->calcMin;
		bool maxAll = calcStats->calcMax;
		bool meanAll = calcStats->calcMean;
		bool stdDevAll = calcStats->calcStdDev;
        bool modeAll = calcStats->calcMode;
        bool sumAll = calcStats->calcSum;
		bool countAll = calcStats->calcCount;
        bool pxlcount = true;
		double minThreshAllVal = calcStats->minThreshold;
		double maxThreshAllVal = calcStats->maxThreshold;

        std::cout << "Calculating: ";
        if(minAll){std::cout << "min, ";}
        if(maxAll){std::cout << "max, ";}
        if(meanAll){std::cout << "mean, ";}
        if(stdDevAll){std::cout << "stdDev, ";}
        if(modeAll){std::cout << "mode, ";}
        if(countAll){std::cout << "cont, ";}
        std::cout << "for pixels with values between " << minThreshAllVal << " and " << maxThreshAllVal << "." << std::endl;
        std::cout << "and pixelcount for all pixels in polygon." << std::endl;

        // Copy attributes to shapefile be default
        bool copyAttributes = true;

        GDALAllRegister();
        OGRRegisterAll();

        rsgis::utils::RSGISFileUtils fileUtils;
        rsgis::vec::RSGISVectorUtils vecUtils;
        rsgis::math::RSGISMathsUtils mathsUtil;

        rsgis::vec::RSGISProcessVector *processVector = NULL;
        rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;

        std::string SHPFileInLayer = vecUtils.getLayerName(inputVecPolys);
        std::string SHPFileOutLayer = "";
        if (!outputToText){SHPFileOutLayer = vecUtils.getLayerName(outputStatsFile);}

        GDALDataset *inputImageDS = NULL;
        GDALDataset *inputRasterFeaturesDS = NULL;
        GDALDataset *inputSHPDS = NULL;
        OGRLayer *inputSHPLayer = NULL;
        GDALDriver *shpFiledriver = NULL;
        GDALDataset *outputSHPDS = NULL;
        OGRLayer *outputSHPLayer = NULL;
        OGRSpatialReference* inputSpatialRef = NULL;

        std::string outputDIR = "";

        try
        {
            outputDIR = fileUtils.getFileDirectoryPath(outputStatsFile);

            // Check is output shapefile exists
            if (!outputToText)
            {
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
            }

            /////////////////////////////////////
            //
            // Open Input Image.
            //
            /////////////////////////////////////

            inputImageDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputImageDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISException(message.c_str());
            }


            // Set up attributes if using all bands
            unsigned int nImageBands = inputImageDS->GetRasterCount();
            std::cout << "Calculating stats for " << nImageBands << " bands." << std::endl;
            numAttributes = nImageBands;
            rsgis::vec::ZonalAttributes **attributeZonalList = new rsgis::vec::ZonalAttributes*[nImageBands];
            for(unsigned int i = 0; i < nImageBands; i++)
            {
                attributeZonalList[i] = new rsgis::vec::ZonalAttributes();
                std::string bandNumberStr = mathsUtil.inttostring(i + 1).c_str();
                attributeZonalList[i]->name = "b" + bandNumberStr;
                attributeZonalList[i]->outMin = minAll;
                attributeZonalList[i]->outMax = maxAll;
                attributeZonalList[i]->outMean = meanAll;
                attributeZonalList[i]->outStDev = stdDevAll;
                attributeZonalList[i]->outMode = modeAll;
                attributeZonalList[i]->outSum = sumAll;
                attributeZonalList[i]->outCount = countAll;
                attributeZonalList[i]->numBands = 1;
                attributeZonalList[i]->bands = new int[1];
                attributeZonalList[i]->minThresholds = new float[1];
                attributeZonalList[i]->maxThresholds = new float[1];
                attributeZonalList[i]->bands[0] = i;
                attributeZonalList[i]->minThresholds[0] = minThreshAllVal;
                attributeZonalList[i]->maxThresholds[0] = maxThreshAllVal;

                // If using band names get names from image
                if(useBandNames)
                {
                    std::string bandName = inputImageDS->GetRasterBand(i+1)->GetDescription();

                    // Replace spaces and parentheses in file name
                    boost::algorithm::replace_all(bandName, " ", "_");
                    boost::algorithm::replace_all(bandName, "(", "_");
                    boost::algorithm::replace_all(bandName, ")", "_");
                    boost::algorithm::replace_all(bandName, "[", "_");
                    boost::algorithm::replace_all(bandName, "]", "_");
                    boost::algorithm::replace_all(bandName, ":", "");
                    boost::algorithm::replace_all(bandName, "?", "");
                    boost::algorithm::replace_all(bandName, ">", "gt");
                    boost::algorithm::replace_all(bandName, "<", "lt");
                    boost::algorithm::replace_all(bandName, "=", "eq");

                    /* Check if band name us longer than maximum length for shapefile field name
                     No limit on CSV but makes management easier with shorter names */
                    if((bandName.length() > 7) && shortenBandNames)
                    {
                        // If not using all of name, append number so unique
                        std::cerr << "WARNING: "<< bandName << " will be truncated to \'" << bandName.substr(0, 5) << i+1 << "\'" << std::endl;
                        attributeZonalList[i]->name = bandName.substr(0, 5) + mathsUtil.inttostring(i+1);
                    }
                    else if(bandName == "")
                    {
                        // If escription is empty, use b1 etc.,
                        attributeZonalList[i]->name  = std::string("b") + mathsUtil.inttostring(i+1);
                    }
                    else{attributeZonalList[i]->name = bandName;}
                }

                else
                {
                    attributeZonalList[i]->name = std::string("b") + mathsUtil.inttostring(i+1);
                }
            }

            if (useRasPoly)
            {
                /////////////////////////////////////
                //
                // Open Rasterised Polygon Image.
                //
                /////////////////////////////////////
                inputRasterFeaturesDS = (GDALDataset *) GDALOpen(inputRasPolys.c_str(), GA_ReadOnly);
                if(inputRasterFeaturesDS == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputRasPolys;
                    throw RSGISException(message.c_str());
                }

                std::cout << "Using raster for point in polygon...\n";
            }

            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVecPolys;
                throw RSGISException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISException(message.c_str());
            }

            inputSpatialRef = inputSHPLayer->GetSpatialRef();

            // Check the projection is the same for shapefile and image
            if(!ignoreProjection)
            {
                const char *pszWKTImg = inputImageDS->GetProjectionRef();
                char **pszWKTShp = new char*[1];
                inputSpatialRef->exportToWkt(pszWKTShp);

                if((std::string(pszWKTImg) != std::string(pszWKTShp[0])))
                {
                    std::cerr << "WARNING: Shapefile and image are not the same projection!\n\tImage is: " + std::string(pszWKTImg) + "\n\tShapefile is: " + std::string(pszWKTShp[0]) << "\n...Continuing anyway" << std::endl;
                }
                CPLFree(pszWKTShp);
            }

            if(!outputToText)
            {
                /////////////////////////////////////
                //
                // Create Output Shapfile.
                //
                /////////////////////////////////////
                const char *pszDriverName = "ESRI Shapefile";
                shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
                if( shpFiledriver == NULL )
                {
                    throw RSGISException("SHP driver not available.");
                }
                outputSHPDS = shpFiledriver->Create(outputStatsFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
                if( outputSHPDS == NULL )
                {
                    std::string message = std::string("Could not create vector file ") + outputStatsFile;
                    throw RSGISException(message.c_str());
                }
                outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
                if( outputSHPLayer == NULL )
                {
                    std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                    throw RSGISException(message.c_str());
                }

            }

            if(useRasPoly)
            {
                processFeature = new rsgis::vec::RSGISZonalStats(inputImageDS, inputRasterFeaturesDS, attributeZonalList, numAttributes, pxlcount, outputStatsFile);
                processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            }
            else
            {
                processFeature = new rsgis::vec::RSGISZonalStatsPoly(inputImageDS, attributeZonalList, numAttributes, pxlcount, pixelInPolyMethod, outputStatsFile);
                processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            }
            if (outputToText)
            {
                processVector->processVectorsNoOutput(inputSHPLayer, true);
            }
            else
            {
                processVector->processVectors(inputSHPLayer, outputSHPLayer, copyAttributes, true, false);
            }


            // TIDY
            GDALClose(inputImageDS); // Close input image
            std::cout << "Image closed OK" << std::endl;
            if(useRasPoly) {GDALClose(inputRasterFeaturesDS);} // Close rasterised poly (if using)
            GDALClose(inputSHPDS); // Close inputshape
            std::cout << "in shp closed OK" << std::endl;
            if (!outputToText)
            {
                GDALClose(outputSHPDS); // Close output shape
                std::cout << "out shp closed OK" << std::endl;
            }
            for(int i = 0; i < numAttributes; i++)
            {
                delete[] attributeZonalList[i]->bands;
                delete[] attributeZonalList[i]->minThresholds;
                delete[] attributeZonalList[i]->maxThresholds;
                delete attributeZonalList[i];
            }
            delete[] attributeZonalList;

            delete processVector;
            delete processFeature;
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

    void executePixelVals2txt(std::string inputImage, std::string inputVecPolys, std::string outputTextBase, std::string polyAttribute, std::string outtxtform,
                              bool ignoreProjection, int pixelInPolyMethodInt) throw(RSGISCmdException)
    {
        // Convert to absolute path
        inputVecPolys = boost::filesystem::absolute(inputVecPolys).string();

        rsgis::math::outTXTform  rOutTextForm = rsgis::math::csv;
        if(outtxtform == "mtxt"){rOutTextForm = rsgis::math::mtxt;}
        else if(outtxtform == "gtxt"){rOutTextForm = rsgis::math::gtxt;}

        rsgis::img::pixelInPolyOption pixelInPolyMethod = rsgis::img::pixelInPolyInt2Enum(pixelInPolyMethodInt);

        GDALAllRegister();
        OGRRegisterAll();
        rsgis::math::RSGISMathsUtils mathsUtil;
        rsgis::utils::RSGISFileUtils fileUtils;
        rsgis::vec::RSGISVectorUtils vecUtils;

        rsgis::vec::RSGISProcessVector *processVector = NULL;
        rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;

        std::string SHPFileInLayer = vecUtils.getLayerName(inputVecPolys);

        GDALDataset *inputImageDS = NULL;
        GDALDataset *inputSHPDS = NULL;
        OGRLayer *inputSHPLayer = NULL;
        OGRSpatialReference* inputSpatialRef = NULL;

        std::string outputDIR = "";

        try
        {

            /////////////////////////////////////
            //
            // Open Input Image.
            //
            /////////////////////////////////////

            inputImageDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputImageDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISException(message.c_str());
            }

            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVecPolys.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);   
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVecPolys;
                throw rsgis::RSGISException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISException(message.c_str());
            }

            inputSpatialRef = inputSHPLayer->GetSpatialRef();

            // Check the projection is the same for shapefile and image
            if(!ignoreProjection)
            {
                const char *pszWKTImg = inputImageDS->GetProjectionRef();
                char **pszWKTShp = new char*[1];
                inputSpatialRef->exportToWkt(pszWKTShp);

                if((std::string(pszWKTImg) != std::string(pszWKTShp[0])))
                {
                    std::cerr << "WARNING: Shapefile and image are not the same projection!\n\tImage is: " + std::string(pszWKTImg) + "\n\tShapefile is: " + std::string(pszWKTShp[0]) << "\n...Continuing anyway" << std::endl;
                }
                CPLFree(pszWKTShp);
            }

            processFeature = new rsgis::vec::RSGISPixelVals22Txt(inputImageDS, outputTextBase, polyAttribute, rOutTextForm, pixelInPolyMethod);
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);

            processVector->processVectorsNoOutput(inputSHPLayer, true);


            // TIDY
            delete processFeature;
            delete processVector;

            GDALClose(inputImageDS); // Close input image
            std::cout << "Image closed OK" << std::endl;
            GDALClose(inputSHPDS); // Close inputshape
            std::cout << "Shapefile closed OK" << std::endl;
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

    void executeZonesImage2HDF5(std::string inputImage, std::string inputVecPolys, std::string outputHDF, bool ignoreProjection, int pixelInPolyMethodInt) throw(RSGISCmdException)
    {
        std::cout.precision(12);
        // Convert to absolute path
        std::string inputVecPolysFullPath = std::string(boost::filesystem::absolute(inputVecPolys).string());

        GDALAllRegister();
        OGRRegisterAll();

        rsgis::vec::RSGISVectorUtils vecUtils;

        std::string SHPFileInLayer = vecUtils.getLayerName(inputVecPolysFullPath);

        GDALDataset *inputImageDS = NULL;
        GDALDataset *inputSHPDS = NULL;
        OGRLayer *inputSHPLayer = NULL;

        std::string outputDIR = "";

        try
        {
            /////////////////////////////////////
            //
            // Open Input Image.
            //
            /////////////////////////////////////
            inputImageDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputImageDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISException(message.c_str());
            }

            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVecPolysFullPath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVecPolysFullPath;
                throw RSGISException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISException(message.c_str());
            }

            rsgis::vec::RSGISZonalImage2HDF zonalImg2HDF;
            rsgis::img::pixelInPolyOption pixelInPolyMethod = rsgis::img::pixelInPolyInt2Enum(pixelInPolyMethodInt);
            zonalImg2HDF.extractBandsToColumns(inputImageDS, inputSHPLayer, outputHDF, pixelInPolyMethod);

            GDALClose(inputImageDS);
            GDALClose(inputSHPDS);
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
        
    void executeExtractAvgEndMembers(std::string inputImage, std::string inputVecPolys, std::string outputMatrixFile, int pixelInPolyMethodInt)throw(RSGISCmdException)
    {
        std::cout.precision(12);
        // Convert to absolute path
        std::string inputVecPolysFullPath = std::string(boost::filesystem::absolute(inputVecPolys).string());
        
        GDALAllRegister();
        OGRRegisterAll();
        
        rsgis::vec::RSGISVectorUtils vecUtils;
        
        std::string SHPFileInLayer = vecUtils.getLayerName(inputVecPolysFullPath);
        
        GDALDataset *inputImageDS = NULL;
        GDALDataset *inputSHPDS = NULL;
        OGRLayer *inputSHPLayer = NULL;
        
        try
        {
            /////////////////////////////////////
            //
            // Open Input Image.
            //
            /////////////////////////////////////
            inputImageDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputImageDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISException(message.c_str());
            }
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVecPolysFullPath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVecPolysFullPath;
                throw RSGISException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISException(message.c_str());
            }
            
            rsgis::img::pixelInPolyOption pixelInPolyMethod = rsgis::img::pixelInPolyInt2Enum(pixelInPolyMethodInt);
            
            rsgis::vec::RSGISExtractEndMembers2Matrix extractEndMembers;
            extractEndMembers.extractColumnPerPolygon2Matrix(inputImageDS, inputSHPLayer, outputMatrixFile, pixelInPolyMethod);
            
            GDALClose(inputImageDS);
            GDALClose(inputSHPDS);
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

    void executePixelBandStatsVecLyr(std::string inputImage, std::string vecfile, std::string veclyr, std::vector<RSGISZonalBandAttrsCmds> *zonBandAtts, int pixelInPolyMethodInt, bool ignoreProjection) throw(RSGISCmdException)
    {
        // Convert from int to enum
        rsgis::img::pixelInPolyOption pixelInPolyMethod = rsgis::img::pixelInPolyInt2Enum(pixelInPolyMethodInt);

        GDALAllRegister();
        OGRRegisterAll();

        GDALDataset *inImageDS = NULL;
        GDALDataset *vecDS = NULL;
        OGRLayer *vecLayer = NULL;

        try
        {
            inImageDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inImageDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISException(message.c_str());
            }
            // Set up attributes if using all bands
            unsigned int nImageBands = inImageDS->GetRasterCount();

            vecDS = (GDALDataset*) GDALOpenEx(vecfile.c_str(), GDAL_OF_VECTOR|GDAL_OF_UPDATE, NULL, NULL, NULL);
            if(vecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + vecfile;
                throw RSGISException(message.c_str());
            }
            vecLayer = vecDS->GetLayerByName(veclyr.c_str());
            if(vecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer '") + veclyr + "'";
                throw RSGISException(message.c_str());
            }
            // Check the projection is the same for shapefile and image
            if(!ignoreProjection)
            {
                OGRSpatialReference* vecProjObj = vecLayer->GetSpatialRef();
                OGRSpatialReference* imgProjObj = new OGRSpatialReference(inImageDS->GetProjectionRef());

                if(!vecProjObj->IsSame(imgProjObj))
                {
                    std::cerr << "WARNING: Vector layer and image are not the same projection! You might want to check these layers." <<  std::endl;
                }
                delete vecProjObj;
                delete imgProjObj;
            }
            std::vector<rsgis::vec::ZonalBandAttrs> *rsgisZonalBandAtts = new std::vector<rsgis::vec::ZonalBandAttrs>();
            for(std::vector<RSGISZonalBandAttrsCmds>::iterator iterAtts = zonBandAtts->begin(); iterAtts != zonBandAtts->end(); ++iterAtts)
            {
                rsgis::vec::ZonalBandAttrs tmpZonalAtt = rsgis::vec::ZonalBandAttrs();
                tmpZonalAtt.band = (*iterAtts).band;
                if((tmpZonalAtt.band < 1) | (tmpZonalAtt.band > nImageBands))
                {
                    throw RSGISException("At least one of the bands specified is not within the input image; note band numbering starts at 1.");
                }

                tmpZonalAtt.baseName = (*iterAtts).baseName;
                if((*iterAtts).minName == "")
                {
                    tmpZonalAtt.minName = tmpZonalAtt.baseName+"Min";
                }
                else
                {
                    tmpZonalAtt.minName = (*iterAtts).minName;
                }
                boost::algorithm::to_lower(tmpZonalAtt.minName);

                if((*iterAtts).maxName == "")
                {
                    tmpZonalAtt.maxName = tmpZonalAtt.baseName+"Max";
                }
                else
                {
                    tmpZonalAtt.maxName = (*iterAtts).maxName;
                }
                boost::algorithm::to_lower(tmpZonalAtt.maxName);

                if((*iterAtts).meanName == "")
                {
                    tmpZonalAtt.meanName = tmpZonalAtt.baseName+"Mean";
                }
                else
                {
                    tmpZonalAtt.meanName = (*iterAtts).meanName;
                }
                boost::algorithm::to_lower(tmpZonalAtt.meanName);

                if((*iterAtts).stdName == "")
                {
                    tmpZonalAtt.stdName = tmpZonalAtt.baseName+"StdDev";
                }
                else
                {
                    tmpZonalAtt.stdName = (*iterAtts).stdName;
                }
                boost::algorithm::to_lower(tmpZonalAtt.stdName);

                if((*iterAtts).countName == "")
                {
                    tmpZonalAtt.countName = tmpZonalAtt.baseName+"Count";
                }
                else
                {
                    tmpZonalAtt.countName = (*iterAtts).countName;
                }
                boost::algorithm::to_lower(tmpZonalAtt.countName);

                if((*iterAtts).modeName == "")
                {
                    tmpZonalAtt.modeName = tmpZonalAtt.baseName+"Mode";
                }
                else
                {
                    tmpZonalAtt.modeName = (*iterAtts).modeName;
                }
                if((*iterAtts).medianName == "")
                {
                    tmpZonalAtt.medianName = tmpZonalAtt.baseName+"Median";
                }
                else
                {
                    tmpZonalAtt.medianName = (*iterAtts).medianName;
                }
                boost::algorithm::to_lower(tmpZonalAtt.medianName);

                if((*iterAtts).sumName == "")
                {
                    tmpZonalAtt.sumName = tmpZonalAtt.baseName+"Sum";
                }
                else
                {
                    tmpZonalAtt.sumName = (*iterAtts).sumName;
                }
                boost::algorithm::to_lower(tmpZonalAtt.sumName);

                tmpZonalAtt.outMin = (*iterAtts).outMin;
                tmpZonalAtt.outMax = (*iterAtts).outMax;
                tmpZonalAtt.outMean = (*iterAtts).outMean;
                tmpZonalAtt.outStDev = (*iterAtts).outStDev;
                tmpZonalAtt.outCount = (*iterAtts).outCount;
                tmpZonalAtt.outMode = (*iterAtts).outMode;
                tmpZonalAtt.outMedian = (*iterAtts).outMedian;
                tmpZonalAtt.outSum = (*iterAtts).outSum;
                tmpZonalAtt.minThres = (*iterAtts).minThres;
                tmpZonalAtt.maxThres = (*iterAtts).maxThres;
                rsgisZonalBandAtts->push_back(tmpZonalAtt);
            }
            delete zonBandAtts;

            rsgis::vec::ZonalStats zonalStatsObj = rsgis::vec::ZonalStats();
            zonalStatsObj.zonalStatsFeatsVectorLyr(inImageDS, vecLayer, rsgisZonalBandAtts, pixelInPolyMethod);

            delete rsgisZonalBandAtts;
            delete vecDS;
            delete inImageDS;
        }
        catch(RSGISException& e)
        {
            if(vecDS != NULL)
            {
                delete vecDS;
            }
            if(inImageDS != NULL)
            {
                delete inImageDS;
            }
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            if(vecDS != NULL)
            {
                delete vecDS;
            }
            if(inImageDS != NULL)
            {
                delete inImageDS;
            }
            throw RSGISCmdException(e.what());
        }
    }
}}

