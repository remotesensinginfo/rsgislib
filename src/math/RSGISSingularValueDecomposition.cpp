/*
 *  RSGISSingularValueDecomposition.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 16/01/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISSingularValueDecomposition.h"

namespace rsgis{namespace math{
	
	RSGISSingularValueDecomposition::RSGISSingularValueDecomposition()
	{
		
	}
	
	void RSGISSingularValueDecomposition::ComputeSVDgsl(gsl_matrix *inA)
	{
		this->inA = inA;
		/// Calculates SVD for matrix in GSL format using GSL libarary
		RSGISVectors vectorUtils;
		RSGISMatrices matrixUtils;
		
		//cout << "A:" << endl;
		matrixUtils.printGSLMatrix(inA);
		
		outV = gsl_matrix_alloc (inA->size2, inA->size2);
		outS = gsl_vector_alloc (inA->size2);
		gsl_vector *out_work = gsl_vector_alloc (inA->size2);
		
		svdCompute = gsl_linalg_SV_decomp(inA, outV, outS, out_work);
		
		// Uncomment to print output matices 
		
		/*cout << "U:" << endl;
		matrixUtils.printGSLMatrix(inA);
		cout << "V:" << endl;
		matrixUtils.printGSLMatrix(outV);
		cout << "S:" << endl;
		vectorUtils.printGSLVector(outS);*/
	}
	
	void RSGISSingularValueDecomposition::ComputeSVDrsgis(Matrix *matrix)
	{
		/// Calculates SVD for matrix in RSGIS format using GSL libarary
		
		RSGISMatrices matrixUtils;
		inA = matrixUtils.convertRSGIS2GSLMatrix(matrix); // Convert to gsl_matrix
		
		outV = gsl_matrix_alloc (matrix->m, matrix->m);
		outS = gsl_vector_alloc (matrix->m);
		gsl_vector *out_work = gsl_vector_alloc (matrix->m);
		
		svdCompute = gsl_linalg_SV_decomp(inA, outV, outS, out_work);
		
		// Uncomment to print output matices 
		/*cout << "GSL matrix U is :" << inA->size1 << " x " << inA->size2 << endl;
		
		RSGISVectors vectorUtils;
		cout << "U:" << endl;
		matrixUtils.printGSLMatrix(inA);
		cout << "V:" << endl;
		matrixUtils.printGSLMatrix(outV);
		cout << "S:" << endl;
		vectorUtils.printGSLVector(outS);
		
		cout << "GSL matrix U is :" << inA->size1 << " x " << inA->size2 << endl;*/
		
	}
	
	void RSGISSingularValueDecomposition::SVDLinSolve(gsl_vector *outX, gsl_vector *inB)
	{
		// Solves linear equation using SVD
		/** This uses the gsl_linalg_SV_solve function to calculate the coefficients 
		 for a linear equation. The number of coefficients are determined by the output
		 gsl_vector outX.
		 */ 
		//cout << "Starting lin solve.." << endl;
		this->inA = inA;
		this->outV = outV;
		this->outS = outS;
		RSGISVectors vectorUtils;
		svdSolve = gsl_linalg_SV_solve (inA, outV, outS, inB, outX);
		cout << "solved!" << endl;
		cout << "coefficients are: ";
		vectorUtils.printGSLVector(outX);
		gsl_matrix_free(outV);
		gsl_vector_free(outS);
	}
	
	RSGISSingularValueDecomposition::~RSGISSingularValueDecomposition()
	{
		
	}
	
}}
