/*
 *  RSGISAllometricEquations.cpp
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

#include "RSGISAllometricEquations.h"

namespace rsgis{namespace utils
{
	RSGISAllometricEquations::RSGISAllometricEquations()
	{
		
	}
	double RSGISAllometricEquations::calculateTotalBiomassHeight(double height, treeSpecies species)
	{
		double biomass = 0;
		if(species == aHarpophylla)
		{
			/**
			* Calculate Biomass using the equations of Scanlan. 
			* Scanlan. Woody overstory and herbaceous understory in Acacia harpophylla (brigalow) woodlands. Australian Journal of Botany (1991) vol. 16 (521 - 529)
			*/ 
			biomass = exp(-4.303+(2.15*log(height)))*exp(0.311/2);
		}
		else
		{
			std::cout << "Species not recognised" << std::endl;
		}
		return biomass;
	}
	
	double RSGISAllometricEquations::calculateTotalBiomassDiameter(double diameter, treeSpecies species)
	{
		double biomass = 0;
		double pi = 3.141592653589793;
		if(species == aHarpophylla)
		{
			/**
			 * Calculate Biomass using the equations of Scanlan.
			 * Function converts diameter (in mm) into circumference
			 * Scanlan. Woody overstory and herbaceous understory in Acacia harpophylla (brigalow) woodlands. Australian Journal of Botany (1991) vol. 16 (521 - 529)
			 */ 
			
			double circumference = diameter * pi;
			biomass = exp(-3.568+(2.384*log(circumference)))*exp(0.062/2);
			
		}
		else
		{
			std::cout << "Species not recognised" << std::endl;
		}
		return biomass;
	}
	
	double RSGISAllometricEquations::calculateLeafBiomassHeight(double height, treeSpecies species)
	{
		double biomass = 0;
		if(species == aHarpophylla)
		{
			/**
			 * Calculate Biomass using the equations of Scanlan. 
			 * Scanlan. Woody overstory and herbaceous understory in Acacia harpophylla (brigalow) woodlands. Australian Journal of Botany (1991) vol. 16 (521 - 529)
			 */
			
			biomass = exp(-2.84+(1.629*log(height)))*exp(0.493/2);
		}
		else
		{
			std::cout << "Species not recognised" << std::endl;
		}
		return biomass;
	}
	
	double RSGISAllometricEquations::calculateBranchBiomassHeight(double height, treeSpecies species)
	{
		double biomass = 0;
		if(species == aHarpophylla)
		{
			/**
			 * Calculate Biomass using the equations of Scanlan. 
			 * Scanlan. Woody overstory and herbaceous understory in Acacia harpophylla (brigalow) woodlands. Australian Journal of Botany (1991) vol. 16 (521 - 529)
			 */
			biomass = exp(-4.056+(1.805*log(height)))*exp(0.401/2);
		}
		else
		{
			std::cout << "Species not recognised" << std::endl;
		}
		return biomass;
	}
	
	double RSGISAllometricEquations::calculateStemBiomassHeight(double height, treeSpecies species)
	{
		double biomass = 0;
		if(species == aHarpophylla)
		{
			/**
			 * Calculate Biomass using the equations of Scanlan. 
			 * Scanlan. Woody overstory and herbaceous understory in Acacia harpophylla (brigalow) woodlands. Australian Journal of Botany (1991) vol. 16 (521 - 529)
			 */
			biomass = exp(-7.49+(2.611*log(height)))*exp(0.314/2);
		}
		else
		{
			std::cout << "Species not recognised" << std::endl;
		}
		return biomass;
	}
	
	RSGISAllometricEquations::~RSGISAllometricEquations()
	{
		
	}
}}


