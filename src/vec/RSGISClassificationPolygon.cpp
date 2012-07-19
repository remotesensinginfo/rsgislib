/*
 *  RSGISClassificationPolygon.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2009.
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

#include "RSGISClassificationPolygon.h"

namespace rsgis{namespace vec{
	
	RSGISClassificationPolygon::RSGISClassificationPolygon()
	{
		
	}
	
	void RSGISClassificationPolygon::setClassification(std::string classification)
	{
		this->classification = classification;
	}
	
	std::string RSGISClassificationPolygon::getClassification()
	{
		return this->classification;
	}
	
	RSGISClassificationPolygon::~RSGISClassificationPolygon()
	{
	
	}
	
}}


