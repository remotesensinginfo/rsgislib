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

#include "RSGISCmdRasterGIS.h"
#include "RSGISCmdParent.h"

#include "common/RSGISVectorException.h"
#include "common/RSGISException.h"

#include "vec/RSGISVectorZonalStats.h"
#include "vec/RSGISProcessVector.h"


namespace rsgis{ namespace cmds {

    void executePointValue2SHP(std::string inputImage, std::string inputVecPolys, std::string outputVecPolys, bool force, bool useBandNames)
    {        
        // Convert to absolute path
        inputVecPolys = boost::filesystem::absolute(inputVecPolys).c_str();
        
        GDALAllRegister();
        OGRRegisterAll();
        
        rsgis::utils::RSGISFileUtils fileUtils;
        rsgis::vec::RSGISVectorUtils vecUtils;
        
        rsgis::vec::RSGISProcessVector *processVector = NULL;
        rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;
        
        std::string SHPFileInLayer = vecUtils.getLayerName(inputVecPolys);
        std::string SHPFileOutLayer = "";
        
        GDALDataset *inputImageDS = NULL;
        OGRDataSource *inputSHPDS = NULL;
        OGRLayer *inputSHPLayer = NULL;
        OGRSFDriver *shpFiledriver = NULL;
        OGRDataSource *outputSHPDS = NULL;
        OGRLayer *outputSHPLayer = NULL;
        OGRSpatialReference* inputSpatialRef = NULL;
        
        std::string outputDIR = "";
        
        try
        {
            SHPFileOutLayer = vecUtils.getLayerName(outputVecPolys);
            outputDIR = fileUtils.getFileDirectoryPath(outputVecPolys);
            
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
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVecPolys.c_str(), FALSE);
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
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw RSGISException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->CreateDataSource(outputVecPolys.c_str(), NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVecPolys;
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
            OGRDataSource::DestroyDataSource(outputSHPDS);
            
            delete processVector;
            delete processFeature;

            GDALClose(inputImageDS);
            OGRDataSource::DestroyDataSource(inputSHPDS);
            
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
    
    void executePointValue2TXT(std::string inputImage, std::string inputVecPolys, std::string outputTextFile, bool useBandNames)
    {

        // Convert to absolute path
        inputVecPolys = boost::filesystem::absolute(inputVecPolys).c_str();

        GDALAllRegister();
        OGRRegisterAll();

        rsgis::utils::RSGISFileUtils fileUtils;
        rsgis::vec::RSGISVectorUtils vecUtils;
        
        rsgis::vec::RSGISProcessVector *processVector = NULL;
        rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;
        
        std::string SHPFileInLayer = vecUtils.getLayerName(inputVecPolys);

        GDALDataset *inputImageDS = NULL;
        OGRDataSource *inputSHPDS = NULL;
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
                throw RSGISException(message.c_str());
            }
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVecPolys.c_str(), FALSE);
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
            OGRFeature::DestroyFeature(feature);
             
            processFeature = new rsgis::vec::RSGISVectorZonalStats(inputImageDS, outputTextFile, useBandNames);
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);
             
            processVector->processVectorsNoOutput(inputSHPLayer, true);
             
            delete processVector;
            delete processFeature;
            
            GDALClose(inputImageDS);
            OGRDataSource::DestroyDataSource(inputSHPDS);
    
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
    
}}

