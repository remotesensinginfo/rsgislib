/*
 *  RSGISCreatePixelPolygons.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/05/2010.
 *  Copyright 2010 RSGISLib.
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

#include "RSGISCreatePixelPolygons.h"


namespace rsgis{namespace img{
	

	RSGISCreatePixelPolygons::RSGISCreatePixelPolygons(vector<Polygon*> *polys, float threshold) : RSGISCalcImageValue(0)
	{
		this->polys = polys;
		this->threshold = threshold;
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException)
	{
		if(bandValues[0] < threshold)
		{
			polys->push_back(this->createPolygonFromEnv(extent));
		}
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	bool RSGISCreatePixelPolygons::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	RSGISCreatePixelPolygons::~RSGISCreatePixelPolygons()
	{
		
	}
	
	Polygon* RSGISCreatePixelPolygons::createPolygonFromEnv(Envelope env)
	{
		vector<Coordinate> *coords = new vector<Coordinate>();
		
		coords->push_back(Coordinate(env.getMaxX(), env.getMaxY(), 0));
		coords->push_back(Coordinate(env.getMaxX(), env.getMinY(), 0));
		coords->push_back(Coordinate(env.getMinX(), env.getMinY(), 0));
		coords->push_back(Coordinate(env.getMinX(), env.getMaxY(), 0));
		coords->push_back(Coordinate(env.getMaxX(), env.getMaxY(), 0));
		
		CoordinateArraySequence *coordSeq = new CoordinateArraySequence(coords);
		
		GeometryFactory* geosGeomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		LinearRing *linearRingShell = new LinearRing(coordSeq, geosGeomFactory);
		Polygon *polygonGeom = geosGeomFactory->createPolygon(linearRingShell, NULL);
		
		return polygonGeom;
	}

}}

