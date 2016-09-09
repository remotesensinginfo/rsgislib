/*
 *  RSGISModelTreeParameters.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 24/11/2009.
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

#include "RSGISModelTreeParameters.h"

namespace rsgis{ namespace modeling {

	RSGISAcaciaHarpophylla::RSGISAcaciaHarpophylla(double treeHeight)
	{
        rsgis::utils::RSGISAllometricEquations allometricEquation;
		
		double pi = 3.1415926536;
		
		double woodDensity = 0.870; // Wood density [g /cm^3]
		
		this->treeHeight = treeHeight;
		double treeHeightCm = treeHeight * 100;
		this->species = rsgis::utils::aHarpophylla;
		
		this->canopyDepth = 0.1130 + 0.3193*treeHeight;
		
		/* Calculate stem radius using allometric equations to calculate stem weight and using density
		* to obtain volume.
		*/
		
		this->stemHeight = treeHeight - canopyDepth;
		double stemBio = allometricEquation.calculateStemBiomassHeight(treeHeightCm, species);
				
        this->stemRadius = (-10.4297 + (11.8029 * exp(treeHeight / 10.0))) / 200.0;
		double stemVolume = (2*pi*stemRadius)*stemHeight;
        double stemMass = stemVolume / woodDensity;
        
        double largeBranchBioTree = stemBio - stemMass;
		
		
		// Calcualate branch radius from stem radius
		this->smallBranchRadius = 0.005;
		this->largeBranchRadius = 0.4 * stemRadius;
		
		// Calculate branch lenght from height
		this->smallBranchLength = 0.37;
		this->largeBranchLength = 0.1492 * treeHeight - 0.1301;
		
		// Calculate number of branches
		double largeBranchVol = pi * ((largeBranchRadius * 100) * (largeBranchRadius * 100)) * (largeBranchLength * 100);
        double largeBranchMass = (largeBranchVol / woodDensity);
		this->numLargeBranches = int((largeBranchBioTree / largeBranchMass)+0.5);

		double branchBio = allometricEquation.calculateBranchBiomassHeight(treeHeightCm, species);
		
		double smallBranchVol = pi * ((smallBranchRadius * 100) * (smallBranchRadius * 100)) * (smallBranchLength * 100);
        double smallBranchMass = (smallBranchVol / woodDensity);
		this->numSmallBranches = int(branchBio / smallBranchMass);

		
	}
	double RSGISAcaciaHarpophylla::getStemRadius()
	{
		return stemRadius;
	}
	double RSGISAcaciaHarpophylla::getLeafLength()
	{
		double leafLength = 0.08;
		return leafLength;
	}
	double RSGISAcaciaHarpophylla::getLeafWidth()
	{
		//double leafWidth = 0.009;
		double leafWidth = 0.01;
		return leafWidth;
	}
	double RSGISAcaciaHarpophylla::getLeafThickness()
	{
		double leafThickness = 0.0004296;
		return leafThickness;
	}
	int RSGISAcaciaHarpophylla::getNumLeaves()
	{
		double leafMass = 0.27; // Leaf mass [g]
		rsgis::utils::RSGISAllometricEquations allometricEquation;
		double leafBiomass = allometricEquation.calculateLeafBiomassHeight((treeHeight * 100), species);
		double numLeaves = leafBiomass / leafMass;
		return int(numLeaves + 0.5);
	}
	double RSGISAcaciaHarpophylla::getSmallBranchLength()
	{		
		return smallBranchLength;
	}
	double RSGISAcaciaHarpophylla::getSmallBranchRadius()
	{
		return smallBranchRadius;
	}
	int RSGISAcaciaHarpophylla::getNumSmallBranches()
	{
		if(numSmallBranches <= 0)
		{
			numSmallBranches = 1;
		}
		return numSmallBranches;
	}
	double RSGISAcaciaHarpophylla::getLargeBranchLength()
	{
		return largeBranchLength;
	}
	double RSGISAcaciaHarpophylla::getLargeBranchRadius()
	{
		return largeBranchRadius;
	}
	int RSGISAcaciaHarpophylla::getNumLargeBranches()
	{
		return numLargeBranches;
	}
	double RSGISAcaciaHarpophylla::getCanopyDepth()
	{
		return canopyDepth;
	}
	double RSGISAcaciaHarpophylla::getStemHeight()
	{
		return stemHeight; 
	}
	RSGISAcaciaHarpophylla::~RSGISAcaciaHarpophylla()
	{
		
	}
	
}}

