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
	
	
	std::string RSGISGeomTestExport::getLayerName(std::string filepath)
	{
		int strSize = filepath.size();
		int lastSlash = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filepath.at(i) == '/')
			{
				lastSlash = i;
			}
		}
		std::string filename = filepath.substr(lastSlash+1);
		
		strSize = filename.size();
		int lastpt = 0;
		for(int i = 0; i < strSize; i++)
		{
			if(filename.at(i) == '.')
			{
				lastpt = i;
			}
		}
		
		std::string layerName = filename.substr(0, lastpt);
		return layerName;
	}
	
	OGRLineString* RSGISGeomTestExport::convertGEOSLineSegment2OGRLineString(geos::geom::LineSegment *line)
	{
		OGRLineString *OGRLine = new OGRLineString();
		OGRLine->addPoint(line->p0.x, line->p0.y, line->p0.z);
		OGRLine->addPoint(line->p1.x, line->p1.y, line->p1.z);
		return OGRLine;
	}
	
	OGRLineString* RSGISGeomTestExport::convertGEOSLineString2OGRLineString(geos::geom::LineString *line)
	{
		OGRLineString *OGRLine = new OGRLineString();
		
		geos::geom::CoordinateSequence *coordSeq = line->getCoordinates();
		geos::geom::Coordinate coord;
		int numCoords = coordSeq->getSize();
		for(int i = 0; i < numCoords; i++)
		{
			coord = coordSeq->getAt(i);
			OGRLine->addPoint(coord.x, coord.y, coord.z);
		}
		
		return OGRLine;
	}
	
	OGRLinearRing* RSGISGeomTestExport::convertGEOSLineString2OGRLinearRing(geos::geom::LineString *line)
	{
		OGRLinearRing *ring = new OGRLinearRing();
		const geos::geom::CoordinateSequence *coords = line->getCoordinatesRO();
		int numCoords = coords->getSize();
		geos::geom::Coordinate coord;

        for(int i = 0; i < numCoords; i++)
		{
			coord = coords->getAt(i);
			ring->addPoint(coord.x, coord.y, coord.z);
		}
		return ring;
	}
	
	OGRPolygon* RSGISGeomTestExport::convertGEOSPolygon2OGRPolygon(geos::geom::Polygon *poly)
	{
		OGRPolygon *ogrPoly = new OGRPolygon();
		
		// Add outer ring!
		const geos::geom::LineString *exteriorRing = poly->getExteriorRing();
		OGRLinearRing *ogrRing = this->convertGEOSLineString2OGRLinearRing(const_cast<geos::geom::LineString *>(exteriorRing));
		ogrPoly->addRing(ogrRing);
		delete ogrRing;
		
		int numInternalRings = poly->getNumInteriorRing();
		
		geos::geom::LineString *innerRing = NULL;
		
		for(int i = 0; i < numInternalRings; i++)
		{
			innerRing = const_cast<geos::geom::LineString *>(poly->getInteriorRingN(i));
			ogrRing = this->convertGEOSLineString2OGRLinearRing(innerRing);
			ogrPoly->addRing(ogrRing);
			delete ogrRing;
		}
		
		return ogrPoly;	}
	
	OGRPoint* RSGISGeomTestExport::convertGEOSPoint2OGRPoint(geos::geom::Point *point)
	{
		OGRPoint *outPoint = new OGRPoint();
		const geos::geom::Coordinate *coord = point->getCoordinate();
		outPoint->setX(coord->x);
		outPoint->setY(coord->y);
		outPoint->setZ(coord->z);
		return outPoint;
	}
	
	OGRPoint* RSGISGeomTestExport::convertGEOSCoordinate2OGRPoint(geos::geom::Coordinate *coord)
	{
		OGRPoint *outPoint = new OGRPoint();
		outPoint->setX(coord->x);
		outPoint->setY(coord->y);
		outPoint->setZ(coord->z);
		return outPoint;
	}

	bool RSGISGeomTestExport::checkDIR4SHP(std::string dir, std::string shp)
	{
        rsgis::utils::RSGISFileUtils fileUtils;
		std::string *dirList = NULL;
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
		catch(rsgis::RSGISException &e)
		{
			std::cout << e.what() << std::endl;
			throw RSGISGeometryException(e.what());
		}
		delete[] dirList;
		
		return returnVal;
	}
	
	void RSGISGeomTestExport::deleteSHP(std::string dir, std::string shp)
	{
		rsgis::utils::RSGISFileUtils fileUtils;
		std::string *dirList = NULL;
		int numFiles = 0;
		
		try
		{
			dirList = fileUtils.getFilesInDIRWithName(dir, shp, &numFiles);
			if(numFiles > 0)
			{
				std::cout << "Deleting shapefile...\n";
				for(int i = 0; i < numFiles; i++)
				{
					if(fileUtils.getExtension(dirList[i]) == ".shp")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw rsgis::RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".shx")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw rsgis::RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".sbx")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw rsgis::RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".sbn")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw rsgis::RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".dbf")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw rsgis::RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".prj")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw rsgis::RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					
				}
			}
		}
		catch(rsgis::RSGISException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		delete[] dirList;
	}
	
	
	void RSGISGeomTestExport::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::list<geos::geom::Polygon*> *polys)
	{
		OGRRegisterAll();
		rsgis::utils::RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		std::string SHPFileOutLayer = this->getLayerName(outputFile);
		std::string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
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
			throw RSGISGeometryException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->Create(outputFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
		if( outputSHPDS == NULL )
		{
			std::string message = std::string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
		if( outputSHPLayer == NULL )
		{
			std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
        std::list<geos::geom::Polygon*>::iterator iterPolys;
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
		GDALClose(outputSHPDS);
	}
	
	void RSGISGeomTestExport::exportGEOSPolygons2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Polygon*> *polys)
	{
		OGRRegisterAll();
        rsgis::utils::RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		std::string SHPFileOutLayer = this->getLayerName(outputFile);
		std::string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
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
			throw RSGISGeometryException("SHP driver not available.");
		}
        outputSHPDS = shpFiledriver->Create(outputFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
		if( outputSHPDS == NULL )
		{
			std::string message = std::string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPolygon, NULL );
		if( outputSHPLayer == NULL )
		{
			std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
        std::vector<geos::geom::Polygon*>::iterator iterPolys;
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
		GDALClose(outputSHPDS);
	}
	
	void RSGISGeomTestExport::exportGEOSCoordinates2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::Coordinate*> *coords)
	{
		OGRRegisterAll();
        rsgis::utils::RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		std::string SHPFileOutLayer = this->getLayerName(outputFile);
		std::string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
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
			throw RSGISGeometryException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->Create(outputFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
		if( outputSHPDS == NULL )
		{
			std::string message = std::string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbPoint, NULL );
		if( outputSHPLayer == NULL )
		{
			std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
        std::vector<geos::geom::Coordinate*>::iterator iterCoords;
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
		GDALClose(outputSHPDS);
	}
	
	void RSGISGeomTestExport::exportGEOSLineStrings2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::LineString*> *lines)
	{
		OGRRegisterAll();
        rsgis::utils::RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		std::string SHPFileOutLayer = this->getLayerName(outputFile);
		std::string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
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
			throw RSGISGeometryException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->Create(outputFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
        
		if( outputSHPDS == NULL )
		{
			std::string message = std::string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbLineString, NULL );
		if( outputSHPLayer == NULL )
		{
			std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
        std::vector<geos::geom::LineString*>::iterator iterlines;
		for(iterlines = lines->begin(); iterlines != lines->end(); iterlines++)
		{
			featureOutput = OGRFeature::CreateFeature(outputDefn);
			featureOutput->SetGeometryDirectly(this->convertGEOSLineString2OGRLineString((*iterlines)));
			
			if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
			{
				throw RSGISGeometryException("Failed to write feature to the output shapefile.");
			}
			OGRFeature::DestroyFeature(featureOutput);
		}
		GDALClose(outputSHPDS);
	}
	
	void RSGISGeomTestExport::exportGEOSLineSegments2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::LineSegment*> *lines)
	{
		OGRRegisterAll();
        rsgis::utils::RSGISFileUtils fileUtils;
		
		/////////////////////////////////////
		//
		// Check whether file already present.
		//
		/////////////////////////////////////
		std::string SHPFileOutLayer = this->getLayerName(outputFile);
		std::string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
		
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
			throw RSGISGeometryException("SHP driver not available.");
		}
		outputSHPDS = shpFiledriver->Create(outputFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
		if( outputSHPDS == NULL )
		{
			std::string message = std::string("Could not create vector file ") + outputFile;
			throw RSGISGeometryException(message.c_str());
		}
		
		outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbLineString, NULL );
		if( outputSHPLayer == NULL )
		{
			std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
			throw RSGISGeometryException(message.c_str());
		}
		
		OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
		OGRFeature *featureOutput = NULL;
		
		// Write Polygons to file
        std::vector<geos::geom::LineSegment*>::iterator iterlines;
		for(iterlines = lines->begin(); iterlines != lines->end(); iterlines++)
		{
			featureOutput = OGRFeature::CreateFeature(outputDefn);
			featureOutput->SetGeometryDirectly(this->convertGEOSLineSegment2OGRLineString((*iterlines)));
			
			if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
			{
				throw RSGISGeometryException("Failed to write feature to the output shapefile.");
			}
			OGRFeature::DestroyFeature(featureOutput);
		}
		GDALClose(outputSHPDS);
	}
    
    void RSGISGeomTestExport::exportGEOSLineSegments2SHP(std::string outputFile, bool deleteIfPresent, std::vector<geos::geom::LineSegment*> *lines, std::vector<double> *vals)
    {
        if(lines->size() != vals->size())
        {
            throw RSGISGeometryException("The number of values and number of lines is not equal.");
        }
        
        OGRRegisterAll();
        rsgis::utils::RSGISFileUtils fileUtils;
        
        /////////////////////////////////////
        //
        // Check whether file already present.
        //
        /////////////////////////////////////
        std::string SHPFileOutLayer = this->getLayerName(outputFile);
        std::string outputDIR = fileUtils.getFileDirectoryPath(outputFile);
        
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
            throw RSGISGeometryException("SHP driver not available.");
        }
        outputSHPDS = (GDALDataset*) shpFiledriver->Create(outputFile.c_str(), 0, 0, 0, GDT_Unknown, NULL );
        if( outputSHPDS == NULL )
        {
            std::string message = std::string("Could not create vector file ") + outputFile;
            throw RSGISGeometryException(message.c_str());
        }
        
        outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), NULL, wkbLineString, NULL );
        if( outputSHPLayer == NULL )
        {
            std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
            throw RSGISGeometryException(message.c_str());
        }
        
        OGRFieldDefn shpField("value", OFTReal);
        shpField.SetPrecision(2);
        if(outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
        {
            throw RSGISGeometryException("Creating shapefile field value has failed");
        }
        
        OGRFeatureDefn *outputDefn = outputSHPLayer->GetLayerDefn();
        OGRFeature *featureOutput = NULL;
        
        // Write Polygons to file
        std::vector<geos::geom::LineSegment*>::iterator iterlines;
        unsigned int i = 0;
        for(iterlines = lines->begin(); iterlines != lines->end(); iterlines++)
        {
            featureOutput = OGRFeature::CreateFeature(outputDefn);
            featureOutput->SetField(outputDefn->GetFieldIndex("value"), vals->at(i));
            featureOutput->SetGeometryDirectly(this->convertGEOSLineSegment2OGRLineString((*iterlines)));
            
            if( outputSHPLayer->CreateFeature(featureOutput) != OGRERR_NONE )
            {
                throw RSGISGeometryException("Failed to write feature to the output shapefile.");
            }
            OGRFeature::DestroyFeature(featureOutput);
            ++i;
        }
        GDALClose(outputSHPDS);
    }
	
	RSGISGeomTestExport::~RSGISGeomTestExport()
	{
		
	}
}}


