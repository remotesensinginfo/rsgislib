/*
 *  RSGISGetOGRGeometries.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/08/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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

#include "RSGISGetOGRGeometries.h"

namespace rsgis{namespace vec{
	

    RSGISGetOGRGeometries::RSGISGetOGRGeometries(std::vector<OGRGeometry*> *geometries):RSGISProcessOGRFeature()
    {
        this->geometries = geometries;
    }
    
	void RSGISGetOGRGeometries::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        throw RSGISVectorException("Not implemented..");
    }
    
	void RSGISGetOGRGeometries::processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
    {
        geometries->push_back(feature->GetGeometryRef()->clone());
    }
		
    void RSGISGetOGRGeometries::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
    {
        // Do nothing...
    }
    
    RSGISGetOGRGeometries::~RSGISGetOGRGeometries()
    {
        
    }
    
}}
