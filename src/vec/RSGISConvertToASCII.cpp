/*
 *  RSGISConvertToASCII.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/04/2009.
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

#include "RSGISConvertToASCII.h"

namespace rsgis{namespace vec{
	
	RSGISConvertToASCII::RSGISConvertToASCII(std::string outputTextFile)
	{
		this->outputTextFile = outputTextFile;
		
		outTxtFile.open(outputTextFile.c_str(), std::ios::out | std::ios::trunc);
	}
	
	void RSGISConvertToASCII::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISConvertToASCII::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		if(outTxtFile.is_open())
		{
			geometry = feature->GetGeometryRef();
			if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
			{
				point = (OGRPoint *) geometry;
			} 
			else
			{
				throw RSGISVectorException("Unsupport data type.");
			}
			outTxtFile.precision(10);
			outTxtFile << fid << "," << point->getX() << "," << point->getY() << "," << point->getZ() ;
			featureDefn = feature->GetDefnRef();
			int fieldCount = featureDefn->GetFieldCount();
			for(int i = 0; i < fieldCount; i++)
			{
				if(i != (fieldCount-1))
				{
					outTxtFile << ",";
				}
				outTxtFile << feature->GetFieldAsString(featureDefn->GetFieldIndex(featureDefn->GetFieldDefn(i)->GetNameRef()));
			}
			outTxtFile << std::endl;
			outTxtFile.flush();
			//cout << "FID = " << fid << endl;
		}
		else
		{
			throw RSGISVectorException("Output Text File was not open.");
		}
	}
	
	void RSGISConvertToASCII::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		// Nothing to do!
	}
	
	RSGISConvertToASCII::~RSGISConvertToASCII()
	{
		outTxtFile.flush();
		outTxtFile.close();
	}
}}


