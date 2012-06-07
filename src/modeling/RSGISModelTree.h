/*
 *  RSGISModelTree.h
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


#ifndef RSGISModelTree_H
#define RSGISModelTree_H

#include "modeling/RSGISModelAddVegetation.h"
#include "modeling/RSGISTransect.h"
#include "modeling/RSGISModelingException.h"
#include "math/RSGISProbDistro.h"

namespace rsgis { namespace modeling{
    
    using namespace std;
    using namespace rsgis::math;
    
	class RSGISModelTree : public RSGISModelAddVegetation
	{
	public:
		RSGISModelTree(vector<double> *modelPar, 
					   RSGISProbDistro *leafPosHDistro, RSGISProbDistro *leafPosVDistro, RSGISProbDistro *leafThetaDistro,
					   RSGISProbDistro *smallBranchPosHDistro, RSGISProbDistro *smallBranchPosVDistro, RSGISProbDistro *smallBranchThetaDistro, RSGISProbDistro *smallBranchPhiDistro, 
					   RSGISProbDistro *largeBranchPosHDistro, RSGISProbDistro *largeBranchPosVDistro, RSGISProbDistro *largeBranchThetaDistro, RSGISProbDistro *largeBranchPhiDistro);
		virtual void createVeg(){};
		virtual void addVegTrans(RSGISTransect *transect, unsigned int centerX, unsigned int centerY, unsigned int sizeX, unsigned int sizeY);
		virtual void addVegTransConvexHull(RSGISTransect *transect, unsigned int centerX, unsigned int centerY, 
										   unsigned int sizeX, unsigned int sizeY, vector<Polygon*> *canopyPolys);
		virtual ~RSGISModelTree();
	private:
		double stemHeight, stemRadius, canopyDepth;
		double leafLenght, leafWidth, leafThickness, leafDensity;
		double smallBranchLenght, smallBranchRadius, smallBranchDensity;
		double largeBranchLenght, largeBranchRadius, largeBranchDensity;
		RSGISProbDistro *leafPosHDistro, *leafPosVDistro, *leafThetaDistro;
		RSGISProbDistro *smallBranchPosHDistro, *smallBranchPosVDistro, *smallBranchThetaDistro, *smallBranchPhiDistro;
		RSGISProbDistro *largeBranchPosHDistro, *largeBranchPosVDistro, *largeBranchThetaDistro, *largeBranchPhiDistro;
	};
	
}}

#endif

