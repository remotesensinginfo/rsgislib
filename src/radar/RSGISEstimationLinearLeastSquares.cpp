/*
 *  RSGISEstimationLinearLeastSquares.cpp
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

#include "RSGISEstimationLinearLeastSquares.h"

namespace rsgis {namespace radar{
	RSGISEstimationLinearLeastSquares::RSGISEstimationLinearLeastSquares(gsl_matrix *coefficients)
	{
		/* First colomn in coefficients contains offsets
		 * Coefficients are therefore split into a vector containing offsets
		 * and a matrix containing the remaining coefficients */
		
		this->a = gsl_matrix_alloc(coefficients->size1, coefficients->size2 - 1); 
		this->offsets = gsl_vector_alloc(coefficients->size1);
		
		for(unsigned int j = 0; j < coefficients->size1; j++)
		{
			gsl_vector_set(this->offsets, j, gsl_matrix_get(coefficients, 0, j));
			for (unsigned int i = 1; i < coefficients->size2; i++) 
			{
				gsl_matrix_set(this->a, j, i-1, gsl_matrix_get(coefficients, j, i));
			}
		}

	}
	int RSGISEstimationLinearLeastSquares::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		rsgis::math::RSGISLinearFit linFit;
		gsl_vector *x;
		gsl_vector *b;
		x = gsl_vector_alloc(initialPar->size);
		b = gsl_vector_alloc(inData->size);
		
		for(unsigned int i = 0; i < inData->size;i++)
		{
			gsl_vector_set(b,i, gsl_vector_get(inData,i)-gsl_vector_get(offsets, i));
		}
		
		// Currently hardcoding, will find a better way of doing
		double error = linFit.linearFit(this->a, x, b);
		double height = exp(gsl_vector_get(x, 1));
		double density = gsl_vector_get(x, 0);
		
		gsl_vector_set(outParError, 0, height);
		gsl_vector_set(outParError, 1, density);
		gsl_vector_set(outParError, 2, error);
		
		gsl_vector_free(x);
		gsl_vector_free(b);
		return 1;
	}
	RSGISEstimationLinearLeastSquares::~RSGISEstimationLinearLeastSquares()
	{
		gsl_matrix_free(a);
		gsl_vector_free(offsets);
	}
	
	RSGISEstimationLinearize::RSGISEstimationLinearize(gsl_matrix *coefficients)
	{
		/* First colomn in coefficients contains offsets
		 * Coefficients are therefore split into a vector containing offsets
		 * and a matrix containing the remaining coefficients */
		
		this->a = gsl_matrix_alloc(coefficients->size1, coefficients->size2 - 1); 
		this->offsets = gsl_vector_alloc(coefficients->size1);
		
		for(unsigned int j = 0; j < coefficients->size1; j++)
		{
			gsl_vector_set(this->offsets, j, gsl_matrix_get(coefficients, j, 0));
			for (unsigned int i = 1; i < coefficients->size2; i++) 
			{
				gsl_matrix_set(this->a, j, i-1, gsl_matrix_get(coefficients, j, i));
			}
		}
		
	}
	int RSGISEstimationLinearize::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		/* Similar to liniear least squares but attempts to linierize a non-linear equation by taking a prior for
		   one of the values */
		
		rsgis::math::RSGISLinearFit linFit;
		gsl_vector *x;
		gsl_vector *b;
		x = gsl_vector_alloc(initialPar->size);
		b = gsl_vector_alloc(inData->size);
		
		// Make a copy of matrix a
		gsl_matrix *aAP;
		aAP = gsl_matrix_alloc(a->size1,a->size2);
		gsl_matrix_memcpy(aAP, a);
		
		gsl_matrix_memcpy(aAP, a);
		gsl_matrix_set(aAP, 0, 0, gsl_matrix_get(a, 0, 0) * gsl_vector_get(initialPar, 1));
		gsl_matrix_set(aAP, 0, 1, gsl_matrix_get(a, 1, 0) * gsl_vector_get(initialPar, 1));
		gsl_vector_set_zero(x);
		
		for(unsigned int i = 0; i < inData->size;i++)
		{
			gsl_vector_set(b,i, gsl_vector_get(inData,i)-gsl_vector_get(offsets, i));
		}

		double error = linFit.linearFit(aAP, x, b);
		double height = exp(gsl_vector_get(x, 1));
		double density = gsl_vector_get(x, 0);
			

		gsl_vector_set(outParError, 0, height);
		gsl_vector_set(outParError, 1, density);
		gsl_vector_set(outParError, 2, error);
		
		gsl_vector_free(x);
		gsl_vector_free(b);
		gsl_matrix_free(aAP);
		return 1;
	}
	RSGISEstimationLinearize::~RSGISEstimationLinearize()
	{
		gsl_matrix_free(a);
		gsl_vector_free(offsets);
	}

	RSGISEstimationLinearizeWithPriors::RSGISEstimationLinearizeWithPriors(gsl_matrix *coefficients,
																		   gsl_matrix *covMatrixP, 
																		   gsl_matrix *covMatrixD)
	{
		/* First colomn in coefficients contains offsets
		 * Coefficients are therefore split into a vector containing offsets
		 * and a matrix containing the remaining coefficients */
		
		this->a = gsl_matrix_alloc(coefficients->size1, coefficients->size2 - 1); 
		this->offsets = gsl_vector_alloc(coefficients->size1);
		
		for(unsigned int j = 0; j < coefficients->size1; j++)
		{
			gsl_vector_set(this->offsets, j, gsl_matrix_get(coefficients, j, 0));
			for (unsigned int i = 1; i < coefficients->size2; i++) 
			{
				gsl_matrix_set(this->a, j, i-1, gsl_matrix_get(coefficients, j, i));
			}
		}
		
		// Calculate the inverse of covarience matrices (only using inverse)
		
		
	}
	int RSGISEstimationLinearizeWithPriors::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		/* Similar to liniear least squares but attempts to linierize a non-linear equation by taking a prior for
		 one of the values */
		
		rsgis::math::RSGISLinearFit linFit;
		gsl_vector *x;
		gsl_vector *b;
		x = gsl_vector_alloc(initialPar->size);
		b = gsl_vector_alloc(inData->size);
		
		// Make a copy of matrix a
		gsl_matrix *aAP;
		aAP = gsl_matrix_alloc(a->size1,a->size2);
		gsl_matrix_memcpy(aAP, a);
		
		gsl_matrix_memcpy(aAP, a);
		gsl_matrix_set(aAP, 0, 0, gsl_matrix_get(a, 0, 0) * gsl_vector_get(initialPar, 1));
		gsl_matrix_set(aAP, 0, 1, gsl_matrix_get(a, 1, 0) * gsl_vector_get(initialPar, 1));
		gsl_vector_set_zero(x);
		
		for(unsigned int i = 0; i < inData->size;i++)
		{
			gsl_vector_set(b,i, gsl_vector_get(inData,i)-gsl_vector_get(offsets, i));
		}
		
		double error = linFit.linearFit(aAP, x, b);
		double height = exp(gsl_vector_get(x, 1));
		double density = gsl_vector_get(x, 0);
		
		
		gsl_vector_set(outParError, 0, height);
		gsl_vector_set(outParError, 1, density);
		gsl_vector_set(outParError, 2, error);
		
		gsl_vector_free(x);
		gsl_vector_free(b);
		gsl_matrix_free(aAP);
		return 1;
	}
	RSGISEstimationLinearizeWithPriors::~RSGISEstimationLinearizeWithPriors()
	{
		gsl_matrix_free(a);
		gsl_vector_free(offsets);
	}
	
}}
