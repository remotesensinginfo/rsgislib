/*
 *  RSGISGEOSMultiPolygonReader.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/09/2009.
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

#ifndef RSGISGEOSMultiPolygonReader_H
#define RSGISGEOSMultiPolygonReader_H

#include <iostream>
#include <string>
#include <vector>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISVectorUtils.h"

#include "geom/RSGISGeometry.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/MultiPolygon.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
	
	class DllExport RSGISGEOSMultiPolygonReader : public RSGISProcessOGRFeature
	{
	public:
		RSGISGEOSMultiPolygonReader(std::vector<geos::geom::MultiPolygon*> *mPolygons);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
		virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
		virtual ~RSGISGEOSMultiPolygonReader();
	protected:
		RSGISVectorUtils *vecUtils;
        std::vector<geos::geom::MultiPolygon*> *mPolygons;
	};
}}

#endif





