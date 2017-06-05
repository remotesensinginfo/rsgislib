/*
 *  RSGISCalcCanopyCover.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 18/01/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISCalcCanopyCover.h"

namespace rsgis { namespace modeling {
	
	RSGISCalcCanopyCover::RSGISCalcCanopyCover(RSGISTransect *transect, std::vector<geos::geom::Polygon*> *canopyPoly)
	{
		this->transect = transect;
		this->canopyPoly = canopyPoly;
	}
	double RSGISCalcCanopyCover::calcCanopyCover()
	{
		double canopyCover = 0;
		double canopyArea = 0;
		double transectArea = transect->getWidth() * transect->getLenth();
		
        rsgis::geom::RSGISGeometry geometry;
		
		// Merge polygons
		geometry.mergeTouchingPolygonsForce(canopyPoly);
		
		// Calculate Area
		for (unsigned int i = 0; i < canopyPoly->size(); i++) 
		{
			canopyArea = canopyArea + canopyPoly->at(i)->getArea();
		}
		canopyCover = (canopyArea / transectArea) * 100;
						
		return canopyCover;
	}
	void RSGISCalcCanopyCover::exportCanopyPoly(std::string outFile)
	{
		rsgis::vec::RSGISVectorIO vectorIO;
		// Write to shapefile
		vectorIO.exportGEOSPolygons2SHP(outFile, true, canopyPoly);
	}
	RSGISCalcCanopyCover::~RSGISCalcCanopyCover()
	{
		
	}
}}

