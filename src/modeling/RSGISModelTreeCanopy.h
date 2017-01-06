/*
 *  RSGISModelTreeCanopy.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 10/11/2009.
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


#ifndef RSGISModelTreeCanopy_H
#define RSGISModelTreeCanopy_H

#include <vector>
#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateArraySequence.h"

#include "modeling/RSGISModelAddVegetation.h"
#include "modeling/RSGISTransect.h"
#include "math/RSGISProbDistro.h"
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

namespace rsgis { namespace modeling{
    
	/// Class to create tree canopy
	/**
	 * Class to create a tree canopy based on statistical distrobutions and tree parameters<br>
	 * similar to those required by the radar backscatter model of Durden et al (1989)<br>
	 * Parametamerers are input in the form of a vectors, and a number of RSGISProbDisros:<br>
	 * The vector of model parameters must contain:<br>
	 * Leaf lenght <br>
	 * Leaf width <br>
	 * Leaf thickness <br>
	 * Number of leaves <br>
	 * Small branch lenght <br>
	 * Small branch radius <br>
	 * Number of small branches <br>
	 * Large branch lenght <br>
	 * Large branch radius <br>
	 * Number of large branches <br>
	 * The required statistical distrobutions are: <br>
	 * Distribution of leaf loctions (horizonal) <br>
	 * Distribution of leaf loctions (vertical) <br>
	 * Distribution of leaf incidence angles <br>
	 * Distribution of small branch locations (horizontal) <br>
	 * Distribution of small branch locations (vertical) <br>
	 * Distribution of small branch indidence angles <br>
	 * Distribution of small branch angles into the incidence plane <br>
	 * Distribution of large branch positions (horizonal) <br>
	 * Distribution of large branch positions (vertical) <br>
	 * Distribution of large branch incidence angles <br>
	 * Distribution of large branch angles into the incidence plane <br>
	 *
	 */
	class DllExport RSGISModelTreeCanopy : public RSGISModelAddVegetation
	{
	public:
		RSGISModelTreeCanopy(std::vector<double> *modelPar, rsgis::math::RSGISProbDistro *leafPosHDistro, rsgis::math::RSGISProbDistro *leafPosVDistro, rsgis::math::RSGISProbDistro *leafThetaDistro,
							 rsgis::math::RSGISProbDistro *smallBranchPosHDistro, rsgis::math::RSGISProbDistro *smallBranchPosVDistro, rsgis::math::RSGISProbDistro *smallBranchThetaDistro, rsgis::math::RSGISProbDistro *smallBranchPhiDistro, 
							 rsgis::math::RSGISProbDistro *largeBranchPosHDistro, rsgis::math::RSGISProbDistro *largeBranchPosVDistro, rsgis::math::RSGISProbDistro *largeBranchThetaDistro, rsgis::math::RSGISProbDistro *largeBranchPhiDistro);
		virtual void createVeg(){};
		virtual void addVegTrans(RSGISTransect *transect, unsigned int centerX, unsigned int centerY, unsigned int sizeX, unsigned int sizeY);
		virtual void addVegTransConvexHull(RSGISTransect *transect, unsigned int centerX, unsigned int centerY, unsigned int sizeX, unsigned int sizeY, std::vector<geos::geom::Polygon*> *canopyPolys);
		virtual ~RSGISModelTreeCanopy();
	private:
		double leafLenght, leafWidth, leafThickness, leafDensity;
		double smallBranchLenght, smallBranchRadius, smallBranchDensity;
		double largeBranchLenght, largeBranchRadius, largeBranchDensity;
		rsgis::math::RSGISProbDistro *leafPosHDistro, *leafPosVDistro, *leafThetaDistro;
		rsgis::math::RSGISProbDistro *smallBranchPosHDistro, *smallBranchPosVDistro, *smallBranchThetaDistro, *smallBranchPhiDistro;
		rsgis::math::RSGISProbDistro *largeBranchPosHDistro, *largeBranchPosVDistro, *largeBranchThetaDistro, *largeBranchPhiDistro;
	};
	
}}

#endif

