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
	
	void RSGISVectorIO::readPoints(OGRLayer *vector, RSGISPointData **data, unsigned long numFeatures) throw(RSGISVectorException)
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
			cout << "Started" << flush;
			vector->ResetReading();
			while( (feature = vector->GetNextFeature()) != NULL )
			{
				if(featureCount > 10)
				{
					if((counter % feedback) == 0)
					{
						cout << ".." << feedbackCounter << ".." << flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					cout << ".." << counter << ".." << flush;
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
				//cout << "Outputted " << counter << " of " << numFeatures << " features\n";
				OGRFeature::DestroyFeature( feature );
			}
			cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::readPolygons(OGRLayer *vector, RSGISPolygonData **data, unsigned long numFeatures) throw(RSGISVectorException)
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
			cout << "Started" << flush;
			vector->ResetReading();
			while( (feature = vector->GetNextFeature()) != NULL )
			{
				if(featureCount > 10)
				{
					if((counter % feedback) == 0)
					{
						cout << ".." << feedbackCounter << ".." << flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					cout << ".." << counter << ".." << flush;
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
						string message = string("Unsupport data type: ") + string(geometry->getGeometryName());
						throw RSGISVectorException(message);
					}
					counter++;
				}
				
				//cout << "Outputted " << counter << " of " << numFeatures << " features\n";
				OGRFeature::DestroyFeature( feature );
			}
			cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::readOGRLineString2GEOSLinsSegment(OGRLineString *inLine, vector <LineSegment> *outLines)
	{
		int dimension = inLine->getDimension();
		cout << "Dimension = " << dimension << endl;
	}
	
	void RSGISVectorIO::exportPoints2Shp(OGRLayer *outputSHP, RSGISPointData **data, int numFeatures) throw(RSGISVectorOutputException)
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
			cout << "Started" << flush;
			for(int i = 0; i < numFeatures; i++)
			{
				if(numFeatures > 10)
				{
					if((counter % feedback) == 0)
					{
						cout << ".." << feedbackCounter << ".." << flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					cout << ".." << counter << ".." << flush;
				}			
				
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				
				data[i]->populateFeature(featureOutput, outputDefn);
				
				if( outputSHP->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
				
				counter++;
				//cout << "Outputted " << counter << " of " << numFeatures << " features\n";
			}
			cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportPolygons2Shp(OGRLayer *outputSHP, RSGISPolygonData **data, int numFeatures) throw(RSGISVectorOutputException)
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
			cout << "Started" << flush;
			for(int i = 0; i < numFeatures; i++)
			{
				if(numFeatures > 10)
				{
					if((counter % feedback) == 0)
					{
						cout << ".." << feedbackCounter << ".." << flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					cout << ".." << counter << ".." << flush;
				}			
				
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				
				data[i]->populateFeature(featureOutput, outputDefn);
				
				if( outputSHP->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
				
				counter++;
				//cout << "Outputted " << counter << " of " << numFeatures << " features\n";
			}
			cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportPolygons2Shp(OGRLayer *outputSHP, vector<RSGISPolygonData*> *data) throw(RSGISVectorOutputException)
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
			cout << "Started" << flush;
			for(unsigned int i = 0; i < data->size(); ++i)
			{
				if(data->size() > 10)
				{
					if((i % feedback) == 0)
					{
						cout << ".." << feedbackCounter << ".." << flush;
						feedbackCounter = feedbackCounter + 10;
					}
				}
				else
				{
					cout << ".." << i << ".." << flush;
				}			
				
				featureOutput = OGRFeature::CreateFeature(outputDefn);
				
				data->at(i)->populateFeature(featureOutput, outputDefn);
				
				if( outputSHP->CreateFeature(featureOutput) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				OGRFeature::DestroyFeature(featureOutput);
				//cout << "Outputted " << i << " of " << data->size() << " features\n";
			}
			cout << " Complete.\n";
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(string outputFile, Polygon **polys, int numFeatures) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;

			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
					
			RSGISPolygonData **polysData = new RSGISPolygonData*[numFeatures];
			for(int i = 0; i < numFeatures; i++)
			{
				polysData[i] = new RSGISEmptyPolygon();
				polysData[i]->setPolygon(polys[i]);
			}
			
			this->exportPolygons2Shp(outputSHPLayer, polysData, numFeatures);
			OGRDataSource::DestroyDataSource(outputSHPDS);
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
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}

			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			vector<Polygon*>::iterator iterPolys;
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, vector<PlotPoly*> *polyDetails) throw(RSGISVectorOutputException)
	{
		try
		{
			if(polys->size() != polyDetails->size())
			{
				throw RSGISVectorOutputException("The number of polygons and attribute objects is different.");
			}
			
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
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
			
			Polygon *poly = NULL;
			PlotPoly *polyAtts = NULL;
			
			// Write Polygons to file
			//for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, vector<ImageFootPrintPoly*> *polyDetails) throw(RSGISVectorOutputException)
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
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
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
			
			Polygon *poly = NULL;
			ImageFootPrintPoly *polyAtts = NULL;
			
			// Write Polygons to file
			//for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, OGRSpatialReference* spatialRef) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			if(polys->size() > 0)
			{
				vector<Polygon*>::iterator iterPolys;
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, OGRSpatialReference* spatialRef, string attribute, string attributeVal) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFieldDefn shpField(attribute.c_str(), OFTString);
			shpField.SetPrecision(4);
			if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				string message = string("Creating shapefile field " ) + attribute + string(" has failed");
				throw RSGISVectorOutputException(message);
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			if(polys->size() > 0)
			{
				vector<Polygon*>::iterator iterPolys;
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, list<Polygon*> *polys, OGRSpatialReference* spatialRef) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			list<Polygon*>::iterator iterPolys;
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSCoordinates2SHP(string outputFile, bool deleteIfPresent, vector<Coordinate*> *coords) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPoint, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			vector<Coordinate*>::iterator iterCoords;
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}	
	}
	
	void RSGISVectorIO::exportGEOSLineStrings2SHP(string outputFile, bool deleteIfPresent, vector<LineString*> *lines) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbLineString, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			vector<LineString*>::iterator iterlines;
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportRSGISPolygonsClusters2SHP(list<RSGIS2DPoint*> **clusters, int numClusters, string outputFile, bool force, RSGISIdentifyNonConvexPolygons *createPolygon)
	{
		RSGISGeometry geomUtils;
		RSGISPolygon *poly = NULL;
		list<RSGIS2DPoint*>::iterator iterPts;
		vector<Polygon*> *clusterPolygons = NULL;
		vector<Polygon*> *polys = new vector<Polygon*>();
		for(int n = 0; n < numClusters; n++)
		{
			clusterPolygons = new vector<Polygon*>();
			for(iterPts = clusters[n]->begin(); iterPts != clusters[n]->end(); iterPts++)
			{
				poly = (RSGISPolygon*) (*iterPts);
				clusterPolygons->push_back(poly->getPolygon());
			}
			polys->push_back(createPolygon->retrievePolygon(clusterPolygons));
			
			delete clusterPolygons;	
		}
		
		this->exportGEOSPolygons2SHP(outputFile, force, polys);
		polys->clear();
		delete polys;
	}
	
	void RSGISVectorIO::exportLinesAsShp(string outputFile, list<double> *x1, list<double> *y1, list<double> *x2, list<double> *y2, bool force) throw(RSGISVectorOutputException)
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
		RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
		string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
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
		
		
		OGRSFDriver *shpFiledriver = NULL;
		OGRDataSource *outputSHPDS = NULL;
		OGRLayer *outputSHPLayer = NULL;
		/////////////////////////////////////
		//
		// Create Output Shapfile.
		//
		/////////////////////////////////////
		const char *pszDriverName = "ESRI Shapefile";
		shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
		if( shpFiledriver == NULL )
		{
			throw RSGISVectorOutputException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
		if( outputSHPDS == NULL )
		{
			string message = string("Could not create vector file ") + outputFile;
			throw RSGISVectorOutputException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbLineString, NULL );
		if( outputSHPLayer == NULL )
		{
			string message = string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISVectorOutputException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		
		list<double>::iterator iterX1;
		list<double>::iterator iterY1;
		list<double>::iterator iterX2;
		list<double>::iterator iterY2;
		
		
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
		OGRDataSource::DestroyDataSource(outputSHPDS);
	}
	
	void RSGISVectorIO::exportGEOSPolygonClusters2SHP(string outputFile, bool deleteIfPresent, list<Polygon*> **polygons, int numClusters, OGRSpatialReference* spatialRef) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
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
			list<Polygon*>::iterator iterPolys;
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSMultiPolygonClusters2SHP(string outputFile, bool deleteIfPresent, list<Polygon*> **polygons, int numClusters, OGRSpatialReference* spatialRef) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbMultiPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
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
			list<Polygon*>::iterator iterPolys;
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}
	
	void RSGISVectorIO::exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys, OGRSpatialReference* spatialRef, vector<string> *numericColsName, vector<string> *textColsName, vector<float> **numericColsData, vector<string> **textColsData) throw(RSGISVectorOutputException)
	{
		try
		{
			OGRRegisterAll();
			RSGISVectorUtils vecUtils;
			RSGISFileUtils fileUtils;
			
			//cout << "Number of Polygons = " << polys->size() << endl;
			
			for(unsigned int i = 0; i < numericColsName->size(); ++i)
			{
				//cout << numericColsName->at(i) << " has size " << numericColsData[i]->size() << endl;
				if(numericColsData[i]->size() != polys->size())
				{
					string message = string("Numeric attribute ") + numericColsName->at(i) + string(" values have not been provided for all polygons.");
					throw RSGISException(message);
				}
			}
			
			for(unsigned int i = 0; i < textColsName->size(); ++i)
			{
				//cout << textColsName->at(i) << " has size " << textColsData[i]->size() << endl;
				if(textColsData[i]->size() != polys->size())
				{
					string message = string("Text attribute ") + textColsName->at(i) + string(" values have not been provided for all polygons.");
					throw RSGISException(message);
				}
			}
			
			/////////////////////////////////////
			//
			// Check whether file already present.
			//
			/////////////////////////////////////
			string SHPFileOutLayer = vecUtils.getLayerName(outputFile);
			string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
			
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
			
			
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			/////////////////////////////////////
			//
			// Create Output Shapfile.
			//
			/////////////////////////////////////
			const char *pszDriverName = "ESRI Shapefile";
			shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
			if( shpFiledriver == NULL )
			{
				throw RSGISVectorOutputException("SHP driver not available.");
			}
			outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
			if( outputSHPDS == NULL )
			{
				string message = string("Could not create vector file ") + outputFile;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), spatialRef, wkbPolygon, NULL );
			if( outputSHPLayer == NULL )
			{
				string message = string("Could not create vector layer ") + SHPFileOutLayer;
				throw RSGISVectorOutputException(message.c_str());
			}
			
			vector<string>::iterator iterColNames;
			for(iterColNames = numericColsName->begin(); iterColNames != numericColsName->end(); ++iterColNames)
			{
				OGRFieldDefn shpField((*iterColNames).c_str(), OFTReal);
				shpField.SetPrecision(4);
				if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					string message = string("Creating shapefile field " ) + *iterColNames + string(" has failed");
					throw RSGISVectorOutputException(message);
				}
			}
			for(iterColNames = textColsName->begin(); iterColNames != textColsName->end(); ++iterColNames)
			{
				OGRFieldDefn shpField((*iterColNames).c_str(), OFTString);
				shpField.SetWidth(50);
				if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					string message = string("Creating shapefile field " ) + *iterColNames + string(" has failed");
					throw RSGISVectorOutputException(message);
				}
			}
			
			OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			vector<Polygon*>::iterator iterPolys;
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
			OGRDataSource::DestroyDataSource(outputSHPDS);
		}
		catch(RSGISException &e)
		{
			throw RSGISVectorOutputException(e.what());
		}
	}

	
	void RSGISVectorIO::exportPolygons2Layer(OGRLayer *outLayer, list<OGRPolygon*> *polys) throw(RSGISVectorOutputException)
	{
		try
		{			
			OGRFeatureDefn *outputDefn = outLayer->GetLayerDefn();
			OGRFeature *featureOutput = NULL;
			
			// Write Polygons to file
			list<OGRPolygon*>::iterator iterPolys;
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
	
	
	RSGISVectorIO::~RSGISVectorIO()
	{
		
	}
}}


