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

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISFileUtils.h"


namespace rsgis{ namespace cmds {

            
    void executeVectorMaths(std::string inputVectorFile, std::string inputVectorLyr, std::string outputVectorFile, std::string outputVectorLyr, std::string outFormat, std::string outColumn, std::string expression, bool delExistVec, std::vector<RSGISVariableFieldCmds> vars)
    {
        std::cout << "NOT IMPLEMENTED\n";
        throw RSGISException("executeVectorMaths is not implemented!");
        /*
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
            
            GDALDataset *inputVecDS = NULL;
            OGRLayer *inputVecLayer = NULL;
            GDALDriver *ogrVecDriver = NULL;
            GDALDataset *outputVecDS = NULL;
            OGRLayer *outputVecLayer = NULL;
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
            inputVecDS = (GDALDataset*) GDALOpenEx(inputVector.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputVecLayer = inputVecDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputVecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputVecLayer->GetSpatialRef();
            inFeatureDefn = inputVecLayer->GetLayerDefn();
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            ogrVecDriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName );
            if( ogrVecDriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputVecDS = ogrVecDriver->Create(outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputVecDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputVecLayer = outputVecDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputVecLayer == NULL )
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
            processVector->processVectors(inputVecLayer, outputVecLayer, true, true, false);
            
            for(unsigned i = 0; i < numVars; ++i)
            {
                delete variables[i];
            }
            delete[] variables;
            
            GDALClose(inputVecDS);
            GDALClose(outputVecDS);
            
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
         */
    }

            
    void executeCreateLinesOfPoints(std::string inputVectorFile, std::string inputVectorLyr, std::string outputVectorFile, std::string outputVectorLyr, std::string outFormat, double step, bool delExistVec)
    {
        std::cout << "NOT IMPLEMENTED\n";
        throw RSGISException("executeCreateLinesOfPoints is not implemented!");
        /*
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

            GDALDataset *inputVecDS = NULL;
            inputVecDS = (GDALDataset*) GDALOpenEx(inputLinesVec.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputLinesVec;
                throw RSGISFileException(message.c_str());
            }
            OGRLayer *inputVecLayer = inputVecDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputVecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            OGRSpatialReference* spatialRef = inputVecLayer->GetSpatialRef();
            

            
            // Read in Geometries...
            std::cout << "Read in the geometries\n";
            std::vector<OGRLineString*> *lines = new std::vector<OGRLineString*>();
            rsgis::vec::RSGISOGRLineReader getOGRLines = rsgis::vec::RSGISOGRLineReader(lines);
            rsgis::vec::RSGISProcessVector processVectorGetGeoms = rsgis::vec::RSGISProcessVector(&getOGRLines);
            processVectorGetGeoms.processVectorsNoOutput(inputVecLayer, false);
            
            std::vector<OGRPoint*> *pts = vecUtils.getRegularStepPoints(lines, step);
            
            rsgis::vec::RSGISVectorIO vecIO;
            vecIO.exportOGRPoints2SHP(outputPtsVec, force, pts, spatialRef);
            
            GDALClose(inputVecDS);
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
         */
    }
            


    void executeCheckValidateGeometries(std::string inputVectorFile, std::string inputVectorLyr, std::string outputVectorFile, std::string outputVectorLyr, std::string outFormat, bool printGeomErrs, bool delExistVec)
    {
        std::cout << "NOT IMPLEMENTED\n";
        throw RSGISException("executeCheckValidateGeometries is not implemented!");
        /*
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
         */
    }
            
}}
            
