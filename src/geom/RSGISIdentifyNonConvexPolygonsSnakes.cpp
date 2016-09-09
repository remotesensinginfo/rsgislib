/*
 *  RSGISIdentifyNonConvexPolygonsSnakes.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/08/2009.
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

#include "RSGISIdentifyNonConvexPolygonsSnakes.h"


namespace rsgis{namespace geom{

	RSGISIdentifyNonConvexPolygonsSnakes::RSGISIdentifyNonConvexPolygonsSnakes(double resolution, OGRSpatialReference* spatialRef, double alpha, double beta, double gamma, double delta, int maxNumIterations)
	{
		GDALAllRegister();
		this->gdalDriver = GetGDALDriverManager()->GetDriverByName("MEM");
        //this->gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
		this->resolution = resolution;
		this->spatialRef = spatialRef;
		this->alpha = alpha;
		this->beta = beta;
		this->gamma = gamma;
		this->delta = delta;
		this->maxNumIterations = maxNumIterations;
	}
			
	std::vector<geos::geom::Polygon*>* RSGISIdentifyNonConvexPolygonsSnakes::retrievePolygons(std::list<RSGIS2DPoint*> **clusters, int numClusters) throw(RSGISGeometryException)
	{
		throw RSGISGeometryException("Not Implemented");
		
		return NULL;
	}
	
	std::vector<geos::geom::Polygon*>* RSGISIdentifyNonConvexPolygonsSnakes::retrievePolygons(std::list<RSGISPolygon*> **clusters, int numClusters) throw(RSGISGeometryException)
	{
		std::vector<geos::geom::Polygon*> *outPolys = new std::vector<geos::geom::Polygon*>();
		
		try
		{
			std::list<RSGISPolygon*>::iterator iterPolys;
			std::list<geos::geom::Polygon*> *polys = new std::list<geos::geom::Polygon*>();
			
			for(int i = 0; i < numClusters; ++i)
			{
				std::cout << "\nPolygon " << i << std::endl;
				for(iterPolys = clusters[i]->begin(); iterPolys != clusters[i]->end(); ++iterPolys)
				{
					polys->push_back((*iterPolys)->getPolygon());
				}
				outPolys->push_back(this->retrievePolygon(polys));
				polys->clear();
				std::cout << "End Calc Polygon\n";
			}	
			delete polys;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return outPolys;
	}
	
	std::vector<geos::geom::Polygon*>* RSGISIdentifyNonConvexPolygonsSnakes::retrievePolygons(std::list<geos::geom::Polygon*> **clusters, int numClusters) throw(RSGISGeometryException)
	{
		std::vector<geos::geom::Polygon*> *outPolys = new std::vector<geos::geom::Polygon*>();
		
		try
		{			
			for(int i = 0; i < numClusters; ++i)
			{
				std::cout << "\nPolygon " << i << std::endl;
				outPolys->push_back(this->retrievePolygon(clusters[i]));
				std::cout << "End Calc Polygon\n";
			}			
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return outPolys;
	}
	
	geos::geom::Polygon* RSGISIdentifyNonConvexPolygonsSnakes::retrievePolygon(std::vector<geos::geom::Polygon*> *polygons) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		geos::geom::Polygon *outPoly = NULL;
		GDALDataset *imageDist = NULL;
		try
		{
			geos::geom::GeometryFactory * geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			geos::geom::GeometryCollection  *geom = this->createGeomCollection(polygons);
			
			if(gdalDriver == NULL)
			{
				throw RSGISGeometryException("MEM (Memory) driver does not exists..");
			}
			
			imageDist = this->createDataset(gdalDriver, geom, "", this->resolution, 0);
			std::cout << "Create distance image\n";
			this->createDistanceImage(imageDist, geom);
			geosGeomFactory->destroyGeometry(geom);
			
			std::cout << "Retrieve Polygons from image pixels\n";
			std::vector<geos::geom::Polygon*> *pxlPolys = new std::vector<geos::geom::Polygon*>();
			this->populatePixelPolygons(imageDist, resolution*2, pxlPolys);
			
			//RSGISGeomTestExport exportGeom;
			//exportGeom.exportGEOSPolygons2SHP("/Users/pete/Temp/pixelpolys.shp", true, pxlPolys);
			
			std::cout << "Merge touching Polygons.\n";
			geomUtils.mergeTouchingPolygonsWithIndex(pxlPolys);
			//exportGeom.exportGEOSPolygons2SHP("/Users/pete/Temp/pixelpoly_merged.shp", true, pxlPolys);
			
			int bufferIterations = 0;
			while(pxlPolys->size() > 1)
			{
                //std::cout << "Buffering " << bufferIterations << " pxlPolys->size() = " << pxlPolys->size() << std::endl;
				geomUtils.performMorphologicalOperation(pxlPolys, dilation, resolution, 6);
				geomUtils.mergeTouchingPolygonsForce(pxlPolys);
				++bufferIterations;
				if(bufferIterations > 7)
				{
					break;
				}
			}
			//exportGeom.exportGEOSPolygons2SHP("/Users/pete/Temp/pixelpoly_merged_single.shp", true, pxlPolys);
            
			geos::geom::Envelope *imgBoundary = new geos::geom::Envelope();
			int imgWidth = imageDist->GetRasterXSize();
			int imgHeight = imageDist->GetRasterYSize();			
			double *trans = new double[6];
			imageDist->GetGeoTransform(trans);
			
			double xMin = trans[0];
			double xMax = trans[0]+(imgWidth*resolution);
			double yMin = trans[3]-(imgHeight*resolution);
			double yMax = trans[3];
			imgBoundary->init(xMin, xMax, yMin, yMax);
			delete[] trans;
			
			std::vector<geos::geom::LineSegment*> *lines = NULL;
			geos::geom::CoordinateSequence *coords = NULL;
			geos::geom::Polygon *convexhull = NULL;
			geos::geom::Polygon *convexHullNodesAdded = NULL;
			geos::geom::Polygon *outlinePoly = NULL;
			std::vector<geos::geom::Coordinate*> *coordsVec = new std::vector<geos::geom::Coordinate*>();
			
			rsgis::math::RSGISGlobalOptimisationFunction *optimiseFunc = NULL;
			rsgis::math::RSGISGlobalOptimiser2D *optimise = NULL;
			
			if(pxlPolys->size() == 1)
			{
				geomUtils.performMorphologicalOperation(pxlPolys, closing, resolution*2, 6);
				outlinePoly = geomUtils.removeHoles(pxlPolys->at(0));
				coords = outlinePoly->getCoordinates();
				
				geos::geom::Coordinate coord;
				for(unsigned int i = 1; i < coords->size(); ++i)
				{
					coord = coords->getAt(i);
					coordsVec->push_back(new geos::geom::Coordinate(coord.x, coord.y, coord.z));
				}
				delete coords;
				
				geomUtils.removeNeighborDuplicates(coordsVec);
				
				optimiseFunc = new RSGISSnakeNonConvexGlobalOptimisationFunction(imageDist, this->alpha, this->beta, this->gamma);
				optimise = new rsgis::math::RSGISGlobalHillClimbingOptimiser2DVaryNumPts(optimiseFunc, false, maxNumIterations);
			}
			else
			{
				std::cout << "Reverting to Convex Hull as starting point.\n";
				convexhull = geomUtils.findConvexHull(polygons);
				convexHullNodesAdded = geomUtils.addNodes(convexhull, resolution);
				coords = convexHullNodesAdded->getCoordinates();
				
				geos::geom::Coordinate coord;
				for(unsigned int i = 1; i < coords->size(); ++i)
				{
					coord = coords->getAt(i);
					coordsVec->push_back(new geos::geom::Coordinate(coord.x, coord.y, coord.z));
				}
				delete coords;
				
				geomUtils.removeNeighborDuplicates(coordsVec);
				
				float lineLength = 0;
				if(imgBoundary->getWidth() > imgBoundary->getHeight())
				{
					lineLength = imgBoundary->getWidth();
				}
				else
				{
					lineLength = imgBoundary->getHeight();
				}
				
				lines = geomUtils.findLineProj(coordsVec, lineLength);
				
				if(lines->size() != coordsVec->size())
				{
					throw RSGISGeometryException("The number of coordinates and lines is not the same.");
				}
				
				optimiseFunc = new RSGISSnakeNonConvexLineProjGlobalOptimisationFunction(imageDist, this->alpha, this->beta, this->gamma, delta, lines);
				optimise = new rsgis::math::RSGISGlobalHillClimbingOptimiser2D(optimiseFunc, false, (maxNumIterations * 3));
			}
			std::cout << "CoordVec has " << coordsVec->size() << " nodes\n";
			
			std::cout << "Perform snakes based refinement of boundary\n";
			std::vector<geos::geom::Coordinate*> *coordsVecOp = optimise->optimise8Neighbor(coordsVec, resolution, imgBoundary);
				
			std::vector<geos::geom::Coordinate*>::iterator iterCoords;
			coords = new geos::geom::CoordinateArraySequence();
			geos::geom::Coordinate *ptrCoord;
			for(iterCoords = coordsVecOp->begin(); iterCoords != coordsVecOp->end(); ++iterCoords)
			{
				ptrCoord = *iterCoords;
				coords->add(geos::geom::Coordinate(ptrCoord->x, ptrCoord->y, ptrCoord->z));
			}
			ptrCoord = coordsVecOp->front();
			coords->add(geos::geom::Coordinate(ptrCoord->x, ptrCoord->y, ptrCoord->z));
						
			geos::geom::LinearRing *ring = geosGeomFactory->createLinearRing(coords);
			outPoly = geosGeomFactory->createPolygon(ring, NULL);
			
			delete optimise;
			delete optimiseFunc;
			
			if(convexhull != NULL)
			{
				delete convexhull;
			}
			
			if(convexHullNodesAdded != NULL)
			{
				delete convexHullNodesAdded;
			}
			
			if(outlinePoly != NULL)
			{
				delete outlinePoly;
			}
			
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			for(iterPolys = pxlPolys->begin(); iterPolys != pxlPolys->end(); )
			{
				delete *iterPolys;
				pxlPolys->erase(iterPolys);
			}
			delete pxlPolys;
			
			for(iterCoords = coordsVec->begin(); iterCoords != coordsVec->end(); )
			{
				delete *iterCoords;
				coordsVec->erase(iterCoords);
			}
			delete coordsVec;
			
			for(iterCoords = coordsVecOp->begin(); iterCoords != coordsVecOp->end(); )
			{
				//delete *iterCoords;
				coordsVecOp->erase(iterCoords);
			}
			delete coordsVecOp;
			
			if(lines != NULL)
			{
				std::vector<geos::geom::LineSegment*>::iterator iterLines;
				for(iterLines = lines->begin(); iterLines != lines->end(); )
				{
					delete *iterLines;
					lines->erase(iterLines);
				}
				delete lines;
			}
			
			delete imgBoundary;
			GDALClose(imageDist);
		}
		catch(geos::util::TopologyException &e)
		{
			GDALClose(imageDist);
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISException &e)
		{
			GDALClose(imageDist);
			throw RSGISGeometryException(e.what());
		}
		catch(geos::util::IllegalArgumentException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		
		return outPoly;
	}
	
	geos::geom::Polygon* RSGISIdentifyNonConvexPolygonsSnakes::retrievePolygon(std::list<geos::geom::Polygon*> *polygons) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		geos::geom::Polygon *outPoly = NULL;
		GDALDataset *imageDist = NULL;
		try
		{
			geos::geom::GeometryFactory * geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			geos::geom::GeometryCollection  *geom = this->createGeomCollection(polygons);
			
			if(gdalDriver == NULL)
			{
				throw RSGISGeometryException("MEM (Memory) driver does not exists..");
			}
			
			imageDist = this->createDataset(gdalDriver, geom, "", this->resolution, 0);
			std::cout << "Create distance image\n";
			this->createDistanceImage(imageDist, geom);
			geosGeomFactory->destroyGeometry(geom);
			
			std::cout << "Retrieve Polygons from image pixels\n";
			std::vector<geos::geom::Polygon*> *pxlPolys = new std::vector<geos::geom::Polygon*>();
			this->populatePixelPolygons(imageDist, resolution*2, pxlPolys);
			
			//RSGISGeomTestExport exportGeom;
			//exportGeom.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/test/snakestest/pixelpolys.shp", true, pxlPolys);
			
			std::cout << "Merge touching Polygons.\n";
			geomUtils.mergeTouchingPolygonsWithIndex(pxlPolys);
			//exportGeom.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/test/snakestest/pixelpoly_merged.shp", true, pxlPolys);
			
			int bufferIterations = 0;
			while(pxlPolys->size() > 1)
			{
				geomUtils.performMorphologicalOperation(pxlPolys, dilation, resolution, 6);
				geomUtils.mergeTouchingPolygonsWithIndex(pxlPolys);
				++bufferIterations;
				if(bufferIterations > 7)
				{
					break;
				}
			}
			//exportGeom.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/test/snakestest/pixelpoly_merged_single.shp", true, pxlPolys);
			
			geos::geom::Envelope  *imgBoundary = new geos::geom::Envelope ();
			int imgWidth = imageDist->GetRasterXSize();
			int imgHeight = imageDist->GetRasterYSize();			
			double *trans = new double[6];
			imageDist->GetGeoTransform(trans);
			
			double xMin = trans[0];
			double xMax = trans[0]+(imgWidth*resolution);
			double yMin = trans[3]-(imgHeight*resolution);
			double yMax = trans[3];
			imgBoundary->init(xMin, xMax, yMin, yMax);
			delete[] trans;
			
			std::vector<geos::geom::LineSegment*> *lines = NULL;
			geos::geom::CoordinateSequence *coords = NULL;
			geos::geom::Polygon *convexhull = NULL;
			geos::geom::Polygon *convexHullNodesAdded = NULL;
			geos::geom::Polygon *outlinePoly = NULL;
			std::vector<geos::geom::Coordinate*> *coordsVec = new std::vector<geos::geom::Coordinate*>();
			
			rsgis::math::RSGISGlobalOptimisationFunction *optimiseFunc = NULL;
			rsgis::math::RSGISGlobalOptimiser2D *optimise = NULL;
			
			if(pxlPolys->size() == 1)
			{
				geomUtils.performMorphologicalOperation(pxlPolys, closing, 4, 6);
				outlinePoly = geomUtils.removeHoles(pxlPolys->at(0));
				coords = outlinePoly->getCoordinates();
				
				
				geos::geom::Coordinate coord;
				for(unsigned int i = 1; i < coords->size(); ++i)
				{
					coord = coords->getAt(i);
					coordsVec->push_back(new geos::geom::Coordinate(coord.x, coord.y, coord.z));
				}
				delete coords;
				
				geomUtils.removeNeighborDuplicates(coordsVec);
				
				optimiseFunc = new RSGISSnakeNonConvexGlobalOptimisationFunction(imageDist, this->alpha, this->beta, this->gamma);
				optimise = new rsgis::math::RSGISGlobalHillClimbingOptimiser2DVaryNumPts(optimiseFunc, false, maxNumIterations);
			}
			else
			{
				std::cout << "Reverting to Convex Hull as starting point.\n";
				convexhull = geomUtils.findConvexHull(polygons);
				convexHullNodesAdded = geomUtils.addNodes(convexhull, resolution);
				coords = convexHullNodesAdded->getCoordinates();
				
				geos::geom::Coordinate coord;
				for(unsigned int i = 1; i < coords->size(); ++i)
				{
					coord = coords->getAt(i);
					coordsVec->push_back(new geos::geom::Coordinate(coord.x, coord.y, coord.z));
				}
				delete coords;
				
				geomUtils.removeNeighborDuplicates(coordsVec);
				
				float lineLength = 0;
				if(imgBoundary->getWidth() > imgBoundary->getHeight())
				{
					lineLength = imgBoundary->getWidth();
				}
				else
				{
					lineLength = imgBoundary->getHeight();
				}
				
				lines = geomUtils.findLineProj(coordsVec, lineLength);
				
				if(lines->size() != coordsVec->size())
				{
					throw RSGISGeometryException("The number of coordinates and lines is not the same.");
				}
				
				optimiseFunc = new RSGISSnakeNonConvexLineProjGlobalOptimisationFunction(imageDist, this->alpha, this->beta, this->gamma, delta, lines);
				optimise = new rsgis::math::RSGISGlobalHillClimbingOptimiser2D(optimiseFunc, false, (maxNumIterations * 3));
			}
			std::cout << "CoordVec has " << coordsVec->size() << " nodes\n";
			
			std::cout << "Perform snakes based refinement of boundary\n";
			std::vector<geos::geom::Coordinate*> *coordsVecOp = optimise->optimise8Neighbor(coordsVec, resolution, imgBoundary);
			
			std::vector<geos::geom::Coordinate*>::iterator iterCoords;
			coords = new geos::geom::CoordinateArraySequence();
			geos::geom::Coordinate *ptrCoord;
			for(iterCoords = coordsVecOp->begin(); iterCoords != coordsVecOp->end(); ++iterCoords)
			{
				ptrCoord = *iterCoords;
				coords->add(geos::geom::Coordinate(ptrCoord->x, ptrCoord->y, ptrCoord->z));
			}
			ptrCoord = coordsVecOp->front();
			coords->add(geos::geom::Coordinate(ptrCoord->x, ptrCoord->y, ptrCoord->z));
			
			geos::geom::LinearRing *ring = geosGeomFactory->createLinearRing(coords);
			outPoly = geosGeomFactory->createPolygon(ring, NULL);
			
			delete optimise;
			delete optimiseFunc;
			
			if(convexhull != NULL)
			{
				delete convexhull;
			}
			
			if(convexHullNodesAdded != NULL)
			{
				delete convexHullNodesAdded;
			}
			
			if(outlinePoly != NULL)
			{
				delete outlinePoly;
			}
			
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			for(iterPolys = pxlPolys->begin(); iterPolys != pxlPolys->end(); )
			{
				delete *iterPolys;
				pxlPolys->erase(iterPolys);
			}
			delete pxlPolys;
			
			for(iterCoords = coordsVec->begin(); iterCoords != coordsVec->end(); )
			{
				delete *iterCoords;
				coordsVec->erase(iterCoords);
			}
			delete coordsVec;
			
			for(iterCoords = coordsVecOp->begin(); iterCoords != coordsVecOp->end(); )
			{
				//delete *iterCoords;
				coordsVecOp->erase(iterCoords);
			}
			delete coordsVecOp;
			
			if(lines != NULL)
			{
				std::vector<geos::geom::LineSegment*>::iterator iterLines;
				for(iterLines = lines->begin(); iterLines != lines->end(); )
				{
					delete *iterLines;
					lines->erase(iterLines);
				}
				delete lines;
			}
			
			delete imgBoundary;
			GDALClose(imageDist);
		}
		catch(geos::util::TopologyException &e)
		{
			GDALClose(imageDist);
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISException &e)
		{
			GDALClose(imageDist);
			throw RSGISGeometryException(e.what());
		}
		catch(geos::util::IllegalArgumentException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		
		return outPoly;
	}
		
	GDALDataset* RSGISIdentifyNonConvexPolygonsSnakes::createDataset(GDALDriver *gdalDriver, geos::geom::Geometry *geom, std::string filename, float resolution, float constVal) throw(RSGISImageException)
	{
		RSGISGeometry geomUtils;
		
		geos::geom::Envelope  *env = geomUtils.getEnvelope (geom);
		
		int imageWidth = (int) ((env->getWidth()/resolution)+0.5) + 100;
		int imageHeight = (int) ((env->getHeight()/resolution)+0.5) + 100;
		
		GDALDataset *imageDS = gdalDriver->Create(filename.c_str(), imageWidth, imageHeight, 1, GDT_Float32, gdalDriver->GetMetadata());
		
		double *gdalTranslation = new double[6];
		
		gdalTranslation[0] = env->getMinX()-(resolution * 50);
		gdalTranslation[1] = resolution;
		gdalTranslation[2] = 0;
		gdalTranslation[3] = env->getMaxY()+(resolution * 50);
		gdalTranslation[4] = 0;
		gdalTranslation[5] = (resolution*(-1));
		
		imageDS->SetGeoTransform(gdalTranslation);
		
		char **spatRefWKT = new char*;
		spatialRef->exportToWkt(spatRefWKT);
		imageDS->SetProjection(spatRefWKT[0]);
		CPLFree(spatRefWKT);

		GDALRasterBand *imageBand = imageDS->GetRasterBand(1);
		float *data = (float *) CPLMalloc(sizeof(float)*imageWidth);
		
		for(int i = 0; i < imageWidth; ++i)
		{
			data[i] = constVal;
		}
		
		for(int i = 0; i < imageHeight; ++i)
		{
			imageBand->RasterIO(GF_Write, 0, i, imageWidth, 1, data, imageWidth, 1, GDT_Float32, 0, 0);
		}
		
		CPLFree(data);
		delete env;
		delete[] gdalTranslation;
		
		return imageDS;
	}
	
	void RSGISIdentifyNonConvexPolygonsSnakes::rasterizeLayer(geos::geom::Geometry *geom, GDALDataset *image, float constVal) throw(rsgis::RSGISImageException)
	{		
		try
		{
			geos::geom::Envelope  *envImage = new geos::geom::Envelope ();
			double *gdalTranslation = new double[6];
			image->GetGeoTransform(gdalTranslation);
			envImage->init(gdalTranslation[0], (gdalTranslation[0]+(image->GetRasterXSize()*resolution)), (gdalTranslation[3]-(image->GetRasterYSize()*resolution)), gdalTranslation[3]);
			delete[] gdalTranslation;
			
			unsigned int imgHeight = image->GetRasterYSize();
			unsigned int imgWidth = image->GetRasterXSize();
			
			GDALRasterBand *imageBand = image->GetRasterBand(1);
			float *inData = (float *) CPLMalloc(sizeof(float)*imgWidth);
			float *outData = (float *) CPLMalloc(sizeof(float)*imgWidth);
			
			geos::geom::GeometryFactory * geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			
			geos::geom::Coordinate coord;
			geos::geom::Point *pt = NULL;
			
			double pxlMinX = envImage->getMinX();
			double pxlMaxY = envImage->getMaxY();
			
			double pxlX = 0;
			double pxlY = 0;
			double halfPxlWidth = resolution/2;
			
			int feedback = imgHeight/10;
			int feedbackCounter = 0;

			if(imgHeight >= 10)
			{
				std::cout << "Started" << std::flush;
			}
			
			for(unsigned int i = 0; i < imgHeight; ++i)
			{
				if((imgHeight >= 10) && ((i % feedback) == 0))
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
						
				pxlMinX = envImage->getMinX();
				
				imageBand->RasterIO(GF_Read, 0, i, imgWidth, 1, inData, imgWidth, 1, GDT_Float32, 0, 0);
							
				for(unsigned int j = 0; j < imgWidth; ++j)
				{
					pxlX = pxlMinX + halfPxlWidth;
					pxlY = pxlMaxY - halfPxlWidth;
					coord = geos::geom::Coordinate(pxlX, pxlY, 0);
					
					pt = geomFactory->createPoint(coord);
										
					if(geom->contains(pt))
					{
						outData[j] = constVal;
					}
					else
					{
						outData[j] = inData[j];
					}
										
					delete pt;
					
					pxlMinX += resolution;
				}
				
				imageBand->RasterIO(GF_Write, 0, i, imgWidth, 1, outData, imgWidth, 1, GDT_Float32, 0, 0);
				
				pxlMaxY = pxlMaxY - resolution;
			}
			if(imgHeight >= 10)
			{
				std::cout << " Complete.\n";
			}
			
			delete[] inData;
			delete[] outData;
		}
		catch(RSGISImageException &e)
		{
			throw e;
		}
	}
	
	void RSGISIdentifyNonConvexPolygonsSnakes::createDistanceImage(GDALDataset *image, geos::geom::Geometry *geom) throw(RSGISImageException, RSGISGeometryException)
	{
		try
		{
			geos::geom::Envelope *envImage = new geos::geom::Envelope();
			double *gdalTranslation = new double[6];
			image->GetGeoTransform(gdalTranslation);
			envImage->init(gdalTranslation[0], (gdalTranslation[0]+(image->GetRasterXSize()*resolution)), (gdalTranslation[3]-(image->GetRasterYSize()*resolution)), gdalTranslation[3]);
			delete[] gdalTranslation;
			
			unsigned int imgHeight = image->GetRasterYSize();
			unsigned int imgWidth = image->GetRasterXSize();
			
			GDALRasterBand *imageBand = image->GetRasterBand(1);
			float *outData = (float *) CPLMalloc(sizeof(float)*imgWidth);
			
			geos::geom::GeometryFactory * geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			
			geos::geom::Coordinate coord;
			geos::geom::Point *pt = NULL;
			
			double pxlMinX = envImage->getMinX();
			double pxlMaxY = envImage->getMaxY();
			
			double pxlX = 0;
			double pxlY = 0;
			double halfPxlWidth = resolution/2;
			
			int feedback = imgHeight/10;
			int feedbackCounter = 0;
			
			if(imgHeight >= 10)
			{
				std::cout << "Started" << std::flush;
			}
			
			for(unsigned int i = 0; i < imgHeight; ++i)
			{
				if((imgHeight >= 10) && ((i % feedback) == 0))
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				pxlMinX = envImage->getMinX();
								
				for(unsigned int j = 0; j < imgWidth; ++j)
				{
					pxlX = pxlMinX + halfPxlWidth;
					pxlY = pxlMaxY - halfPxlWidth;
					coord = geos::geom::Coordinate(pxlX, pxlY, 0);
					
					pt = geomFactory->createPoint(coord);
					
					//if(geom->covers(pt))
					//{
					//	outData[j] = 0;
					//}
					//else
					//{
					outData[j] = geom->distance(pt);
					//}
					
					
					delete pt;
					
					pxlMinX += resolution;
				}
				
				imageBand->RasterIO(GF_Write, 0, i, imgWidth, 1, outData, imgWidth, 1, GDT_Float32, 0, 0);
				
				pxlMaxY = pxlMaxY - resolution;
			}
			if(imgHeight >= 10)
			{
				std::cout << " Complete.\n";
			}
			
			CPLFree(outData);
			delete envImage;
		}
		catch(RSGISImageException &e)
		{
			throw e;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	geos::geom::GeometryCollection * RSGISIdentifyNonConvexPolygonsSnakes::createGeomCollection(std::vector<geos::geom::Polygon*> *polys) throw(RSGISGeometryException)
	{
		geos::geom::GeometryCollection  *geom = NULL;
		try
		{
			std::vector<geos::geom::Geometry*> *geoms = new std::vector<geos::geom::Geometry*>();
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
			{
				geoms->push_back((*iterPolys)->getBoundary());
			}
			
			geos::geom::GeometryFactory * geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			geom = geomFactory->createGeometryCollection(geoms);
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return geom;
	}
	
	geos::geom::GeometryCollection * RSGISIdentifyNonConvexPolygonsSnakes::createGeomCollection(std::list<geos::geom::Polygon*> *polys) throw(RSGISGeometryException)
	{
		geos::geom::GeometryCollection  *geom = NULL;
		try
		{
			std::vector<geos::geom::Geometry*> *geoms = new std::vector<geos::geom::Geometry*>();
			std::list<geos::geom::Polygon*>::iterator iterPolys;
			for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
			{
				geoms->push_back((*iterPolys)->getBoundary());
			}
			
			geos::geom::GeometryFactory * geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			geom = geomFactory->createGeometryCollection (geoms);
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return geom;
	}
	
	void RSGISIdentifyNonConvexPolygonsSnakes::populatePixelPolygons(GDALDataset *image, float threshold, std::vector<geos::geom::Polygon*> *polys) throw(RSGISGeometryException)
	{
		double *gdalTranslation = new double[6];
		int height = 0;
		int width = 0;
		
		float *inputData = NULL;
		
		GDALRasterBand *inputRasterBand = NULL;
		
		geos::geom::Envelope  extent;
		double pxlTLX = 0;
		double pxlTLY = 0;
		double pxlWidth = 0;
		double pxlHeight = 0;
        double pxlHWidth = 0;
		double pxlHHeight = 0;
		
		try
		{
			// Find image overlap
			//imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation);
			
			width = image->GetRasterXSize();
			height = image->GetRasterYSize();
			image->GetGeoTransform(gdalTranslation);
			
			pxlTLX = gdalTranslation[0];
			pxlTLY = gdalTranslation[3];
			pxlWidth = gdalTranslation[1];
			pxlHeight = gdalTranslation[1];
            pxlHWidth = gdalTranslation[1]/2;
			pxlHHeight = gdalTranslation[1]/2;
            
			
			// Get Image Input Band
			inputRasterBand = image->GetRasterBand(1);

			// Allocate memory
			inputData = (float *) CPLMalloc(sizeof(float)*width);
			
			int feedback = height/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				//std::cout << i << " of " << height << std::endl;
				
				if((i % feedback) == 0)
				{
					std::cout << ".." << feedbackCounter << ".." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				inputRasterBand->RasterIO(GF_Read, 0, i, width, 1, inputData, width, 1, GDT_Float32, 0, 0);
				
				for(int j = 0; j < width; j++)
				{
					
					if(inputData[j] < threshold)
					{
						extent.init(pxlTLX-pxlHWidth, (pxlTLX+pxlWidth)+pxlHWidth, pxlTLY+pxlHHeight, (pxlTLY-pxlHeight)-pxlHHeight);
						polys->push_back(this->createPolygonFromEnv(extent));
					}
					
					pxlTLX += pxlWidth;
				}
				pxlTLY -= pxlHeight;
				pxlTLX = gdalTranslation[0];
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISException& e)
		{
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(inputData != NULL)
			{
				delete[] inputData;
			}
			throw RSGISGeometryException(e.what());
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(inputData != NULL)
		{
			delete[] inputData;
		}
	}
	
	geos::geom::Polygon* RSGISIdentifyNonConvexPolygonsSnakes::createPolygonFromEnv(geos::geom::Envelope  env)
	{
		std::vector<geos::geom::Coordinate> *coords = new std::vector<geos::geom::Coordinate>();
		
		coords->push_back(geos::geom::Coordinate((env.getMaxX() + 0.1), (env.getMaxY() + 0.1), 0));
		coords->push_back(geos::geom::Coordinate((env.getMaxX() + 0.1), (env.getMinY() - 0.1), 0));
		coords->push_back(geos::geom::Coordinate((env.getMinX() - 0.1), (env.getMinY() - 0.1), 0));
		coords->push_back(geos::geom::Coordinate((env.getMinX() - 0.1), (env.getMaxY() + 0.1), 0));
		coords->push_back(geos::geom::Coordinate((env.getMaxX() + 0.1), (env.getMaxY() + 0.1), 0));
		
		geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence(coords);
		
		geos::geom::GeometryFactory * geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		geos::geom::LinearRing *linearRingShell = new geos::geom::LinearRing(coordSeq, geosGeomFactory);
		geos::geom::Polygon *polygonGeom = geosGeomFactory->createPolygon(linearRingShell, NULL);
		
		return polygonGeom;
	}
	
	RSGISIdentifyNonConvexPolygonsSnakes::~RSGISIdentifyNonConvexPolygonsSnakes()
	{
		//GDALDestroyDriver(gdalDriver);
		GDALDestroyDriverManager();
	}
	
	/***
	 * 
	 * RSGISSnakeNonConvexGlobalOptimisationFunction
	 *
	 */
	
	RSGISSnakeNonConvexGlobalOptimisationFunction::RSGISSnakeNonConvexGlobalOptimisationFunction(GDALDataset *image, double alpha, double beta, double gamma)
	{
		this->image = image;
		this->imgWidth = image->GetRasterXSize();
		this->imgHeight = image->GetRasterYSize();
		
		this->env = new geos::geom::Envelope ();
		double *trans = new double[6];
		image->GetGeoTransform(trans);
		this->resolution = trans[1];		
		
		double xMin = trans[0];
		double xMax = trans[0]+(imgWidth*resolution);
		double yMin = trans[3]-(imgHeight*resolution);
		double yMax = trans[3];
		this->env->init(xMin, xMax, yMin, yMax);
		delete[] trans;
		this->data = (float *) CPLMalloc(sizeof(float)*imgWidth);
		imageBand = image->GetRasterBand(1);
		this->alpha = alpha;
		this->beta = beta;
		this->gamma = gamma;
	}
	
	double RSGISSnakeNonConvexGlobalOptimisationFunction::calcValue(std::vector<geos::geom::Coordinate*> *coords) throw(rsgis::math::RSGISOptimisationException)
	{
        rsgis::math::RSGISMathsUtils mathUtils;
		
		double energyValue = 0;
		double internalEnergy = 0;
		double externalEnergy = 0;
		double currentNextElas = 0;
		double currentNextDist = 0;
		double currentNextStiff = 0;
		double currentNextStiffX = 0;
		double currentNextStiffY = 0;
		double currentNextStiffPart = 0;
		
		int y = 0;
		double diffY = 0;
		int x = 0;
		double diffX = 0;
		
		int nodeCount = 0;
		
		geos::geom::Coordinate *prev;
		geos::geom::Coordinate *next;
		geos::geom::Coordinate *current2 = new geos::geom::Coordinate();
		
		std::vector<geos::geom::Coordinate*>::iterator iterCoords;
		for(iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
		{
			if(iterCoords == coords->begin())
			{
				next = (*(iterCoords+1));
				prev = coords->back();
			}
			else if(*iterCoords == coords->back())
			{
				next = coords->front();
				prev = (*(iterCoords-1));
			}
			else 
			{
				next = (*(iterCoords+1));
				prev = (*(iterCoords-1));
			}

			diffY = (env->getMaxY() - (*iterCoords)->y)/resolution;
			y = mathUtils.roundUp(diffY);
			diffX = ((*iterCoords)->x - env->getMinX())/resolution;
			x = mathUtils.roundUp(diffX);
			
			if((y < 0) | (y >= imgHeight))
			{
                std::cout << "Image Height: " << imgHeight << std::endl;
                std::cout << "Image Width: " << imgWidth << std::endl;
				std::string message = std::string("Y Does not fit within the image: ") + mathUtils.inttostring(y); 
				throw rsgis::math::RSGISOptimisationException(message);
			}
			
			if((x < 0) | (x >= imgWidth))
			{
                std::cout << "Image Height: " << imgHeight << std::endl;
                std::cout << "Image Width: " << imgWidth << std::endl;
                
				std::string message = std::string("X Does not fit within the image: ") + mathUtils.inttostring(x); 
				throw rsgis::math::RSGISOptimisationException(message);
			}
						
			imageBand->RasterIO(GF_Read, 0, y, imgWidth, 1, data, imgWidth, 1, GDT_Float32, 0, 0);
			externalEnergy += (data[x] * gamma);
			
			currentNextDist = sqrt(((next->x - (*iterCoords)->x)*(next->x - (*iterCoords)->x)) + (((next)->y - (*iterCoords)->y)*((next)->y - (*iterCoords)->y)));
			// alpha * sq(abs(V+1 - V))
			currentNextElas = alpha * (currentNextDist * currentNextDist);
			
			current2->x = (*iterCoords)->x * 2;
			current2->y = (*iterCoords)->y * 2;
			
			currentNextStiffX = next->x - current2->x + prev->x;
			currentNextStiffY = next->y - current2->y + prev->y;
			currentNextStiffPart = sqrt((currentNextStiffX * currentNextStiffX) + (currentNextStiffY * currentNextStiffY));
			// beta * sq(abs(V+1 - 2V + V-1))
			currentNextStiff = beta * (currentNextStiffPart * currentNextStiffPart);
			
			internalEnergy += (currentNextElas + currentNextStiff);
						
			++nodeCount;
		}
		
		//internalEnergy = internalEnergy/nodeCount;
		//externalEnergy = externalEnergy/nodeCount;
		
		delete current2;
		
		energyValue = externalEnergy + internalEnergy;
		
		//std::cout << "external Energy = " << externalEnergy << std::endl;
		//std::cout << "internal Energy = " << internalEnergy << std::endl;
		//std::cout << "Total Energy = " << energyValue << std::endl;
		
		return energyValue;
	}
	
	RSGISSnakeNonConvexGlobalOptimisationFunction::~RSGISSnakeNonConvexGlobalOptimisationFunction()
	{
		delete env;
		CPLFree(data);
	}
	
	
	/***
	 * 
	 * RSGISSnakeNonConvexGlobalOptimisationFunction
	 *
	 */
	
	RSGISSnakeNonConvexLineProjGlobalOptimisationFunction::RSGISSnakeNonConvexLineProjGlobalOptimisationFunction(GDALDataset *image, double alpha, double beta, double gamma, double delta, std::vector<geos::geom::LineSegment*> *lines)
	{
		this->image = image;
		this->imgWidth = image->GetRasterXSize();
		this->imgHeight = image->GetRasterYSize();
		
		this->env = new geos::geom::Envelope ();
		double *trans = new double[6];
		image->GetGeoTransform(trans);
		this->resolution = trans[1];		
		
		double xMin = trans[0];
		double xMax = trans[0]+(imgWidth*resolution);
		double yMin = trans[3]-(imgHeight*resolution);
		double yMax = trans[3];
		this->env->init(xMin, xMax, yMin, yMax);
		delete[] trans;
		this->data = (float *) CPLMalloc(sizeof(float)*imgWidth);
		imageBand = image->GetRasterBand(1);
		this->alpha = alpha;
		this->beta = beta;
		this->gamma = gamma;
		this->delta = delta;
		this->lines = lines;
	}
	
	double RSGISSnakeNonConvexLineProjGlobalOptimisationFunction::calcValue(std::vector<geos::geom::Coordinate*> *coords) throw(rsgis::math::RSGISOptimisationException)
	{
        rsgis::math::RSGISMathsUtils mathUtils;
		
		double energyValue = 0;
		double internalEnergy = 0;
		double externalEnergy = 0;
		double currentNextElas = 0;
		double currentNextDist = 0;
		double currentNextStiff = 0;
		double currentNextStiffX = 0;
		double currentNextStiffY = 0;
		double currentNextStiffPart = 0;
		double currentNextDist2Line = 0;
				
		int y = 0;
		double diffY = 0;
		int x = 0;
		double diffX = 0;
		
		int nodeCount = 0;
		
        geos::geom::Coordinate *prev;
		geos::geom::Coordinate *next;
		geos::geom::Coordinate *current2 = new geos::geom::Coordinate();
		
		std::vector<geos::geom::Coordinate*>::iterator iterCoords;
		std::vector<geos::geom::LineSegment*>::iterator iterLines;
		iterLines = lines->begin();
		for(iterCoords = coords->begin(); iterCoords != coords->end(); ++iterCoords)
		{
			if(iterLines == lines->end())
			{
				throw RSGISGeometryException("Not enough projected lines.");
			}
			
			if(iterCoords == coords->begin())
			{
				next = (*(iterCoords+1));
				prev = coords->back();
			}
			else if(*iterCoords == coords->back())
			{
				next = coords->front();
				prev = (*(iterCoords-1));
			}
			else 
			{
				next = (*(iterCoords+1));
				prev = (*(iterCoords-1));
			}
			
			diffY = (env->getMaxY() - (*iterCoords)->y)/resolution;
			y = mathUtils.roundUp(diffY);
			diffX = ((*iterCoords)->x - env->getMinX())/resolution;
			x = mathUtils.roundUp(diffX);
			
			if((y < 0) | (y >= imgHeight))
			{
				std::cout << "Image Height: " << imgHeight << std::endl;
                std::cout << "Image Width: " << imgWidth << std::endl;
				
				std::string message = std::string("Y Does not fit within the image: ") + mathUtils.inttostring(y); 
				throw rsgis::math::RSGISOptimisationException(message);
			}
			
			if((x < 0) | (x >= imgWidth))
			{
				std::cout << "Image Height: " << imgHeight << std::endl;
                std::cout << "Image Width: " << imgWidth << std::endl;
				
				std::string message = std::string("X Does not fit within the image: ") + mathUtils.inttostring(x); 
				throw rsgis::math::RSGISOptimisationException(message);
			}
			
			imageBand->RasterIO(GF_Read, 0, y, imgWidth, 1, data, imgWidth, 1, GDT_Float32, 0, 0);
			externalEnergy += (data[x] * gamma);
			
			currentNextDist = sqrt(((next->x - (*iterCoords)->x)*(next->x - (*iterCoords)->x)) + (((next)->y - (*iterCoords)->y)*((next)->y - (*iterCoords)->y)));
			// alpha * sq(abs(V+1 - V))
			currentNextElas = alpha * (currentNextDist * currentNextDist);
			
			current2->x = (*iterCoords)->x * 2;
			current2->y = (*iterCoords)->y * 2;
			
			currentNextStiffX = next->x - current2->x + prev->x;
			currentNextStiffY = next->y - current2->y + prev->y;
			currentNextStiffPart = sqrt((currentNextStiffX * currentNextStiffX) + (currentNextStiffY * currentNextStiffY));
			// beta * sq(abs(V+1 - 2V + V-1))
			currentNextStiff = beta * (currentNextStiffPart * currentNextStiffPart);
			
			currentNextDist2Line = delta * ((*iterLines)->distancePerpendicular(*(*iterCoords)));
			//std::cout << "((*iterLines)->distance(*(*iterCoords))) = " << ((*iterLines)->distance(*(*iterCoords))) << std::endl;
			//std::cout << "Coords: " << (*(*iterCoords)) << std::endl;
			//std::cout << "Line: " << (*(*iterLines)) << std::endl;
			//std::cout << "currentNextDist2Line = " << currentNextDist2Line << std::endl;
			
			internalEnergy += (currentNextElas + currentNextStiff + currentNextDist2Line);
			
			++nodeCount;
			++iterLines;
		}
		delete current2;
		
		//internalEnergy = internalEnergy/nodeCount;
		//externalEnergy = externalEnergy/nodeCount;
		
		energyValue = externalEnergy + internalEnergy;
		
		//std::cout << "external Energy = " << externalEnergy << std::endl;
		//std::cout << "internal Energy = " << internalEnergy << std::endl;
		//std::cout << "Total Energy = " << energyValue << std::endl;
		
		return energyValue;
	}
	
	RSGISSnakeNonConvexLineProjGlobalOptimisationFunction::~RSGISSnakeNonConvexLineProjGlobalOptimisationFunction()
	{
		delete env;
		CPLFree(data);
	}
}}


