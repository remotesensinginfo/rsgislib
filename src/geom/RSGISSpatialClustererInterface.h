/*
 *  RSGISSpatialClustererInterface.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/08/2009.
 *  Copyright 2009 RSGISLib.
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

#ifndef RSGISSpatialClustererInterface_H
#define RSGISSpatialClustererInterface_H

#include <iostream>
#include <string>
#include <list>

#include "geom/RSGIS2DPoint.h"

#include "math/RSGISClustererException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_geom_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace geom{
	/// Abstract interface to implement Spatial Clusterers
	class DllExport RSGISSpatialClustererInterface
		{
		public:
			virtual std::list<RSGIS2DPoint*>** clusterData(std::vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold) throw(rsgis::math::RSGISClustererException) = 0;
			virtual ~RSGISSpatialClustererInterface(){};
		};
}}

#endif




