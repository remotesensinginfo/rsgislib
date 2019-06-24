/*
 *  RSGISVectorMorphology.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/08/2009.
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

#include "RSGISVectorMorphology.h"


namespace rsgis{namespace vec{
	
	RSGISVectorMorphology::RSGISVectorMorphology(float buffer, rsgis::geom::Morphology morphologytype)
	{
		this->buffer = buffer;
		this->morphologytype = morphologytype;
	}
	
	void RSGISVectorMorphology::processGeometry(OGRPolygon *polygon)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISVectorMorphology::processGeometry(OGRMultiPolygon *multiPolygon)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISVectorMorphology::processGeometry(OGRPoint *point)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISVectorMorphology::processGeometry(OGRLineString *line)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	OGRPolygon* RSGISVectorMorphology::processGeometry(OGRGeometry *geom)
	{
		OGRPolygon *polygon = NULL;
		if(morphologytype == rsgis::geom::closing)
		{
			OGRGeometry *geom1 = geom->Buffer(this->buffer, 30);
			polygon = (OGRPolygon *) geom1->Buffer((this->buffer*(-1)), 30);
			delete geom1;
		}
		else if(morphologytype == rsgis::geom::opening)
		{
			OGRGeometry *geom1 = geom->Buffer((this->buffer*(-1)), 30);
			polygon = (OGRPolygon *) geom1->Buffer(this->buffer, 30);
			delete geom1;
		}
		else if(morphologytype == rsgis::geom::dilation)
		{
			polygon = (OGRPolygon *) geom->Buffer(this->buffer, 30);
		}
		else if(morphologytype == rsgis::geom::erosion)
		{
			polygon = (OGRPolygon *) geom->Buffer((this->buffer*(-1)), 30);
		}
		else
		{
			throw RSGISVectorException("Did not recognise morphological operator.");
		}
		 
		return polygon;
	}
	
	RSGISVectorMorphology::~RSGISVectorMorphology()
	{
		
	}
}}


