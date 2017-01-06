/*
 *  RSGISRemoveContainedPolygons.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/03/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef RSGISRemoveContainedPolygons_H
#define RSGISRemoveContainedPolygons_H

#include <iostream>
#include <string>
#include <list>

#include "ogrsf_frmts.h"

#include "geos/geom/Envelope.h"
#include "geos/util/TopologyException.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorUtils.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_vec_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace vec{
	
	class DllExport RSGISRemoveContainedPolygons
	{
	public:
		RSGISRemoveContainedPolygons();
		long unsigned removeContainedPolygons(OGRLayer *input, OGRLayer *output)throw(RSGISVectorException);
		long unsigned removeContainedPolygons(OGRLayer *input, OGRLayer *output, std::vector<OGRPolygon*> *inputPolys)throw(RSGISVectorException);
		void copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		void copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn);
		~RSGISRemoveContainedPolygons();
	};
	
}}

#endif



