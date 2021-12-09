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

#include "common/RSGISException.h"

#include "img/RSGISExtractImageValues.h"

#include "vec/RSGISZonalImage2HDF.h"
#include "vec/RSGISExtractEndMembers2Matrix.h"


namespace rsgis{ namespace cmds {

    void executeZonesImage2HDF5(std::string inputImage, std::string inputVecFile, std::string inputVecLyr, std::string outputHDF, bool ignoreProjection, int pixelInPolyMethodInt)
    {
        std::cout.precision(12);
        // Convert to absolute path
        inputVecFile = std::string(boost::filesystem::absolute(inputVecFile).string());

        GDALAllRegister();
        OGRRegisterAll();

        GDALDataset *inputImageDS = NULL;
        GDALDataset *inputVecDS = NULL;
        OGRLayer *inputVecLyrObj = NULL;

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
            inputVecDS = (GDALDataset*) GDALOpenEx(inputVecFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVecFile;
                throw RSGISException(message.c_str());
            }
            inputVecLyrObj = inputVecDS->GetLayerByName(inputVecLyr.c_str());
            if(inputVecLyrObj == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + inputVecLyr;
                throw RSGISException(message.c_str());
            }

            rsgis::vec::RSGISZonalImage2HDF zonalImg2HDF;
            rsgis::img::pixelInPolyOption pixelInPolyMethod = rsgis::img::pixelInPolyInt2Enum(pixelInPolyMethodInt);
            zonalImg2HDF.extractBandsToColumns(inputImageDS, inputVecLyrObj, outputHDF, pixelInPolyMethod);

            GDALClose(inputImageDS);
            GDALClose(inputVecDS);
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
        
    void executeExtractAvgEndMembers(std::string inputImage, std::string inputVecFile, std::string inputVecLyr,
                                     std::string outputMatrixFile, int pixelInPolyMethodInt)
    {
        std::cout.precision(12);
        // Convert to absolute path
        inputVecFile = std::string(boost::filesystem::absolute(inputVecFile).string());
        
        GDALAllRegister();
        OGRRegisterAll();

        GDALDataset *inputImageDS = NULL;
        GDALDataset *inputVecDS = NULL;
        OGRLayer *inputVecLyrObj = NULL;
        
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
            inputVecDS = (GDALDataset*) GDALOpenEx(inputVecFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVecFile;
                throw RSGISException(message.c_str());
            }
            inputVecLyrObj = inputVecDS->GetLayerByName(inputVecLyr.c_str());
            if(inputVecLyrObj == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + inputVecLyr;
                throw RSGISException(message.c_str());
            }
            
            rsgis::img::pixelInPolyOption pixelInPolyMethod = rsgis::img::pixelInPolyInt2Enum(pixelInPolyMethodInt);
            
            rsgis::vec::RSGISExtractEndMembers2Matrix extractEndMembers;
            extractEndMembers.extractColumnPerPolygon2Matrix(inputImageDS, inputVecLyrObj, outputMatrixFile, pixelInPolyMethod);
            
            GDALClose(inputImageDS);
            GDALClose(inputVecDS);
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

    void executeImageRasterZone2HDF(std::string imageFile, std::string maskImage, std::string outputHDF, float maskVal, RSGISLibDataType dataType)
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
            extractVals.extractDataWithinMask2HDF(maskDS, imageDS, outputHDF, maskVal, dataType);

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


    void executeImageBandRasterZone2HDF(std::vector<std::pair<std::string, std::vector<unsigned int> > > imageFiles, std::string maskImage, std::string outputHDF, float maskVal, RSGISLibDataType dataType)
    {
        try
        {
            rsgis::img::RSGISExtractImageValues extractVals;
            extractVals.extractImgBandDataWithinMask2HDF(imageFiles, maskImage, outputHDF, maskVal, dataType);
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

    void executeRandomSampleH5File(std::string inputH5, std::string outputH5, unsigned int nSample, int seed, RSGISLibDataType dataType)
    {
        try
        {
            rsgis::img::RSGISExtractImageValues extractVals;
            extractVals.sampleExtractedHDFData(inputH5, outputH5, nSample, seed, dataType);
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

    void executeSplitSampleH5File(std::string inputH5, std::string outputP1H5, std::string outputP2H5, unsigned int nSample, int seed, RSGISLibDataType dataType)
    {
        try
        {
            rsgis::img::RSGISExtractImageValues extractVals;
            extractVals.splitExtractedHDFData(inputH5, outputP1H5, outputP2H5, nSample, seed, dataType);
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

