/*
 *  RSGISCalcFPC.h
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


#ifndef RSGISCalcFPC_H
#define RSGISCalcFPC_H

#include <gsl/gsl_rng.h>
#include "modeling/RSGISTransect.h"

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

namespace rsgis { namespace modeling {
        
	/*
	 * Class to calculate FPC from a transect by replicating the field sampling stratergy used by SLATS.<br>
	 * The field techniqe for obtaining FPC is equation one in the following paper:<br>
	 * Armston et al. Prediction and validation of foliage projective cover from Landsat-5 TM and Landsat-7 ETM+ imagery. <br>
	 * J. Appl. Remote Sens. (2009) vol. 3 (1) pp. 033540<br>
	 * Takes RSGISTransect, to which trees have been added.<br>
	 */
	class DllExport RSGISCalcFPC
	{
	public:
		RSGISCalcFPC(RSGISTransect *transect);
		/// Calculate FPC by randomly sampling points
		double calcFPCGroundRand(unsigned int nMeas, double randSeed);
		/// Calculate FPC by sampling point along the lenght (y) of the transect in the center (sizeX / 2)
		double calcFPCGroundTrans(unsigned int spaceing);
		/// Calculate FPC by measuring all points in transect
		double calcFPCGroundAll();
		/// Calculate FPC by randomly sampling points
		double calcFPCHeightRand(unsigned int nMeas, double randSeed);
		/// Calculate FPC by sampling point along the lenght (y) of the transect in the center (sizeX / 2)
		double calcFPCHeightTrans(unsigned int spaceing);
		/// Calculate FPC by randomly sampling points
		double calcFPCTopRand(unsigned int nMeas, double randSeed);
		/// Calculate FPC by sampling point along the lenght (y) of the transect in the center (sizeX / 2)
		double calcFPCTopTrans(unsigned int spaceing);
		~RSGISCalcFPC();
	private:
		RSGISTransect *transect;
	};
	
}}

#endif

