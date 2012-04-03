/*
 *  RSGISAllometricEquations.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 04/03/2009.
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

#ifndef RSGISAllometricEquations_H
#define RSGISAllometricEquations_H

#include <math.h>
#include <iostream>
#include <string>

#include "utils/RSGISAllometricSpecies.h"

using namespace std;
using namespace rsgis::utils;

namespace rsgis 
{
	namespace utils
	{
		class RSGISAllometricEquations
			{
			public:
				RSGISAllometricEquations();
				double calculateTotalBiomassHeight(double height, treeSpecies species);
				double calculateTotalBiomassDiameter(double diameter, treeSpecies species);
				double calculateLeafBiomassHeight(double height, treeSpecies species);
				double calculateBranchBiomassHeight(double height, treeSpecies species);
				double calculateStemBiomassHeight(double height, treeSpecies species);
				~RSGISAllometricEquations();
			private:
				treeSpecies species;
			};
	}
}

#endif
