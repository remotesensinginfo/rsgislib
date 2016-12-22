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
	
    geos::geom::Polygon* RSGISVectorProcessing::bufferPolygon(geos::geom::Polygon *polygon, double distance)
	{
		return dynamic_cast<geos::geom::Polygon*>(polygon->buffer(distance));
	}
	
	void RSGISVectorProcessing::generateCircles(RSGISCirclePoint **points, RSGISCirclePolygon **circles, int numFeatures, float resolution)
	{
        rsgis::geom::RSGISGeometry geomUtils;
		
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
		
		std::cout << "Shapefile Fields (" << fieldCount << ")\n";
		std::cout << "----------------------\n";
		
		for(int i = 0; i < fieldCount; i++)
		{
			field = layerDef->GetFieldDefn(i);
			std::cout << i << ":\t\'" << field->GetNameRef() << "\' - " << field->GetFieldTypeName(field->GetType()) << std::endl;
		}
	}
	
	void RSGISVectorProcessing::printAttribute(OGRLayer *inputLayer, std::string attribute)
	{
		OGRFeature *feature = NULL;
		const char* fieldValue = NULL;
		
		OGRFeatureDefn *layerDef = inputLayer->GetLayerDefn();
		int fieldIdx = layerDef->GetFieldIndex(attribute.c_str());
		if(fieldIdx < 0)
		{
			std::string message = "This layer does not contain a field with the name \'" + attribute + "\'";
			throw RSGISVectorException(message.c_str());
		}
		std::cout << "Field index = " << fieldIdx << std::endl;
		
		int i = 0;
		
		inputLayer->ResetReading();
		while( (feature = inputLayer->GetNextFeature()) != NULL )
		{
			fieldValue = feature->GetFieldAsString(fieldIdx);
			std::cout << i << ": " << fieldValue << std::endl;
			i++; 
		}
	}
	
	void RSGISVectorProcessing::splitFeatures(OGRLayer *inputLayer, std::string outputBase, bool force)
	{
		std::string outputFileName = "";
		
        rsgis::utils::RSGISFileUtils fileUtils;
		RSGISVectorUtils vecUtils;
        rsgis::math::RSGISMathsUtils mathsUtils;
		
		OGRFeature *inFeature = NULL;
		OGRFeatureDefn *inFeatureDefn = NULL;
		GDALDriver *shpFiledriver = NULL;
		GDALDataset *outputSHPDS = NULL;
		OGRLayer *outputSHPLayer = NULL;
		OGRSpatialReference* inputSpatialRef = NULL;
		
		long fid = 0;
		std::string outputDIR = "";
		std::string outputVector = "";
		std::string SHPFileOutLayer = "";
		try
		{
			inFeatureDefn = inputLayer->GetLayerDefn();
			inputSpatialRef = inputLayer->GetSpatialRef();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			int i = 0;
			std::cout << "Started, " << numFeatures << " features to process.\n";
			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if(numFeatures >= 10 && (i % feedback) == 0)
				{
					std::cout << feedbackCounter << "% Done" << std::endl;
					
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
				shpFiledriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->Create(outputVector.c_str(), 0, 0, 0, GDT_Unknown, NULL );
				if( outputSHPDS == NULL )
				{
					std::string message = std::string("Could not create vector file ") + outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
				if( outputSHPLayer == NULL )
				{
					std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}
				
				if( outputSHPLayer->CreateFeature(inFeature) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}				
				
				OGRFeature::DestroyFeature(inFeature);
				GDALClose(outputSHPDS);
				i++;
			}
			std::cout << " Complete.\n";
			
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
	
	void RSGISVectorProcessing::createPlotPolygons(std::vector<rsgis::utils::PlotPoly*> *polyDetails, std::string output, bool force) throw(RSGISVectorException)
	{
        rsgis::math::RSGISMathsUtils mathUtils;
        rsgis::utils::RSGISPlotPolygonsCSVParse parse;
		
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
		
        geos::geom::GeometryFactory *geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		geos::geom::LinearRing *ring = NULL;
		
        std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
		
		geos::geom::CoordinateSequence *coords = NULL;
		
		std::vector<rsgis::utils::PlotPoly*>::iterator iterPolyDetails;
		for(iterPolyDetails = polyDetails->begin(); iterPolyDetails != polyDetails->end(); ++iterPolyDetails)
		{			
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
			
			coords = new geos::geom::CoordinateArraySequence();
			
			coords->add(geos::geom::Coordinate(((*iterPolyDetails)->eastings + diffXR), ((*iterPolyDetails)->northings + diffYR)));
			coords->add(geos::geom::Coordinate((topCentreX + diffXR),(topCentreY + diffYR)));
			coords->add(geos::geom::Coordinate((topCentreX + diffXL),(topCentreY + diffYL)));
			coords->add(geos::geom::Coordinate(((*iterPolyDetails)->eastings + diffXL), ((*iterPolyDetails)->northings + diffYL)));
			coords->add(geos::geom::Coordinate(((*iterPolyDetails)->eastings + diffXR), ((*iterPolyDetails)->northings + diffYR)));
			
			ring = geomFactory->createLinearRing(coords);
			
			polys->push_back(geomFactory->createPolygon(ring, NULL));
		}	
		
		RSGISVectorIO vecIO;
		vecIO.exportGEOSPolygons2SHP(output, force, polys, polyDetails);
		
        std::vector<geos::geom::Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); )
		{
			geomFactory->destroyGeometry(*iterPolys);
			polys->erase(iterPolys);
		}
		delete polys;
		
		delete rsgis::utils::RSGISGEOSFactoryGenerator::getInstance();
	}
	
	void RSGISVectorProcessing::createImageFootprintPolygons(std::vector<rsgis::utils::ImageFootPrintPoly*> *polyDetails, std::string output, bool force) throw(RSGISVectorException)
	{
        rsgis::math::RSGISMathsUtils mathUtils;
		
		geos::geom::GeometryFactory *geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		geos::geom::LinearRing *ring = NULL;
		
        std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
		
		geos::geom::CoordinateSequence *coords = NULL;
		
        std::vector<rsgis::utils::ImageFootPrintPoly*>::iterator iterPolyDetails;
		for(iterPolyDetails = polyDetails->begin(); iterPolyDetails != polyDetails->end(); ++iterPolyDetails)
		{
			
			coords = new geos::geom::CoordinateArraySequence();
			
			coords->add(geos::geom::Coordinate(((*iterPolyDetails)->ulE),((*iterPolyDetails)->ulN)));
			coords->add(geos::geom::Coordinate(((*iterPolyDetails)->urE),((*iterPolyDetails)->urN)));
			coords->add(geos::geom::Coordinate(((*iterPolyDetails)->lrE),((*iterPolyDetails)->lrN)));
			coords->add(geos::geom::Coordinate(((*iterPolyDetails)->llE),((*iterPolyDetails)->llN)));
			coords->add(geos::geom::Coordinate(((*iterPolyDetails)->ulE),((*iterPolyDetails)->ulN)));
			
			ring = geomFactory->createLinearRing(coords);
			
			polys->push_back(geomFactory->createPolygon(ring, NULL));
		}	
		
		RSGISVectorIO vecIO;
		vecIO.exportGEOSPolygons2SHP(output, force, polys, polyDetails);
		
        std::vector<geos::geom::Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); )
		{
			geomFactory->destroyGeometry(*iterPolys);
			polys->erase(iterPolys);
		}
		delete polys;
		
		delete rsgis::utils::RSGISGEOSFactoryGenerator::getInstance();
	}
	
	void RSGISVectorProcessing::createGrid(std::string outputShapefile, OGRSpatialReference* spatialRef, bool deleteIfPresent, double xTLStart, double yTLStart, double resolutionX, double resolutionY, double width, double height) throw(RSGISVectorException)
	{
		geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
        std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
		
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
                std::vector<geos::geom::Coordinate> *coords = new std::vector<geos::geom::Coordinate>();
				coords->push_back(geos::geom::Coordinate(tlX, tlY, 0));
				coords->push_back(geos::geom::Coordinate(brX, tlY, 0));
				coords->push_back(geos::geom::Coordinate(brX, brY, 0));
				coords->push_back(geos::geom::Coordinate(tlX, brY, 0));
				coords->push_back(geos::geom::Coordinate(tlX, tlY, 0));
				
				geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence(coords);
				geos::geom::LinearRing *linearRingShell = new geos::geom::LinearRing(coordSeq, geosGeomFactory);
				
				polys->push_back(geosGeomFactory->createPolygon(linearRingShell, NULL));
				
				tlX += resolutionX;
				brX += resolutionX;
			}
			
			tlY -= resolutionY;
			brY -= resolutionY;
		}
		
		
		RSGISVectorIO vecIO;
		vecIO.exportGEOSPolygons2SHP(outputShapefile, deleteIfPresent, polys, spatialRef);
		
        std::vector<geos::geom::Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); )
		{
			geosGeomFactory->destroyGeometry(*iterPolys);
			polys->erase(iterPolys);
		}
		delete polys;
		
		
		delete rsgis::utils::RSGISGEOSFactoryGenerator::getInstance();
	}
    
    float RSGISVectorProcessing::calcMeanMinDistance(std::vector<OGRGeometry*> *geometries) throw(RSGISVectorException)
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
                for(std::vector<OGRGeometry*>::iterator iterGeomsOuter = geometries->begin(); iterGeomsOuter != geometries->end(); ++iterGeomsOuter)
                {
                    minDist = 0;
                    firstGeom = true;
                    for(std::vector<OGRGeometry*>::iterator iterGeomsInner = geometries->begin(); iterGeomsInner != geometries->end(); ++iterGeomsInner)
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


