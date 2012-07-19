/*
 *  RSGISPolyFit.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 25/01/2009.
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

#include "RSGISPolyFit.h"

namespace rsgis {namespace math {
	
	RSGISPolyFit::RSGISPolyFit()
	{
	}
	
	gsl_vector* RSGISPolyFit::PolyfitOneDimensionQuiet(int order, gsl_matrix *inData)
	{
		/// Fit one-dimensional n-1th order polynomial
		/**
		 * A gsl_matrix containing the independent and dependent variables is passed in the form: \n
		 * x, y. \n
		 * A gsl_vector is returned containing the coeffients. \n
		 * Polynomial coefficients are obtained using a least squares fit. \n
		 */ 
		RSGISMatrices matrixUtils;
		RSGISVectors vectorUtils;
		
		// Set up matrix of powers
		gsl_matrix *indVarPow;
		gsl_vector *depVar;
		gsl_vector *outCoefficients;
		
		indVarPow = gsl_matrix_alloc(inData->size1, order); // Matrix to hold powers of x
		depVar = gsl_vector_alloc(inData->size1); // Vector to hold y term
		outCoefficients = gsl_vector_alloc(order); // Vector to hold output coefficients and ChiSq
		
		for(unsigned int i = 0; i < inData->size1; i++)
		{
			// Populate devVar vector with y values
			double yelement = gsl_matrix_get(inData, i, 1);
			gsl_vector_set(depVar, i, yelement);
			// Populate indVarPow with x^n
			for(int j = 0; j < order; j++)
			{
				double xelement = gsl_matrix_get(inData, i, 0);
				double xelementtPow = pow(xelement, (j));
				gsl_matrix_set(indVarPow, i, j, xelementtPow);
			}
		}
		
		// Perform Least Squared Fit
		gsl_multifit_linear_workspace *workspace;
		workspace = gsl_multifit_linear_alloc(inData->size1, order);
		gsl_matrix *cov;
		double chisq;
		cov = gsl_matrix_alloc(order, order);
		gsl_multifit_linear(indVarPow, depVar, outCoefficients, cov, &chisq, workspace);
		
		
		/*
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << "coefficients are : ";
		vectorUtils.printGSLVector(outCoefficients); 
		std::cout << " chisq = " << chisq << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << std::endl;
		*/
		
		// Clean up
		gsl_multifit_linear_free(workspace);
		gsl_matrix_free(indVarPow);
		gsl_vector_free(depVar);
		gsl_matrix_free(cov);
		
		return outCoefficients;
	}
	
	gsl_vector* RSGISPolyFit::PolyfitOneDimension(int order, gsl_matrix *inData)
	{
		/// Fit one-dimensional n-1th order polynomial
		/**
		 * A gsl_matrix containing the independent and dependent variables is passed in the form: \n
		 * x, y. \n
		 * A gsl_vector is returned containing the coeffients. \n
		 * Polynomial coefficients are obtained using a least squares fit. \n
		 */ 
		RSGISMatrices matrixUtils;
		RSGISVectors vectorUtils;
		
		// Set up matrix of powers
		gsl_matrix *indVarPow;
		gsl_vector *depVar;
		gsl_vector *outCoefficients;
		
		indVarPow = gsl_matrix_alloc(inData->size1, order); // Matrix to hold powers of x
		depVar = gsl_vector_alloc(inData->size1); // Vector to hold y term
		outCoefficients = gsl_vector_alloc(order); // Vector to hold output coefficients and ChiSq
		
		for(unsigned int i = 0; i < inData->size1; i++)
		{
			// Populate devVar vector with y values
			double yelement = gsl_matrix_get(inData, i, 1);
			gsl_vector_set(depVar, i, yelement);
			// Populate indVarPow with x^n
			for(int j = 0; j < order; j++)
			{
				double xelement = gsl_matrix_get(inData, i, 0);
				double xelementtPow = pow(xelement, (j));
				gsl_matrix_set(indVarPow, i, j, xelementtPow);
			}
		}
		
		// Perform Least Squared Fit
		gsl_multifit_linear_workspace *workspace;
		workspace = gsl_multifit_linear_alloc(inData->size1, order);
		gsl_matrix *cov;
		double chisq;
		cov = gsl_matrix_alloc(order, order);
		gsl_multifit_linear(indVarPow, depVar, outCoefficients, cov, &chisq, workspace);
		
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << "coefficients are : ";
		vectorUtils.printGSLVector(outCoefficients); 
		std::cout << " chisq = " << chisq << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << std::endl;
		
		// Clean up
		gsl_multifit_linear_free(workspace);
		gsl_matrix_free(indVarPow);
		gsl_vector_free(depVar);
		gsl_matrix_free(cov);
		
		return outCoefficients;
	}
	
	gsl_vector* RSGISPolyFit::PolyfitOneDimensionSVD(int order, gsl_matrix *inData)
	{	
		/// Fit one-dimensional n-1th order polynomial
		/**
		 * A gsl_matrix containing the independent and dependent variables is passed in the form: \n
		 * x, y. \n
		 * A gsl_vector is returned containing the coeffients. \n
		 * A coefficients are obtained using SVD. \n
		 * Use this version when there are equal number of coefficients and variables as it will solve the equations. \n
		 * Otherwise use PolyfitOneDimension to estimate coefficents using Least squares fitting. \n
		 */ 
		
		RSGISMatrices matrixUtils;
		RSGISVectors vectorUtils;
		
		// Set up matrix of powers
		gsl_matrix *indVarPow;
		gsl_vector *depVar;
		gsl_vector *outCoefficients;
		
		indVarPow = gsl_matrix_alloc(inData->size1, order); // Matrix to hold powers of x
		depVar = gsl_vector_alloc(inData->size1); // Vector to hold y term
		outCoefficients = gsl_vector_alloc(order); // Vector to hold output coefficients and ChiSq
		
		for(unsigned int i = 0; i < inData->size1; i++)
		{
			// Populate devVad vector with y values
			double yelement = gsl_matrix_get(inData, i, 1);
			gsl_vector_set(depVar, i, yelement);
			
			// Populate indVarPow with x^n
			for(int j = 0; j < order; j++)
			{
				double xelement = gsl_matrix_get(inData, i, 0);
				double xelementtPow = pow(xelement, (j));
				gsl_matrix_set(indVarPow, i, j, xelementtPow);
			}
		}
		
		// Calculate SVD
		RSGISSingularValueDecomposition svd;
		svd.ComputeSVDgsl(indVarPow);
		
		// Solve Equation
		svd.SVDLinSolve(outCoefficients, depVar);
		std::cout << "coefficents are : ";
		vectorUtils.printGSLVector(outCoefficients);
		
		// Clean up
		gsl_matrix_free(indVarPow);
		gsl_vector_free(depVar);
		
		return outCoefficients;
	}
	
	gsl_matrix* RSGISPolyFit::PolyTestOneDimension(int order, gsl_matrix *inData, gsl_vector *coefficients)
	{
		/// Tests one dimensional polynomal equation, outputs measured and predicted values to a matrix.
		
		RSGISMatrices matrixUtils;
		gsl_matrix *measuredVpredictted;
		measuredVpredictted = gsl_matrix_alloc(inData->size1, 2); // Set up matrix to hold measured and predicted y values.
		
		for(unsigned int i = 0; i < inData->size1; i++) // Loop through inData
		{
			double xVal;
			double yMeasured;
			double yPredicted;
			
			xVal = gsl_matrix_get(inData, i, 0); // Get x value
			yMeasured = gsl_matrix_get(inData, i, 1); // Get measured y value.
			yPredicted = 0;
			
			for(int j = 0; j < order ; j++)
			{
				double xPow = pow(xVal, j); // x^n;
				double coeff = gsl_vector_get(coefficients, j); // a_n
				double coeffXPow = coeff * xPow; // a_n * x^n				
				yPredicted = yPredicted + coeffXPow;
			}
			
			//std::cout << "measured = " << yMeasured << " predicted = " << yPredicted << std::endl;
			
			gsl_matrix_set(measuredVpredictted, i, 0, yMeasured);
			gsl_matrix_set(measuredVpredictted, i, 1, yPredicted);
		}

		this->calcRSquaredGSLMatrix(measuredVpredictted);
		this->calcRMSErrorGSLMatrix(measuredVpredictted);
		
		return measuredVpredictted;
	}
	
	gsl_matrix* RSGISPolyFit::PolyfitTwoDimension(int numX, int numY, int orderX, int orderY, gsl_matrix *inData)
	{
		/// Fit n-1th order two dimensional polynomal equation.
		/**
		 * Using least squares, two sets of fits are performed to obtain a two dimensional polynomal equation \n
		 * of the form z(x,y) = a_0(y) + a_1(y)*x + a_2(y)*x^2 + ... + a_n(y)*x^n. \n
		 * where a_n(y) = b_0 + b_1*y + b_2*y^2 + .... + b_n*y^n \n
		 * Data is inputted using mtxt format with data stored: x, y, z. \n
		 * For example: \n
		 * x_1, y_1, z_11 \n
		 * x_2, y_1, z_12 \n
		 * x_3, y_1, z_13 \n
		 * x_1, y_2, z_21 \n
		 * x_2, y_2, z_22 \n
		 * x_3, y_2, z_23 \n
		 * Where the number of x terms (numX) is 3 and the number of y terms (numY) is 2. \n
		 * The b coefficients are outputted as a gsl_matrix with the errors stored in the last column. \n
		 */
		
		RSGISMatrices matrixUtils;
		RSGISVectors vectorUtils;
		
		gsl_matrix *aCoeff;
		gsl_matrix *bCoeff;
		
		aCoeff = gsl_matrix_alloc(numY, orderX+1);
		bCoeff = gsl_matrix_alloc(orderX, orderY+1);
		
		// PERFORM FIRST SET OF FITS
		//gsl_vector *indVar;
		gsl_matrix *indVarPow;
		gsl_vector *depVar;
		gsl_vector *tempAcoeff;
		gsl_vector *indVar2;
		indVarPow = gsl_matrix_alloc(numX, orderX); // Set up matrix to hold powers of x term for each fit
		indVar2 = gsl_vector_alloc(numY); // Set up vector to hold y values for each fit
		depVar = gsl_vector_alloc(numX); // Set up vector to hold z values for each fit
		tempAcoeff = gsl_vector_alloc(orderX); // Set up vector to hold output coefficients for each fit
		
		double errorA = 0;
		int indexY = 0;
			
		for(int y = 0; y < numY; y++)
		{
			// Populate matrix
			indexY = y * numX;
			//std::cout << "solving set " << y + 1 << "...." << std::endl;
			double yelement = gsl_matrix_get(inData, indexY, 1);
			gsl_vector_set(indVar2, y, yelement); // Add y values to indVar2 vector
			
			// Create matrix of powers for x term
			for(int i = 0; i < numX; i++)
			{
				double melement = gsl_matrix_get(inData, indexY+i, 0);
				double melementDep = gsl_matrix_get(inData, indexY+i, 2);
				gsl_vector_set(depVar, i, melementDep); // Fill dependent variable vector
				
				for(int j = 0; j < orderX; j++)
				{
					double melementPow = pow(melement, (j));
					gsl_matrix_set(indVarPow, i, j, melementPow);
				}
			}
			
			//std::cout << "Starting to solve " << std::endl;
			
			// Solve
			gsl_multifit_linear_workspace *workspace;
			workspace = gsl_multifit_linear_alloc(numX, orderX);
			gsl_matrix *cov;
			double chisq;
			cov = gsl_matrix_alloc(orderX, orderX);
			gsl_multifit_linear(indVarPow, depVar, tempAcoeff, cov, &chisq, workspace); // Perform least squares fit
			//std::cout << "solved!" << std::endl;
			//vectorUtils.printGSLVector(tempAcoeff);
			
			// Add coefficents to Matrix
			for(int k = 0; k < orderX; k++)
			{
				double coeffElement = gsl_vector_get(tempAcoeff, k);
				gsl_matrix_set(aCoeff, y, k, coeffElement);
			}
			// ChiSq
			gsl_matrix_set(aCoeff, y, orderX, chisq);
			
			errorA = errorA + chisq;
			
		}
		
		errorA = errorA / numY; // Calculate average ChiSq
		std::cout << "-----------------------------" << std::endl;
		std::cout << "First set of fits complete!" << std::endl;
		std::cout << " Average ChiSq = " << errorA << std::endl;
		std::cout << std::endl;
		//matrixUtils.printGSLMatrix(aCoeff);
		//matrixUtils.saveGSLMatrix2GridTxt(aCoeff, "/users/danclewley/Documents/Temp/L_HH_aCoeff");
		
		//Clean up
		gsl_vector_free(tempAcoeff);
		gsl_vector_free(depVar);
		gsl_matrix_free(indVarPow);
		
		// PERFORM SECOND SET OF FITS
		gsl_matrix *indVar2Pow; // Set up matrix to hold powers of y term for each fit
		gsl_vector *depVar2; // Set up vector to hold a coefficeints for each fit
		gsl_vector *tempBcoeff; // Set up matrix to hold B coefficients for each fit
		indVar2Pow = gsl_matrix_alloc(numY, orderY);
		depVar2 = gsl_vector_alloc(numY);
		tempBcoeff = gsl_vector_alloc(orderY);
		double errorB  = 0;
		
		// Create matrix of powers for y term.
		for(int i = 0; i < numY; i++)
		{
			double melement = gsl_vector_get(indVar2, i);
			
			for(int j = 0; j < orderY; j++)
			{
				double melementPow = pow(melement, (j));
				gsl_matrix_set(indVar2Pow, i, j, melementPow);
			}
		}
		
		// Loop through fits
		for(int i = 0; i < orderX; i++)
		{
			for(int j = 0; j < numY; j++)
			{
				double melement = gsl_matrix_get(aCoeff, j, i);
				gsl_vector_set(depVar2, j, melement);
			}
			// Solve
			gsl_multifit_linear_workspace *workspace;
			workspace = gsl_multifit_linear_alloc(numY, orderY);	
			gsl_matrix *cov;
			double chisq;
			cov = gsl_matrix_alloc(orderY, orderY);
			gsl_multifit_linear(indVar2Pow, depVar2, tempBcoeff, cov, &chisq, workspace);
			gsl_multifit_linear_free(workspace);
			gsl_matrix_free(cov);
			//matrixUtils.printGSLMatrix(indVar2Pow);
			//vectorUtils.printGSLVector(depVar2);
			
			// Add coefficents to Matrix
			for(int k = 0; k < orderY; k++)
			{
				double coeffElement = gsl_vector_get(tempBcoeff, k);
				gsl_matrix_set(bCoeff, i, k, coeffElement);
			}
			
			// ChiSq
			//std::cout << "chisq = "<< chisq << std::endl;
			errorB = errorB + chisq;
			gsl_matrix_set(bCoeff, i, orderY, chisq);
		}
		
		errorB = errorB / orderX; // Calculate average ChiSq
		std::cout << "Second set of fits complete!" << std::endl;
		std::cout << " Average ChiSq = " << errorB << std::endl;
		std::cout << "-----------------------------" << std::endl;
		std::cout << std::endl;
		//std::cout << "Coefficients are : " << std::endl;
		//matrixUtils.printGSLMatrix(bCoeff);
		
		// Clean up
		gsl_matrix_free(indVar2Pow);
		gsl_vector_free(depVar2);
		gsl_vector_free(tempBcoeff);
		gsl_matrix_free(aCoeff);
		
		return bCoeff;
	}
	
	gsl_matrix* RSGISPolyFit::PolyTestTwoDimension(int orderX, int orderY, gsl_matrix *inData, gsl_matrix *coefficients)
	{
		/// Tests one dimensional polynomal equation, outputs measured and predicted values to a matrix.
		
		RSGISMatrices matrixUtils;
		gsl_matrix *measuredVpredictted;
		measuredVpredictted = gsl_matrix_alloc(inData->size1, 2); // Set up matrix to hold measured and predicted y values.
		
		for(unsigned int i = 0; i < inData->size1; i++) // Loop through inData
		{
			double xVal;
			double yVal;
			double zMeasured;
			double zPredicted;
			
			xVal = gsl_matrix_get(inData, i, 0); // Get x value
			yVal = gsl_matrix_get(inData, i, 1); // Get y value
			zMeasured = gsl_matrix_get(inData, i, 2); // Get measured z value.
			zPredicted = 0;
			for(int x = 0; x < orderX; x ++) 
			{
				double xPow = pow(xVal, x); // x^n;
				
				double aCoeff = 0.0; 
				
				for(int y = 0; y < orderY ; y++) // Calculate a_n(y)
				{
					double yPow = pow(yVal, y); // y^n;
					double bcoeff = gsl_matrix_get(coefficients, x, y); // b_n
					double bcoeffYPow = bcoeff * yPow; // b_n * y^n				
					aCoeff = aCoeff + bcoeffYPow;
				}
				double acoeffXPow = xPow * aCoeff;
				zPredicted = zPredicted + acoeffXPow;
			}
			
			//std::cout << "measured = " << yMeasured << " predicted = " << yPredicted << std::endl;
			
			gsl_matrix_set(measuredVpredictted, i, 0, zMeasured);
			gsl_matrix_set(measuredVpredictted, i, 1, zPredicted);
		}
		
		this->calcRSquaredGSLMatrix(measuredVpredictted);
		this->calcRMSErrorGSLMatrix(measuredVpredictted);
		
		return measuredVpredictted;
	}
	
	gsl_matrix* RSGISPolyFit::PolyfitThreeDimension(int numX, int numY, int numZ, int orderX, int orderY, int orderZ, gsl_matrix *inData)
	{
		/// Fit n-1th order three dimensional polynomal equation.
		/**
		 * Using least squares, three sets of fits are performed to obtain a three dimensional polynomal equation \n
		 * of the form z(x,y) = a_0(y) + a_1(y)*x + a_2(y)*x^2 + ... + a_n(y)*x^n. \n
		 * where a_n(y) = b_0 + b_1*y + b_2*y^2 + .... + b_n*y^n \n
		 * Data is inputted using mtxt format with data stored: x, y, z. \n
		 * For example:  \n
		 * x_1, y_1, z_1 \n
		 * x_2, y_1, z_1 \n
		 * x_3, y_1, z_1 \n
		 * x_1, y_2, z_1 \n
		 * x_2, y_2, z_1 \n
		 * x_3, y_2, z_1 \n
		 * x_1, y_1, z_2 \n
		 * x_2, y_1, z_2 \n
		 * x_3, y_1, z_2 \n
		 * x_1, y_2, z_2 \n
		 * x_2, y_2, z_2 \n
		 * x_3, y_2, z_2 \n
		 * Where the number of x terms (numX) is 3, the number of y terms (numY) is 2 and the number of z terms (numZ) is 2\n
		 * The b coefficients are outputted as a gsl_matrix with the errors stored in the last column. \n
		 */
		
		RSGISMatrices matrixUtils;
		RSGISVectors vectorUtils;
		
		std::cout << "Order X, Y, Z = " << orderX << ", " << orderY << ", " << orderZ << std::endl;
		
		gsl_matrix *aCoeff, *bCoeff, *cCoeff, *indVarXPow;
		
		gsl_vector *depVarX, *depVarY, *depVarZ;
		gsl_vector *tempAcoeff, *tempBcoeff, *tempCcoeff;
		gsl_vector *indVarY, *indVarZ;
		gsl_matrix *indVarYPow, *indVarZPow; 
		
		aCoeff = gsl_matrix_alloc(numY, orderX+1); // Set up matrix to hold a coefficients (and chi squared)
		bCoeff = gsl_matrix_alloc(orderX * numZ, orderY+1); // Set up matrix to hold b coefficients (and chi squared)
		cCoeff = gsl_matrix_alloc(orderX * orderY, orderZ+1); // Set up matrix to hold b coefficients (and chi squared)
		
		indVarXPow = gsl_matrix_alloc(numX, orderX); // Set up matrix to hold powers of x term for each fit
		indVarY = gsl_vector_alloc(numY); // Set up vector to hold y values for each fit
		indVarZ = gsl_vector_alloc(numZ); // Set up vector to hold z values for each fit
		depVarX = gsl_vector_alloc(numX); // Set up vector to hold independent values for each fit
		tempAcoeff = gsl_vector_alloc(orderX); // Set up vector to hold output coefficients for each fit
		
		indVarYPow = gsl_matrix_alloc(numY, orderY);  // Set up matrix to hold powers of y term for each fit
		depVarY = gsl_vector_alloc(numY); // Set up vector to hold dependent varieble (a coefficents) for second set of fits
		tempBcoeff = gsl_vector_alloc(orderY); // Set up matrix to hold B coefficients for each fit
		
		indVarZPow = gsl_matrix_alloc(numZ, orderZ); // Set up matrix to hold powers of z
		depVarZ = gsl_vector_alloc(numZ); // Set up vector to hold dependent varieble (b coefficents) for third set of fits
		tempCcoeff = gsl_vector_alloc(orderZ); // Set up vectro to hold the c coefficents from each fit
		
		/*********************************
		 *  PERFORM FIRST SET OF FITS    *
		 *********************************/
		
		int indexZ = 0;
		double errorA = 0;
		double errorB  = 0;
		
		for(int z = 0; z < numZ; z++)
		{
			indexZ = z * (numX * numY); // Index moving through z variable
			
			int indexY = 0;
			
			double zelement = gsl_matrix_get(inData, indexZ, 2);
			gsl_vector_set(indVarZ, z, zelement); // Add z values to indVarZ vector
			
			for(int y = 0; y < numY; y++)
			{
				// Populate matrix
				indexY = (y * numX) + (indexZ);  // Index is the starting point for each y term
				double yelement = gsl_matrix_get(inData, indexY, 1);
				gsl_vector_set(indVarY, y, yelement); // Add y values to indVarY vector
				
				// Create matrix of powers for x term
				for(int i = 0; i < numX; i++)
				{
					double melement = gsl_matrix_get(inData, indexY+i, 0);
					double melementDep = gsl_matrix_get(inData, indexY+i, 3);
					gsl_vector_set(depVarX, i, melementDep); // Fill dependent variable vector
					
					for(int j = 0; j < orderX; j++)
					{
						double melementPow = pow(melement, j);
						gsl_matrix_set(indVarXPow, i, j, melementPow);
					}
				}
				
				/*std::cout << "indVarX = " << std::endl;
				 matrixUtils.printGSLMatrix(indVarXPow);
				 std::cout << "depVarX = " << std::endl;
				 vectorUtils.printGSLVector(depVarX);
				 std::cout << std::endl;*/
				
				// Solve
				gsl_multifit_linear_workspace *workspace;
				workspace = gsl_multifit_linear_alloc(numX, orderX);
				gsl_matrix *cov;
				double chisq;
				cov = gsl_matrix_alloc(orderX, orderX);
				gsl_multifit_linear(indVarXPow, depVarX, tempAcoeff, cov, &chisq, workspace); // Perform least squares fit
				//std::cout << "a Coeff = " << std::endl;
				//vectorUtils.printGSLVector(tempAcoeff);
				
				// Add coefficents to Matrix
				for(int k = 0; k < orderX; k++)
				{
					double coeffElement = gsl_vector_get(tempAcoeff, k);
					gsl_matrix_set(aCoeff, y, k, coeffElement);
				}
				// ChiSq
				gsl_matrix_set(aCoeff, y, orderX, chisq);
				//std::cout << "chiSq for first set of fits  = " << chisq << std::endl;
				errorA = errorA + chisq;
			}
			
			/*********************************
			 *  PERFORM SECOND SET OF FITS    *
			 **********************************/
			
			// Create matrix of powers for y term.
			for(int i = 0; i < numY; i++)
			{
				double melement = gsl_vector_get(indVarY, i);
				
				for(int j = 0; j < orderY; j++)
				{
					double melementPow = pow(melement, (j));
					gsl_matrix_set(indVarYPow, i, j, melementPow);
				}
			}
			
			// Loop through fits
			for(int i = 0; i < orderX; i++)
			{
				//std::cout << "Adding b coefficients for z = " << z << ", x order = " << i << "...";
				for(int j = 0; j < numY; j++)
				{
					double melement = gsl_matrix_get(aCoeff, j, i);
					gsl_vector_set(depVarY, j, melement);
				}
				
				/*
				 std::cout << "indVarY = " << std::endl;
				 vectorUtils.printGSLVector(indVarY);
				 std::cout << "depVarY = " << std::endl;
				 vectorUtils.printGSLVector(depVarY);
				 std::cout << std::endl;
				 */
				
				// Solve
				gsl_multifit_linear_workspace *workspace;
				workspace = gsl_multifit_linear_alloc(numY, orderY);
				gsl_matrix *cov;
				double chisq;
				cov = gsl_matrix_alloc(orderY, orderY);
				gsl_multifit_linear(indVarYPow, depVarY, tempBcoeff, cov, &chisq, workspace);
				gsl_multifit_linear_free(workspace);
				gsl_matrix_free(cov);
				
				// Add coefficents to Matrix
				for(int k = 0; k < orderY; k++)
				{
					double coeffElement = gsl_vector_get(tempBcoeff, k);
					//std::cout << "..a" << i + (z * orderX) << "-" << k;
					gsl_matrix_set(bCoeff, i + (z * orderX), k, coeffElement);
					//gsl_matrix_set(bCoeff, cInd, k, coeffElement);

					//std::cout << "b";
				}
				// ChiSq
				//std::cout << "..Added OK." << std::endl;
				errorB = errorB + chisq;
				gsl_matrix_set(bCoeff, i + (z * orderX), orderY, chisq);
				//gsl_matrix_set(bCoeff, cInd, orderY, chisq);
			}
			
		}
		
		errorA = errorA / (numY * numZ); // Calculate average ChiSq
		errorB = errorB / (orderY * numZ); // Calculate average ChiSq	
		
		std::cout << "----------------------------------------------" << std::endl;
		std::cout << "First and second set of fits complete " << std::endl;
		std::cout << " Average ChiSq for first set of fits = " << errorA << std::endl;
		std::cout << " Average ChiSq for second set of fits = " << errorB << std::endl;
		
		
		 /*std::cout << "==============================================================" << std::endl;
		 matrixUtils.printGSLMatrix(bCoeff);
		 std::cout << "==============================================================" << std::endl;*/
		 
		
		/*std::cout << "indVarZ = " << std::endl;
		 vectorUtils.printGSLVector(indVarZ);
		 std::cout << "coeffB = " << std::endl;
		 matrixUtils.printGSLMatrix(bCoeff);
		 std::cout << std::endl;*/
		
		/***************************************************
		 * PERFORM THIRD SET OF FITS                       
		 * 
		 * Coefficients are in the form:
		 * z1: 
		 * b0_0 b0_1 b0_2 ... b0_n
		 * b1_0 b1_1 b1_2 ... b0_n
		 *  .    .     .        .
		 * bn_0 bn_1 bn_2 ... bn_n
		 * z2 
		 * b0_0 b0_1 b0_2 ... b0_n
		 * b1_0 b1_1 b1_2 ... b0_n
		 *  .    .     .        .
		 * bn_0 bn_1 bn_2 ... bn_n 
		 *
		 *
		 ***************************************************/
		
		// Create matrix of powers for z term.
		for(int i = 0; i < numZ; i++)
		{
			double melement = gsl_vector_get(indVarZ, i);
			for(int j = 0; j < orderZ; j++)
			{
				double melementPow = pow(melement, j);
				gsl_matrix_set(indVarZPow, i, j, melementPow);
			}
		}
		
		/*std::cout << "IndVar: ";
		vectorUtils.printGSLVector(indVarZ);*/
		
		// Loop through fits
		
		double errorC = 0;
		int c = 0;
		for (int a = 0; a < orderX; a++) // a \/ Go through b terms that make up a coefficients
		{
			for(int i = 0; i < orderY; i++) // b - > Loop through powers of b coefficients for a_i
			{
				for(int j = 0; j < numZ; j++) // b_n \/ Get b_ij coefficients for each value of z
				{
					double melement = gsl_matrix_get(bCoeff, a + (j * orderX), i);
					gsl_vector_set(depVarZ, j, melement);
				}
				
				
				 /*std::cout << "IndVar " << std::endl;
				 vectorUtils.printGSLVector(indVarZ);
				 std::cout << "DepVar_" << a <<  "-" << i << ": ";
				 vectorUtils.printGSLVector(depVarZ);*/
							
				// Solve
				gsl_multifit_linear_workspace *workspace;
				workspace = gsl_multifit_linear_alloc(numZ, orderZ);
				gsl_matrix *cov;
				double chisq;//
				cov = gsl_matrix_alloc(orderZ, orderZ);
				gsl_multifit_linear(indVarZPow, depVarZ, tempCcoeff, cov, &chisq, workspace);
				gsl_multifit_linear_free(workspace);
				gsl_matrix_free(cov);
				
				// Add coefficents to Matrix
				for(int k = 0; k < orderZ; k++)
				{
					double coeffElement = gsl_vector_get(tempCcoeff, k);
					gsl_matrix_set(cCoeff, c, k, coeffElement);
				}
				
				// ChiSq
				//std::cout << "chisq = "<< chisq << std::endl;
				errorC = errorC + chisq;
				gsl_matrix_set(cCoeff, c, orderZ, chisq);
				c++;
			}
		}
		
		errorC = errorC / c; // Calculate average ChiSq
		std::cout << "Third set of fits complete " << std::endl;
		std::cout << " Average ChiSq for third set of fits = " << errorC << std::endl;
		std::cout << "----------------------------------------------" << std::endl;
		std::cout << std::endl;
		
		// Clean up
		
		gsl_matrix_free(aCoeff);
		gsl_matrix_free(bCoeff);
		
		gsl_matrix_free(indVarXPow);
		gsl_vector_free(depVarX);
		gsl_vector_free(tempAcoeff);
		gsl_vector_free(indVarY);
		gsl_vector_free(indVarZ);
		
		gsl_matrix_free(indVarYPow);
		gsl_vector_free(depVarY); 
		gsl_vector_free(tempBcoeff); 
		
		gsl_matrix_free(indVarZPow); 
		gsl_vector_free(depVarZ); 
		gsl_vector_free(tempCcoeff); 
		
		//matrixUtils.printGSLMatrix(cCoeff);
		
		return cCoeff;
	}
	
	gsl_matrix* RSGISPolyFit::PolyTestThreeDimension(int orderX, int orderY, int orderZ, gsl_matrix *inData, gsl_matrix *coefficients)
	{
		/// Tests a three dimensional polynomal equation, outputs measured and predicted values to a matrix.
		RSGISMatrices matrixUtils;
		gsl_matrix *measuredVpredictted;
		measuredVpredictted = gsl_matrix_alloc(inData->size1, 2); // Set up matrix to hold measured and predicted y values.
				
		for(unsigned int i = 0; i < inData->size1; i++) // Loop through inData
		{
			double xVal, yVal, zVal;
			double xPow, yPow, zPow;
			double fMeasured, fPredicted;
			double bcoeffPowY, cCoeffPowZ;
			long double cCoeff;
			
			xVal = gsl_matrix_get(inData, i, 0); // Get x value
			yVal = gsl_matrix_get(inData, i, 1); // Get y value
			zVal = gsl_matrix_get(inData, i, 2); // Get z value
			
			unsigned int c = 0;
			fMeasured = gsl_matrix_get(inData, i, 3); // Get measured f value.
			fPredicted = 0.0;
			for(int x = 0; x < orderX; x ++) 
			{
				bcoeffPowY = 0.0; 
				for(int y = 0; y < orderY; y++)
				{
					cCoeffPowZ = 0.0;
					//std::cout << "cCoeff = ";
					for(int z = 0; z < orderZ; z++)
					{     
						zPow = pow(zVal, z);
						cCoeff = gsl_matrix_get(coefficients, c, z);
						//cCoeff = gsl_matrix_get(coefficients, y + (x * orderX), z);
						//std::cout << cCoeff;
						cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
					}
					//std::cout << "c = " << c << std::endl;
					c++; // Itterate through lines in coefficients file
					yPow = pow(yVal, y); // y^n;
					bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
				}
				xPow = pow(xVal, x); // dielectric^n;
				fPredicted = fPredicted + (bcoeffPowY * xPow);
			}
			
			//std::cout << "measured = " << fMeasured << " predicted = " << fPredicted << std::endl;
			
			gsl_matrix_set(measuredVpredictted, i, 0, fMeasured);
			gsl_matrix_set(measuredVpredictted, i, 1, fPredicted);
		}
		
		this->calcRSquaredGSLMatrix(measuredVpredictted);
		this->calcRMSErrorGSLMatrix(measuredVpredictted);
		
		//matrixUtils.printGSLMatrix(measuredVpredictted);
		return measuredVpredictted;
	}
	
	void RSGISPolyFit::calcRSquaredGSLMatrix(gsl_matrix *dataXY)
	{
		double sumX = 0;
		double sumY = 0;
		
		// Calc mean
		for(unsigned int i = 0; i < dataXY->size1; i++)
		{
			sumX = sumX + gsl_matrix_get(dataXY,i , 0);
			sumY = sumY + gsl_matrix_get(dataXY,i , 1);
		}
		
		double xMean = sumX / dataXY->size1;
		double yMean = sumY / dataXY->size1;
		
		double xMeanSq = xMean * xMean;
		double yMeanSq = yMean * yMean;
		double xyMean = xMean * yMean;
		
		double ssXX = 0;
		double ssYY = 0;
		double ssXY = 0;
		
		for(unsigned int i = 0; i < dataXY->size1; i++)
		{
			
			double dataX = gsl_matrix_get(dataXY,i , 0);
			double dataY = gsl_matrix_get(dataXY,i , 1);
			
			ssXX = ssXX + ((dataX * dataX) - xMeanSq);
			ssYY = ssYY + ((dataY * dataY) - yMeanSq);
			ssXY = ssXY + ((dataX * dataY) - xyMean);
		}
		
		double rSq = (ssXY * ssXY ) / (ssXX * ssYY);
		
		std::cout << "**************************" << std::endl;
		std::cout << "  R squared = " << rSq << std::endl;
		std::cout << "**************************" << std::endl;
	}
	
	double RSGISPolyFit::calcRSquaredGSLMatrixQuiet(gsl_matrix *dataXY)
	{
		double sumX = 0;
		double sumY = 0;
		
		// Calc mean
		for(unsigned int i = 0; i < dataXY->size1; i++)
		{
			sumX = sumX + gsl_matrix_get(dataXY,i , 0);
			sumY = sumY + gsl_matrix_get(dataXY,i , 1);
		}
		
		double xMean = sumX / dataXY->size1;
		double yMean = sumY / dataXY->size1;
		
		double xMeanSq = xMean * xMean;
		double yMeanSq = yMean * yMean;
		double xyMean = xMean * yMean;
		
		double ssXX = 0;
		double ssYY = 0;
		double ssXY = 0;
		
		for(unsigned int i = 0; i < dataXY->size1; i++)
		{
			
			double dataX = gsl_matrix_get(dataXY,i , 0);
			double dataY = gsl_matrix_get(dataXY,i , 1);
			
			ssXX = ssXX + ((dataX * dataX) - xMeanSq);
			ssYY = ssYY + ((dataY * dataY) - yMeanSq);
			ssXY = ssXY + ((dataX * dataY) - xyMean);
		}
		
		double rSq = (ssXY * ssXY ) / (ssXX * ssYY);
		
		return rSq;
	}
	
	void RSGISPolyFit::calcRMSErrorGSLMatrix(gsl_matrix *dataXY)
	{
		double sqSum = 0;
		
		// Calc mean
		for(unsigned int i = 0; i < dataXY->size1; i++)
		{
			sqSum = sqSum + pow((gsl_matrix_get(dataXY,i , 0) - gsl_matrix_get(dataXY,i , 1)),2);
		}
		
		double sqMean = sqSum / double(dataXY->size1);
		
		double rmse = sqrt(sqMean);
		
		std::cout << "**************************" << std::endl;
		std::cout << "  RMSE = " << rmse << std::endl;
		std::cout << "**************************" << std::endl;
	}
	
	void RSGISPolyFit::calcMeanErrorGSLMatrix(gsl_matrix *dataXY)
	{
		double sum = 0;
		
		// Calc mean
		for(unsigned int i = 0; i < dataXY->size1; i++)
		{
			sum = sum + (gsl_matrix_get(dataXY,i , 0) - gsl_matrix_get(dataXY,i , 1));
		}
		
		double meanError = sum / double(dataXY->size1);
		
		std::cout << "**************************" << std::endl;
		std::cout << "  Mean Error = " << meanError << std::endl;
		std::cout << "**************************" << std::endl;
	}
	
	double RSGISPolyFit::calcRMSErrorGSLMatrixQuiet(gsl_matrix *dataXY)
	{
		double sqSum = 0;
		
		// Calc mean
		for(unsigned int i = 0; i < dataXY->size1; i++)
		{
			sqSum = sqSum + pow((gsl_matrix_get(dataXY,i , 0) - gsl_matrix_get(dataXY,i , 1)),2);
		}
		
		double sqMean = sqSum / double(dataXY->size1);
		
		double rmse = sqrt(sqMean);
		return rmse;
	}
	
	double RSGISPolyFit::calcMeanErrorGSLMatrixQuiet(gsl_matrix *dataXY)
	{
		double sum = 0;
		
		// Calc mean
		for(unsigned int i = 0; i < dataXY->size1; i++)
		{
			sum = sum + (gsl_matrix_get(dataXY,i , 0) - gsl_matrix_get(dataXY,i , 1));
		}
		
		double meanError = sum / double(dataXY->size1);
		return meanError;
	}
	
	RSGISPolyFit::~RSGISPolyFit()
	{
	}	
	
}}

