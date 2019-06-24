/*
 *  RSGISProcessFeatureCopyVector.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/03/2009.
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

#include "RSGISProcessFeatureCopyVector.h"

namespace rsgis{namespace vec{
	
	RSGISProcessFeatureCopyVector::RSGISProcessFeatureCopyVector()
	{
		
	}
	
	void RSGISProcessFeatureCopyVector::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)
	{
		// Nothing to do - just copying the data.
	}
	
	void RSGISProcessFeatureCopyVector::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISProcessFeatureCopyVector::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn)
	{
		// Nothing to do!
	}
	
	RSGISProcessFeatureCopyVector::~RSGISProcessFeatureCopyVector()
	{
		
	}
}}


