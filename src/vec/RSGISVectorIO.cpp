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
	
	void RSGISVectorIO::readPoints(OGRLayer *vector, RSGISPointData **data, unsigned long numFeatures)
	{
		try
		{
			OGRPoint *point = NULL;
			OGRGeometry *geometry = NULL;
			OGRFeature *feature = NULL;
			
			unsigned long featureCount = vector->GetFeatureCount();
			if(featureCount != numFeatures)
			{
				throw RSGISVectorException("The data buffer is not large enough for all the input features.");
			}
			OGRFeatureDefn *featDefn = vector->GetLayerDefn();
			int counter = 0;
			int feedback = int((double(featureCount)/10) + 0.5);
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			vector->ResetReading();
			while( (feature = vector->GetNextFeature()) != NULL )
			{
				if(featureCount > 10)
				{
					if((counter % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					std::cout << ".." << counter << ".." << std::flush;
				}
				
				geometry = feature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
				{
					point = (OGRPoint *) geometry;
					data[counter]->readPoint(point);
					data[counter]->readAttribtues(feature, featDefn);
				}
				else
				{
					throw RSGISVectorException("Unsupport data type.");
				}
				counter++;
				OGRFeature::DestroyFeature( feature );
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::readPolygons(OGRLayer *vector, RSGISPolygonData **data, unsigned long numFeatures)
	{
		try
		{
			OGRPolygon *polygon = NULL;
			OGRMultiPolygon *multiPolygon = NULL;
			OGRGeometry *geometry = NULL;
			OGRFeature *feature = NULL;
			
			unsigned long featureCount = vector->GetFeatureCount();
			if(featureCount != numFeatures)
			{
				throw RSGISVectorException("The data buffer is not large enough for all the input features.");
			}
			OGRFeatureDefn *featDefn = vector->GetLayerDefn();
			int counter = 0;
			int feedback = int((double(featureCount)/10) + 0.5);
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			vector->ResetReading();
			while( (feature = vector->GetNextFeature()) != NULL )
			{
				if(featureCount > 10)
				{
					if((counter % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					std::cout << ".." << counter << ".." << std::flush;
				}
				
				geometry = feature->GetGeometryRef();
				if(geometry != NULL)
				{
					if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
					{
						polygon = (OGRPolygon *) geometry;
						data[counter]->readPolygon(polygon);
						data[counter]->readAttribtues(feature, featDefn);
					} 
					else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
					{
						multiPolygon = (OGRMultiPolygon *) geometry;
						data[counter]->readMultiPolygon(multiPolygon);
						data[counter]->readAttribtues(feature, featDefn);
					}
					else
					{
						std::string message = std::string("Unsupport data type: ") + std::string(geometry->getGeometryName());
						throw RSGISVectorException(message);
					}
					counter++;
				}
				OGRFeature::DestroyFeature( feature );
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::readOGRLineString2GEOSLinsSegment(OGRLineString *inLine, std::vector<geos::geom::LineSegment> *outLines)
	{
		int dimension = inLine->getDimension();
		std::cout << "Dimension = " << dimension << std::endl;
	}
	
	void RSGISVectorIO::exportPoints2Shp(OGRLayer *outputSHP, RSGISPointData **data, int numFeatures)
	{
		try
		{
			if(numFeatures <= 0)
			{
				throw RSGISVectorOutputException("There are no features to output.");
			}
			
			data[0]->createLayerDefinition(outputSHP);
			
			OGRFeatureDefn *outputDefn = outputSHP->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			int counter = 0;
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			for(int i = 0; i < numFeatures; i++)
			{
				if(numFeatures > 10)
				{
					if((counter % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					std::cout << ".." << counter << ".." << std::flush;
				}			
				
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				
				data[i]->populateFeature(featureOutput, outputDefn);
				
				if( outputSHP->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
				
				counter++;
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportPolygons2Shp(OGRLayer *outputSHP, RSGISPolygonData **data, int numFeatures)
	{
		try
		{
			if(numFeatures <= 0)
			{
				throw RSGISVectorOutputException("There are no features to output.");
			}
			
			data[0]->createLayerDefinition(outputSHP);
			
			OGRFeatureDefn *outputDefn = outputSHP->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			int counter = 0;
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			for(int i = 0; i < numFeatures; i++)
			{
				if(numFeatures > 10)
				{
					if((counter % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					std::cout << ".." << counter << ".." << std::flush;
				}			
				
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				
				data[i]->populateFeature(featureOutput, outputDefn);
				
				if( outputSHP->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
				
				counter++;
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportPolygons2Shp(OGRLayer *outputSHP, std::vector<RSGISPolygonData*> *data)
	{
		try
		{
			if(data->size() <= 0)
			{
				throw RSGISVectorOutputException("There are no features to output.");
			}
			
			data->at(0)->createLayerDefinition(outputSHP);
			
			OGRFeatureDefn *outputDefn = outputSHP->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			int feedback = data->size()/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			for(unsigned int i = 0; i < data->size(); ++i)
			{
				if(data->size() > 10)
				{
					if((i % feedback) == 0)
					{
						std::cout << ".." << feedbackCounter << ".." << std::flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					std::cout << ".." << i << ".." << std::flush;
				}			
				
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				
				data->at(i)->populateFeature(featureOutput, outputDefn);
				
				if( outputSHP->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(std::string outputFile, geos::geom::Polygon **polys, int numFeatures)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;

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
			std::string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
					
			RSGISPolygonData **polysData = new RSGISPolygonData*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				polysData[i] = new RSGISEmptyPolygon();
				polysData[i]->setPolygon(polys[i]);
			}
			
			this->exportPolygons2Shp(outputSHPLayer, polysData, numFeatures);
			GDALClose(outputSHPDS);
			for(int i = 0; i < numFeatures; i++)
			{
				delete polysData[i];
			}
			delete[] polysData;
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys)
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

			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
			{
				if((*iterPolys) != NULL)
                {
                    featureOutput = OGRFeature::CreateFeature(outputDefn);
                    featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon((*iterPolys)));
                    
                    if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
                    {
                        throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
                    }
                    OGRFeature::DestroyFeature(featureOutput);
                }
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, std::vector<rsgis::utils::PlotPoly*> *polyDetails)
	{
		try
		{
			if(polys->size() != polyDetails->size())
			{
				throw RSGISVectorOutputException("The number of polygons and attribute objects is different.");
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
			shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName );
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFieldDefn shpField1("id", OFTInteger);
			if(outputSHPLayer->CreateField( &shpField1 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'id\' has failed");
			}
			
			OGRFieldDefn shpField2("zone", OFTInteger);
			if(outputSHPLayer->CreateField( &shpField2 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'zone\' has failed");
			}
			
			OGRFieldDefn shpField3("eastings", OFTReal);
			shpField3.SetPrecision(10);
			if(outputSHPLayer->CreateField( &shpField3 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'eastings\' has failed");
			}
			
			OGRFieldDefn shpField4("northings", OFTReal);
			shpField4.SetPrecision(10);
			if(outputSHPLayer->CreateField( &shpField4 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'northings\' has failed");
			}
			
			OGRFieldDefn shpField5("sw_east", OFTReal);
			shpField5.SetPrecision(10);
			if(outputSHPLayer->CreateField( &shpField5 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'sw_east\' has failed");
			}
			
			OGRFieldDefn shpField6("sw_north", OFTReal);
			shpField6.SetPrecision(10);
			if(outputSHPLayer->CreateField( &shpField6 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'sw_north\' has failed");
			}
			
			OGRFieldDefn shpField7("eastside", OFTReal);
			shpField7.SetPrecision(10);
			if(outputSHPLayer->CreateField( &shpField7 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'eastside\' has failed");
			}
			
			OGRFieldDefn shpField8("northside", OFTReal);
			shpField8.SetPrecision(10);
			if(outputSHPLayer->CreateField( &shpField8 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'northside\' has failed");
			}
			
			OGRFieldDefn shpField9("orient", OFTReal);
			shpField9.SetPrecision(10);
			if(outputSHPLayer->CreateField( &shpField9 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'orient\' has failed");
			}
			
			OGRFieldDefn shpField10("sitename", OFTString);
			shpField10.SetWidth(25);
			if( outputSHPLayer->CreateField( &shpField10 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'sitename\' has failed");
			}
			
			OGRFieldDefn shpField11("study", OFTString);
			shpField11.SetWidth(25);
			if( outputSHPLayer->CreateField( &shpField11 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'study\' has failed");
			}
			
			OGRFieldDefn shpField12("site", OFTString);
			shpField12.SetWidth(25);
			if( outputSHPLayer->CreateField( &shpField12 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'site\' has failed");
			}
			
			OGRFieldDefn shpField13("extraid", OFTInteger);
			if(outputSHPLayer->CreateField( &shpField13 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'extraid\' has failed");
			}
			
			OGRFieldDefn shpField14("issues", OFTString);
			shpField14.SetWidth(255);
			if( outputSHPLayer->CreateField( &shpField14 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'issues\' has failed");
			}
			
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			geos::geom::Polygon *poly = NULL;
            rsgis::utils::PlotPoly *polyAtts = NULL;
			
			// Write Polygons to file
			for(unsigned int i = 0; i < polys->size(); ++i)
			{
				poly = polys->at(i);
				polyAtts = polyDetails->at(i);
				
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				
				featureOutput->SetField(outputDefn->GetFieldIndex("id"), polyAtts->fid);
				featureOutput->SetField(outputDefn->GetFieldIndex("zone"), polyAtts->zone);
				featureOutput->SetField(outputDefn->GetFieldIndex("eastings"), polyAtts->eastings);
				featureOutput->SetField(outputDefn->GetFieldIndex("northings"), polyAtts->northings);
				featureOutput->SetField(outputDefn->GetFieldIndex("sw_east"), polyAtts->sw_Eastings);
				featureOutput->SetField(outputDefn->GetFieldIndex("sw_north"), polyAtts->sw_Northings);
				featureOutput->SetField(outputDefn->GetFieldIndex("eastside"), polyAtts->eastSide);
				featureOutput->SetField(outputDefn->GetFieldIndex("northside"), polyAtts->northSide);
				featureOutput->SetField(outputDefn->GetFieldIndex("orient"), polyAtts->orientation);
				featureOutput->SetField(outputDefn->GetFieldIndex("sitename"), polyAtts->siteName.c_str());
				featureOutput->SetField(outputDefn->GetFieldIndex("study"), polyAtts->study.c_str());
				featureOutput->SetField(outputDefn->GetFieldIndex("site"), polyAtts->site.c_str());
				featureOutput->SetField(outputDefn->GetFieldIndex("extraid"), polyAtts->extraID);
				featureOutput->SetField(outputDefn->GetFieldIndex("issues"), polyAtts->issues.c_str());
				
				featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon(poly));
				
				if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, std::vector<rsgis::utils::ImageFootPrintPoly*> *polyDetails)
	{
		// Takes vector of GEOS polygons and vector of ImageFootPrintPolys (used for attributes)
		try
		{
			if(polys->size() != polyDetails->size())
			{
				throw RSGISVectorOutputException("The number of polygons and attribute objects is different.");
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFieldDefn shpField1("id", OFTInteger);
			if(outputSHPLayer->CreateField( &shpField1 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'id\' has failed");
			}
			
			OGRFieldDefn shpField2("sceneName", OFTString);
			shpField2.SetWidth(50);
			if( outputSHPLayer->CreateField( &shpField2 ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field \'SceneName\' has failed");
			}			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
            geos::geom::Polygon *poly = NULL;
            rsgis::utils::ImageFootPrintPoly *polyAtts = NULL;
			
			// Write Polygons to file
			for(unsigned int i = 0; i < polys->size(); ++i)
			{
				poly = polys->at(i);
				polyAtts = polyDetails->at(i);
				
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				
				featureOutput->SetField(outputDefn->GetFieldIndex("id"), polyAtts->fid);
				featureOutput->SetField(outputDefn->GetFieldIndex("sceneName"), polyAtts->scene.c_str());
				featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon(poly));
				
				if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef)
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			if(polys->size() > 0)
			{
				std::vector<geos::geom::Polygon*>::iterator iterPolys;
				for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
				{
                    if((*iterPolys) != NULL)
                    {
                        featureOutput = OGRFeature::CreateFeature(outputDefn);
                        featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon((*iterPolys)));
                        
                        if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
                        {
                            throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
                        }
                        OGRFeature::DestroyFeature(featureOutput);
                    }
				}
				
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
    
    void RSGISVectorIO::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef, std::vector<std::string> *outAtts, std::string attName)
    {
        try
		{
            if(polys->size() != outAtts->size())
            {
                throw RSGISVectorOutputException("The list of polygons and attribute values are different lengths.");
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
            
            OGRFieldDefn shpField(attName.c_str(), OFTString);
			shpField.SetWidth(254);
			if( outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
                std::string message = std::string("Creating shapefile field \'") + attName + std::string("\' has failed");
				throw RSGISVectorOutputException(message);
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
            int outColIdx = outputDefn->GetFieldIndex(attName.c_str());
            unsigned int polyIdx = 0;
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			if(polys->size() > 0)
			{
				std::vector<geos::geom::Polygon*>::iterator iterPolys;
				for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
				{
                    if((*iterPolys) != NULL)
                    {
                        featureOutput = OGRFeature::CreateFeature(outputDefn);
                        featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon((*iterPolys)));
                        
                        featureOutput->SetField(outColIdx, outAtts->at(polyIdx++).c_str());
                        
                        if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
                        {
                            throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
                        }
                        OGRFeature::DestroyFeature(featureOutput);
                    }
				}
				
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
    }
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef, std::string attribute, std::string attributeVal)
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFieldDefn shpField(attribute.c_str(), OFTString);
			shpField.SetPrecision(4);
			if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field " ) + attribute + std::string(" has failed");
				throw RSGISVectorOutputException(message);
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			if(polys->size() > 0)
			{
				std::vector<geos::geom::Polygon*>::iterator iterPolys;
				for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
				{
                    if((*iterPolys) != NULL)
                    {
                        featureOutput = OGRFeature::CreateFeature(outputDefn);
                        featureOutput->SetField(outputDefn->GetFieldIndex(attribute.c_str()), attributeVal.c_str());
                        featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon((*iterPolys)));
                        
                        if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
                        {
                            throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
                        }
                        OGRFeature::DestroyFeature(featureOutput);
                    }
				}
				
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef)
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
			shpFiledriver =  GetGDALDriverManager()->GetDriverByName(pszDriverName );
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			std::list<geos::geom::Polygon*>::iterator iterPolys;
			for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
			{
                if((*iterPolys) != NULL)
                {
                    featureOutput = OGRFeature::CreateFeature(outputDefn);
                    featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon((*iterPolys)));
                    
                    if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
                    {
                        throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
                    }
                    OGRFeature::DestroyFeature(featureOutput);
                }
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSCoordinates2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Coordinate*> *coords)
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPoint, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			std::vector<geos::geom::Coordinate*>::iterator iterCoords;
			for(iterCoords = coords->begin(); iterCoords != coords->end(); iterCoords++)
			{
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				featureOutput->SetGeometryDirectly(vecUtils.convertGEOSCoordinate2OGRPoint((*iterCoords)));
				
				if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}	
	}
	
	void RSGISVectorIO::exportGEOSLineStrings2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::LineString*> *lines)
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
			
			// Write Polygons to file
			std::vector<geos::geom::LineString*>::iterator iterlines;
			for(iterlines = lines->begin(); iterlines != lines->end(); iterlines++)
			{
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				featureOutput->SetGeometryDirectly(vecUtils.convertGEOSLineString2OGRLineString((*iterlines)));
				
				if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportRSGISPolygonsClusters2SHP(std::list<rsgis::geom::RSGIS2DPoint*> **clusters, int numClusters, std::string outputFile, bool force, rsgis::geom::RSGISIdentifyNonConvexPolygons *createPolygon)
	{
        rsgis::geom::RSGISGeometry geomUtils;
		rsgis::geom::RSGISPolygon *poly = NULL;
		std::list<rsgis::geom::RSGIS2DPoint*>::iterator iterPts;
		std::vector<geos::geom::Polygon*> *clusterPolygons = NULL;
		std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
		for(int n = 0; n < numClusters; n++)
		{
			clusterPolygons = new std::vector<geos::geom::Polygon*>();
			for(iterPts = clusters[n]->begin(); iterPts != clusters[n]->end(); iterPts++)
			{
				poly = (rsgis::geom::RSGISPolygon*) (*iterPts);
				clusterPolygons->push_back(poly->getPolygon());
			}
			polys->push_back(createPolygon->retrievePolygon(clusterPolygons));
			
			delete clusterPolygons;	
		}
		
		this->exportGEOSPolygons2SHP(outputFile, force, polys);
		polys->clear();
		delete polys;
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
	
	void RSGISVectorIO::exportGEOSPolygonClusters2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Polygon*> **polygons, int numClusters, OGRSpatialReference* spatialRef)
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFieldDefn shpField("cluster", OFTReal);
			shpField.SetPrecision(2);
			if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field cluster has failed");
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			std::list<geos::geom::Polygon*>::iterator iterPolys;
			for(int i = 0; i < numClusters; ++i)
			{
				for(iterPolys = polygons[i]->begin(); iterPolys != polygons[i]->end(); ++iterPolys)
				{
					featureOutput = OGRFeature::CreateFeature(outputDefn);
					featureOutput->SetField(outputDefn->GetFieldIndex("cluster"), i);
					featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPolygon2OGRPolygon((*iterPolys)));
					
					if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
					{
						throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
					}
					OGRFeature::DestroyFeature(featureOutput);
				}
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
    
    void RSGISVectorIO::exportGEOSPointClusters2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Point*> **points, int numClusters, OGRSpatialReference* spatialRef)
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
            
            OGRFieldDefn shpField("cluster", OFTReal);
            shpField.SetPrecision(2);
            if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
            {
                throw RSGISVectorOutputException("Creating shapefile field cluster has failed");
            }
            
            OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
            OGRFeature *featureOutput = NULL;
            
            // Write Polygons to file
            std::list<geos::geom::Point*>::iterator iterPoints;
            for(int i = 0; i < numClusters; ++i)
            {
                for(iterPoints = points[i]->begin(); iterPoints != points[i]->end(); ++iterPoints)
                {
                    featureOutput = OGRFeature::CreateFeature(outputDefn);
                    featureOutput->SetField(outputDefn->GetFieldIndex("cluster"), i);
                    featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPoint2OGRPoint((*iterPoints)));
                    
                    if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
                    {
                        throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
                    }
                    OGRFeature::DestroyFeature(featureOutput);
                }
            }
            GDALClose(outputSHPDS);
        }
        catch(RSGISException &e)
        {
            throw RSGISVectorOutputException(e.what());
        }
    }
	
	void RSGISVectorIO::exportGEOSMultiPolygonClusters2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Polygon*> **polygons, int numClusters, OGRSpatialReference* spatialRef)
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbMultiPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFieldDefn shpField("cluster", OFTReal);
			shpField.SetPrecision(2);
			if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				throw RSGISVectorOutputException("Creating shapefile field cluster has failed");
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			std::list<geos::geom::Polygon*>::iterator iterPolys;
			for(int i = 0; i < numClusters; ++i)
			{
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				featureOutput->SetField(outputDefn->GetFieldIndex("cluster"), i);
				featureOutput->SetGeometryDirectly(vecUtils.convertGEOSPolygons2OGRMultiPolygon(polygons[i]));
				
				if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys, OGRSpatialReference* spatialRef, std::vector<std::string> *numericColsName, std::vector<std::string> *textColsName, std::vector<float> **numericColsData, std::vector<std::string> **textColsData)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			rsgis::utils::RSGISFileUtils fileUtils;
						
			for(unsigned int i = 0; i < numericColsName->size(); ++i)
			{
				if(numericColsData[i]->size() != polys->size())
				{
					std::string message = std::string("Numeric attribute ") + numericColsName->at(i) + std::string(" values have not been provided for all polygons.");
					throw RSGISException(message);
				}
			}
			
			for(unsigned int i = 0; i < textColsName->size(); ++i)
			{
				if(textColsData[i]->size() != polys->size())
				{
					std::string message = std::string("Text attribute ") + textColsName->at(i) + std::string(" values have not been provided for all polygons.");
					throw RSGISException(message);
				}
			}
			
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
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			std::vector<std::string>::iterator iterColNames;
			for(iterColNames = numericColsName->begin(); iterColNames != numericColsName->end(); ++iterColNames)
			{
				OGRFieldDefn shpField((*iterColNames).c_str(), OFTReal);
				shpField.SetPrecision(4);
				if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field " ) + *iterColNames + std::string(" has failed");
					throw RSGISVectorOutputException(message);
				}
			}
			for(iterColNames = textColsName->begin(); iterColNames != textColsName->end(); ++iterColNames)
			{
				OGRFieldDefn shpField((*iterColNames).c_str(), OFTString);
				shpField.SetWidth(50);
				if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					std::string message = std::string("Creating shapefile field " ) + *iterColNames + std::string(" has failed");
					throw RSGISVectorOutputException(message);
				}
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			OGRPolygon *poly;
			for(unsigned int i = 0; i < polys->size(); ++i)
			{
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				for(unsigned int j = 0; j < numericColsName->size(); ++j)
				{
					featureOutput->SetField(outputDefn->GetFieldIndex((numericColsName->at(j)).c_str()), numericColsData[j]->at(i));
				}
				for(unsigned int j = 0; j < textColsName->size(); ++j)
				{
					featureOutput->SetField(outputDefn->GetFieldIndex((textColsName->at(j)).c_str()), textColsData[j]->at(i).c_str());
				}
				poly = vecUtils.convertGEOSPolygon2OGRPolygon(polys->at(i));
				featureOutput->SetGeometryDirectly(poly);
				
				if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
			}
			GDALClose(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
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


