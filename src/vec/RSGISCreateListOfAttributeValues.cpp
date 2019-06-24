/*
 *  RSGISCreateListOfAttributeValues.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/02/2010.
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

#include "RSGISCreateListOfAttributeValues.h"

namespace rsgis{namespace vec{
	
	RSGISCreateListOfAttributeValues::RSGISCreateListOfAttributeValues(std::vector<std::string> *attributes, std::string attribute)
	{
		this->attribute = attribute;
		this->attributes = attributes;
	}
		
	void RSGISCreateListOfAttributeValues::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{
		throw RSGISVectorException("Not Implemented...");
	}
	
	void RSGISCreateListOfAttributeValues::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		OGRFeatureDefn *featureDefn = feature->GetDefnRef();
		int fieldIdx = featureDefn->GetFieldIndex(attribute.c_str());		
		if(fieldIdx < 0)
		{
			std::string message = "This layer does not contain a field with the name \'" + attribute + "\'";
			throw RSGISVectorException(message.c_str());
		}
		std::string value = feature->GetFieldAsString(fieldIdx);
		
		std::vector<std::string>::iterator iterAttr;
		bool present = false;
		for(iterAttr = attributes->begin(); iterAttr != attributes->end(); ++iterAttr)
		{
			if((*iterAttr) == value)
			{
				present = true;
				break;
			}
		}
		
		if(!present)
		{
			attributes->push_back(value);
		}
	}
	
	void RSGISCreateListOfAttributeValues::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{
		// Do nothing
	}
	
	RSGISCreateListOfAttributeValues::~RSGISCreateListOfAttributeValues()
	{
		
	}
}}


