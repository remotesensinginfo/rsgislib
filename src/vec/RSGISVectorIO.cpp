/*
 *  RSGISVectorIO.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISVectorIO.h"

namespace rsgis{namespace vec{

	RSGISVectorIO::RSGISVectorIO()
	{
		
	}

    void RSGISVectorIO::exportLinesAsShp(std::string outputFile, std::list<double> *x1, std::list<double> *y1, std::list<double> *x2, std::list<double> *y2, bool force)
    {
        int x1Size = x1->size();
        int y1Size = y1->size();
        int x2Size = x2->size();
        int y2Size = y2->size();

        if((x1Size != y1Size) & (y1Size != x2Size) & (x2Size != y2Size))
        {
            throw RSGISVectorOutputException("Coordinate lists are different sizes.");
        }

        OGRRegisterAll();
        RSGISVectorUtils vecUtils;
        rsgis::utils::RSGISFileUtils fileUtils;

        /////////////////////////////////////
        //
        // Check whether file already present.
        //
        /////////////////////////////////////
        std::string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
        std::string outputDIR = fileUtils.getFileDirectoryPath(outputFile);

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

        GDALDriver *shpFiledriver = NULL;
        GDALDataset *outputSHPDS = NULL;
        OGRLayer *outputSHPLayer = NULL;
        /////////////////////////////////////
        //
        // Create Output Shapfile.
        //
        /////////////////////////////////////
        const char *pszDriverName = "ESRI Shapefile";
        shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
        if( shpFiledriver == NULL )
        {
            throw RSGISVectorOutputException("SHP driver not available.");
        }
        outputSHPDS = shpFiledriver->Create(outputFile.c_str(), 0, 0, 0, GDT_Unknown, NULL);
        if( outputSHPDS == NULL )
        {
            std::string message = std::string("Could not create vector file ") + outputFile;
            throw RSGISVectorOutputException(message.c_str());
        }

        outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbLineString, NULL );
        if( outputSHPLayer == NULL )
        {
            std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
            throw RSGISVectorOutputException(message.c_str());
        }

        OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
        OGRFeature *featureOutput = NULL;

        std::list<double>::iterator iterX1;
        std::list<double>::iterator iterY1;
        std::list<double>::iterator iterX2;
        std::list<double>::iterator iterY2;

        iterX1 = x1->begin();
        iterY1 = y1->begin();
        iterX2 = x2->begin();
        iterY2 = y2->begin();
        OGRLineString *line;
        while(iterX1 != x1->end())
        {
            featureOutput = OGRFeature::CreateFeature(outputDefn);

            line = new OGRLineString();
            line->addPoint(*iterX1, *iterY1, 0);
            line->addPoint(*iterX2, *iterY2, 0);
            featureOutput->SetGeometryDirectly(line);

            if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
            {
                throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
            }
            OGRFeature::DestroyFeature(featureOutput);

            iterX1++;
            iterY1++;
            iterX2++;
            iterY2++;
        }
        GDALClose(outputSHPDS);
    }

	
	void RSGISVectorIO::exportPolygons2Layer(OGRLayer *outLayer, std::list<OGRPolygon*> *polys)
	{
		try
		{			
			OGRFeatureDefn *outputDefn = outLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			std::list<OGRPolygon*>::iterator iterPolys;
			for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
			{
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				featureOutput->SetGeometryDirectly(*iterPolys);
				
				if( outLayer->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
			}
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
    
    void RSGISVectorIO::exportOGRPoints2SHP(std::string outputFile, bool deleteIfPresent, std::vector<OGRPoint*> *pts, OGRSpatialReference* spatialRef)
    {
        try
        {
            OGRRegisterAll();
            RSGISVectorUtils vecUtils;
            rsgis::utils::RSGISFileUtils fileUtils;
            
            /////////////////////////////////////
            //
            // Check whether file already present.
            //
            /////////////////////////////////////
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
            std::string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
            
            if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
            {
                if(deleteIfPresent)
                {
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    throw RSGISException("Shapefile already exists, either delete or select force.");
                }
            }
            
            GDALDriver *shpFiledriver = NULL;
            GDALDataset *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
            if( shpFiledriver == NULL )
            {
                throw RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->Create(outputFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputFile;
                throw RSGISVectorOutputException(message.c_str());
            }
            
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPoint, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw RSGISVectorOutputException(message.c_str());
            }
            
            OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
            OGRFeature *featureOutput = NULL;
            
            // Write Polygons to file
            if(pts->size() > 0)
            {
                for(std::vector<OGRPoint*>::iterator iterPts = pts->begin(); iterPts != pts->end(); iterPts++)
                {
                    if((*iterPts) != NULL)
                    {
                        featureOutput = OGRFeature::CreateFeature(outputDefn);
                        featureOutput->SetGeometryDirectly(*iterPts);
                        
                        if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
                        {
                            throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
                        }
                        OGRFeature::DestroyFeature(featureOutput);
                    }
                }
            }
            GDALClose(outputSHPDS);
            delete pts;
        }
        catch(RSGISException &e)
        {
            throw RSGISVectorOutputException(e.what());
        }
    }
	
	
	RSGISVectorIO::~RSGISVectorIO()
	{
		
	}
}}


