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

#include "RSGISCmdVectorUtils.h"
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
#include "vec/RSGISCalcPolygonArea.h"
#include "vec/RSGISCopyPolygonsInPolygon.h"
#include "vec/RSGISPopulateFeatsElev.h"
#include "vec/RSGISGetOGRGeometries.h"
#include "vec/RSGISOGRPolygonReader.h"
#include "vec/RSGISVectorMaths.h"
#include "vec/RSGISCopyFeatures.h"
#include "vec/RSGISVectorProcessing.h"
#include "vec/RSGISCalcDistanceStats.h"
#include "vec/RSGISPolygonReader.h"
#include "vec/RSGISGetAttributeValues.h"
#include "vec/RSGISFitActiveContour4Polys.h"
#include "vec/RSGISCopyCheckPolygons.h"

#include "geom/RSGISFitPolygon2Points.h"
#include "geom/RSGISMinSpanTreeClustererStdDevThreshold.h"


#include "img/RSGISImageUtils.h"
#include "img/RSGISExtractImageValues.h"

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISFileUtils.h"
#include "utils/RSGISGEOSFactoryGenerator.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/Polygon.h"
#include "geos/index/strtree/STRtree.h"
#include "geos/index/quadtree/Quadtree.h"

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
            inputVector = boost::filesystem::absolute(inputVector).string();
            outputVector = boost::filesystem::absolute(outputVector).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            GDALDataset *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
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
            // Open Input ShapfLinesVec.
            //
            /////////////////////////////////////
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);  
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
            shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
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
            
            GDALClose(inputSHPDS);
            GDALClose(outputSHPDS);
            
            delete processVector;
            delete processFeature;
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
            inputVector = boost::filesystem::absolute(inputVector).string();
            outputVector = boost::filesystem::absolute(outputVector).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            GDALDataset *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
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
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
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
            shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
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
            
            GDALClose(inputSHPDS);
            GDALClose(outputSHPDS);
            
            delete processVector;
            delete processGeom;
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
            inputVector = boost::filesystem::absolute(inputVector).string();
            
            OGRRegisterAll();
            
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            
            GDALDataset *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            
            //rsgis::vec::RSGISVectorIO *vecIO = NULL;
            //rsgis::vec::RSGISPolygonData **polygons = NULL;
            //int numFeatures = 0;
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
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
            
            rsgis::vec::RSGISProcessOGRFeature *processFeature = new rsgis::vec::RSGISPrintGeometryToConsole();
            rsgis::vec::RSGISProcessVector *processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectorsNoOutput(inputSHPLayer, true);
            
            delete processVector;
            delete processFeature;
            
            GDALClose(inputSHPDS);            
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
            inputVector = boost::filesystem::absolute(inputVector).string();
            
            
            OGRRegisterAll();
            
            rsgis::vec::RSGISVectorUtils vecUtils;
            rsgis::vec::RSGISProcessVector *processVector = NULL;
            rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            
            GDALDataset *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_UPDATE, NULL, NULL, NULL);  
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
            
            GDALClose(inputSHPDS);
            delete processVector;
            delete processFeature;
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
    
    void executeCalcPolyArea(std::string inputVector, std::string outputVector, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).string();
            outputVector = boost::filesystem::absolute(outputVector).string();
            
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            GDALDataset *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRFeatureDefn *inFeatureDefn = NULL;
            
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
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
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
            shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            processFeature = new rsgis::vec::RSGISCalcPolygonArea();
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectors(inputSHPLayer, outputSHPLayer, true, false, false);
            
            GDALClose(inputSHPDS);
            GDALClose(outputSHPDS);
            
            delete processVector;
            delete processFeature;
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
    
    void executePolygonsInPolygon(std::string inputVector, std::string inputCoverVector, std::string output_DIR, std::string attributeName, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).string();
            inputCoverVector = boost::filesystem::absolute(inputCoverVector).string();
            output_DIR = boost::filesystem::absolute(output_DIR).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string coverSHPFileInLayer = vecUtils.getLayerName(inputCoverVector);
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            
            GDALDataset *inputCoverSHPDS = NULL;
            OGRLayer *inputCoverSHPLayer = NULL;
            OGRSpatialReference* inputCoverSpatialRef = NULL;
            
            GDALDataset *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRGeometry *coverGeometry = NULL; // OGRGeometry representation of feature in cover layer
            
            // Output shapefile
            GDALDriver *shpFiledriver = NULL;
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            std::string coverFeatureName = "";
            std::string outVector = ""; // Filename of out vector

            /************************
             * OPEN COVER SHAPEFILE *
             ************************/
            
            inputCoverSHPDS = (GDALDataset*) GDALOpenEx(inputCoverVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputCoverSHPDS == NULL)
            {
                std::string message = std::string("Could not open cover vector file ") + inputCoverVector;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputCoverSHPLayer = inputCoverSHPDS->GetLayerByName(coverSHPFileInLayer.c_str());
            if(inputCoverSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + coverSHPFileInLayer;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputCoverSpatialRef = inputCoverSHPLayer->GetSpatialRef();
            
            /*********************************
             * OPEN SHAPEFILE                *
             * This shapefile contains the   *
             * polygons to check if          *
             * covered by 'inputCoverVector' *
             *********************************/
            
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
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
            
            if(inputCoverSpatialRef != inputSpatialRef)
            {
                std::cerr << "Shapefiles are of different projection!" << std::endl;
            }
            
            // Get Geometry Type.
            OGRFeature *feature = inputSHPLayer->GetFeature(0);
            OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
            
            /********************************************
             * Loop through features in input shapefile *
             ********************************************/
            OGRFeature *inCoverFeature = NULL;
            while( (inCoverFeature = inputCoverSHPLayer->GetNextFeature()) != NULL )
            {
                
                // Get name of feature
                int fieldIdx = inCoverFeature->GetFieldIndex(attributeName.c_str());
                coverFeatureName = inCoverFeature->GetFieldAsString(fieldIdx);
                
                // Represent as OGRGeometry
                coverGeometry = inCoverFeature->GetGeometryRef();
                
                outVector = output_DIR + "/" + coverFeatureName + ".shp";
                std::string SHPFileOutLayer = vecUtils.getLayerName(outVector);
                
                GDALDataset *outputSHPDS = NULL;
                OGRLayer *outputSHPLayer = NULL;
                
                std::string outputDIR = "";
                outputDIR = fileUtils.getFileDirectoryPath(outVector);
                
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
                
                /**************************************
                 * CREATE OUTPUT SHAPEFILE            *
                 * This is the shapefile representing *
                 * polygons contained within the	  *
                 * cover feature                      *
                 **************************************/
                
                outputSHPDS = shpFiledriver->Create(outVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
                
                std::cout << "Creating new shapefile" << std::endl;
                if( outputSHPDS == NULL )
                {
                    std::string message = std::string("Could not create vector file ") + outVector;
                    throw rsgis::vec::RSGISVectorOutputException(message.c_str());
                }
                outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
                if( outputSHPLayer == NULL )
                {
                    std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                    throw rsgis::vec::RSGISVectorOutputException(message.c_str());
                }
                
                std::cout << "Created shapefile " << outVector << std::endl;
                
                /************************************
                 * ADD POLYGONS IN POLYGON TO NEW   *
                 * SHAPEFILE						*
                 ************************************/
                rsgis::vec::RSGISCopyPolygonsInPolygon copyPolysinPoly;
                long unsigned numPolygonsInCover = 0;
                try
                {
                    numPolygonsInCover = copyPolysinPoly.copyPolygonsInPoly(inputSHPLayer, outputSHPLayer, coverGeometry);
                    GDALClose(outputSHPDS);
                }
                catch (RSGISVectorException &e)
                {
                    GDALClose(outputSHPDS);
                    throw e;
                }
                // If no polygons in cover polygons, delete shapefile (will be empty)
                if(numPolygonsInCover == 0)
                {
                    std::cout << SHPFileOutLayer << " covers no polygons and will be deleted" << std::endl;
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    std::cout << coverFeatureName << " covers " << numPolygonsInCover << " polygons" << std::endl;
                }
                
                
            }
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
            
            
    void executePopulateGeomZField(std::string inputVector, std::string inputImage, unsigned int imgBand, std::string outputVector, bool force) throw(RSGISCmdException)
    {
        // Convert to absolute path
        inputVector = boost::filesystem::absolute(inputVector).string();
        
        GDALAllRegister();
        OGRRegisterAll();
        
        rsgis::utils::RSGISFileUtils fileUtils;
        rsgis::vec::RSGISVectorUtils vecUtils;
        
        std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
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
            SHPFileOutLayer = vecUtils.getLayerName(outputVector);
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
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
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
            
            if(geometryType == wkbPoint)
            {
                geometryType = wkbPoint25D;
            }
            else if(geometryType == wkbLineString)
            {
                geometryType = wkbLineString25D;
            }
            else if(geometryType == wkbPolygon)
            {
                geometryType = wkbPolygon25D;
            }
            else if(geometryType == wkbMultiPoint)
            {
                geometryType = wkbMultiPoint25D;
            }
            else if(geometryType == wkbMultiLineString)
            {
                geometryType = wkbMultiLineString25D;
            }
            else if(geometryType == wkbMultiPolygon)
            {
                geometryType = wkbMultiPolygon25D;
            }
            else if(geometryType == wkbGeometryCollection)
            {
                geometryType = wkbGeometryCollection25D;
            }
            else
            {
                throw RSGISException("Geometry Type not recognised.");
            }
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////

            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw RSGISException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw RSGISException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw RSGISException(message.c_str());
            }
            
            rsgis::vec::RSGISProcessOGRFeature *processFeature = new rsgis::vec::RSGISPopulateFeatsElev(inputImageDS, imgBand);
            rsgis::vec::RSGISProcessVector *processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectors(inputSHPLayer, outputSHPLayer, true, false, false);
            
            delete processVector;
            delete processFeature;
            
            GDALClose(inputImageDS);
            GDALClose(inputSHPDS);
            GDALClose(outputSHPDS);
            
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
            
            
    void executeVectorMaths(std::string inputVector, std::string outputVector, std::string outColumn, std::string expression, bool force, std::vector<RSGISVariableFieldCmds> vars) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).string();
            outputVector = boost::filesystem::absolute(outputVector).string();
            
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            GDALDataset *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRFeatureDefn *inFeatureDefn = NULL;
            
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
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
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
            shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            unsigned int numVars = vars.size();
            rsgis::vec::VariableFields **variables = new rsgis::vec::VariableFields*[numVars];
            for(unsigned i = 0; i < numVars; ++i)
            {
                variables[i] = new rsgis::vec::VariableFields();
                variables[i]->name = vars.at(i).name;
                variables[i]->fieldName = vars.at(i).fieldName;
            }
            
            processFeature = new rsgis::vec::RSGISVectorMaths(variables, numVars, expression, outColumn);
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);
            
            for(unsigned i = 0; i < numVars; ++i)
            {
                delete variables[i];
            }
            delete[] variables;
            
            GDALClose(inputSHPDS);
            GDALClose(outputSHPDS);
            
            delete processVector;
            delete processFeature;            
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
            
            
    void executeAddFIDColumn(std::string inputVector, std::string outputVector, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).string();
            outputVector = boost::filesystem::absolute(outputVector).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            GDALDataset *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRFeatureDefn *inFeatureDefn = NULL;
            
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
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
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
            shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName);
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            rsgis::vec::RSGISCopyFeaturesAddFIDCol *copyFeatures = new rsgis::vec::RSGISCopyFeaturesAddFIDCol(1);
            rsgis::vec::RSGISProcessVector *processVector = new rsgis::vec::RSGISProcessVector(copyFeatures);
            processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);
            
            delete copyFeatures;
            delete processVector;
            
            GDALClose(inputSHPDS);
            GDALClose(outputSHPDS);
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
            
            
    void executeFindCommonImgExtent(std::vector<std::string> inputImages, std::string outputVector, bool force) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            OGRRegisterAll();
            
            GDALDataset **imgDatasets = new GDALDataset*[inputImages.size()];
            
            for(size_t i = 0; i < inputImages.size(); ++i)
            {
                imgDatasets[i] = (GDALDataset *) GDALOpenShared(inputImages.at(i).c_str(), GA_ReadOnly);
                if(imgDatasets[i] == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages.at(i);
                    throw RSGISImageException(message.c_str());
                }
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            geos::geom::Envelope *env = new geos::geom::Envelope();
            
            imgUtils.getImageOverlap(imgDatasets, inputImages.size(), env);
            
            std::cout << "[xMin,xMax][yMin,yMax][" << env->getMinX() << ", " << env->getMaxX() << "][" << env->getMinY() << ", " << env->getMaxY() << "]\n";
            
            outputVector = boost::filesystem::absolute(outputVector).string();

            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* ogrSpatialRef = NULL;
            
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
            ogrSpatialRef = new OGRSpatialReference(imgDatasets[0]->GetProjectionRef());
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName);
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), ogrSpatialRef, wkbPolygon, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            OGRFeatureDefn *featDefn = outputSHPLayer->GetLayerDefn();
            OGRFeature *poFeature = new OGRFeature(featDefn);
            OGRPolygon *poly = vecUtils.createOGRPolygon(env);
            poFeature->SetGeometryDirectly(poly);
            outputSHPLayer->CreateFeature(poFeature);
            GDALClose(outputSHPDS);
            
            delete env;
            
            for(size_t i = 0; i < inputImages.size(); ++i)
            {
                GDALClose(imgDatasets[i]);
            }
            delete[] imgDatasets;
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
            
    void executeSplitFeatures(std::string inputVector, std::string outputVectorBase, bool force) throw(RSGISCmdException)
    {
        // Convert to absolute path
        inputVector = boost::filesystem::absolute(inputVector).string();
        outputVectorBase = boost::filesystem::absolute(outputVectorBase).string();
        
        OGRRegisterAll();
        
        rsgis::utils::RSGISFileUtils fileUtils;
        rsgis::vec::RSGISVectorUtils vecUtils;
        rsgis::vec::RSGISVectorProcessing vecProcessing;
        
        std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
        

        GDALDataset *inputSHPDS = NULL;
        OGRLayer *inputSHPLayer = NULL;
        
        try
        {
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
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
            
            vecProcessing.splitFeatures(inputSHPLayer, outputVectorBase, force);
            
            GDALClose(inputSHPDS);
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
           
            
    void executeExportPxls2Pts(std::string inputImage, std::string outputVec, bool force, float maskVal) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            OGRRegisterAll();
            
            GDALDataset *imgDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(imgDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISCmdException(message.c_str());
            }
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            outputVec = boost::filesystem::absolute(outputVec).string();
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVec);
            
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* ogrSpatialRef = NULL;
            
            std::string outputDIR = "";
            
            outputDIR = fileUtils.getFileDirectoryPath(outputVec);
            
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
            outputSHPDS = shpFiledriver->Create(outputVec.c_str(), 0, 0, 0, GDT_Unknown, NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVec;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), ogrSpatialRef, wkbPoint, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            rsgis::img::RSGISExtractPxlsAsPts exportPts;
            exportPts.exportPixelsAsPoints(imgDataset, outputSHPLayer, maskVal);
            
            GDALClose(outputSHPDS);
            GDALClose(imgDataset);
        }
        catch(RSGISCmdException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    double executeCalcDist2NearestGeom(std::string inputVec, std::string outputVec, std::string outColName, bool force, bool useIdx, double idxMaxSearch) throw(RSGISCmdException)
    {
        double maxMinDist = 0.0;
        try
        {
            // Convert to absolute path
            inputVec = boost::filesystem::absolute(inputVec).string();
            outputVec = boost::filesystem::absolute(outputVec).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVec);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVec);

            std::string outputDIR = fileUtils.getFileDirectoryPath(outputVec);
            
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
            GDALDataset *inputSHPDS = NULL;
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);  
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            OGRSpatialReference* inputSpatialRef = inputSHPLayer->GetSpatialRef();
            OGRFeatureDefn *inFeatureDefn = inputSHPLayer->GetLayerDefn();
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            GDALDriver *shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            GDALDataset *outputSHPDS = shpFiledriver->Create(outputVec.c_str(), 0, 0, 0, GDT_Unknown, NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVec;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            OGRLayer *outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            if(useIdx)
            {
                // Read in Geometries...
                std::cout << "Read in the geometries\n";
                geos::index::quadtree::Quadtree *geomsIdx = new geos::index::quadtree::Quadtree();
                rsgis::vec::RSGISGetOGRGeometriesInIdx getOGRGeoms = rsgis::vec::RSGISGetOGRGeometriesInIdx(geomsIdx);
                rsgis::vec::RSGISProcessVector processVectorGetGeoms = rsgis::vec::RSGISProcessVector(&getOGRGeoms);
                processVectorGetGeoms.processVectorsNoOutput(inputSHPLayer, false);
                
                // Iterate through features and calc dist...
                std::cout << "Calculate Distances\n";
                rsgis::vec::RSGISCalcMinDist2GeomsUseIdx calcMinDist = rsgis::vec::RSGISCalcMinDist2GeomsUseIdx(outColName, geomsIdx, idxMaxSearch);
                rsgis::vec::RSGISProcessVector processVector = rsgis::vec::RSGISProcessVector(&calcMinDist);
                processVector.processVectors(inputSHPLayer, outputSHPLayer, true, false, false);
                
                maxMinDist = calcMinDist.getMaxMinDist();
            }
            else
            {
                // Read in Geometries...
                std::cout << "Read in the geometries\n";
                std::vector<OGRGeometry*> *ogrGeoms = new std::vector<OGRGeometry*>();
                rsgis::vec::RSGISGetOGRGeometries getOGRGeoms = rsgis::vec::RSGISGetOGRGeometries(ogrGeoms);
                rsgis::vec::RSGISProcessVector processVectorGetGeoms = rsgis::vec::RSGISProcessVector(&getOGRGeoms);
                processVectorGetGeoms.processVectorsNoOutput(inputSHPLayer, false);
                
                // Iterate through features and calc dist...
                std::cout << "Calculate Distances\n";
                rsgis::vec::RSGISCalcMinDist2Geoms calcMinDist = rsgis::vec::RSGISCalcMinDist2Geoms(outColName, ogrGeoms);
                rsgis::vec::RSGISProcessVector processVector = rsgis::vec::RSGISProcessVector(&calcMinDist);
                processVector.processVectors(inputSHPLayer, outputSHPLayer, true, false, false);
                
                maxMinDist = calcMinDist.getMaxMinDist();
            }
            
            GDALClose(inputSHPDS);
            GDALClose(outputSHPDS);
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
        
        return maxMinDist;
    }
            
            
    double executeCalcMaxDist2NearestGeom(std::string inputVec) throw(RSGISCmdException)
    {
        double maxMinDist = 0.0;
        try
        {
            // Convert to absolute path
            inputVec = boost::filesystem::absolute(inputVec).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVec);
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            GDALDataset *inputSHPDS = NULL;
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);  
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            
            // Read in Geometries...
            std::cout << "Read in the geometries\n";
            std::vector<OGRGeometry*> *ogrGeoms = new std::vector<OGRGeometry*>();
            rsgis::vec::RSGISGetOGRGeometries getOGRGeoms = rsgis::vec::RSGISGetOGRGeometries(ogrGeoms);
            rsgis::vec::RSGISProcessVector processVectorGetGeoms = rsgis::vec::RSGISProcessVector(&getOGRGeoms);
            processVectorGetGeoms.processVectorsNoOutput(inputSHPLayer, false);
            
            // Iterate through features and calc dist...
            std::cout << "Calculate Distances\n";
            rsgis::vec::RSGISCalcMinDist2Geoms calcMinDist = rsgis::vec::RSGISCalcMinDist2Geoms("MinDist", ogrGeoms);
            rsgis::vec::RSGISProcessVector processVector = rsgis::vec::RSGISProcessVector(&calcMinDist);
            processVector.processVectorsNoOutput(inputSHPLayer, false);
            maxMinDist = calcMinDist.getMaxMinDist();
            
            GDALClose(inputSHPDS);
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
        
        return maxMinDist;
    }
            
    double executeCalcDist2NearestGeom(std::string inputVec, std::string inDist2Vec, std::string outputVec, std::string outColName, bool force, bool useIdx, double idxMaxSearch) throw(RSGISCmdException)
    {
        double maxMinDist = 0.0;
        try
        {
            // Convert to absolute path
            inputVec = boost::filesystem::absolute(inputVec).string();
            inDist2Vec = boost::filesystem::absolute(inDist2Vec).string();
            outputVec = boost::filesystem::absolute(outputVec).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVec);
            std::string SHPFileInDistLayer = vecUtils.getLayerName(inDist2Vec);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVec);
            
            std::string outputDIR = fileUtils.getFileDirectoryPath(outputVec);
            
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
            GDALDataset *inputSHPDS = NULL;
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            OGRSpatialReference* inputSpatialRef = inputSHPLayer->GetSpatialRef();
            OGRFeatureDefn *inFeatureDefn = inputSHPLayer->GetLayerDefn();
            
            GDALDataset *inDistToSHPDS = NULL;
            inDistToSHPDS = (GDALDataset*) GDALOpenEx(inDist2Vec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inDistToSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inDist2Vec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inDistToSHPLayer = inDistToSHPDS->GetLayerByName(SHPFileInDistLayer.c_str());
            if(inDistToSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInDistLayer;
                throw RSGISFileException(message.c_str());
            }
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            GDALDriver *shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            GDALDataset *outputSHPDS = shpFiledriver->Create(outputVec.c_str(), 0, 0, 0, GDT_Unknown, NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVec;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            OGRLayer *outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            if(useIdx)
            {
                // Read in Geometries...
                std::cout << "Read in the geometries\n";
                geos::index::quadtree::Quadtree *geomsIdx = new geos::index::quadtree::Quadtree();
                rsgis::vec::RSGISGetOGRGeometriesInIdx getOGRGeoms = rsgis::vec::RSGISGetOGRGeometriesInIdx(geomsIdx);
                rsgis::vec::RSGISProcessVector processVectorGetGeoms = rsgis::vec::RSGISProcessVector(&getOGRGeoms);
                processVectorGetGeoms.processVectorsNoOutput(inDistToSHPLayer, false);
                
                // Iterate through features and calc dist...
                std::cout << "Calculate Distances\n";
                rsgis::vec::RSGISCalcMinDist2GeomsUseIdx calcMinDist = rsgis::vec::RSGISCalcMinDist2GeomsUseIdx(outColName, geomsIdx, idxMaxSearch);
                rsgis::vec::RSGISProcessVector processVector = rsgis::vec::RSGISProcessVector(&calcMinDist);
                processVector.processVectors(inputSHPLayer, outputSHPLayer, true, false, false);
                
                maxMinDist = calcMinDist.getMaxMinDist();
            }
            else
            {
                // Read in Geometries...
                std::cout << "Read in the geometries\n";
                std::vector<OGRGeometry*> *ogrGeoms = new std::vector<OGRGeometry*>();
                rsgis::vec::RSGISGetOGRGeometries getOGRGeoms = rsgis::vec::RSGISGetOGRGeometries(ogrGeoms);
                rsgis::vec::RSGISProcessVector processVectorGetGeoms = rsgis::vec::RSGISProcessVector(&getOGRGeoms);
                processVectorGetGeoms.processVectorsNoOutput(inDistToSHPLayer, false);
                
                // Iterate through features and calc dist...
                std::cout << "Calculate Distances\n";
                rsgis::vec::RSGISCalcMinDist2Geoms calcMinDist = rsgis::vec::RSGISCalcMinDist2Geoms(outColName, ogrGeoms);
                rsgis::vec::RSGISProcessVector processVector = rsgis::vec::RSGISProcessVector(&calcMinDist);
                processVector.processVectors(inputSHPLayer, outputSHPLayer, true, false, false);
                
                maxMinDist = calcMinDist.getMaxMinDist();
            }
            
            GDALClose(inputSHPDS);
            GDALClose(outputSHPDS);
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
        
        return maxMinDist;
    }
            
    void executeSpatialGraphClusterGeoms(std::string inputVec, std::string outputVec, bool useMinSpanTree, float edgeLenSDThres, double maxEdgeLen, bool force, std::string shpFileEdges, bool outShpEdges, std::string h5EdgeLengths, bool outH5EdgeLens) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVec = boost::filesystem::absolute(inputVec).string();
            outputVec = boost::filesystem::absolute(outputVec).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVec);
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            GDALDataset *inputSHPDS = NULL;
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);  
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            OGRSpatialReference *spatialRef = inputSHPLayer->GetSpatialRef();
            OGRFeatureDefn *inFeatureDefn = inputSHPLayer->GetLayerDefn();
            OGRwkbGeometryType wktGeomType = inFeatureDefn->GetGeomType();
            
            
            std::vector<rsgis::geom::RSGIS2DPoint*> *polyData = new std::vector<rsgis::geom::RSGIS2DPoint*>();
            rsgis::vec::RSGISProcessVector *processVector = NULL;
            if(wktGeomType == wkbPolygon)
            {
                rsgis::vec::RSGISPolygonReader processFeature = rsgis::vec::RSGISPolygonReader(polyData);
                processVector = new rsgis::vec::RSGISProcessVector(&processFeature);
            }
            else if(wktGeomType == wkbPoint)
            {
                rsgis::vec::RSGISPointReader processFeature = rsgis::vec::RSGISPointReader(polyData);
                processVector = new rsgis::vec::RSGISProcessVector(&processFeature);
            }
            else
            {
                throw RSGISFileException("Can only process point or polygon shapefiles...");
            }
            std::cout << "Read input Shapefile\n";
            processVector->processVectorsNoOutput(inputSHPLayer, false);
            delete processVector;
            
            rsgis::geom::RSGISGraphGeomClusterer clusterer = rsgis::geom::RSGISGraphGeomClusterer(useMinSpanTree, edgeLenSDThres, maxEdgeLen, shpFileEdges, outShpEdges, h5EdgeLengths, outH5EdgeLens);
            int numClusters = 0;
            double sdThresVal = 0.0;
            std::list<rsgis::geom::RSGIS2DPoint*> **outClusters = clusterer.clusterData(polyData, &numClusters, &sdThresVal);
            
            rsgis::vec::RSGISVectorIO vecIO;
            if(wktGeomType == wkbPolygon)
            {
                std::list<geos::geom::Polygon*> **polygons = new std::list<geos::geom::Polygon*>*[numClusters];
                std::list<rsgis::geom::RSGIS2DPoint*>::iterator iter2DPts;
                rsgis::geom::RSGISPolygon *tmpRSGISPoly;
                for(int i = 0; i < numClusters; ++i)
                {
                    polygons[i] = new std::list<geos::geom::Polygon*>();
                    for(iter2DPts = outClusters[i]->begin(); iter2DPts != outClusters[i]->end(); ++iter2DPts)
                    {
                        tmpRSGISPoly = (rsgis::geom::RSGISPolygon*) *iter2DPts;
                        polygons[i]->push_back(tmpRSGISPoly->getPolygon());
                    }
                }
                vecIO.exportGEOSPolygonClusters2SHP(outputVec, force, polygons, numClusters, spatialRef);
            }
            else if(wktGeomType == wkbPoint)
            {
                std::list<geos::geom::Point*> **points = new std::list<geos::geom::Point*>*[numClusters];
                std::list<rsgis::geom::RSGIS2DPoint*>::iterator iter2DPts;
                for(int i = 0; i < numClusters; ++i)
                {
                    points[i] = new std::list<geos::geom::Point*>();
                    for(iter2DPts = outClusters[i]->begin(); iter2DPts != outClusters[i]->end(); ++iter2DPts)
                    {
                        points[i]->push_back((*iter2DPts)->getAsGeosPoint());
                    }
                }
                
                vecIO.exportGEOSPointClusters2SHP(outputVec, force, points, numClusters, spatialRef);
            }
            else
            {
                throw RSGISFileException("Can only export point or polygon shapefiles...");
            }
            
            GDALClose(inputSHPDS);
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
    
    
    void executeFitPolygonToPoints(std::string inputVec, std::string outputVec, double alphaVal, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVec = boost::filesystem::absolute(inputVec).string();
            outputVec = boost::filesystem::absolute(outputVec).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVec);
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            GDALDataset *inputSHPDS = NULL;
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);  
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            OGRSpatialReference *spatialRef = inputSHPLayer->GetSpatialRef();
            OGRFeatureDefn *inFeatureDefn = inputSHPLayer->GetLayerDefn();
            OGRwkbGeometryType wktGeomType = inFeatureDefn->GetGeomType();
            
            if(wktGeomType != wkbPoint)
            {
                throw rsgis::RSGISVectorException("Input shapefile must be of type points.");
                GDALClose(inputSHPDS);
            }
            
            std::vector<OGRPoint*> *inPts = new std::vector<OGRPoint*>();
            rsgis::vec::RSGISOGRPointReader processFeature = rsgis::vec::RSGISOGRPointReader(inPts);
            rsgis::vec::RSGISProcessVector *processVector = new rsgis::vec::RSGISProcessVector(&processFeature);
        
            std::cout << "Read input Shapefile\n";
            processVector->processVectorsNoOutput(inputSHPLayer, false);
            delete processVector;
            
            rsgis::geom::RSGISFitAlphaShapesPolygonToPoints fitPoly;
            geos::geom::Polygon *poly = fitPoly.fitPolygon(inPts, alphaVal);
            
            std::vector<geos::geom::Polygon*> polys;
            polys.push_back(poly);
            rsgis::vec::RSGISVectorIO vecIO;
            vecIO.exportGEOSPolygons2SHP(outputVec, force, &polys, spatialRef);

            delete inPts;
        
            GDALClose(inputSHPDS);
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
            
            
    void executeFitPolygonsToPointClusters(std::string inputVec, std::string outputVec, std::string clusterField, double alphaVal, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVec = boost::filesystem::absolute(inputVec).string();
            outputVec = boost::filesystem::absolute(outputVec).string();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVec);
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            GDALDataset *inputSHPDS = NULL;
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);  
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            OGRSpatialReference *spatialRef = inputSHPLayer->GetSpatialRef();
            OGRFeatureDefn *inFeatureDefn = inputSHPLayer->GetLayerDefn();
            OGRwkbGeometryType wktGeomType = inFeatureDefn->GetGeomType();
            
            if(wktGeomType != wkbPoint)
            {
                throw rsgis::RSGISVectorException("Input shapefile must be of type points.");
                GDALClose(inputSHPDS);
            }
            
            std::list<double> *clusters = new std::list<double>();
            rsgis::vec::RSGISGetAttributeValues getAttVals = rsgis::vec::RSGISGetAttributeValues(clusters, clusterField);
            rsgis::vec::RSGISProcessVector *processVector = new rsgis::vec::RSGISProcessVector(&getAttVals);
            
            std::cout << "Read attributes from input shapefile\n";
            processVector->processVectorsNoOutput(inputSHPLayer, false);
            delete processVector;
            
            std::vector<double> clusterIDs = std::vector<double>();
            clusterIDs.reserve(clusters->size());
            
            for(std::list<double>::iterator iterVals = clusters->begin(); iterVals != clusters->end(); ++iterVals)
            {
                clusterIDs.push_back(*iterVals);
            }
            
            clusters->sort();
            clusters->unique();
            
            std::cout << "There are " << clusters->size() << " clusters\n";
            
            std::vector<OGRPoint*> *inPts = new std::vector<OGRPoint*>();
            rsgis::vec::RSGISOGRPointReader processFeature = rsgis::vec::RSGISOGRPointReader(inPts);
            processVector = new rsgis::vec::RSGISProcessVector(&processFeature);
            
            std::cout << "Read Points from input Shapefile\n";
            processVector->processVectorsNoOutput(inputSHPLayer, false);
            delete processVector;
            
            std::vector<geos::geom::Polygon*> polys;
            rsgis::geom::RSGISFitAlphaShapesPolygonToPoints fitPoly;
            std::vector<OGRPoint*> *pts = NULL;
            for(std::list<double>::iterator iterClusters = clusters->begin(); iterClusters != clusters->end(); ++iterClusters)
            {
                std::cout << "Processing Cluster " << *iterClusters;
                pts = new std::vector<OGRPoint*>();
                
                for(size_t i = 0; i < inPts->size(); ++i)
                {
                    if(clusterIDs.at(i) == *iterClusters)
                    {
                        pts->push_back(inPts->at(i));
                    }
                }
                std::cout << " has " << pts->size() << " points\n";
                if(pts->size() > 4)
                {
                    geos::geom::Polygon *poly = fitPoly.fitPolygon(pts, alphaVal);
                    if(poly != NULL)
                    {
                        polys.push_back(poly);
                    }
                }
                else
                {
                    // envelope??
                }
                delete pts;
            }
            
            rsgis::vec::RSGISVectorIO vecIO;
            vecIO.exportGEOSPolygons2SHP(outputVec, force, &polys, spatialRef);
            
            delete inPts;
            delete clusters;
            
            GDALClose(inputSHPDS);
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
            
            
    void executeCreateLinesOfPoints(std::string inputLinesVec, std::string outputPtsVec, double step, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputLinesVec = boost::filesystem::absolute(inputLinesVec).string();
            outputPtsVec = boost::filesystem::absolute(outputPtsVec).string();
            
            OGRRegisterAll();
            
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputLinesVec);
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////

            GDALDataset *inputSHPDS = NULL;
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputLinesVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);  
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputLinesVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            OGRSpatialReference* spatialRef = inputSHPLayer->GetSpatialRef();
            

            
            // Read in Geometries...
            std::cout << "Read in the geometries\n";
            std::vector<OGRLineString*> *lines = new std::vector<OGRLineString*>();
            rsgis::vec::RSGISOGRLineReader getOGRLines = rsgis::vec::RSGISOGRLineReader(lines);
            rsgis::vec::RSGISProcessVector processVectorGetGeoms = rsgis::vec::RSGISProcessVector(&getOGRLines);
            processVectorGetGeoms.processVectorsNoOutput(inputSHPLayer, false);
            
            std::vector<OGRPoint*> *pts = vecUtils.getRegularStepPoints(lines, step);
            
            rsgis::vec::RSGISVectorIO vecIO;
            vecIO.exportOGRPoints2SHP(outputPtsVec, force, pts, spatialRef);
            
            GDALClose(inputSHPDS);
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
            
            
    void executeFitActiveContourBoundaries(std::string inputPolysVec, std::string outputPolysVec, std::string externalForceImg, double interAlpha, double interBeta, double interGamma, double minExtThres, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputPolysVec = boost::filesystem::absolute(inputPolysVec).string();
            outputPolysVec = boost::filesystem::absolute(outputPolysVec).string();
            
            OGRRegisterAll();
            GDALAllRegister();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputPolysVec);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputPolysVec);
            
            std::string outputDIR = fileUtils.getFileDirectoryPath(outputPolysVec);
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
            GDALDataset *inputSHPDS = NULL;
            inputSHPDS = (GDALDataset*) GDALOpenEx(inputPolysVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputPolysVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            OGRSpatialReference* spatialRef = inputSHPLayer->GetSpatialRef();
            OGRFeatureDefn *inFeatureDefn = inputSHPLayer->GetLayerDefn();
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            GDALDriver *shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            GDALDataset *outputSHPDS = shpFiledriver->Create(outputPolysVec.c_str(), 0, 0, 0, GDT_Unknown, NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputPolysVec;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            OGRLayer *outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            GDALDataset *extImgDS = (GDALDataset *) GDALOpen(externalForceImg.c_str(), GA_ReadOnly);
            if(extImgDS == NULL)
            {
                std::string message = std::string("Could not open image ") + externalForceImg;
                throw RSGISImageException(message.c_str());
            }
            
            std::cout.precision(12);
            
            rsgis::vec::RSGISFitActiveContour2Geoms fitContours;
            fitContours.fitActiveContours2Polys(inputSHPLayer, outputSHPLayer, extImgDS, interAlpha, interBeta, interGamma, minExtThres, force);
            
            GDALClose(inputSHPDS);
            GDALClose(outputSHPDS);
            GDALClose(extImgDS);
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

    void executeCheckValidateGeometries(std::string inputVec, std::string lyrName, std::string outputVec, std::string vecDriver, bool printGeomErrs) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVec = boost::filesystem::absolute(inputVec).string();
            outputVec = boost::filesystem::absolute(outputVec).string();

            OGRRegisterAll();
            GDALAllRegister();

            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            GDALDataset *inputVecDS = (GDALDataset*) GDALOpenEx(inputVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputVecLayer = inputVecDS->GetLayerByName(lyrName.c_str());
            if(inputVecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + lyrName;
                throw RSGISFileException(message.c_str());
            }
            OGRSpatialReference* spatialRef = inputVecLayer->GetSpatialRef();
            OGRFeatureDefn *inFeatureDefn = inputVecLayer->GetLayerDefn();

            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            GDALDriver *gdaldriver = GetGDALDriverManager()->GetDriverByName(vecDriver.c_str());
            if( gdaldriver == NULL )
            {
                std::string message = std::string("Driver not avaiable: ") + vecDriver;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            GDALDataset *outputVecDS = gdaldriver->Create(outputVec.c_str(), 0, 0, 0, GDT_Unknown, NULL);
            if( outputVecDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVec;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            OGRLayer *outputVecLayer = outputVecDS->CreateLayer(lyrName.c_str(), spatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputVecLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + lyrName;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }

            std::cout.precision(12);
            rsgis::vec::RSGISCopyCheckPolygons checkPolys;
            checkPolys.copyCheckPolygons(inputVecLayer, outputVecLayer, printGeomErrs);

            GDALClose(inputVecDS);
            GDALClose(outputVecDS);
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
            
