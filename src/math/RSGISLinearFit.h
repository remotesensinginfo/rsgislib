 /*
 *  RSGISLinearFit.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 17/05/2010.
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

#ifndef RSGISLinearFit_H
#define RSGISLinearFit_H

#include <iostream>
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include "math/RSGISVectors.h"
#include "math/RSGISMatrices.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis {namespace math{
    
	/// Class to perform polynomaial fitting	
	class DllExport RSGISLinearFit
	{
	public:
		RSGISLinearFit();
		/// Solves a.x = b for x
		double linearFit(gsl_matrix *a, gsl_vector *x, gsl_vector *b);
		/// Solves a.x = b for x, and prints to screen.
		double linearFitPrint(gsl_matrix *a, gsl_vector *x, gsl_vector *b);
		/// Solves a.x = b for x. The variance of each data point is given in the vector 'var'
		double linearFitW(gsl_matrix *a, gsl_vector *x, gsl_vector *b, gsl_vector *var);
		/// Solves a.x = b for x, and prints to screen. The variance of each data point is given in the vector 'var'
		double linearFitWPrint(gsl_matrix *a, gsl_vector *x, gsl_vector *b, gsl_vector *var);
		~RSGISLinearFit();
	};
}}

#endif

