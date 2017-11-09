/*
 *  RSGISVectorUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/04/2008.
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

#include "RSGISVectorUtils.h"


namespace rsgis{namespace vec{
	
	std::string RSGISVectorUtils::getLayerName(std::string filepath)
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
	
	
	geos::geom::LineString* RSGISVectorUtils::convertOGRLineString2GEOSLineString(OGRLineString *line)
	{
		int numPoints = line->getNumPoints();
		OGRPoint *point = new OGRPoint();
		std::vector<geos::geom::Coordinate> *coords = new std::vector<geos::geom::Coordinate>();
		for(int i = 0; i < numPoints; i++)
		{
			line->getPoint(i, point);
			coords->push_back(geos::geom::Coordinate(point->getX(), point->getY(), point->getZ()));
		}
		
		geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence(coords);
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		geos::geom::LineString *lineString = geosGeomFactory->createLineString(coordSeq);
		
		delete point;
		return lineString;
		
	}
	
	OGRLineString* RSGISVectorUtils::convertGEOSLineString2OGRLineString(geos::geom::LineString *line)
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
	
	geos::geom::LinearRing* RSGISVectorUtils::convertOGRLinearRing2GEOSLinearRing(OGRLinearRing *ring)
	{
		int numPoints = ring->getNumPoints();
		OGRPoint *point = new OGRPoint();
		std::vector<geos::geom::Coordinate> *coords = new std::vector<geos::geom::Coordinate>();
		for(int i = 0; i < numPoints; i++)
		{
			ring->getPoint(i, point);
			geos::geom::Coordinate coord = geos::geom::Coordinate(point->getX(), point->getY(), point->getZ());
            coords->push_back(coord);
		}
		
		geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence(coords);
		
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		geos::geom::LinearRing *linearRingShell = new geos::geom::LinearRing(coordSeq, geosGeomFactory);

		delete point;
		return linearRingShell;
	}
	
	OGRLinearRing* RSGISVectorUtils::convertGEOSLineString2OGRLinearRing(geos::geom::LineString *line)
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
	
	geos::geom::Polygon* RSGISVectorUtils::convertOGRPolygon2GEOSPolygon(OGRPolygon *poly)
	{
		/// Converts OGR Polygon into a GEOS Polygon
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		OGRLinearRing *exteriorRing = poly->getExteriorRing();
		geos::geom::LinearRing *linearRingShell = this->convertOGRLinearRing2GEOSLinearRing(exteriorRing);
		
		std::vector<geos::geom::Geometry*> *geomHoles = new std::vector<geos::geom::Geometry*>();
		int numHoles = poly->getNumInteriorRings();
		for(int i = 0; i < numHoles; i++)
		{
			geomHoles->push_back(this->convertOGRLinearRing2GEOSLinearRing(poly->getInteriorRing(i)));
		}
		geos::geom::Polygon *polygonGeom = geosGeomFactory->createPolygon(linearRingShell, geomHoles); 
		
		return polygonGeom;
	}
	
	geos::geom::MultiPolygon* RSGISVectorUtils::convertOGRMultiPolygonGEOSMultiPolygon(OGRMultiPolygon *mPoly)
	{
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		OGRGeometry *tmpGeom = NULL;
		OGRPolygon *tmpPolygon = NULL;
		std::vector<geos::geom::Geometry*> *geoms = new std::vector<geos::geom::Geometry*>();
		int numGeometries = mPoly->getNumGeometries();
		for(int i = 0; i < numGeometries; i++)
		{
			tmpGeom = mPoly->getGeometryRef(i);
			if(wkbFlatten(tmpGeom->getGeometryType()) == wkbPolygon)
			{
				tmpPolygon = (OGRPolygon *) tmpGeom;
				geoms->push_back(this->convertOGRPolygon2GEOSPolygon(tmpPolygon));
			}
		}
		geos::geom::MultiPolygon *mPolyGeom = geosGeomFactory->createMultiPolygon(geoms);

		return mPolyGeom;
	}
	
	geos::geom::Point* RSGISVectorUtils::convertOGRPoint2GEOSPoint(OGRPoint *point)
	{
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		geos::geom::Coordinate *coord = new geos::geom::Coordinate(point->getX(), point->getY(), point->getZ());
		
		return geosGeomFactory->createPoint(*coord);
	}
	
	OGRPolygon* RSGISVectorUtils::convertGEOSPolygon2OGRPolygon(geos::geom::Polygon *poly)
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
		
		return ogrPoly;
	}
	
	OGRMultiPolygon* RSGISVectorUtils::convertGEOSMultiPolygon2OGRMultiPolygon(geos::geom::MultiPolygon *mPoly)
	{
		OGRMultiPolygon *ogrMPoly = new OGRMultiPolygon();
		int numGeometries = mPoly->getNumGeometries();
		
		geos::geom::Geometry *geom = NULL;
		geos::geom::Polygon *poly = NULL;
		
		for(int i = 0; i < numGeometries; i++)
		{
			geom = const_cast<geos::geom::Geometry *>(mPoly->getGeometryN(i));
			if(geom->getGeometryType() == std::string("Polygon"))
			{
				poly = dynamic_cast<geos::geom::Polygon*>(geom);
				ogrMPoly->addGeometryDirectly(this->convertGEOSPolygon2OGRPolygon(poly));
			}
		}
		return ogrMPoly;
	}
	
	OGRMultiPolygon* RSGISVectorUtils::convertGEOSPolygons2OGRMultiPolygon(std::list<geos::geom::Polygon*> *polys)
	{
		OGRMultiPolygon *ogrMPoly = new OGRMultiPolygon();
		std::list<geos::geom::Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
		{
			ogrMPoly->addGeometryDirectly(this->convertGEOSPolygon2OGRPolygon(*iterPolys));
		}
		
		return ogrMPoly;
	}
	
	geos::geom::MultiPolygon* RSGISVectorUtils::convertGEOSPolygons2GEOSMultiPolygon(std::vector<geos::geom::Polygon*> *polys)
	{
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		std::vector<geos::geom::Geometry*> *geoms = new std::vector<geos::geom::Geometry*>();
		std::vector<geos::geom::Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
		{
			geoms->push_back((*iterPolys)->clone());
		}
		
		return geosGeomFactory->createMultiPolygon(geoms);
	}
	
	OGRPoint* RSGISVectorUtils::convertGEOSPoint2OGRPoint(geos::geom::Point *point)
	{
		OGRPoint *outPoint = new OGRPoint();
		const geos::geom::Coordinate *coord = point->getCoordinate();
		outPoint->setX(coord->x);
		outPoint->setY(coord->y);
		outPoint->setZ(coord->z);
		return outPoint;
	}
	
	OGRPoint* RSGISVectorUtils::convertGEOSCoordinate2OGRPoint(geos::geom::Coordinate *coord)
	{
		OGRPoint *outPoint = new OGRPoint();
		outPoint->setX(coord->x);
		outPoint->setY(coord->y);
		outPoint->setZ(coord->z);
		return outPoint;
	}
	
	geos::geom::Envelope* RSGISVectorUtils::getEnvelope(geos::geom::Geometry *geom)
	{
		geos::geom::Envelope *env = new geos::geom::Envelope();
		geos::geom::CoordinateSequence *coordSeq = geom->getCoordinates();

		int numPts = coordSeq->getSize();
		for(int i = 0; i < numPts; i++)
		{
			env->expandToInclude(coordSeq->getAt(i));
		}
		
		delete coordSeq;		
		return env;
	}
	
	geos::geom::Envelope* RSGISVectorUtils::getEnvelope(OGRGeometry *geom)
	{
		OGREnvelope *ogrEnv = new OGREnvelope();
		geom->getEnvelope(ogrEnv);
		
		geos::geom::Envelope *env = new geos::geom::Envelope(ogrEnv->MinX, ogrEnv->MaxX, ogrEnv->MinY, ogrEnv->MaxY);
		delete ogrEnv;
		return env;
	}
	
	geos::geom::Envelope* RSGISVectorUtils::getEnvelopePixelBuffer(OGRGeometry *geom, double imageRes)
	{
		/// Gets the envelope of an OGRGeometry buffered by one pixel.
		/* When rasterising small polygons, getEnvelope can return an envelope that is smaller than a pixel.
		 This class buffers the envelope by a 1/2 a pixel to ensure the envelope covers at least 1 pixel */
		
		OGREnvelope *ogrEnv = new OGREnvelope();
		geom->getEnvelope(ogrEnv);
		
		double buffer = imageRes / 2;
		
		geos::geom::Envelope *env = new geos::geom::Envelope(ogrEnv->MinX - buffer, ogrEnv->MaxX + buffer, ogrEnv->MinY - buffer, ogrEnv->MaxY + buffer);
		delete ogrEnv;
		return env;
	}
	
	geos::geom::Point* RSGISVectorUtils::createPoint(geos::geom::Coordinate *coord)
	{
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		return geosGeomFactory->createPoint(*coord);
	}
	
	bool RSGISVectorUtils::checkDIR4SHP(std::string dir, std::string shp) throw(RSGISVectorException)
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
		catch(RSGISException e)
		{
			std::cout << e.what() << std::endl;
			throw RSGISVectorException(e.what());
		}
		delete[] dirList;
		
		return returnVal;
	}
	
	void RSGISVectorUtils::deleteSHP(std::string dir, std::string shp) throw(RSGISVectorException)
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
							throw RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".shx")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".sbx")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".sbn")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".dbf")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					else if(fileUtils.getExtension(dirList[i]) == ".prj")
					{
						std::cout << dirList[i];
						if( remove( dirList[i].c_str() ) != 0 )
						{
							throw RSGISException("Could not delete file.");
						}
						std::cout << " deleted\n";
					}
					
				}
			}
		}
		catch(RSGISException e)
		{
			throw RSGISVectorException(e.what());
		}
		delete[] dirList;
	}

	
	geos::geom::GeometryCollection* RSGISVectorUtils::createGeomCollection(std::vector<geos::geom::Polygon*> *polys) throw(RSGISVectorException)
	{
		geos::geom::GeometryCollection *geom = NULL;
		try
		{
			std::vector<geos::geom::Geometry*> *geoms = new std::vector<geos::geom::Geometry*>();
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
			{
				geoms->push_back((*iterPolys)->clone());
			}
			
			const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			geom = geosGeomFactory->createGeometryCollection(geoms);
		}
		catch(RSGISVectorException &e)
		{
			throw e;
		}
		
		return geom;
	}
	
	geos::geom::Polygon* RSGISVectorUtils::createPolygon(double tlX, double tlY, double brX, double brY) throw(RSGISVectorException)
	{
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		std::vector<geos::geom::Coordinate> *coords = new std::vector<geos::geom::Coordinate>();
		coords->push_back(geos::geom::Coordinate(tlX, tlY, 0));
		coords->push_back(geos::geom::Coordinate(brX, tlY, 0));
		coords->push_back(geos::geom::Coordinate(brX, brY, 0));
		coords->push_back(geos::geom::Coordinate(tlX, brY, 0));
		coords->push_back(geos::geom::Coordinate(tlX, tlY, 0));
		
		geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence(coords);
		geos::geom::LinearRing *linearRingShell = new geos::geom::LinearRing(coordSeq, geosGeomFactory);
		
		geos::geom::Polygon *polygonGeom = geosGeomFactory->createPolygon(linearRingShell, NULL); 
		
		return polygonGeom;
		
	}
	
	OGRPolygon* RSGISVectorUtils::createOGRPolygon(double tlX, double tlY, double brX, double brY) throw(RSGISVectorException)
	{
		OGRPolygon *ogrPoly = new OGRPolygon();
		
		OGRLinearRing *ogrRing = new OGRLinearRing();
		ogrRing->addPoint(tlX, tlY, 0);
		ogrRing->addPoint(brX, tlY, 0);
		ogrRing->addPoint(brX, brY, 0);
		ogrRing->addPoint(tlX, brY, 0);
		ogrRing->addPoint(tlX, tlY, 0);
		ogrPoly->addRingDirectly(ogrRing);

		return ogrPoly;
	}
    
    OGRPolygon* RSGISVectorUtils::createOGRPolygon(geos::geom::Envelope *env) throw(RSGISVectorException)
    {
        OGRPolygon *ogrPoly = new OGRPolygon();
        
        OGRLinearRing *ogrRing = new OGRLinearRing();
        ogrRing->addPoint(env->getMinX(), env->getMaxY(), 0);
        ogrRing->addPoint(env->getMaxX(), env->getMaxY(), 0);
        ogrRing->addPoint(env->getMaxX(), env->getMinY(), 0);
        ogrRing->addPoint(env->getMinX(), env->getMinY(), 0);
        ogrRing->addPoint(env->getMinX(), env->getMaxY(), 0);
        ogrPoly->addRingDirectly(ogrRing);
        
        return ogrPoly;
    }
	
	OGRPolygon* RSGISVectorUtils::checkCloseOGRPolygon(OGRPolygon *poly) throw(RSGISVectorException)
	{
		OGRPolygon *ogrPoly = new OGRPolygon();
		OGRLinearRing *ogrRing = new OGRLinearRing();
		OGRLinearRing *exteriorRing = poly->getExteriorRing();
		
		int numPoints = exteriorRing->getNumPoints();
		
		if(numPoints < 4)
		{
			throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
		}
		
		OGRPoint *pt = new OGRPoint();
		OGRPoint *pt0 = new OGRPoint();
		for(int i = 0; i < numPoints; i++)
		{
			exteriorRing->getPoint(i, pt);
			
			if(i == 0)
			{
				pt0->setX(pt->getX());
				pt0->setY(pt->getY());
				pt0->setZ(pt->getZ());
			}
			ogrRing->addPoint(pt->getX(), pt->getY(), pt->getZ());
		}
		
		if(pt != pt0)
		{
			ogrRing->addPoint(pt0->getX(), pt0->getY(), pt0->getZ());
		}
		
		delete pt;
		delete pt0;
		
		ogrPoly->addRingDirectly(ogrRing);
		
		return ogrPoly;
		
	}
	
	OGRPolygon* RSGISVectorUtils::removeHolesOGRPolygon(OGRPolygon *poly) throw(RSGISVectorException)
	{
		OGRPolygon *ogrPoly = new OGRPolygon();
		OGRLinearRing *ogrRing = new OGRLinearRing();
		OGRLinearRing *exteriorRing = poly->getExteriorRing();
		
		int numPoints = exteriorRing->getNumPoints();
		
		if(numPoints < 4)
		{
			throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
		}
		
		OGRPoint *pt = new OGRPoint();
		OGRPoint *pt0 = new OGRPoint();
		for(int i = 0; i < numPoints; i++)
		{
			exteriorRing->getPoint(i, pt);
			
			if(i == 0)
			{
				pt0->setX(pt->getX());
				pt0->setY(pt->getY());
				pt0->setZ(pt->getZ());
			}
			ogrRing->addPoint(pt->getX(), pt->getY(), pt->getZ());
		}
		
		if(pt != pt0)
		{
			ogrRing->addPoint(pt0->getX(), pt0->getY(), pt0->getZ());
		}
		
		delete pt;
		delete pt0;
		
		ogrPoly->addRingDirectly(ogrRing);
		
		return ogrPoly;
		
	}
    
    OGRPolygon* RSGISVectorUtils::removeHolesOGRPolygon(OGRPolygon *poly, float areaThreshold) throw(RSGISVectorException)
	{
		OGRPolygon *ogrPoly = new OGRPolygon();
		OGRLinearRing *ogrRing = new OGRLinearRing();
		OGRLinearRing *exteriorRing = poly->getExteriorRing();
		
		int numPoints = exteriorRing->getNumPoints();
		
		if(numPoints < 4)
		{
			throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
		}
		
		OGRPoint *pt = new OGRPoint();
		OGRPoint *pt0 = new OGRPoint();
		for(int i = 0; i < numPoints; i++)
		{
			exteriorRing->getPoint(i, pt);
			
			if(i == 0)
			{
				pt0->setX(pt->getX());
				pt0->setY(pt->getY());
				pt0->setZ(pt->getZ());
			}
			ogrRing->addPoint(pt->getX(), pt->getY(), pt->getZ());
		}
		
		if(pt != pt0)
		{
			ogrRing->addPoint(pt0->getX(), pt0->getY(), pt0->getZ());
		}
		ogrPoly->addRingDirectly(ogrRing);
        
        
        if(poly->getNumInteriorRings() > 0)
        {
            const OGRLinearRing *tmpRing = NULL;
            int numHoles = poly->getNumInteriorRings();
            for(int n = 0; n < numHoles; ++n)
            {
                tmpRing = poly->getInteriorRing(n);
                if(tmpRing->get_Area() > areaThreshold)
                {
                    OGRLinearRing *ogrIntRing = new OGRLinearRing();
                    int numIntPoints = tmpRing->getNumPoints();
                    
                    if(numIntPoints < 4)
                    {
                        throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
                    }
                    
                    for(int i = 0; i < numIntPoints; i++)
                    {
                        tmpRing->getPoint(i, pt);
                        
                        if(i == 0)
                        {
                            pt0->setX(pt->getX());
                            pt0->setY(pt->getY());
                            pt0->setZ(pt->getZ());
                        }
                        ogrIntRing->addPoint(pt->getX(), pt->getY(), pt->getZ());
                    }
                    
                    if(pt != pt0)
                    {
                        ogrIntRing->addPoint(pt0->getX(), pt0->getY(), pt0->getZ());
                    }
                    ogrPoly->addRingDirectly(ogrIntRing);
                }
            }
        }
        
        
        
		delete pt;
		delete pt0;
				
		return ogrPoly;
		
	}
	
	OGRPolygon* RSGISVectorUtils::moveOGRPolygon(OGRPolygon *poly, double shiftX, double shiftY, double shiftZ) throw(RSGISVectorException)
	{
		OGRPolygon *ogrPoly = new OGRPolygon();
		OGRLinearRing *ogrRing = new OGRLinearRing();
		OGRLinearRing *exteriorRing = poly->getExteriorRing();
		
		int numPoints = exteriorRing->getNumPoints();
		
		if(numPoints < 4)
		{
			throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
		}
		
		OGRPoint *pt = new OGRPoint();
		for(int i = 0; i < numPoints; i++)
		{
			exteriorRing->getPoint(i, pt);
			ogrRing->addPoint(pt->getX() + shiftX, pt->getY() + shiftY, pt->getZ() + shiftZ);
		}
		
		delete pt;
		
		ogrPoly->addRingDirectly(ogrRing);
		
		return ogrPoly;
	}
	
	std::vector<std::string>* RSGISVectorUtils::findUniqueVals(OGRLayer *layer, std::string attribute) throw(RSGISVectorException)
	{
		std::vector<std::string>::iterator iterVals;
		std::vector<std::string> *values = new std::vector<std::string>();

		OGRFeature *feature = NULL;
		OGRFeatureDefn *featureDefn = layer->GetLayerDefn();
		
		bool found = false;
		
		layer->ResetReading();
		while( (feature = layer->GetNextFeature()) != NULL )
		{			
			std::string attrVal = feature->GetFieldAsString(featureDefn->GetFieldIndex(attribute.c_str()));
			
			found = false;
			for(iterVals = values->begin(); iterVals != values->end(); ++iterVals)
			{
				if((*iterVals) == attrVal)
				{
					found = true;
					break;
				}
			}
			
			if(!found)
			{
				values->push_back(attrVal);
			}
			
			OGRFeature::DestroyFeature(feature);
		}
		
		
		return values;
	}
    
    
    std::vector<std::string>* RSGISVectorUtils::getColumnNames(OGRLayer *layer) throw(RSGISVectorException)
    {
        std::vector<std::string> *colNames = new std::vector<std::string>();
        try
        {
            OGRFeatureDefn *ogrFeatDef = layer->GetLayerDefn();
            int numFields = ogrFeatDef->GetFieldCount();
            for(int i = 0; i < numFields; ++i)
            {
                OGRFieldDefn *fieldDef = ogrFeatDef->GetFieldDefn(i);
                colNames->push_back(std::string(fieldDef->GetNameRef()));
            }
            
        }
        catch (RSGISVectorException &e)
        {
            throw e;
        }
        return colNames;
    }
    
    
    
    std::vector<OGRPoint*>* RSGISVectorUtils::getRegularStepPoints(std::vector<OGRLineString*> *lines, double step) throw(RSGISVectorException)
    {
        std::vector<OGRPoint*> *pts = new std::vector<OGRPoint*>();
        try
        {
            long numPts = 0;
           	OGRPoint *pt = NULL;
            for(std::vector<OGRLineString*>::iterator iterLines = lines->begin(); iterLines != lines->end(); ++iterLines)
            {
                (*iterLines)->segmentize(step);
                numPts = (*iterLines)->getNumPoints();
                for(long i = 0; i < numPts; ++i)
                {
                    pt = new OGRPoint();
                    (*iterLines)->getPoint(i, pt);
                    pts->push_back(pt);
                }
            }
            
        }
        catch (RSGISVectorException &e)
        {
            throw e;
        }
        return pts;
    }
	
}}
