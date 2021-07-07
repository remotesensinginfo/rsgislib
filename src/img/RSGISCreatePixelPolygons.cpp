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
	

	RSGISCreatePixelPolygons::RSGISCreatePixelPolygons(std::vector<OGRPolygon*> *polys, float threshold) : RSGISCalcImageValue(0)
	{
		this->polys = polys;
		this->threshold = threshold;
	}
	
	void RSGISCreatePixelPolygons::calcImageValue(float *bandValues, int numBands, OGREnvelope extent)
	{
		if(bandValues[0] < threshold)
		{
			polys->push_back(this->createPolygonFromEnv(extent));
		}
	}
	
	RSGISCreatePixelPolygons::~RSGISCreatePixelPolygons()
	{
		
	}
	
	OGRPolygon* RSGISCreatePixelPolygons::createPolygonFromEnv(OGREnvelope env)
	{
        OGRLinearRing *ogrRing = new OGRLinearRing();
        ogrRing->addPoint(env.MinX, env.MaxY, 0);
        ogrRing->addPoint(env.MaxX, env.MaxY, 0);
        ogrRing->addPoint(env.MaxX, env.MinY, 0);
        ogrRing->addPoint(env.MinX, env.MinY, 0);
        ogrRing->addPoint(env.MinX, env.MaxY, 0);

        OGRPolygon *ogrPoly = new OGRPolygon();
        ogrPoly->addRingDirectly(ogrRing);
		return ogrPoly;
	}

}}

