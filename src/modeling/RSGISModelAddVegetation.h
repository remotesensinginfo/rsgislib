/*
 *  RSGISModelAddVegetation.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 28/10/2009.
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


#ifndef RSGISModelAddVegetation_H
#define RSGISModelAddVegetation_H


#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateArraySequence.h"

#include "geom/RSGISGeometry.h"
#include "vec/RSGISVectorIO.h"
#include "math/RSGISRandomDistro.h"
#include "math/RSGISProbDistro.h"
#include "RSGISTransect.h"
#include "RSGISModelingException.h"
#include "RSGISModelAddVegetation.h"

#include "modeling/RSGISTransect.h"

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

namespace rsgis{namespace modeling{
    
	class DllExport RSGISModelAddVegetation
	{
	/** Abstract class to model vegetation
	 *  Passed into RSGISModelTransect to add vegetation to a transect
	 */
	public:
		RSGISModelAddVegetation(){};
		virtual void createVeg() = 0;
		virtual void addVegTrans(RSGISTransect *transect, unsigned int centerX, unsigned int centerY, unsigned int sizeX, unsigned int sizeY) = 0;
		virtual void addVegTransConvexHull(RSGISTransect *transect, unsigned int centerX, unsigned int centerY, unsigned int sizeX, unsigned int sizeY, std::vector<geos::geom::Polygon*> *canopyPolys) = 0;
		virtual ~RSGISModelAddVegetation(){};
	};
}}

#endif

