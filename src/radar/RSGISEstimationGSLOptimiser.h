 /*
 *  RSGISEstimationGSLOptimiser.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 09/05/2010.
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

#ifndef RSGISEstimationGSLOptimiser_H
#define RSGISEstimationGSLOptimiser_H

#include <math.h>
#include <iostream>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_rng.h>
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMathFunction.h"
#include "math/RSGISFunctions.h"
#include "radar/RSGISEstimationOptimiser.h"

namespace rsgis {namespace radar{
	
	enum gslOptimizer 
	{
		conjGradFR,
		conjGradPR,
		bfgs,
		bfgs2
	};

	double function(const gsl_vector *predictParams, void *parameters, std::string coeffHVFile, std::string coeffFPCFile);
	void dFunction(const gsl_vector *predictParams, void *parameters, gsl_vector *df);
	void completeFunction(const gsl_vector *predictParams, void *parameters, double *f, gsl_vector *df);

	class DllExport RSGISEstimationGSLOptimiser : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationGSLOptimiser();
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return unknown;}; 
		virtual void printOptimiser(){std::cout << "GSL Optimiser" << std::endl;};
		~RSGISEstimationGSLOptimiser();
	};
	
	class DllExport RSGISEstimationGSLOptimiserNoGradient : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationGSLOptimiserNoGradient();
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return unknown;}; 
		virtual void printOptimiser(){std::cout << "GSL Optimiser - no gradients" << std::endl;};
		~RSGISEstimationGSLOptimiserNoGradient();
	};
	
}}

#endif
