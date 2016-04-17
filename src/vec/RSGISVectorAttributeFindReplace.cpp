/*
 *  RSGISVectorAttributeFindReplace.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/02/2009.
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

#include "RSGISVectorAttributeFindReplace.h"

namespace rsgis{namespace vec{
	
	
	RSGISVectorAttributeFindReplace::RSGISVectorAttributeFindReplace(std::string attribute, std::string find, std::string replace)
	{
		this->attribute = attribute;
		this->find = find;
		this->replace = replace;
	}
	
	void RSGISVectorAttributeFindReplace::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISVectorAttributeFindReplace::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		OGRFeatureDefn *featureDefn = feature->GetDefnRef();
		int fieldIdx = featureDefn->GetFieldIndex(attribute.c_str());
		if(fieldIdx < 0)
		{
			std::string message = "This layer does not contain a field with the name \'" + attribute + "\'";
			throw RSGISVectorException(message.c_str());
		}
        OGRFieldDefn *fieldDef = featureDefn->GetFieldDefn(fieldIdx);
        OGRFieldType fieldType = fieldDef->GetType();
        
        rsgis::utils::RSGISTextUtils textUtils = rsgis::utils::RSGISTextUtils();
        
        //std::cout << "fieldType = " << fieldType << std::endl;
        
        if(fieldType == OFTInteger)
        {
            //std::cout << "Integer\n";
            int findInt = textUtils.strtoInt(find);
            int replaceInt = textUtils.strtoInt(replace);
            
            //std::cout << "Test to see if \'" << feature->GetFieldAsInteger(fieldIdx) << "\' is equal to \'" << findInt << "\' if replace with \'" << replaceInt << "\'\n";
            
            if(feature->GetFieldAsInteger(fieldIdx) == findInt)
            {
                //std::cout << "Replacing \'" << findInt << "\' with \'" << replaceInt << "\'\n";
                feature->SetField(fieldIdx, replaceInt);
            }
        }
        else if(fieldType == OFTReal)
        {
            //std::cout << "Float\n";
            double findFloat = textUtils.strtofloat(find);
            double replaceFloat = textUtils.strtofloat(replace);
            
            //std::cout << "Test to see if \'" << feature->GetFieldAsInteger(fieldIdx) << "\' is equal to \'" << findFloat << "\' if replace with \'" << replaceFloat << "\'\n";
            
            if(feature->GetFieldAsDouble(fieldIdx) == findFloat)
            {
                //std::cout << "Replacing \'" << findFloat << "\' with \'" << replaceFloat << "\'\n";
                feature->SetField(fieldIdx, replaceFloat);
            }
        }
        else
        {
            //std::cout << "Other / String\n";
            if(std::string(feature->GetFieldAsString(fieldIdx)) == find)
            {
                feature->SetField(fieldIdx, replace.c_str());
            }
        }
			
	}
	
	void RSGISVectorAttributeFindReplace::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		
	}
	
	RSGISVectorAttributeFindReplace::~RSGISVectorAttributeFindReplace()
	{
		
	}
	
}}


