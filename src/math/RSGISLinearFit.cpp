/*
 *  RSGISLinearFit.cpp
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

#include "RSGISLinearFit.h"

namespace rsgis {namespace math{

	RSGISLinearFit::RSGISLinearFit()
	{
	
	}

	double RSGISLinearFit::linearFit(gsl_matrix *a, gsl_vector *x, gsl_vector *b)
	{
		RSGISVectors vectorUtils;
		
		// Perform Least Squared Fit
		gsl_multifit_linear_workspace *workspace;
		workspace = gsl_multifit_linear_alloc(a->size1, a->size2);
		gsl_matrix *cov;
		double chisq;
		cov = gsl_matrix_alloc(a->size2, a->size2);
		gsl_multifit_linear(a, b, x, cov, &chisq, workspace);
		
		// Clean up
		gsl_multifit_linear_free(workspace);
		gsl_matrix_free(cov);
		
		return chisq;
		
	}
	
	double RSGISLinearFit::linearFitPrint(gsl_matrix *a, gsl_vector *x, gsl_vector *b)
	{
		RSGISVectors vectorUtils;
		RSGISMatrices matrixUtils;
		
		// Perform Least Squared Fit
		gsl_multifit_linear_workspace *workspace;
		workspace = gsl_multifit_linear_alloc(a->size1, a->size2);
		gsl_matrix *cov;
		double chisq;
		cov = gsl_matrix_alloc(a->size2, a->size2);
		gsl_multifit_linear(a, b, x, cov, &chisq, workspace);
		
        std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << "coefficients are : ";
		vectorUtils.printGSLVector(x); 
		std::cout << " chisq = " << chisq << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << std::endl;
		
		// Clean up
		gsl_multifit_linear_free(workspace);
		gsl_matrix_free(cov);
		
		return chisq;

		
	}
	
	double RSGISLinearFit::linearFitW(gsl_matrix *a, gsl_vector *x, gsl_vector *b, gsl_vector *var)
	{
		RSGISVectors vectorUtils;
		
		// Invert variance to get weights
		gsl_vector *weights;
		weights = gsl_vector_alloc(var->size);
		
		for(unsigned int i = 0; i < var->size; i++)
		{
			double weightElement = 1 / gsl_vector_get(var, i);
			gsl_vector_set(weights, i, weightElement);
		}
		
		// Perform Least Squared Fit
		gsl_multifit_linear_workspace *workspace;
		workspace = gsl_multifit_linear_alloc(a->size1, a->size2);
		gsl_matrix *cov;
		double chisq;
		cov = gsl_matrix_alloc(a->size2, a->size2);
		gsl_multifit_wlinear(a, weights, b, x, cov, &chisq, workspace);
		
		// Clean up
		gsl_multifit_linear_free(workspace);
		gsl_matrix_free(cov);
		gsl_vector_free(weights);
		
		return chisq;
	}
	
	double RSGISLinearFit::linearFitWPrint(gsl_matrix *a, gsl_vector *x, gsl_vector *b, gsl_vector *var)
	{
		RSGISVectors vectorUtils;
		
		// Invert variance to get weights
		gsl_vector *weights;
		weights = gsl_vector_alloc(var->size);
		
		for(unsigned int i = 0; i < var->size; i++)
		{
			double weightElement = 1 / gsl_vector_get(var, i);
			gsl_vector_set(weights, i, weightElement);
		}
		
		// Perform Least Squared Fit
		gsl_multifit_linear_workspace *workspace;
		workspace = gsl_multifit_linear_alloc(a->size1, a->size2);
		gsl_matrix *cov;
		double chisq;
		cov = gsl_matrix_alloc(a->size2, a->size2);
		gsl_multifit_wlinear(a, weights, b, x, cov, &chisq, workspace);
		
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << "coefficients are : ";
		vectorUtils.printGSLVector(x); 
		std::cout << " chisq = " << chisq << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << std::endl;
		
		// Clean up
		gsl_multifit_linear_free(workspace);
		gsl_matrix_free(cov);
		gsl_vector_free(weights);
		
		return chisq;
	}
	
	RSGISLinearFit::~RSGISLinearFit()
	{
		
	}

}}
