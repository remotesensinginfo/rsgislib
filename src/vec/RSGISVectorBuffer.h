/*
 *  RSGISVectorBuffer.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/11/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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


#ifndef RSGISVectorBuffer_H
#define RSGISVectorBuffer_H

#include <iostream>
#include <fstream>
#include <string>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRGeometry.h"

#include "math/RSGISMathsUtils.h"

#include "geos/geom/Coordinate.h"

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
	
	class DllExport RSGISVectorBuffer : public RSGISProcessOGRGeometry
		{
		public:
			RSGISVectorBuffer(float buffer);
			virtual void processGeometry(OGRPolygon *polygon) throw(RSGISVectorException);
			virtual void processGeometry(OGRMultiPolygon *multiPolygon) throw(RSGISVectorException);
			virtual void processGeometry(OGRPoint *point) throw(RSGISVectorException);
			virtual void processGeometry(OGRLineString *line) throw(RSGISVectorException);
			virtual OGRPolygon* processGeometry(OGRGeometry *geom) throw(RSGISVectorException);
			virtual ~RSGISVectorBuffer();
		protected:
			float buffer;
		};
}}


#endif



