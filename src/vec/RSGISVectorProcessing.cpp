/*
 *  RSGISVectorProcessing.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/01/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#include "RSGISVectorProcessing.h"


namespace rsgis{namespace vec{
	
	RSGISVectorProcessing::RSGISVectorProcessing()
	{
		
	}
	
	Polygon* RSGISVectorProcessing::bufferPolygon(Polygon *polygon, double distance)
	{
		return dynamic_cast<Polygon*>(polygon->buffer(distance));
	}
	
	void RSGISVectorProcessing::generateCircles(RSGISCirclePoint **points, RSGISCirclePolygon **circles, int numFeatures, float resolution)
	{
		RSGISGeometry geomUtils;
		
		for(int i = 0; i < numFeatures; i++)
		{
			circles[i]->setPolygon(geomUtils.createCircle(points[i]->getPoint()->getX(), points[i]->getPoint()->getY(), points[i]->getRadius(), resolution));
			circles[i]->setArea(points[i]->getArea());
			circles[i]->setHeight(points[i]->getHeight());
			circles[i]->setRadius(points[i]->getRadius());
		}
	}
	
	void RSGISVectorProcessing::listAttributes(OGRLayer *inputSHPLayer)
	{
		OGRFeatureDefn *layerDef = inputSHPLayer->GetLayerDefn();
		OGRFieldDefn *field = NULL;
		
		int fieldCount = layerDef->GetFieldCount();
		
		cout << "Shapefile Fields (" << fieldCount << ")\n";
		cout << "----------------------\n";
		
		for(int i = 0; i < fieldCount; i++)
		{
			field = layerDef->GetFieldDefn(i);
			cout << i << ":\t\'" << field->GetNameRef() << "\' - " << field->GetFieldTypeName(field->GetType()) << endl;
		}
	}
	
	void RSGISVectorProcessing::printAttribute(OGRLayer *inputLayer, string attribute)
	{
		OGRFeature *feature = NULL;
		const char* fieldValue = NULL;
		
		OGRFeatureDefn *layerDef = inputLayer->GetLayerDefn();
		int fieldIdx = layerDef->GetFieldIndex(attribute.c_str());
		if(fieldIdx < 0)
		{
			string message = "This layer does not contain a field with the name \'" + attribute + "\'";
			throw RSGISVectorException(message.c_str());
		}
		cout << "Field index = " << fieldIdx << endl;
		
		int i = 0;
		
		inputLayer->ResetReading();
		while( (feature = inputLayer->GetNextFeature()) != NULL )
		{
			fieldValue = feature->GetFieldAsString(fieldIdx);
			cout << i << ": " << fieldValue << endl;
			i++; 
		}
	}
	
	void RSGISVectorProcessing::splitFeatures(OGRLayer *inputLayer, string outputBase, bool force)
	{
		string outputFileName = "";
		
		RSGISFileUtils fileUtils;
		RSGISVectorUtils vecUtils;
		RSGISMathsUtils mathsUtils;
		
		OGRFeature *inFeature = NULL;
		OGRFeatureDefn *inFeatureDefn = NULL;
		OGRSFDriver *shpFiledriver = NULL;
		OGRDataSource *outputSHPDS = NULL;
		OGRLayer *outputSHPLayer = NULL;
		OGRSpatialReference* inputSpatialRef = NULL;
		
		long fid = 0;
		string outputDIR = "";
		string outputVector = "";
		string SHPFileOutLayer = "";
		try
		{
			inFeatureDefn = inputLayer->GetLayerDefn();
			inputSpatialRef = inputLayer->GetSpatialRef();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			int i = 0;
			cout << "Started, " << numFeatures << " features to process.\n";
			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if(numFeatures >= 10 && (i % feedback) == 0)
				{
					cout << feedbackCounter << "% Done" << endl;
					
					feedbackCounter = feedbackCounter + 10;
				}
				
				fid = inFeature->GetFID();
				outputVector = outputBase + "_" + mathsUtils.longtostring(fid) + ".shp";
				
				SHPFileOutLayer = vecUtils.getLayerName(outputVector);
				
				/////////////////////////////////////
				//
				// Check whether Output Shapfile Exists.
				//
				/////////////////////////////////////
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
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->CreateDataSource(outputVector.c_str(), NULL);
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}
				
				if( outputSHPLayer->CreateFeature(inFeature) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}				
				
				OGRFeature::DestroyFeature(inFeature);
				OGRDataSource::DestroyDataSource(outputSHPDS);
				i++;
			}
			cout << " Complete.\n";
			
		}
		catch(RSGISVectorOutputException& e)
		{
			throw RSGISVectorException(e.what());
		}
		catch(RSGISVectorException& e)
		{
			throw e;
		}
	}
	
	void RSGISVectorProcessing::createPlotPolygons(vector<PlotPoly*> *polyDetails, string output, bool force) throw(RSGISVectorException)
	{
		RSGISMathsUtils mathUtils;
		RSGISPlotPolygonsCSVParse parse;
		
		double diffXC = 0;
		double diffYC = 0;
		double diffXR = 0;
		double diffYR = 0;
		double diffXL = 0;
		double diffYL = 0;
		double radOrien = 0;
		double radOrienP90 = 0;
		double radOrienM90 = 0;
		double halfEastSide = 0;
		
		double topCentreX = 0;
		double topCentreY = 0;
		
		GeometryFactory *geomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		LinearRing *ring = NULL;
		
		vector<Polygon*> *polys = new vector<Polygon*>();
		
		CoordinateSequence *coords = NULL;
		
		vector<PlotPoly*>::iterator iterPolyDetails;
		for(iterPolyDetails = polyDetails->begin(); iterPolyDetails != polyDetails->end(); ++iterPolyDetails)
		{
			//cout << parse.formatedString(*iterPolyDetails) << endl;
			
			radOrien = mathUtils.degreesToRadians((*iterPolyDetails)->orientation);
			radOrienP90 = mathUtils.degreesToRadians(((*iterPolyDetails)->orientation + 90));
			radOrienM90 = mathUtils.degreesToRadians(((*iterPolyDetails)->orientation - 90));
			
			halfEastSide = (*iterPolyDetails)->eastSide / 2;
			
			diffXC = (*iterPolyDetails)->northSide * sin(radOrien);
			diffYC = (*iterPolyDetails)->northSide * cos(radOrien);
			
			diffXR = halfEastSide * sin(radOrienP90);
			diffYR = halfEastSide * cos(radOrienP90);
			
			diffXL = halfEastSide * sin(radOrienM90);
			diffYL = halfEastSide * cos(radOrienM90);
			
			topCentreX = (*iterPolyDetails)->eastings + diffXC;
			topCentreY = (*iterPolyDetails)->northings + diffYC;
			
			coords = new CoordinateArraySequence();
			
			coords->add(Coordinate(((*iterPolyDetails)->eastings + diffXR), ((*iterPolyDetails)->northings + diffYR)));
			coords->add(Coordinate((topCentreX + diffXR),(topCentreY + diffYR)));
			coords->add(Coordinate((topCentreX + diffXL),(topCentreY + diffYL)));
			coords->add(Coordinate(((*iterPolyDetails)->eastings + diffXL), ((*iterPolyDetails)->northings + diffYL)));
			coords->add(Coordinate(((*iterPolyDetails)->eastings + diffXR), ((*iterPolyDetails)->northings + diffYR)));
			
			ring = geomFactory->createLinearRing(coords);
			
			polys->push_back(geomFactory->createPolygon(ring, NULL));
		}	
		
		RSGISVectorIO vecIO;
		vecIO.exportGEOSPolygons2SHP(output, force, polys, polyDetails);
		
		vector<Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); )
		{
			geomFactory->destroyGeometry(*iterPolys);
			polys->erase(iterPolys);
		}
		delete polys;
		
		delete RSGISGEOSFactoryGenerator::getInstance();
	}
	
	void RSGISVectorProcessing::createImageFootprintPolygons(vector<ImageFootPrintPoly*> *polyDetails, string output, bool force) throw(RSGISVectorException)
	{
		RSGISMathsUtils mathUtils;
		
		GeometryFactory *geomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		LinearRing *ring = NULL;
		
		vector<Polygon*> *polys = new vector<Polygon*>();
		
		CoordinateSequence *coords = NULL;
		
		vector<ImageFootPrintPoly*>::iterator iterPolyDetails;
		for(iterPolyDetails = polyDetails->begin(); iterPolyDetails != polyDetails->end(); ++iterPolyDetails)
		{
			
			coords = new CoordinateArraySequence();
			
			coords->add(Coordinate(((*iterPolyDetails)->ulE),((*iterPolyDetails)->ulN)));
			coords->add(Coordinate(((*iterPolyDetails)->urE),((*iterPolyDetails)->urN)));
			coords->add(Coordinate(((*iterPolyDetails)->lrE),((*iterPolyDetails)->lrN)));
			coords->add(Coordinate(((*iterPolyDetails)->llE),((*iterPolyDetails)->llN)));
			coords->add(Coordinate(((*iterPolyDetails)->ulE),((*iterPolyDetails)->ulN)));
			
			ring = geomFactory->createLinearRing(coords);
			
			polys->push_back(geomFactory->createPolygon(ring, NULL));
		}	
		
		RSGISVectorIO vecIO;
		vecIO.exportGEOSPolygons2SHP(output, force, polys, polyDetails);
		
		vector<Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); )
		{
			geomFactory->destroyGeometry(*iterPolys);
			polys->erase(iterPolys);
		}
		delete polys;
		
		delete RSGISGEOSFactoryGenerator::getInstance();
	}
	
	void RSGISVectorProcessing::createGrid(string outputShapefile, OGRSpatialReference* spatialRef, bool deleteIfPresent, double xTLStart, double yTLStart, double resolutionX, double resolutionY, double width, double height) throw(RSGISVectorException)
	{
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		vector<Polygon*> *polys = new vector<Polygon*>();
		
		unsigned int numXCells = width/resolutionX;
		unsigned int numYCells = height/resolutionY;
		
		double tlX = 0;
		double tlY = yTLStart;
		double brX = 0;
		double brY = yTLStart - resolutionY;
		
		for(unsigned int i = 0; i < numYCells; ++i)
		{
			tlX = xTLStart;
			brX = xTLStart + resolutionX;
			
			for(unsigned int j = 0; j < numXCells; ++j)
			{
				vector<Coordinate> *coords = new vector<Coordinate>();
				coords->push_back(Coordinate(tlX, tlY, 0));
				coords->push_back(Coordinate(brX, tlY, 0));
				coords->push_back(Coordinate(brX, brY, 0));
				coords->push_back(Coordinate(tlX, brY, 0));
				coords->push_back(Coordinate(tlX, tlY, 0));
				
				CoordinateArraySequence *coordSeq = new CoordinateArraySequence(coords);
				LinearRing *linearRingShell = new LinearRing(coordSeq, geosGeomFactory);
				
				polys->push_back(geosGeomFactory->createPolygon(linearRingShell, NULL));
				
				tlX += resolutionX;
				brX += resolutionX;
			}
			
			tlY -= resolutionY;
			brY -= resolutionY;
		}
		
		
		RSGISVectorIO vecIO;
		vecIO.exportGEOSPolygons2SHP(outputShapefile, deleteIfPresent, polys, spatialRef);
		
		vector<Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); )
		{
			geosGeomFactory->destroyGeometry(*iterPolys);
			polys->erase(iterPolys);
		}
		delete polys;
		
		
		delete RSGISGEOSFactoryGenerator::getInstance();
	}
    
    float RSGISVectorProcessing::calcMeanMinDistance(vector<OGRGeometry*> *geometries) throw(RSGISVectorException)
    {
        float meanMinDistance = 0;
        try
        {
            if(geometries->size() > 0)
            {
                double minDistanceSum = 0;
                unsigned int countVals = 0;
                double minDist = 0;
                double distance = 0;
                bool firstGeom = true;
                for(vector<OGRGeometry*>::iterator iterGeomsOuter = geometries->begin(); iterGeomsOuter != geometries->end(); ++iterGeomsOuter)
                {
                    minDist = 0;
                    firstGeom = true;
                    for(vector<OGRGeometry*>::iterator iterGeomsInner = geometries->begin(); iterGeomsInner != geometries->end(); ++iterGeomsInner)
                    {
                        if(!(*iterGeomsOuter)->Equals(*iterGeomsInner))
                        {
                            distance = (*iterGeomsOuter)->Distance(*iterGeomsInner);
                            if(firstGeom)
                            {
                                minDist = distance;
                                firstGeom = false;
                            }
                            else if(distance < minDist)
                            {
                                minDist = distance;
                            }
                        }
                    }
                    
                    if(!firstGeom)
                    {
                        minDistanceSum += minDist;
                        ++countVals;
                    }
                }
                
                meanMinDistance = minDistanceSum/countVals;
            }
        }
        catch(RSGISVectorException &e)
        {
            throw e;
        }
        
        return meanMinDistance;
    }
	
	RSGISVectorProcessing::~RSGISVectorProcessing()
	{
		
	}
}}


