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

#include "vec/RSGISProcessVector.h"
#include "vec/RSGISVectorMaths.h"
#include "vec/RSGISCopyCheckPolygons.h"
#include "vec/RSGISGetOGRGeometries.h"
#include "vec/RSGISVectorIO.h"

namespace rsgis{ namespace cmds {

            
    void executeVectorMaths(std::string inputVectorFile, std::string inputVectorLyr, std::string outputVectorFile, std::string outputVectorLyr, std::string outFormat, std::string outColumn, std::string expression, bool delExistVec, std::vector<RSGISVariableFieldCmds> vars)
    {
        try
        {
            OGRRegisterAll();

            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;

            // Convert to absolute path
            inputVectorFile = boost::filesystem::absolute(inputVectorFile).string();
            outputVectorFile = boost::filesystem::absolute(outputVectorFile).string();
            
            GDALDataset *inputVecDS = NULL;
            OGRLayer *inputVecLayer = NULL;
            GDALDriver *ogrVecDriver = NULL;
            GDALDataset *outputVecDS = NULL;
            OGRLayer *outputVecLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRFeatureDefn *inFeatureDefn = NULL;
            
            rsgis::vec::RSGISProcessVector *processVector = NULL;
            rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;

            if(outFormat == "ESRI Shapefile")
            {
                std::string outputDIR = fileUtils.getFileDirectoryPath(outputVectorFile);
                if (vecUtils.checkDIR4SHP(outputDIR, outputVectorLyr))
                {
                    if(delExistVec)
                    {
                        vecUtils.deleteSHP(outputDIR, outputVectorLyr);
                    } else
                    {
                        throw RSGISException("Shapefile already exists, either delete or select force.");
                    }
                }
            }
            else
            {
                if(delExistVec)
                {
                    fileUtils.removeFileIfPresent(outputVectorFile);
                }
            }
            
            /////////////////////////////////////
            //
            // Open Input Vector Layer.
            //
            /////////////////////////////////////
            inputVecDS = (GDALDataset*) GDALOpenEx(inputVectorFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVectorFile;
                throw RSGISFileException(message.c_str());
            }
            inputVecLayer = inputVecDS->GetLayerByName(inputVectorLyr.c_str());
            if(inputVecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + inputVectorLyr;
                throw RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputVecLayer->GetSpatialRef();
            inFeatureDefn = inputVecLayer->GetLayerDefn();
            
            /////////////////////////////////////
            //
            // Create Output Vector Layer.
            //
            /////////////////////////////////////
            ogrVecDriver =  GetGDALDriverManager()->GetDriverByName(outFormat.c_str());
            if( ogrVecDriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("Vector driver not available: " + outFormat);
            }
            outputVecDS = ogrVecDriver->Create(outputVectorFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputVecDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVectorFile;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputVecLayer = outputVecDS->CreateLayer(outputVectorLyr.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputVecLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + outputVectorLyr;
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
    }

            
    void executeCreateLinesOfPoints(std::string inputVectorFile, std::string inputVectorLyr, std::string outputVectorFile, std::string outputVectorLyr, std::string outFormat, double step, bool delExistVec)
    {
        try
        {
            OGRRegisterAll();
            GDALAllRegister();

            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;

            // Convert to absolute path
            inputVectorFile = boost::filesystem::absolute(inputVectorFile).string();
            outputVectorFile = boost::filesystem::absolute(outputVectorFile).string();

            GDALDataset *inputVecDS = NULL;
            OGRLayer *inputVecLayer = NULL;
            GDALDriver *ogrVecDriver = NULL;
            GDALDataset *outputVecDS = NULL;
            OGRLayer *outputVecLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRFeatureDefn *inFeatureDefn = NULL;

            rsgis::vec::RSGISProcessVector *processVector = NULL;
            rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;

            if(outFormat == "ESRI Shapefile")
            {
                std::string outputDIR = fileUtils.getFileDirectoryPath(outputVectorFile);
                if (vecUtils.checkDIR4SHP(outputDIR, outputVectorLyr))
                {
                    if(delExistVec)
                    {
                        vecUtils.deleteSHP(outputDIR, outputVectorLyr);
                    } else
                    {
                        throw RSGISException("Shapefile already exists, either delete or select force.");
                    }
                }
            }
            else
            {
                if(delExistVec)
                {
                    fileUtils.removeFileIfPresent(outputVectorFile);
                }
            }

            /////////////////////////////////////
            //
            // Open Input Vector Layer.
            //
            /////////////////////////////////////
            inputVecDS = (GDALDataset*) GDALOpenEx(inputVectorFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVectorFile;
                throw RSGISFileException(message.c_str());
            }
            inputVecLayer = inputVecDS->GetLayerByName(inputVectorLyr.c_str());
            if(inputVecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + inputVectorLyr;
                throw RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputVecLayer->GetSpatialRef();
            inFeatureDefn = inputVecLayer->GetLayerDefn();

            /////////////////////////////////////
            //
            // Create Output Vector Layer.
            //
            /////////////////////////////////////
            ogrVecDriver =  GetGDALDriverManager()->GetDriverByName(outFormat.c_str());
            if( ogrVecDriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("Vector driver not available: " + outFormat);
            }
            outputVecDS = ogrVecDriver->Create(outputVectorFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputVecDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVectorFile;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputVecLayer = outputVecDS->CreateLayer(outputVectorLyr.c_str(), inputSpatialRef, wkbPoint, NULL );
            if( outputVecLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + outputVectorLyr;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }


            
            // Read in Geometries...
            std::cout << "Read in the geometries\n";
            std::vector<OGRLineString*> *lines = new std::vector<OGRLineString*>();
            rsgis::vec::RSGISOGRLineReader getOGRLines = rsgis::vec::RSGISOGRLineReader(lines);
            rsgis::vec::RSGISProcessVector processVectorGetGeoms = rsgis::vec::RSGISProcessVector(&getOGRLines);
            processVectorGetGeoms.processVectorsNoOutput(inputVecLayer, false);
            
            std::vector<OGRPoint*> *pts = vecUtils.getRegularStepPoints(lines, step);
            
            rsgis::vec::RSGISVectorIO vecIO;
            vecIO.exportOGRPoints2Layer(outputVecLayer, pts);
            
            GDALClose(inputVecDS);
            GDALClose(outputVecDS);
            delete pts;
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
            

    void executeCheckValidateGeometries(std::string inputVectorFile, std::string inputVectorLyr, std::string outputVectorFile, std::string outputVectorLyr, std::string outFormat, bool printGeomErrs, bool delExistVec)
    {
        try
        {
            OGRRegisterAll();
            GDALAllRegister();

            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;

            // Convert to absolute path
            inputVectorFile = boost::filesystem::absolute(inputVectorFile).string();
            outputVectorFile = boost::filesystem::absolute(outputVectorFile).string();

            GDALDataset *inputVecDS = NULL;
            OGRLayer *inputVecLayer = NULL;
            GDALDriver *ogrVecDriver = NULL;
            GDALDataset *outputVecDS = NULL;
            OGRLayer *outputVecLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRFeatureDefn *inFeatureDefn = NULL;

            if(outFormat == "ESRI Shapefile")
            {
                std::string outputDIR = fileUtils.getFileDirectoryPath(outputVectorFile);
                if (vecUtils.checkDIR4SHP(outputDIR, outputVectorLyr))
                {
                    if(delExistVec)
                    {
                        vecUtils.deleteSHP(outputDIR, outputVectorLyr);
                    } else
                    {
                        throw RSGISException("Shapefile already exists, either delete or select force.");
                    }
                }
            }
            else
            {
                if(delExistVec)
                {
                    fileUtils.removeFileIfPresent(outputVectorFile);
                }
            }

            /////////////////////////////////////
            //
            // Open Input Vector Layer.
            //
            /////////////////////////////////////
            inputVecDS = (GDALDataset*) GDALOpenEx(inputVectorFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if(inputVecDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVectorFile;
                throw RSGISFileException(message.c_str());
            }
            inputVecLayer = inputVecDS->GetLayerByName(inputVectorLyr.c_str());
            if(inputVecLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + inputVectorLyr;
                throw RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputVecLayer->GetSpatialRef();
            inFeatureDefn = inputVecLayer->GetLayerDefn();

            /////////////////////////////////////
            //
            // Create Output Vector Layer.
            //
            /////////////////////////////////////
            ogrVecDriver =  GetGDALDriverManager()->GetDriverByName(outFormat.c_str());
            if( ogrVecDriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("Vector driver not available: " + outFormat);
            }
            outputVecDS = ogrVecDriver->Create(outputVectorFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputVecDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVectorFile;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputVecLayer = outputVecDS->CreateLayer(outputVectorLyr.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputVecLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + outputVectorLyr;
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
            
