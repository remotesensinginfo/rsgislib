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
	

	RSGISCreatePixelPolygons::RSGISCreatePixelPolygons(std::vector<geos::geom::Polygon*> *polys, float threshold) : RSGISCalcImageValue(0)
	{
		this->polys = polys;
		this->threshold = threshold;
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float *bandValues, int numBands) 
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) 
	{
		if(bandValues[0] < threshold)
		{
			polys->push_back(this->createPolygonFromEnv(extent));
		}
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) 
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	bool RSGISCreatePixelPolygons::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	RSGISCreatePixelPolygons::~RSGISCreatePixelPolygons()
	{
		
	}
	
	geos::geom::Polygon* RSGISCreatePixelPolygons::createPolygonFromEnv(geos::geom::Envelope env)
	{
        std::vector<geos::geom::Coordinate> *coords = new std::vector<geos::geom::Coordinate>();
		
		coords->push_back(geos::geom::Coordinate(env.getMaxX(), env.getMaxY(), 0));
		coords->push_back(geos::geom::Coordinate(env.getMaxX(), env.getMinY(), 0));
		coords->push_back(geos::geom::Coordinate(env.getMinX(), env.getMinY(), 0));
		coords->push_back(geos::geom::Coordinate(env.getMinX(), env.getMaxY(), 0));
		coords->push_back(geos::geom::Coordinate(env.getMaxX(), env.getMaxY(), 0));
		
		geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence(coords);
		
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		geos::geom::LinearRing *linearRingShell = new geos::geom::LinearRing(coordSeq, geosGeomFactory);
		geos::geom::Polygon *polygonGeom = geosGeomFactory->createPolygon(linearRingShell, NULL);
		
		return polygonGeom;
	}

}}

