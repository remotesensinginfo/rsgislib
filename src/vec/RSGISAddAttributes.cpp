/*
 *  RSGISAddAttributes.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/02/2009.
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

#include "RSGISAddAttributes.h"


namespace rsgis{namespace vec{
	
	
	RSGISAddAttributes::RSGISAddAttributes(Attribute **attributes, int numAttributes)
	{
		this->attributes = attributes;
		this->numAttributes = numAttributes;
	}
		
	void RSGISAddAttributes::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{
        // Do nothing - just adding column.
	}
	
	void RSGISAddAttributes::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISAddAttributes::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{
		for(int i = 0; i < numAttributes; i++)
		{
			OGRFieldDefn shpField(attributes[i]->name.c_str(), attributes[i]->type);
			if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating vector field") + attributes[i]->name + std::string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
	}
	
	
	RSGISAddAttributes::~RSGISAddAttributes()
	{

	}
	
}}


