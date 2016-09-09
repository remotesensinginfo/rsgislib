/*
 *  RSGISEstimationGSLOptimiser.cpp
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

#include "RSGISEstimationGSLOptimiser.h"

namespace rsgis { namespace radar {

	double function(const gsl_vector *predictParams, void *inData)
	{
        rsgis::math::RSGISMatrices matrixUtils;
		double *inDataDouble = (double *) inData;
		
		// Function to minimise
		double h = gsl_vector_get(predictParams, 0);
		double d = gsl_vector_get(predictParams, 1);
		
		std::string coeffHVFile = "";
		std::string coeffFPCFile = "";
		gsl_matrix *coeffHV = matrixUtils.readGSLMatrixFromTxt(coeffHVFile);
		gsl_matrix *coeffFPC = matrixUtils.readGSLMatrixFromTxt(coeffFPCFile);

		rsgis::math::RSGISFunction2DPoly *functionHV = new rsgis::math::RSGISFunction2DPoly(coeffHV);
		rsgis::math::RSGISFunction2DPoly *functionFPC = new rsgis::math::RSGISFunction2DPoly(coeffFPC);
		
		double diffA = inDataDouble[0] - functionFPC->calcFunction(h, d);
		double diffB = (inDataDouble[1] - functionHV->calcFunction(h, d)) * 0.1;
		double leastSquares =  (pow(diffA, 2) + pow(diffB, 2));
		
		return leastSquares;
	}
	
	void dFunction(const gsl_vector *predictParams, void *inData, gsl_vector *df)
	{
		double *inDataDouble = (double *) inData;
		
		double aHH = 20;
		double bHH = 10.829782474903741;
		double cHH = 15.597720851189392;
		
		double aHV = 30;
		double bHV = 13.433385265347955;
		double cHV = 20.33373392991146;
		
		// Gradient of function to differentiate
		double h = gsl_vector_get(predictParams, 0);
		double d = gsl_vector_get(predictParams, 1);
		double dhA = aHV + (bHV * d) + inDataDouble[1] - (cHV*d*log(h));
		dhA = dhA * (2 * cHV * d);
		dhA = dhA / h;
		
		double dhB = aHH + (bHH * d) + inDataDouble[0] - (cHH*d*log(h));
		dhB = dhB * (2 * cHH * d);
		dhB = dhB / h;
		
		double dh = dhA + dhB;
		
		double ddA = aHV + (bHV *d) + inDataDouble[1] - (cHV * d * log(h));
		ddA = ddA * (bHV - cHV * log(h));
		double ddB = aHH + (bHH*d) + inDataDouble[0] - (cHH * d * log(h));
		ddB = ddB * (bHH - cHH * log(h));
		double dd = (2*ddA) + (2*ddB);
		
		gsl_vector_set(df, 0, dh);
		gsl_vector_set(df, 1, dd);
		
	}
	
	void completeFunction(const gsl_vector *predictParams, void *parameters, double *f, gsl_vector *df)
	{
		*f = function(predictParams, parameters);
		dFunction(predictParams, parameters, df);
	}
	
	RSGISEstimationGSLOptimiser::RSGISEstimationGSLOptimiser()
	{
		//this->functionHH = functionHH;
		//this->functionHV = functionHV;
		
	}
	int RSGISEstimationGSLOptimiser::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		unsigned int ittMax = 10;
		int status = 0;
		double *inDataDouble = new double[2];
		inDataDouble[0] = gsl_vector_get(inData, 0);
		inDataDouble[1] = gsl_vector_get(inData, 1);
				
		gsl_vector *predictParams;
		predictParams = gsl_vector_alloc(2);
		gsl_vector *df;
		df = gsl_vector_alloc(2);
		
		// set inital parameters
		gsl_vector_set(predictParams, 0, gsl_vector_get(initialPar, 0));
		gsl_vector_set(predictParams, 1, gsl_vector_get(initialPar, 1));
		
		gsl_multimin_function_fdf minFunction;
		minFunction.n = 2;
		minFunction.f = &function;
		minFunction.df = &dFunction;
		minFunction.fdf = &completeFunction;
		minFunction.params = (void *) inDataDouble;
		
		const gsl_multimin_fdfminimizer_type *T;
		gsl_multimin_fdfminimizer *s;
		
		T = gsl_multimin_fdfminimizer_conjugate_pr;
		s = gsl_multimin_fdfminimizer_alloc(T, 2);
		
		gsl_multimin_fdfminimizer_set(s, &minFunction, predictParams, 0.01, 0.1);
		
		//cout << "Starting minimisation" << endl;
		for(unsigned int i = 0; i < ittMax; i++)
		{
			
			//cout << "height = " << gsl_vector_get(s->x, 0) << " density = " << gsl_vector_get(s->x, 1) << endl;
			
			status = gsl_multimin_fdfminimizer_iterate(s);
			/*if (status == GSL_SUCCESS)
			{
				cout << "SUCCESS!!!" << endl;
				cout << "height = " << gsl_vector_get(s->x, 0) << " density = " << gsl_vector_get(s->x, 1) << endl;
				return 1;
			}*/
			status = gsl_multimin_test_gradient(s->gradient, 1e-3);
			
		}
		
		gsl_vector_set(outParError, 0, gsl_vector_get(s->x, 0));
		gsl_vector_set(outParError, 1, gsl_vector_get(s->x, 1));
		gsl_vector_set(outParError, 2, gsl_multimin_fdfminimizer_minimum(s)); // Error
		
		gsl_vector_free(df);
		gsl_vector_free(predictParams);
		gsl_multimin_fdfminimizer_free(s);
		delete[] inDataDouble;
					   
		return 0;
	}
	RSGISEstimationGSLOptimiser::~RSGISEstimationGSLOptimiser()
	{
		
	}
	
	RSGISEstimationGSLOptimiserNoGradient::RSGISEstimationGSLOptimiserNoGradient()
	{
		//this->functionHH = functionHH;
		//this->functionHV = functionHV;
		
	}
	int RSGISEstimationGSLOptimiserNoGradient::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		unsigned int ittMax = 1000;
		int status = 0;
		double *inDataDouble = new double[2];
		inDataDouble[0] = gsl_vector_get(inData, 0);
		inDataDouble[1] = gsl_vector_get(inData, 1);
		
		gsl_vector *predictParams;
		predictParams = gsl_vector_alloc(2);
		gsl_vector *df;
		df = gsl_vector_alloc(2);
		
		// set inital parameters
		gsl_vector_set(predictParams, 0, gsl_vector_get(initialPar, 0));
		gsl_vector_set(predictParams, 1, gsl_vector_get(initialPar, 1));
						
		gsl_multimin_function minFunction;
		minFunction.n = 2;
		minFunction.f = &function;
		minFunction.params = (void *) inDataDouble;
		
		const gsl_multimin_fminimizer_type *T;
		gsl_multimin_fminimizer *s;
		
		T = gsl_multimin_fminimizer_nmsimplex;
		s = gsl_multimin_fminimizer_alloc(T, 2);
		
		gsl_vector *stepSize;
		
		stepSize = gsl_vector_alloc(2);
		gsl_vector_set(stepSize, 0, 0.1);
		gsl_vector_set(stepSize, 1, 0.1);
		
		gsl_multimin_fminimizer_set(s, &minFunction, initialPar, stepSize);
		
		//cout << "Starting minimisation" << endl;
		for(unsigned int i = 0; i < ittMax; i++)
		{
			
			//cout << "height = " << gsl_vector_get(s->x, 0) << " density = " << gsl_vector_get(s->x, 1) << endl;
			status = gsl_multimin_fminimizer_iterate(s);
			
		}
		//cout << "height = " << gsl_vector_get(s->x, 0) << " density = " << gsl_vector_get(s->x, 1) << endl;
		gsl_vector_set(outParError, 0, gsl_vector_get(s->x, 0));
		gsl_vector_set(outParError, 1, gsl_vector_get(s->x, 1));
		gsl_vector_set(outParError, 2, gsl_multimin_fminimizer_minimum(s)); // Error
		
		return 0;
	}
	RSGISEstimationGSLOptimiserNoGradient::~RSGISEstimationGSLOptimiserNoGradient()
	{
		
	}

	
}}

