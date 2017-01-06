/*
 *  RSGISProcessOGRGeometry.h
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


#ifndef RSGISProcessOGRGeometry_H
#define RSGISProcessOGRGeometry_H

#include <iostream>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"

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
	
	class DllExport RSGISProcessOGRGeometry
		{
		public:
			RSGISProcessOGRGeometry(){};
			virtual void processGeometry(OGRPolygon *polygon) throw(RSGISVectorException)= 0;
			virtual void processGeometry(OGRMultiPolygon *multiPolygon) throw(RSGISVectorException)= 0;
			virtual void processGeometry(OGRPoint *point) throw(RSGISVectorException)= 0;
			virtual void processGeometry(OGRLineString *line) throw(RSGISVectorException)= 0;
			virtual OGRPolygon* processGeometry(OGRGeometry *geom) throw(RSGISVectorException)= 0;
			virtual ~RSGISProcessOGRGeometry(){};
		};
}}

#endif





