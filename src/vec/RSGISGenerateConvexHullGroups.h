/*
 *  RSGISGenerateConvexHullGroups.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/07/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
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

#ifndef RSGISGenerateConvexHullGroups_H
#define RSGISGenerateConvexHullGroups_H

#include <iostream>
#include <string>
#include <list>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "geom/RSGISGeometry.h"

#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISVectorIO.h"

#include "geos/geom/Coordinate.h"

#include "utils/RSGISTextUtils.h"

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
	
	class DllExport RSGISGenerateConvexHullGroups
	{
	public:
		RSGISGenerateConvexHullGroups();
        std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* >* getCoordinateGroups(std::string inputFile, unsigned int eastingsColIdx, unsigned int northingsColIdx, unsigned int attributeColIdx)throw(RSGISVectorException);
		void createPolygonsAsShapefile(std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* > *coordGrps, std::string outputFile, std::string outProj, bool force)throw(RSGISVectorException);
        
        ~RSGISGenerateConvexHullGroups();
	};
	
}}

#endif





