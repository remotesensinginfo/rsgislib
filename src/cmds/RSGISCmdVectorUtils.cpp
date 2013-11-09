/*
 *  RSGISCmdVectorUtils.cpp
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

#include "RSGISCmdRasterGIS.h"
#include "RSGISCmdParent.h"

#include "common/RSGISVectorException.h"
#include "common/RSGISException.h"

#include "vec/RSGISProcessVector.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISProcessGeometry.h"
#include "vec/RSGISGenerateConvexHullGroups.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISProcessFeatureCopyVector.h"
#include "vec/RSGISVectorAttributeFindReplace.h"
#include "vec/RSGISVectorBuffer.h"

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISFileUtils.h"

#include "geos/geom/Coordinate.h"

namespace rsgis{ namespace cmds {

    void executeGenerateConvexHullsGroups(std::string inputFile, std::string outputVector, std::string outVecProj, bool force, unsigned int eastingsColIdx, unsigned int northingsColIdx, unsigned int attributeColIdx)throw(RSGISCmdException)
    {
        try
        {
            rsgis::utils::RSGISTextUtils textUtils;
            std::string wktProj = textUtils.readFileToString(outVecProj);
            
            rsgis::vec::RSGISGenerateConvexHullGroups genConvexGrps;
            
            std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* > *coordGrps = NULL;
            
            coordGrps = genConvexGrps.getCoordinateGroups(inputFile, eastingsColIdx, northingsColIdx, attributeColIdx);
            
            /*
            for(std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* >::iterator iterGrps = coordGrps->begin(); iterGrps != coordGrps->end(); ++iterGrps)
            {
                std::cout << (*iterGrps)->first << std::endl;
                std::cout << "\t";
                for(std::vector<geos::geom::Coordinate>::iterator iterCoords = (*iterGrps)->second->begin(); iterCoords != (*iterGrps)->second->end(); ++iterCoords)
                {
                    std::cout << "[" << (*iterCoords).x << "," << (*iterCoords).y << "]";
                }
                std::cout << std::endl;
            }
            */
            
            std::cout << "Creating Polygons\n";
            genConvexGrps.createPolygonsAsShapefile(coordGrps, outputVector, wktProj, force);
            
            for(std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* >::iterator iterGrps = coordGrps->begin(); iterGrps != coordGrps->end(); ++iterGrps)
            {
                delete (*iterGrps);
                delete (*iterGrps)->second;
            }
            delete coordGrps;
            
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeRemoveAttributes(std::string inputVector, std::string outputVector, bool force)throw(RSGISCmdException)
    {
        try
        {
            
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            outputVector = boost::filesystem::absolute(outputVector).c_str();
            
			OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
            
            rsgis::vec::RSGISProcessVector *processVector = NULL;
            rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;
            
            std::string outputDIR = "";

            outputDIR = fileUtils.getFileDirectoryPath(outputVector);
            
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
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputSHPLayer->GetSpatialRef();
            
            // Get Geometry Type.
            OGRFeature *feature = inputSHPLayer->GetFeature(0);
            OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->CreateDataSource(outputVector.c_str(), NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            processFeature = new rsgis::vec::RSGISProcessFeatureCopyVector();
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectors(inputSHPLayer, outputSHPLayer, false, false, false);
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            OGRDataSource::DestroyDataSource(outputSHPDS);
            
            delete processVector;
            delete processFeature;
            
            //OGRCleanupAll();
            
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeBufferVector(std::string inputVector, std::string outputVector, float bufferDist, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            outputVector = boost::filesystem::absolute(outputVector).c_str();
            
			OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
			OGRFeatureDefn *inFeatureDefn = NULL;
            
            rsgis::vec::RSGISProcessGeometry *processVector = NULL;
            rsgis::vec::RSGISProcessOGRGeometry *processGeom = NULL;
            
            std::string outputDIR = "";

            outputDIR = fileUtils.getFileDirectoryPath(outputVector);
            
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
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputSHPLayer->GetSpatialRef();
            inFeatureDefn = inputSHPLayer->GetLayerDefn();
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->CreateDataSource(outputVector.c_str(), NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            processGeom = new rsgis::vec::RSGISVectorBuffer(bufferDist);
            processVector = new rsgis::vec::RSGISProcessGeometry(processGeom);
            
            processVector->processGeometryPolygonOutput(inputSHPLayer, outputSHPLayer, true, false);
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            OGRDataSource::DestroyDataSource(outputSHPDS);
            
            delete processVector;
            delete processGeom;
            
            //OGRCleanupAll();
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void  executePrintPolyGeom(std::string inputVector) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            
			OGRRegisterAll();
            
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
            
            rsgis::vec::RSGISVectorIO *vecIO = NULL;
            rsgis::vec::RSGISPolygonData **polygons = NULL;
			int numFeatures = 0;

            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            
            numFeatures = inputSHPLayer->GetFeatureCount(true);
            
            std::cout << "Shapefile has " << numFeatures << " features\n";
            vecIO = new rsgis::vec::RSGISVectorIO();
            polygons = new rsgis::vec::RSGISPolygonData*[numFeatures];
            for(int i = 0; i < numFeatures; i++)
            {
                polygons[i] = new rsgis::vec::RSGISEmptyPolygon();
            }
            vecIO->readPolygons(inputSHPLayer, polygons, numFeatures);
            
            std::cout.precision(8);
            
            for(int i = 0; i < numFeatures; i++)
            {
                std::cout << "Polygon " << i << ":\t" << polygons[i]->getGeometry()->toText() << std::endl;
            }
            
            if(vecIO != NULL)
            {
                delete vecIO;
            }
            if(polygons != NULL)
            {
                for(int i = 0; i < numFeatures; i++)
                {
                    delete polygons[i];
                }
                delete polygons;
            }
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            
            //OGRCleanupAll();

        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

            void  executeFindReplaceText(std::string inputVector, std::string attribute, std::string find, std::string replace) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            
            
			OGRRegisterAll();
            
            rsgis::vec::RSGISVectorUtils vecUtils;
            rsgis::vec::RSGISProcessVector *processVector = NULL;
            rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), TRUE, NULL);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            
            processFeature = new rsgis::vec::RSGISVectorAttributeFindReplace(attribute, find, replace);
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectors(inputSHPLayer, false);
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            delete processVector;
            delete processFeature;
            
            //OGRCleanupAll();
            
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

}}

