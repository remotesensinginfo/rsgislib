 /*
 *  RSGISModelTreeParameters.h
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

#ifndef RSGISModelTreeParameters_H
#define RSGISModelTreeParameters_H

#include "utils/RSGISAllometricSpecies.h"
#include "utils/RSGISAllometricEquations.h"
#include "math/RSGISProbDistro.h"
#include "math/RSGISRandomDistro.h"

namespace rsgis { namespace modeling {
    
	/// Abstract tree class
	class DllExport RSGISTree 
	{
	public:
		RSGISTree(){};
		virtual double getStemRadius() = 0;
		virtual double getLeafLength() = 0;
		virtual double getLeafWidth() = 0;
		virtual double getLeafThickness() = 0;
		virtual int getNumLeaves() = 0;
		virtual double getSmallBranchLength() = 0;
		virtual double getSmallBranchRadius() = 0;
		virtual int getNumSmallBranches() = 0;
		virtual double getLargeBranchLength() = 0;
		virtual double getLargeBranchRadius() = 0;
		virtual int getNumLargeBranches() = 0;
		virtual double getCanopyDepth() = 0;
		virtual double getStemHeight() = 0;
		virtual ~RSGISTree(){}
	};
	
	/** Class for estimating parameters for <i>Acacia Harpophylla</i> based on
	 *  allometric equations and parameters derived from fieldwork
	 *  See:
	 *  Lucas et al. Integration of radar and Landsat-derived foliage projected cover for woody regrowth mapping, 
	 *  Queensland, Australia. Remote Sensing of Environment (2006) vol. 100 (3) pp. 388-406
	 *  and:
	 *  Dwyer et al. Carbon for conservation: Assessing the potential for win–win investment in an extensive …. Agriculture (2009)
	 *  http://linkinghub.elsevier.com/retrieve/pii/S0167880909001741
	 */
	class DllExport RSGISAcaciaHarpophylla : public RSGISTree 
	{
	public:
		RSGISAcaciaHarpophylla(double treeHeight);
		virtual double getStemRadius();
		virtual double getLeafLength();
		virtual double getLeafWidth();
		virtual double getLeafThickness();
		virtual int getNumLeaves();
		virtual double getSmallBranchLength();
		virtual double getSmallBranchRadius();
		virtual int getNumSmallBranches();
		virtual double getLargeBranchLength();
		virtual double getLargeBranchRadius();
		virtual int getNumLargeBranches();
		virtual double getCanopyDepth();
		virtual double getStemHeight();
		virtual ~RSGISAcaciaHarpophylla();
	public:
		double treeHeight;
		double stemRadius, stemHeight;
		double canopyDepth;
		double smallBranchRadius, largeBranchRadius;
		double smallBranchLength, largeBranchLength;
		int numLargeBranches, numSmallBranches;
        rsgis::utils::treeSpecies species;
	};
}}

#endif

