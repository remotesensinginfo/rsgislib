/*
 *  RSGISModelTransect.h
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

#ifndef RSGISModelTransect_H
#define RSGISModelTransect_H

#include <vector>
#include <iostream>
#include "geos/geom/Polygon.h"

#include "math/RSGISRandomDistro.h"
#include "math/RSGISProbDistro.h"
#include "RSGISTransect.h"
#include "RSGISModelingException.h"
#include "RSGISModelAddVegetation.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_modeling_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace modeling  {
    
	/// Class to model a transect
	/**  
	 * Takes a transect and adds vegetaion based on the 'RSGISModelAddVegetation' abstract class.
	 * Currently there is only a single size, single species implementation to add vegetation to the transct 
	 */
	class DllExport RSGISModelTransect
	{
	public:
		RSGISModelTransect(RSGISTransect *transect);
		/** populateSingleSizeSpecies - Add a single species, single size tree to a transect
		 *  quadratSize - The number length of a single quadrat used to determine the location of trees
		 *  vegDistro - Probability distrobution used to describe how trees are distrobuted along the transect, i.e. random (uniform distro),
		 *  centered in the middle of the transect (gaussian), clumped together (Neyman type A).
		 *  treePoxXDistro - Random number sampling from distrobution to describe the location of trees within a quadrat
		 *  treePoxYDistro - Random number sampling from distrobution to describe the location of trees within a quadrat
		 *  addVeg - Class to add vegetation to transect
		 */
		void createConvexHull(std::vector<geos::geom::Polygon*> *canopyPoly);
		void populateSingleSizeSpecies(double quadratSize, unsigned int numTrees, rsgis::math::RSGISProbDistro *vegDistro, rsgis::math::RSGISProbDistro *treePosXDistro, rsgis::math::RSGISProbDistro *treePosYDistro, RSGISModelAddVegetation *addVeg);
		~RSGISModelTransect();
	private:
		RSGISTransect *transect;
		unsigned int transectLenght;
		unsigned int transectWidth;
		unsigned int transectHeight;
		double transectRes;
        std::vector<geos::geom::Polygon*> *canopyPoly;
		bool convexHull;
	};
}}

#endif
