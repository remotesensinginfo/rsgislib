 /*
 *  RSGISEstimationOptimiser.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 13/04/2010.
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

#ifndef RSGISEstimationOptimiser_H
#define RSGISEstimationOptimiser_H

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_radar_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis {namespace radar{
    
	enum estOptimizerType
	{
		conjugateGradient,
		simulatedAnnealing,
		threasholdAccepting,
		exhaustiveSearch,
		assignAP,
        noOptimiser,
		unknown
	};
	
	class DllExport RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationOptimiser(){};
		virtual int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError) = 0;
		virtual estOptimizerType getOptimiserType() = 0;
		virtual void modifyAPriori(gsl_vector *newAPrioriPar) = 0;
		virtual gsl_vector* getAPrioriPar(){throw RSGISException("Not available for this optimiser");};
		virtual void printOptimiser() = 0;
		virtual ~RSGISEstimationOptimiser(){};
	};
}}

#endif
