/*
 *  RSGISGeomTestExport.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/06/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISGeomTestExport.h"

namespace rsgis{namespace geom{
	
	RSGISGeomTestExport::RSGISGeomTestExport()
	{
		
	}
	
	
	string RSGISGeomTestExport::getLayerName(string filepath)
	{
		//cout << filepath << endl;
		int strSize = filepath.size();
		int lastSlash = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filepath.at(i) == '/')
			{
				lastSlash = i;
			}
		}
		string filename = filepath.substr(lastSlash+1);
		//cout << filename << endl;
		
		strSize = filename.size();
		int lastpt = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filename.at(i) == '.')
			{
				lastpt = i;
			}
		}
		
		string layerName = filename.substr(0, lastpt);
		//cout << layerName << endl;
		return layerName;		
	}
	
	OGRLineString* RSGISGeomTestExport::convertGEOSLineSegment2OGRLineString(LineSegment *line)
	{
		OGRLineString *OGRLine = new OGRLineString();
		OGRLine->addPoint(line->p0.x, line->p0.y, line->p0.z);
		OGRLine->addPoint(line->p1.x, line->p1.y, line->p1.z);
		return OGRLine;
	}
	
	OGRLineString* RSGISGeomTestExport::convertGEOSLineString2OGRLineString(LineString *line)
	{
		OGRLineString *OGRLine = new OGRLineString();
		
		CoordinateSequence *coordSeq = line->getCoordinates();
		Coordinate coord;
		int numCoords = coordSeq->getSize();
		for(int i = 0; i < numCoords; i++)
		{
			coord = coordSeq->getAt(i);
			OGRLine->addPoint(coord.x, coord.y, coord.z);
		}
		
		return OGRLine;
	}
	
	OGRLinearRing* RSGISGeomTestExport::convertGEOSLineString2OGRLinearRing(LineString *line)
	{
		OGRLinearRing *ring = new OGRLinearRing();
		const CoordinateSequence *coords = line->getCoordinatesRO();
		int numCoords = coords->getSize();
		Coordinate coord;
		//cout << "numCoords = " << numCoords << endl;
		for(int i = 0; i < numCoords; i++)
		{
			coord = coords->getAt(i);
			ring->addPoint(coord.x, coord.y, coord.z);
		}
		return ring;
	}
	
	OGRPolygon* RSGISGeomTestExport::convertGEOSPolygon2OGRPolygon(Polygon *poly)
	{
		OGRPolygon *ogrPoly = new OGRPolygon();
		
		// Add outer ring!
		const LineString *exteriorRing = poly->getExteriorRing();
		OGRLinearRing *ogrRing = this->convertGEOSLineString2OGRLinearRing(const_cast<LineString *>(exteriorRing));
		ogrPoly->addRing(ogrRing);
		delete ogrRing;
		
		int numInternalRings = poly->getNumInteriorRing();
		
		LineString *innerRing = NULL;
		
		for(int i = 0; i < numInternalRings; i++)
		{
			innerRing = const_cast<LineString *>(poly->getInteriorRingN(i));
			ogrRing = this->convertGEOSLineString2OGRLinearRing(innerRing);
			ogrPoly->addRing(ogrRing);
			delete ogrRing;
		}
		
		return ogrPoly;	}
	
	OGRPoint* RSGISGeomTestExport::convertGEOSPoint2OGRPoint(Point *point)
	{
		OGRPoint *outPoint = new OGRPoint();
		const Coordinate *coord = point->getCoordinate();
		outPoint->setX(coord->x);
		outPoint->setY(coord->y);
		outPoint->setZ(coord->z);
		return outPoint;
	}
	
	OGRPoint* RSGISGeomTestExport::convertGEOSCoordinate2OGRPoint(Coordinate *coord)
	{
		OGRPoint *outPoint = new OGRPoint();
		outPoint->setX(coord->x);
		outPoint->setY(coord->y);
		outPoint->setZ(coord->z);
		return outPoint;
	}

	bool RSGISGeomTestExport::checkDIR4SHP(string dir, string shp) throw(RSGISGeometryException)
	{
		RSGISFileUtils fileUtils;
		string *dirList = NULL;
		int numFiles = 0;
		bool returnVal = false;
		
		try
		{
			dirList = fileUtils.getFilesInDIRWithName(dir, shp, &numFiles);
			if(numFiles > 0)
			{
				for(int i = 0; i < numFiles; i++)
				{
					if(fileUtils.getExtension(dirList[i]) == ".shp")
					{
						returnVal = true;
					}
				}
			}
		}
		catch(RSGISException e)
		{
			cout << e.what() << endl;
			throw RSGISGeometryException(e.what());
		}
		delete[] dirList;
		
		return returnVal;
	}
	
	void RSGISGeomTestExport::deleteSHP(string dir, string shp) throw(RSGISGeometryException)
	{
		RSGISFileUtils fileUtils;
		string *dirList = NULL;
		int numFiles = 0;
		
		try
		{
			dirList = fileUtils.getFilesInDIRWithName(dir, shp, &numFiles);
			if(numFiles > 0)
			{
				cout << "Deleting shapefile...\n";
				for(int i = 0; i < numFiles; i++)
				{
					if(fileUtils.getExtension(dirList[i]) == ".shp")
					{
						cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".shx")
					{
						cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".sbx")
					{
						cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".sbn")
					{
						cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".dbf")
					{
						cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".prj")
					{
						cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						cout << " deleted\n";
					}
					
				}
			}
		}
		catch(RSGISException e)
		{
			throw RSGISGeometryException(e.what());
		}
		delete[] dirList;
	}
	
	
	void RSGISGeomTestExport::exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, list<Polygon*> *polys) throw(RSGISGeometryException)
	{
		OGRRegisterAll();
		RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		string SHPFileOutLayer = this->getLayerName(outputFile);
		string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
		if(this->checkDIR4SHP(outputDIR, SHPFileOutLayer))
		{
			if(deleteIfPresent)
			{
				this->deleteSHP(outputDIR, SHPFileOutLayer);
			}
			else
			{
				throw RSGISGeometryException("Shapefile already exists, either delete or select force.");
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
			throw RSGISGeometryException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
		if( outputSHPDS == NULL )
		{
			string message = string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
		if( outputSHPLayer == NULL )
		{
			string message = string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
		list<Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
		{
			featureOutput = OGRFeature::CreateFeature(outputDefn);
			featureOutput->SetGeometryDirectly(this->convertGEOSPolygon2OGRPolygon((*iterPolys)));
			
			if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
			{
				throw RSGISGeometryException("Failed to write feature to the output shapefile.");
			}
			OGRFeature::DestroyFeature(featureOutput);
		}
		OGRDataSource::DestroyDataSource(outputSHPDS);
	}
	
	void RSGISGeomTestExport::exportGEOSPolygons2SHP(string outputFile, bool deleteIfPresent, vector<Polygon*> *polys) throw(RSGISGeometryException)
	{
		OGRRegisterAll();
		RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		string SHPFileOutLayer = this->getLayerName(outputFile);
		string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
		if(this->checkDIR4SHP(outputDIR, SHPFileOutLayer))
		{
			if(deleteIfPresent)
			{
				this->deleteSHP(outputDIR, SHPFileOutLayer);
			}
			else
			{
				throw RSGISGeometryException("Shapefile already exists, either delete or select force.");
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
			throw RSGISGeometryException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
		if( outputSHPDS == NULL )
		{
			string message = string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
		if( outputSHPLayer == NULL )
		{
			string message = string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
		vector<Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
		{
			featureOutput = OGRFeature::CreateFeature(outputDefn);
			featureOutput->SetGeometryDirectly(this->convertGEOSPolygon2OGRPolygon((*iterPolys)));
			
			if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
			{
				throw RSGISGeometryException("Failed to write feature to the output shapefile.");
			}
			OGRFeature::DestroyFeature(featureOutput);
		}
		OGRDataSource::DestroyDataSource(outputSHPDS);
	}
	
	void RSGISGeomTestExport::exportGEOSCoordinates2SHP(string outputFile, bool deleteIfPresent, vector<Coordinate*> *coords) throw(RSGISGeometryException)
	{
		OGRRegisterAll();
		RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		string SHPFileOutLayer = this->getLayerName(outputFile);
		string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
		if(this->checkDIR4SHP(outputDIR, SHPFileOutLayer))
		{
			if(deleteIfPresent)
			{
				this->deleteSHP(outputDIR, SHPFileOutLayer);
			}
			else
			{
				throw RSGISGeometryException("Shapefile already exists, either delete or select force.");
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
			throw RSGISGeometryException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
		if( outputSHPDS == NULL )
		{
			string message = string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPoint, NULL );
		if( outputSHPLayer == NULL )
		{
			string message = string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
		vector<Coordinate*>::iterator iterCoords;
		for(iterCoords = coords->begin(); iterCoords != coords->end(); iterCoords++)
		{
			featureOutput = OGRFeature::CreateFeature(outputDefn);
			featureOutput->SetGeometryDirectly(this->convertGEOSCoordinate2OGRPoint((*iterCoords)));
			
			if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
			{
				throw RSGISGeometryException("Failed to write feature to the output shapefile.");
			}
			OGRFeature::DestroyFeature(featureOutput);
		}
		OGRDataSource::DestroyDataSource(outputSHPDS);
	}
	
	void RSGISGeomTestExport::exportGEOSLineStrings2SHP(string outputFile, bool deleteIfPresent, vector<LineString*> *lines) throw(RSGISGeometryException)
	{
		OGRRegisterAll();
		RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		string SHPFileOutLayer = this->getLayerName(outputFile);
		string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
		if(this->checkDIR4SHP(outputDIR, SHPFileOutLayer))
		{
			if(deleteIfPresent)
			{
				this->deleteSHP(outputDIR, SHPFileOutLayer);
			}
			else
			{
				throw RSGISGeometryException("Shapefile already exists, either delete or select force.");
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
			throw RSGISGeometryException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
		if( outputSHPDS == NULL )
		{
			string message = string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbLineString, NULL );
		if( outputSHPLayer == NULL )
		{
			string message = string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
		vector<LineString*>::iterator iterlines;
		for(iterlines = lines->begin(); iterlines != lines->end(); iterlines++)
		{
			//cout << "GML: " << (*iterlines)->toString() << endl;
			featureOutput = OGRFeature::CreateFeature(outputDefn);
			featureOutput->SetGeometryDirectly(this->convertGEOSLineString2OGRLineString((*iterlines)));
			
			if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
			{
				throw RSGISGeometryException("Failed to write feature to the output shapefile.");
			}
			OGRFeature::DestroyFeature(featureOutput);
		}
		OGRDataSource::DestroyDataSource(outputSHPDS);
	}
	
	void RSGISGeomTestExport::exportGEOSLineSegments2SHP(string outputFile, bool deleteIfPresent, vector<LineSegment*> *lines) throw(RSGISGeometryException)
	{
		OGRRegisterAll();
		RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		string SHPFileOutLayer = this->getLayerName(outputFile);
		string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
		if(this->checkDIR4SHP(outputDIR, SHPFileOutLayer))
		{
			if(deleteIfPresent)
			{
				this->deleteSHP(outputDIR, SHPFileOutLayer);
			}
			else
			{
				throw RSGISGeometryException("Shapefile already exists, either delete or select force.");
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
			throw RSGISGeometryException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->CreateDataSource(outputFile.c_str(), NULL);
		if( outputSHPDS == NULL )
		{
			string message = string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbLineString, NULL );
		if( outputSHPLayer == NULL )
		{
			string message = string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
		vector<LineSegment*>::iterator iterlines;
		for(iterlines = lines->begin(); iterlines != lines->end(); iterlines++)
		{
			//cout << "GML: " << (*iterlines)->toString() << endl;
			featureOutput = OGRFeature::CreateFeature(outputDefn);
			featureOutput->SetGeometryDirectly(this->convertGEOSLineSegment2OGRLineString((*iterlines)));
			
			if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
			{
				throw RSGISGeometryException("Failed to write feature to the output shapefile.");
			}
			OGRFeature::DestroyFeature(featureOutput);
		}
		OGRDataSource::DestroyDataSource(outputSHPDS);
	}
	
	RSGISGeomTestExport::~RSGISGeomTestExport()
	{
		
	}
}}


