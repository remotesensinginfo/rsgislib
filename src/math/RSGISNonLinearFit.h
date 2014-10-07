 /*
 *  RSGISNonLinearFit.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 14/06/2010.
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

#ifndef RSGISNonLinearFit_H
#define RSGISNonLinearFit_H

#include <iostream>
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "math/RSGISVectors.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISMathFunction.h"

namespace rsgis {namespace math{
    
	/// Class to perform polynomaial fitting	
	class DllExport RSGISNonLinearFit
	{
	public:
		RSGISNonLinearFit(){};
		double nonLinearFit1Var(gsl_vector *indVar, gsl_vector *depVar, gsl_vector *variance, gsl_vector *coeff, bool *fixCoeff, RSGISMathFunction *function);
		double nonLinearFit2Var(gsl_matrix *indVar, gsl_vector *depVar, gsl_vector *variance, gsl_vector *coeff, bool *fixCoeff, RSGISMathTwoVariableFunction *function);
		gsl_matrix* nonLinearTest1Var(gsl_vector *indVar, gsl_vector *depVar, RSGISMathFunction *function);
		gsl_matrix* nonLinearTest2Var(gsl_matrix *indVar, gsl_vector *depVar, RSGISMathTwoVariableFunction *function);
		int minimiseChiSqSA1Var(gsl_vector *indVar, gsl_vector *depVar, gsl_vector *variance, gsl_vector *outCoeffError, bool *fixCoeff, RSGISMathFunction *function);
		int minimiseChiSqSA2Var(gsl_matrix *indVar, gsl_vector *depVar, gsl_vector *variance, gsl_vector *outCoeffError, bool *fixCoeff, RSGISMathTwoVariableFunction *function);
		double chiSq1Var(gsl_vector *indVar, gsl_vector *depVar, gsl_vector *variance, RSGISMathFunction *function);
		double chiSq2Var(gsl_matrix *indVar, gsl_vector *depVar, gsl_vector *variance, RSGISMathTwoVariableFunction *function);
		~RSGISNonLinearFit(){};
	};
}}

#endif


