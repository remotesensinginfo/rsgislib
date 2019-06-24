/*
 *  RSGISPrintFIDs.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/03/2009.
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

#include "RSGISPrintFIDs.h"


namespace rsgis{namespace vec{
	
	RSGISPrintFIDs::RSGISPrintFIDs()
	{
		
	}
	
	void RSGISPrintFIDs::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{
		std::cout << "FID: " << fid << std::endl;
	}
	
	void RSGISPrintFIDs::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		std::cout << "FID: " << fid << std::endl;
	}
	
	void RSGISPrintFIDs::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{
		
	}
	RSGISPrintFIDs::~RSGISPrintFIDs()
	{
		
	}
}}

