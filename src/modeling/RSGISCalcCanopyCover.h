 /*
 *  RSGISCalcCanopyCover.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 18/01/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#ifndef RSGISCalcCanopyCover_H
#define RSGISCalcCanopyCover_H

#include "geos/geom/Coordinate.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/CoordinateArraySequence.h"

#include "modeling/RSGISTransect.h"
#include "geom/RSGISGeometry.h"
#include "vec/RSGISVectorIO.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_modeling_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis { namespace modeling {

	class DllExport RSGISCalcCanopyCover
	{
		/** Class to calculate the percent crown cover from a transect.
		 * Crown cover (CC) is defined as the percentage of ground area covered by 
		 * the vertical projection of crowns (Scarth et. al., 2008).
		 * 
		 * When adding vegetation to the transect the addVegTransConvexHull option
		 * must be used, which calculates the convex hull of each canopy and
		 * save as a vector of geos polygons
		 *
		 */
	public:
		RSGISCalcCanopyCover(RSGISTransect *transect, std::vector<geos::geom::Polygon*> *canopyPoly);
		double calcCanopyCover();
		void exportCanopyPoly(std::string outFile);
		~RSGISCalcCanopyCover();
	private:
		RSGISTransect *transect;
        std::vector<geos::geom::Polygon*> *canopyPoly;
	};
	
}}

#endif


