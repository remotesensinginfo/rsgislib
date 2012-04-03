/*
 *  RSGISGetAttributeValues.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/08/2009.
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

#include "RSGISGetAttributeValues.h"

namespace rsgis{namespace vec{
	
	RSGISGetAttributeValues::RSGISGetAttributeValues(list<double> *values, string attribute)
	{
		this->values = values;
		this->attribute = attribute;
	}
	
	void RSGISGetAttributeValues::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISGetAttributeValues::processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		OGRFeatureDefn *featureDefn = feature->GetDefnRef();
		int fieldIdx = featureDefn->GetFieldIndex(attribute.c_str());
		if(fieldIdx < 0)
		{
			string message = "This layer does not contain a field with the name \'" + attribute + "\'";
			throw RSGISVectorException(message.c_str());
		}
		
		values->push_back(feature->GetFieldAsDouble(fieldIdx));
	}
	
	void RSGISGetAttributeValues::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		// Nothing to do!
	}
	
	RSGISGetAttributeValues::~RSGISGetAttributeValues()
	{

	}
}}


