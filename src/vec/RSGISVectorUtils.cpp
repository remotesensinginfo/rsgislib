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
	
	string RSGISVectorUtils::getLayerName(string filepath)
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
		string filename = filepath.substr(lastSlash+1);
		
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
		return layerName;		
	}
	
	
	LineString* RSGISVectorUtils::convertOGRLineString2GEOSLineString(OGRLineString *line)
	{
		int numPoints = line->getNumPoints();
		OGRPoint *point = new OGRPoint();
		vector<Coordinate> *coords = new vector<Coordinate>();
		for(int i = 0; i < numPoints; i++)
		{
			line->getPoint(i, point);
			coords->push_back(Coordinate(point->getX(), point->getY(), point->getZ()));
		}
		
		CoordinateArraySequence *coordSeq = new CoordinateArraySequence(coords);
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		LineString *lineString = geosGeomFactory->createLineString(coordSeq);
		
		delete point;
		delete geosGeomFactory;
		return lineString;
		
	}
	
	OGRLineString* RSGISVectorUtils::convertGEOSLineString2OGRLineString(LineString *line)
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
	
	LinearRing* RSGISVectorUtils::convertOGRLinearRing2GEOSLinearRing(OGRLinearRing *ring)
	{
		int numPoints = ring->getNumPoints();
		OGRPoint *point = new OGRPoint();
		vector<Coordinate> *coords = new vector<Coordinate>();
		for(int i = 0; i < numPoints; i++)
		{
			ring->getPoint(i, point);
			Coordinate coord = Coordinate(point->getX(), point->getY(), point->getZ());
			coords->push_back(coord);
		}
		
		CoordinateArraySequence *coordSeq = new CoordinateArraySequence(coords);
		
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		LinearRing *linearRingShell = new LinearRing(coordSeq, geosGeomFactory);

		delete point;
		return linearRingShell;
	}
	
	OGRLinearRing* RSGISVectorUtils::convertGEOSLineString2OGRLinearRing(LineString *line)
	{
		OGRLinearRing *ring = new OGRLinearRing();
		const CoordinateSequence *coords = line->getCoordinatesRO();
		int numCoords = coords->getSize();
		Coordinate coord;
		for(int i = 0; i < numCoords; i++)
		{
			coord = coords->getAt(i);
			ring->addPoint(coord.x, coord.y, coord.z);
		}
		return ring;
	}
	
	Polygon* RSGISVectorUtils::convertOGRPolygon2GEOSPolygon(OGRPolygon *poly)
	{
		/// Converts OGR Polygon into a GEOS Polygon
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		OGRLinearRing *exteriorRing = poly->getExteriorRing();
		LinearRing *linearRingShell = this->convertOGRLinearRing2GEOSLinearRing(exteriorRing);
		
		vector<Geometry*> *geomHoles = new vector<Geometry*>();
		int numHoles = poly->getNumInteriorRings();
		for(int i = 0; i < numHoles; i++)
		{
			geomHoles->push_back(this->convertOGRLinearRing2GEOSLinearRing(poly->getInteriorRing(i)));
		}
		Polygon *polygonGeom = geosGeomFactory->createPolygon(linearRingShell, geomHoles); 
		
		return polygonGeom;
	}
	
	MultiPolygon* RSGISVectorUtils::convertOGRMultiPolygonGEOSMultiPolygon(OGRMultiPolygon *mPoly)
	{
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		OGRGeometry *tmpGeom = NULL;
		OGRPolygon *tmpPolygon = NULL;
		vector<Geometry*> *geoms = new vector<Geometry*>();
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
		MultiPolygon *mPolyGeom = geosGeomFactory->createMultiPolygon(geoms);

		return mPolyGeom;
	}
	
	Point* RSGISVectorUtils::convertOGRPoint2GEOSPoint(OGRPoint *point)
	{
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		Coordinate *coord = new Coordinate(point->getX(), point->getY(), point->getZ());
		
		return geosGeomFactory->createPoint(*coord);
	}
	
	OGRPolygon* RSGISVectorUtils::convertGEOSPolygon2OGRPolygon(Polygon *poly)
	{
		//cout << "Poly area = " << poly->getArea() << endl;
		
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
		
		return ogrPoly;
	}
	
	OGRMultiPolygon* RSGISVectorUtils::convertGEOSMultiPolygon2OGRMultiPolygon(MultiPolygon *mPoly)
	{
		OGRMultiPolygon *ogrMPoly = new OGRMultiPolygon();
		int numGeometries = mPoly->getNumGeometries();
		
		Geometry *geom = NULL;
		Polygon *poly = NULL;
		
		for(int i = 0; i < numGeometries; i++)
		{
			geom = const_cast<Geometry *>(mPoly->getGeometryN(i));
			if(geom->getGeometryType() == string("Polygon"))
			{
				poly = dynamic_cast<Polygon*>(geom);
				ogrMPoly->addGeometryDirectly(this->convertGEOSPolygon2OGRPolygon(poly));
			}
		}
		return ogrMPoly;
	}
	
	OGRMultiPolygon* RSGISVectorUtils::convertGEOSPolygons2OGRMultiPolygon(list<Polygon*> *polys)
	{
		OGRMultiPolygon *ogrMPoly = new OGRMultiPolygon();
		list<Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
		{
			ogrMPoly->addGeometryDirectly(this->convertGEOSPolygon2OGRPolygon(*iterPolys));
		}
		
		return ogrMPoly;
	}
	
	MultiPolygon* RSGISVectorUtils::convertGEOSPolygons2GEOSMultiPolygon(vector<Polygon*> *polys)
	{
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		vector<Geometry*> *geoms = new vector<Geometry*>();
		vector<Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
		{
			geoms->push_back((*iterPolys)->clone());
		}
		
		return geosGeomFactory->createMultiPolygon(geoms);
	}
	
	OGRPoint* RSGISVectorUtils::convertGEOSPoint2OGRPoint(Point *point)
	{
		OGRPoint *outPoint = new OGRPoint();
		const Coordinate *coord = point->getCoordinate();
		outPoint->setX(coord->x);
		outPoint->setY(coord->y);
		outPoint->setZ(coord->z);
		return outPoint;
	}
	
	OGRPoint* RSGISVectorUtils::convertGEOSCoordinate2OGRPoint(Coordinate *coord)
	{
		OGRPoint *outPoint = new OGRPoint();
		outPoint->setX(coord->x);
		outPoint->setY(coord->y);
		outPoint->setZ(coord->z);
		return outPoint;
	}
	
	Envelope* RSGISVectorUtils::getEnvelope(Geometry *geom)
	{
		Envelope *env = new Envelope();
		CoordinateSequence *coordSeq = geom->getCoordinates();

		int numPts = coordSeq->getSize();
		for(int i = 0; i < numPts; i++)
		{
			env->expandToInclude(coordSeq->getAt(i));
		}
		
		delete coordSeq;		
		return env;
	}
	
	Envelope* RSGISVectorUtils::getEnvelope(OGRGeometry *geom)
	{
		OGREnvelope *ogrEnv = new OGREnvelope();
		geom->getEnvelope(ogrEnv);
		
		Envelope *env = new Envelope(ogrEnv->MinX, ogrEnv->MaxX, ogrEnv->MinY, ogrEnv->MaxY);
		delete ogrEnv;
		return env;
	}
	
	Envelope* RSGISVectorUtils::getEnvelopePixelBuffer(OGRGeometry *geom, double imageRes)
	{
		/// Gets the envelope of an OGRGeometry buffered by one pixel.
		/* When rasterising small polygons, getEnvelope can return an envelope that is smaller than a pixel.
		 This class buffers the envelope by a 1/2 a pixel to ensure the envelope covers at least 1 pixel */
		
		OGREnvelope *ogrEnv = new OGREnvelope();
		geom->getEnvelope(ogrEnv);
		
		double buffer = imageRes / 2;
		
		Envelope *env = new Envelope(ogrEnv->MinX - buffer, ogrEnv->MaxX + buffer, ogrEnv->MinY - buffer, ogrEnv->MaxY + buffer);
		delete ogrEnv;
		return env;
	}
	
	Point* RSGISVectorUtils::createPoint(Coordinate *coord)
	{
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		return geosGeomFactory->createPoint(*coord);
	}
	
	bool RSGISVectorUtils::checkDIR4SHP(string dir, string shp) throw(RSGISVectorException)
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
			throw RSGISVectorException(e.what());
		}
		delete[] dirList;
		
		return returnVal;
	}
	
	void RSGISVectorUtils::deleteSHP(string dir, string shp) throw(RSGISVectorException)
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
			throw RSGISVectorException(e.what());
		}
		delete[] dirList;
	}

	
	GeometryCollection* RSGISVectorUtils::createGeomCollection(vector<Polygon*> *polys) throw(RSGISVectorException)
	{
		GeometryCollection *geom = NULL;
		try
		{
			vector<Geometry*> *geoms = new vector<Geometry*>();
			vector<Polygon*>::iterator iterPolys;
			for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
			{
				geoms->push_back((*iterPolys)->clone());
			}
			
			GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			geom = geosGeomFactory->createGeometryCollection(geoms);
		}
		catch(RSGISVectorException &e)
		{
			throw e;
		}
		
		return geom;
	}
	
	Polygon* RSGISVectorUtils::createPolygon(double tlX, double tlY, double brX, double brY) throw(RSGISVectorException)
	{
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		vector<Coordinate> *coords = new vector<Coordinate>();
		coords->push_back(Coordinate(tlX, tlY, 0));
		coords->push_back(Coordinate(brX, tlY, 0));
		coords->push_back(Coordinate(brX, brY, 0));
		coords->push_back(Coordinate(tlX, brY, 0));
		coords->push_back(Coordinate(tlX, tlY, 0));
		
		CoordinateArraySequence *coordSeq = new CoordinateArraySequence(coords);
		LinearRing *linearRingShell = new LinearRing(coordSeq, geosGeomFactory);
		
		Polygon *polygonGeom = geosGeomFactory->createPolygon(linearRingShell, NULL); 
		
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
	
	vector<string>* RSGISVectorUtils::findUniqueVals(OGRLayer *layer, string attribute) throw(RSGISVectorException)
	{
		vector<string>::iterator iterVals;
		vector<string> *values = new vector<string>();

		OGRFeature *feature = NULL;
		OGRFeatureDefn *featureDefn = layer->GetLayerDefn();
		
		bool found = false;
		
		layer->ResetReading();
		while( (feature = layer->GetNextFeature()) != NULL )
		{			
			string attrVal = feature->GetFieldAsString(featureDefn->GetFieldIndex(attribute.c_str()));
			
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
	
}}
