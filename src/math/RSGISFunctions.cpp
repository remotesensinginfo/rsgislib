/*
 *  RSGISFunctions.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 05/01/2010.
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

#include "RSGISFunctions.h"

namespace rsgis{namespace math{
    
	RSGISFunctionPolynomial::RSGISFunctionPolynomial(double *coefficients, int order)
	{
		this->coefficients = coefficients;
		this->order = order;
	}
	double RSGISFunctionPolynomial::calcFunction(double xVal) throw(RSGISMathException)
	{
		double yVal = 0; 
		for(int i = 0; i < order ; i++)
		{
			double xPow = pow(xVal, i); // x^n;
			double coeff = coefficients[i]; // a_n
			double coeffXPow = coeff * xPow; // a_n * x^n				
			yVal = yVal + coeffXPow;
		}
		
		return yVal;
	}
	double RSGISFunctionPolynomial::dX(double xVal) throw(RSGISMathException)
	{
		double yVal = 0; 
		for(int i = 0; i < order ; i++)
		{
			double xPow = i * pow(xVal, i - 1); // x^n;
			double coeff = coefficients[i]; // a_n
			double coeffXPow = coeff * xPow; // a_n * x^n				
			yVal = yVal + coeffXPow;
		}
		
		return yVal;
	}
	RSGISFunctionPolynomial::~RSGISFunctionPolynomial()
	{
		
	}

	RSGISFunctionPolynomialGSL::RSGISFunctionPolynomialGSL(gsl_vector *coefficients, int order)
	{
		this->coefficients = coefficients;
		this->order = order;
	}
	double RSGISFunctionPolynomialGSL::calcFunction(double xVal) throw(RSGISMathException)
	{
		double yVal = 0; 
		for(int i = 0; i < order ; i++)
		{
			double xPow = pow(xVal, i); // x^n;
			double coeff = gsl_vector_get(coefficients, i); // a_n
			double coeffXPow = coeff * xPow; // a_n * x^n				
			yVal = yVal + coeffXPow;
		}
		
		return yVal;
	}
	double RSGISFunctionPolynomialGSL::dX(double xVal) throw(RSGISMathException)
	{
		double yVal = 0; 
		for(int i = 0; i < order ; i++)
		{
			double xPow = i * pow(xVal, i - 1); // x^n;
			double coeff = gsl_vector_get(coefficients, i); // a_n
			double coeffXPow = coeff * xPow; // a_n * x^n				
			yVal = yVal + coeffXPow;
		}
		
		return yVal;
	}
	
	void RSGISFunctionPolynomialGSL::updateCoefficents(double *newCoefficents) throw(RSGISMathException)
	{
		for(int i = 0; i < order; i++)
		{
			gsl_vector_set(coefficients, i, newCoefficents[i]);
		}
	}
	
	RSGISFunctionPolynomialGSL::~RSGISFunctionPolynomialGSL()
	{
		
	}
	
	RSGISFunctionLn::RSGISFunctionLn(double coeffA, double coeffB)
	{
		this->coeffA = coeffA;
		this->coeffB = coeffB;
	}
	double RSGISFunctionLn::calcFunction(double value) throw(RSGISMathException)
	{
		return this->coeffA + (this->coeffB * log(value));
	}
	double RSGISFunctionLn::dX(double value) throw(RSGISMathException)
	{
		return this->coeffB / value;
	}
	RSGISFunctionLn::~RSGISFunctionLn()
	{
		
	}
	
	RSGISFunction2VarLn::RSGISFunction2VarLn(double coeffA, double coeffB, double coeffC)
	{
		this->coeffA = coeffA;
		this->coeffB = coeffB;
		this->coeffC = coeffC;
	}
	double RSGISFunction2VarLn::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		return this->coeffA + (this->coeffB * valueY) + (this->coeffC * valueY * log(valueX));
	}
	double RSGISFunction2VarLn::dX(double valueX, double valueY) throw(RSGISMathException)
	{
		return (this->coeffC * valueY) / (valueX);
	}
	double RSGISFunction2VarLn::dY(double valueX, double valueY) throw(RSGISMathException)
	{
		return this->coeffB + (this->coeffC * log(valueX));
	}
	RSGISFunction2VarLn::~RSGISFunction2VarLn()
	{
		
	}
	
	RSGISFunctionLinXfLinY::RSGISFunctionLinXfLinY(double coeffB0, double coeffB1, double coeffC0, double coeffC1)
	{
		this->coeffB0 = coeffB0;
		this->coeffB1 = coeffB1;
		this->coeffC0 = coeffC0;
		this->coeffC1 = coeffC1;

	}
	double RSGISFunctionLinXfLinY::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		double a0 = coeffB0 + (coeffB1 * valueY);
		double a1 = coeffC0 + (coeffC1 * valueY);
		return a0 + (a1 * valueX);
	}
	double RSGISFunctionLinXfLinY::dX(double valueX, double valueY) throw(RSGISMathException)
	{
		return coeffC0 + (coeffC1 * valueY);
	}
	double RSGISFunctionLinXfLinY::dY(double valueX, double valueY) throw(RSGISMathException)
	{
		return coeffB1 + (coeffC1 * valueX);
	}
	RSGISFunctionLinXfLinY::~RSGISFunctionLinXfLinY()
	{
		
	}
	
	RSGISFunction2VarLnQuadratic::RSGISFunction2VarLnQuadratic(double coeffA, double coeffB, double coeffC)
	{
		this->coeffA = coeffA;
		this->coeffB = coeffB;
		this->coeffC = coeffC;
	}
	double RSGISFunction2VarLnQuadratic::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		return this->coeffA + (this->coeffB * log(valueX)) + (this->coeffC * valueY * log(valueX * valueX));
	}
	double RSGISFunction2VarLnQuadratic::dX(double valueX, double valueY) throw(RSGISMathException)
	{
		return (this->coeffC * valueY) / (valueX);
	}
	double RSGISFunction2VarLnQuadratic::dY(double valueX, double valueY) throw(RSGISMathException)
	{
		return this->coeffB + (this->coeffC * log(valueX));
	}
	RSGISFunction2VarLnQuadratic::~RSGISFunction2VarLnQuadratic()
	{
		
	}
	
	RSGISFunction2Var2DataLeastSquares::RSGISFunction2Var2DataLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, double dataA, double dataB)
	{
		this->functionA = functionA;
		this->functionB = functionB;
		this->dataA = dataA;
		this->dataB = dataB;
	}
	double RSGISFunction2Var2DataLeastSquares::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		double diffA = dataA - functionA->calcFunction(valueX, valueY);
		double diffB = dataB - functionB->calcFunction(valueX, valueY);
		double returnVal =  (pow(diffA, 2) + pow(diffB, 2));
		
		if(returnVal < 0)
		{
            std::cout << "ERROR - but continuing!" << std::endl;
		}
		return returnVal;
	}
	RSGISFunction2Var2DataLeastSquares::~RSGISFunction2Var2DataLeastSquares()
	{
		
	}
	
	RSGISFunction2Var3DataLeastSquares::RSGISFunction2Var3DataLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, RSGISMathTwoVariableFunction *functionC, double dataA, double dataB, double dataC)
	{
		this->functionA = functionA;
		this->functionB = functionB;
		this->functionC = functionC;
		this->dataA = dataA;
		this->dataB = dataB;
		this->dataC = dataC;
	}
	double RSGISFunction2Var3DataLeastSquares::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		
		double diffA = dataA - functionA->calcFunction(valueX, valueY);
		double diffB = dataB - functionB->calcFunction(valueX, valueY);
		double diffC = dataC - functionC->calcFunction(valueX, valueY);
		
		//std::cout << diffA << ", " << diffB << ", " << diffC << std::endl;
		
		return pow(diffA, 2) + pow(diffB, 2) + pow(diffC, 2);
	}
	RSGISFunction2Var3DataLeastSquares::~RSGISFunction2Var3DataLeastSquares()
	{
		
	}
	
	RSGISFunction2Var2DataPreconditionedLeastSquares::RSGISFunction2Var2DataPreconditionedLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, 
																									   double dataA, double dataB, double apX1, double apX2, gsl_matrix *invCovMatrixP, gsl_matrix *invCovMatrixD)	
	{
		this->functionA = functionA;
		this->functionB = functionB;
		this->dataA = dataA;
		this->dataB = dataB;
		this->apX1 = apX1;
		this->apX2 = apX2;
		this->invCovMatrixD = invCovMatrixD;
		this->invCovMatrixP = invCovMatrixP;
		this->deltaD = gsl_vector_alloc(2);
		this->deltaX = gsl_vector_alloc(2);
		this->tempA = gsl_vector_alloc(2);
		this->useAP = true;
		if ((gsl_matrix_get(invCovMatrixP, 0, 0) < 1e-8) && (gsl_matrix_get(invCovMatrixP, 1, 1) < 1e-8)) 
		{
			this->useAP = false;
		}

	}
	double RSGISFunction2Var2DataPreconditionedLeastSquares::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		/** L(X) = 1/2 { || f(X) - d0 || ^2 + || X - Xap || ^2 } */
		
		RSGISMatrices matrixUtils;
		RSGISVectors vectorUtils;
		//matrixUtils.printGSLMatrix(this->invCovMatrixP);
		
		// || f(X) - d0 || ^2
		double diffD1 = dataA - functionA->calcFunction(valueX, valueY);
		double diffD2 = dataB - functionB->calcFunction(valueX, valueY);
		
		gsl_vector_set(deltaD, 0, diffD1);
		gsl_vector_set(deltaD, 1, diffD2);
		
		matrixUtils.productMatrixVectorGSL(invCovMatrixD, deltaD, tempA);
		
		double diffD = vectorUtils.dotProductVectorVectorGSL(tempA, deltaD);
		
		// || X - Xap || ^2 
		//std::cout << "\tdiffX1 = " << diffX1 << " diffX2 = " << diffX2 << std::endl;
		double diffX = 0;
		if (useAP) 
		{
			double diffX1 = valueX - apX1;
			double diffX2 = valueY - apX2;
			
			gsl_vector_set(deltaX, 0, diffX1);
			gsl_vector_set(deltaX, 1, diffX2);
			
			matrixUtils.productMatrixVectorGSL(invCovMatrixP, deltaX, tempA);
			
			diffX = vectorUtils.dotProductVectorVectorGSL(tempA, deltaX);
		}
		
		//std::cout << "\tdiffD = " << diffD << std::endl;
		//std::cout << "\tdiffX = " << diffX << std::endl;
		
		return (diffD + diffX) / 2;
	}
	RSGISFunction2Var2DataPreconditionedLeastSquares::~RSGISFunction2Var2DataPreconditionedLeastSquares()
	{
		gsl_vector_free(deltaD);
		gsl_vector_free(deltaX);
		gsl_vector_free(tempA);
	}
	
	RSGISFunction2Var3DataPreconditionedLeastSquares::RSGISFunction2Var3DataPreconditionedLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, RSGISMathTwoVariableFunction *functionC, 
																									   double dataA, double dataB, double dataC, double apX1, double apX2, gsl_matrix *invCovMatrixP, gsl_matrix *invCovMatrixD)	
	{
		this->functionA = functionA;
		this->functionB = functionB;
		this->functionC = functionC;
		this->dataA = dataA;
		this->dataB = dataB;
		this->dataC = dataC;
		this->apX1 = apX1;
		this->apX2 = apX2;
		this->invCovMatrixD = invCovMatrixD;
		this->invCovMatrixP = invCovMatrixP;
		this->deltaD = gsl_vector_alloc(3);
		this->deltaX = gsl_vector_alloc(2);
		this->tempA = gsl_vector_alloc(2); 
		this->tempB = gsl_vector_alloc(3);
		this->useAP = true;
		if ((gsl_matrix_get(invCovMatrixP, 0, 0) < 1e-8) && (gsl_matrix_get(invCovMatrixP, 1, 1) < 1e-8)) 
		{
			this->useAP = false;
		}
	}
	double RSGISFunction2Var3DataPreconditionedLeastSquares::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		/** L(X) = 1/2 { || f(X) - d0 || ^2 + || X - Xap || ^2 } */
		
		RSGISMatrices matrixUtils;
		RSGISVectors vectorUtils;
		
		// || f(X) - d0 || ^2
		double diffD1 = dataA - functionA->calcFunction(valueX, valueY);
		double diffD2 = dataB - functionB->calcFunction(valueX, valueY);
		double diffD3 = dataC - functionC->calcFunction(valueX, valueY);
		
		gsl_vector_set(deltaD, 0, diffD1);
		gsl_vector_set(deltaD, 1, diffD2);
		gsl_vector_set(deltaD, 2, diffD3);
		
		matrixUtils.productMatrixVectorGSL(invCovMatrixD, deltaD, tempB);
		
		double diffD = vectorUtils.dotProductVectorVectorGSL(tempB, deltaD);
		
		// || X - Xap || ^2 
		double diffX = 0;
		if (useAP) 
		{
			double diffX1 = valueX - apX1;
			double diffX2 = valueY - apX2;
			
			gsl_vector_set(deltaX, 0, diffX1);
			gsl_vector_set(deltaX, 1, diffX2);
			
			matrixUtils.productMatrixVectorGSL(invCovMatrixP, deltaX, tempA);
			
			diffX = vectorUtils.dotProductVectorVectorGSL(tempA, deltaX);
		}
		
		double returnVal = (diffD + diffX) / 2.0;
		if(returnVal < 0)
		{
			std::cout << "Negative!" << std::endl;
			returnVal = 999999;
		}
		return returnVal;
		
	}
	RSGISFunction2Var3DataPreconditionedLeastSquares::~RSGISFunction2Var3DataPreconditionedLeastSquares()
	{
		gsl_vector_free(deltaD);
		gsl_vector_free(deltaX);
		gsl_vector_free(tempA);
		gsl_vector_free(tempB);
	}
	
	RSGISFunction3Var3DataPreconditionedLeastSquares::RSGISFunction3Var3DataPreconditionedLeastSquares(RSGISMathThreeVariableFunction *functionA, RSGISMathThreeVariableFunction *functionB, RSGISMathThreeVariableFunction *functionC, 
																									   double dataA, double dataB, double dataC, double apX1, double apX2, double apX3, gsl_matrix *invCovMatrixP, gsl_matrix *invCovMatrixD)	
	{
		this->functionA = functionA;
		this->functionB = functionB;
		this->functionC = functionC;
		this->dataA = dataA;
		this->dataB = dataB;
		this->dataC = dataC;
		this->apX1 = apX1;
		this->apX2 = apX2;
		this->apX3 = apX3;
		this->invCovMatrixD = invCovMatrixD;
		this->invCovMatrixP = invCovMatrixP;
		this->deltaD = gsl_vector_alloc(3);
		this->deltaX = gsl_vector_alloc(3);
		this->tempA = gsl_vector_alloc(3); 
		this->tempB = gsl_vector_alloc(3);
		this->useAP = true;
		
		if ((gsl_matrix_get(invCovMatrixP, 0, 0) < 1e-8) && (gsl_matrix_get(invCovMatrixP, 1, 1) < 1e-8) && (gsl_matrix_get(invCovMatrixP, 2, 2) < 1e-8)) 
		{
			this->useAP = false;
		}
		
	}
	double RSGISFunction3Var3DataPreconditionedLeastSquares::calcFunction(double valueX, double valueY, double valueZ) throw(RSGISMathException)
	{
		/** L(X) = 1/2 { || f(X) - d0 || ^2 + || X - Xap || ^2 } */
		
		RSGISMatrices matrixUtils;
		RSGISVectors vectorUtils;
		
		// || f(X) - d0 || ^2
		double diffD1 = dataA - functionA->calcFunction(valueX, valueY, valueZ);
		double diffD2 = dataB - functionB->calcFunction(valueX, valueY, valueZ);
		double diffD3 = dataC - functionC->calcFunction(valueX, valueY, valueZ);
		
		gsl_vector_set(deltaD, 0, diffD1);
		gsl_vector_set(deltaD, 1, diffD2);
		gsl_vector_set(deltaD, 2, diffD3);
		
		matrixUtils.productMatrixVectorGSL(invCovMatrixD, deltaD, tempB);
		
		double diffD = vectorUtils.dotProductVectorVectorGSL(tempB, deltaD);
		
		// || X - Xap || ^2 
		double diffX = 0;
		if (this->useAP) 
		{
			double diffX1 = valueX - apX1;
			double diffX2 = valueY - apX2;
			double diffX3 = valueZ - apX3;
			
			gsl_vector_set(deltaX, 0, diffX1);
			gsl_vector_set(deltaX, 1, diffX2);
			gsl_vector_set(deltaX, 2, diffX3);
			
			matrixUtils.productMatrixVectorGSL(invCovMatrixP, deltaX, tempA);
			
			diffX = vectorUtils.dotProductVectorVectorGSL(tempA, deltaX);
		}

		/*if (diffX < 0)
		{
			std::cout << "Negative AP!" << std::endl;
			std::cout << "tempA: ";
			vectorUtils.printGSLVector(tempA);
			std::cout << "deltaX:";
			vectorUtils.printGSLVector(deltaX);
		}
		if (diffD < 0) 
		{
			std::cout << "Negative D!" << std::endl;
		}*/
			
		return (diffD + diffX) / 2;
		
	}
	RSGISFunction3Var3DataPreconditionedLeastSquares::~RSGISFunction3Var3DataPreconditionedLeastSquares()
	{
		gsl_vector_free(deltaD);
		gsl_vector_free(deltaX);
		gsl_vector_free(tempA);
		gsl_vector_free(tempB);
	}
	
	RSGISFunction3Var4DataPreconditionedLeastSquares::RSGISFunction3Var4DataPreconditionedLeastSquares(RSGISMathThreeVariableFunction *functionA, RSGISMathThreeVariableFunction *functionB, RSGISMathThreeVariableFunction *functionC, RSGISMathThreeVariableFunction *functionD, 
																									   double dataA, double dataB, double dataC, double dataD, double apX1, double apX2, double apX3, gsl_matrix *invCovMatrixP, gsl_matrix *invCovMatrixD)	
	{
		this->functionA = functionA;
		this->functionB = functionB;
		this->functionC = functionC;
		this->functionD = functionD;
		this->dataA = dataA;
		this->dataB = dataB;
		this->dataC = dataC;
		this->dataD = dataD;
		this->apX1 = apX1;
		this->apX2 = apX2;
		this->apX3 = apX3;
		this->invCovMatrixD = invCovMatrixD;
		this->invCovMatrixP = invCovMatrixP;
		this->deltaD = gsl_vector_alloc(4);
		this->deltaX = gsl_vector_alloc(3);
		this->tempA = gsl_vector_alloc(3); 
		this->tempB = gsl_vector_alloc(4);
		this->useAP = true;
		if ((gsl_matrix_get(invCovMatrixP, 0, 0) < 1e-8) && (gsl_matrix_get(invCovMatrixP, 1, 1) < 1e-8) && (gsl_matrix_get(invCovMatrixP, 2, 2) < 1e-8)) 
		{
			this->useAP = false;
		}
	}
	double RSGISFunction3Var4DataPreconditionedLeastSquares::calcFunction(double valueX, double valueY, double valueZ) throw(RSGISMathException)
	{
		/** L(X) = 1/2 { || f(X) - d0 || ^2 + || X - Xap || ^2 } */
		
		RSGISMatrices matrixUtils;
		RSGISVectors vectorUtils;
		
		// || f(X) - d0 || ^2
		double diffD1 = dataA - functionA->calcFunction(valueX, valueY, valueZ);
		double diffD2 = dataB - functionB->calcFunction(valueX, valueY, valueZ);
		double diffD3 = dataC - functionC->calcFunction(valueX, valueY, valueZ);
		double diffD4 = dataD - functionD->calcFunction(valueX, valueY, valueZ);
		
		gsl_vector_set(deltaD, 0, diffD1);
		gsl_vector_set(deltaD, 1, diffD2);
		gsl_vector_set(deltaD, 2, diffD3);
		gsl_vector_set(deltaD, 3, diffD4);
		
		matrixUtils.productMatrixVectorGSL(invCovMatrixD, deltaD, tempB);
		
		double diffD = vectorUtils.dotProductVectorVectorGSL(tempB, deltaD);
		
		// || X - Xap || ^2 
		double diffX = 0;
		if (this->useAP) 
		{
			double diffX1 = valueX - apX1;
			double diffX2 = valueY - apX2;
			double diffX3 = valueZ - apX3;
			
			gsl_vector_set(deltaX, 0, diffX1);
			gsl_vector_set(deltaX, 1, diffX2);
			gsl_vector_set(deltaX, 2, diffX3);
			
			matrixUtils.productMatrixVectorGSL(invCovMatrixP, deltaX, tempA);
			
			diffX = vectorUtils.dotProductVectorVectorGSL(tempA, deltaX);
		}
		
		return (diffD + diffX) / 2;
		
	}
	RSGISFunction3Var4DataPreconditionedLeastSquares::~RSGISFunction3Var4DataPreconditionedLeastSquares()
	{
		gsl_vector_free(deltaD);
		gsl_vector_free(deltaX);
		gsl_vector_free(tempA);
		gsl_vector_free(tempB);
	}
	
	
	RSGISFunctionEstimationLeastSquares::RSGISFunctionEstimationLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, double dataA, double dataB)
	{
		this->functionA = functionA;
		this->functionB = functionB;
		this->dataA = dataA;
		this->dataB = dataB;
	}
	double RSGISFunctionEstimationLeastSquares::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		double fHH = functionA->calcFunction(valueX, valueY);
		double fHV = functionB->calcFunction(valueX, valueY);
		
		if(fHH > 0)
		{
			return 1e10;
		}
		if (fHV > 0) 
		{
			return 1e10;
		}
		
		double diffA = dataA - fHH;
		double diffB = dataB - fHV;
		
		return (pow(diffA, 2) + pow(diffB, 2));
	}
	RSGISFunctionEstimationLeastSquares::~RSGISFunctionEstimationLeastSquares()
	{
		
	}
	
	RSGISFunction2DPoly::RSGISFunction2DPoly(gsl_matrix *inCoefficients)
	{
		this->coefficients = gsl_matrix_alloc(inCoefficients->size1, inCoefficients->size2);
		gsl_matrix_memcpy(this->coefficients, inCoefficients);
		this->orderX = coefficients->size1; // Get polynomial order
		this->orderY = coefficients->size2 - 1; // Get polynomial order
	}
	double RSGISFunction2DPoly::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{		
		double outVal = 0.0;
		
		for(int x = 0; x < orderX; x ++) 
		{
			double xPow = pow(valueX, x); // x^n;
			
			double aCoeff = 0.0; 
			
			for(int y = 0; y < orderY ; y++) // Calculate a_n(density)
			{
				double yPow = pow(valueY,y); // y^n;
				double bcoeff = gsl_matrix_get(coefficients, x, y); // b_n
				aCoeff = aCoeff + (bcoeff * yPow);
			}
			
			double acoeffXPow = xPow * aCoeff;
			
			outVal = outVal + acoeffXPow;
		}
		
		/*if ((valueX < 0) || (valueY < 0)) 
		{
			outVal = 0;
		}*/
		return outVal;
	}
	double RSGISFunction2DPoly::dX(double valueX, double valueY) throw(RSGISMathException)
	{
		double outValdX = 0.0;
		
		for(int x = 0; x < orderX; x ++) 
		{
			double dxPow = x * pow(valueX, x - 1); // n.x^(n-1);
			
			double aCoeff = 0.0;
			
			for(int y = 0; y < orderY ; y++) // 
			{
				double yPow = pow(valueY, y); // y^n;
				double bcoeff = gsl_matrix_get(coefficients, x, y); // b_n
				aCoeff = aCoeff + (bcoeff * yPow);
			}
			
			double acoeffXPowF = dxPow * aCoeff; // d(f_nl)/ dx (partial derivative with respect to x)
			
			outValdX = outValdX + acoeffXPowF;

		}
		
		/*if ((valueX < 0)) 
		{
			outValdX = valueX;
		}*/
		
		return outValdX;
	}
	double RSGISFunction2DPoly::dY(double valueX, double valueY) throw(RSGISMathException)
	{

		double outValdY = 0.0;
		
		for(int x = 0; x < orderX; x ++) 
		{
			double xPow = pow(valueX, x); // x^n;
			
			double aCoeffF = 0.0; 
			
			for(int y = 0; y < orderY ; y++) // Calculate a_n(y)
			{
				double dyPow = y * pow(valueY, y - 1); // n * y^(n-1)
				double bcoeff = gsl_matrix_get(coefficients, x, y); // b_n
				aCoeffF = aCoeffF + (bcoeff * dyPow);
			}
			
			outValdY = xPow * aCoeffF;  // d(f_nl)/ dy (partial derivative with respect to density)
			
		}
		
		/*if ((valueY < 0)) 
		{
			outValdY = valueY;
		}*/
		
		return outValdY;
	}
	RSGISFunction2DPoly::~RSGISFunction2DPoly()
	{
		gsl_matrix_free(this->coefficients);
	}
	
	RSGISFunction3DPoly::RSGISFunction3DPoly(gsl_matrix *inCoefficients, int orderX, int orderY, int orderZ)
	{
		this->coefficients = gsl_matrix_alloc(inCoefficients->size1, inCoefficients->size2);
		gsl_matrix_memcpy(this->coefficients, inCoefficients);
		this->orderX = orderX;
		this->orderY = orderY;
		this->orderZ = orderZ;
	}
	double RSGISFunction3DPoly::calcFunction(double valueX, double valueY, double valueZ) throw(RSGISMathException)
	{		
		double outVal = 0.0;
		double aCoeffPowX = 0.0;
		double bcoeffPowY = 0;
		double cCoeffPowZ = 0;
		double zPow = 0;
		double cCoeff = 0;
		double yPow = 0;
		double xPow = 0;
		
		unsigned int c = 0;
		for(int x = 0; x < orderX; x ++) 
		{
			bcoeffPowY = 0.0; 
			for(int y = 0; y < orderY; y++)
			{
				cCoeffPowZ = 0.0;
				for(int z = 0; z < orderZ; z++)
				{     
					zPow = pow(valueZ, z); // z^n;
					//cCoeff = gsl_matrix_get(coefficients, y + (x * orderX), z);
					cCoeff = gsl_matrix_get(coefficients, c, z);
					cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
				}
				yPow = pow(valueY, y); // y^n;
				bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
				c++;
			}
			xPow = pow(valueX, x); // dielectric^n;
			aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
			//std::cout << "HH - b_" << x << " = " << bcoeffPowY << std::endl;
		}
		outVal = aCoeffPowX;
        
		return outVal;
	}
	double RSGISFunction3DPoly::dX(double valueX, double valueY, double valueZ) throw(RSGISMathException)
	{
		double outValdX = 0.0;
		double aCoeffPowX = 0.0;
		double aCoeffPowXdX = 0.0;
		double aCoeffPowXdY = 0.0;
		double aCoeffPowXdZ = 0.0;
		double bcoeffPowY = 0;
		double bcoeffPowYdY = 0;
		double bcoeffPowYdZ = 0;
		double cCoeffPowZ = 0;
		double cCoeffPowZdZ = 0;
		double zPow = 0;
		double dzPow = 0;
		double cCoeff = 0;
		double yPow = 0;
		double dyPow = 0;
		double xPow = 0;
		double dxPow = 0;
		
		unsigned int c = 0;
		for(int x = 0; x < orderX; x ++) 
		{
			bcoeffPowY = 0.0; 
			bcoeffPowYdY = 0.0;
			bcoeffPowYdZ = 0.0;
			for(int y = 0; y < orderY; y++)
			{
				
				cCoeffPowZ = 0.0;
				cCoeffPowZdZ = 0.0;
				for(int z = 0; z < orderZ; z++)
				{     
					zPow = pow(valueZ, z); // z^n;
					dzPow = z*pow(valueZ, z-1); // n * z^(n -1)
					cCoeff = gsl_matrix_get(coefficients, c, z); 
					cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
					cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
				}
				yPow = pow(valueY, y); // y^n;
				dyPow = y*pow(valueY, y-1); // n * y^(n -1);
				bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
				bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
				bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
				c++;
			}
			xPow = pow(valueX, x); // dielectric^n;
			dxPow = x*pow(valueX, x-1); // n * dielectric^(n -1);
			aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
			aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
			aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
			aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
			//std::cout << "HH - b_" << x << " = " << bcoeffPowY << std::endl;
		}
		outValdX = aCoeffPowXdX;
		
		return outValdX;
	}
	double RSGISFunction3DPoly::dY(double valueX, double valueY, double valueZ) throw(RSGISMathException)
	{
		double outValdY = 0.0;
		double aCoeffPowX = 0.0;
		double aCoeffPowXdX = 0.0;
		double aCoeffPowXdY = 0.0;
		double aCoeffPowXdZ = 0.0;
		double bcoeffPowY = 0;
		double bcoeffPowYdY = 0;
		double bcoeffPowYdZ = 0;
		double cCoeffPowZ = 0;
		double cCoeffPowZdZ = 0;
		double zPow = 0;
		double dzPow = 0;
		double cCoeff = 0;
		double yPow = 0;
		double dyPow = 0;
		double xPow = 0;
		double dxPow = 0;
		
		unsigned int c = 0;
		for(int x = 0; x < orderX; x ++) 
		{
			bcoeffPowY = 0.0; 
			bcoeffPowYdY = 0.0;
			bcoeffPowYdZ = 0.0;
			for(int y = 0; y < orderY; y++)
			{
				
				cCoeffPowZ = 0.0;
				cCoeffPowZdZ = 0.0;
				for(int z = 0; z < orderZ; z++)
				{     
					zPow = pow(valueZ, z); // z^n;
					dzPow = z*pow(valueZ, z-1); // n * z^(n -1)
					cCoeff = gsl_matrix_get(coefficients,c, z); 
					cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
					cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
				}
				yPow = pow(valueY, y); // y^n;
				dyPow = y*pow(valueY, y-1); // n * y^(n -1);
				bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
				bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
				bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
				c++;
			}
			xPow = pow(valueX, x); // dielectric^n;
			dxPow = x*pow(valueX, x-1); // n * dielectric^(n -1);
			aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
			aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
			aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
			aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
			//std::cout << "HH - b_" << x << " = " << bcoeffPowY << std::endl;
		}
		outValdY = aCoeffPowXdY;
		return outValdY;
	}
	double RSGISFunction3DPoly::dZ(double valueX, double valueY, double valueZ) throw(RSGISMathException)
	{
		double outValdZ = 0.0;
		double aCoeffPowX = 0.0;
		double aCoeffPowXdX = 0.0;
		double aCoeffPowXdY = 0.0;
		double aCoeffPowXdZ = 0.0;
		double bcoeffPowY = 0;
		double bcoeffPowYdY = 0;
		double bcoeffPowYdZ = 0;
		double cCoeffPowZ = 0;
		double cCoeffPowZdZ = 0;
		double zPow = 0;
		double dzPow = 0;
		double cCoeff = 0;
		double yPow = 0;
		double dyPow = 0;
		double xPow = 0;
		double dxPow = 0;
		
		unsigned int c = 0;
		for(int x = 0; x < orderX; x ++) 
		{
			bcoeffPowY = 0.0; 
			bcoeffPowYdY = 0.0;
			bcoeffPowYdZ = 0.0;
			for(int y = 0; y < orderY; y++)
			{
				
				cCoeffPowZ = 0.0;
				cCoeffPowZdZ = 0.0;
				for(int z = 0; z < orderZ; z++)
				{     
					zPow = pow(valueZ, z); // z^n;
					dzPow = z*pow(valueZ, z-1); // n * z^(n -1)
					cCoeff = gsl_matrix_get(coefficients, c, z); 
					cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
					cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
				}
				yPow = pow(valueY, y); // y^n;
				dyPow = y*pow(valueY, y-1); // n * y^(n -1);
				bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
				bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
				bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
				c++;
			}
			xPow = pow(valueX, x); // dielectric^n;
			dxPow = x*pow(valueX, x-1); // n * dielectric^(n -1);
			aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
			aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
			aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
			aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
			//std::cout << "HH - b_" << x << " = " << bcoeffPowY << std::endl;
		}
		outValdZ = aCoeffPowXdZ;
		return outValdZ;
	}
	RSGISFunction3DPoly::~RSGISFunction3DPoly()
	{
		gsl_matrix_free(this->coefficients);
	}
    
	RSGISFunctionNDPoly::RSGISFunctionNDPoly(gsl_matrix *inCoefficients, std::vector<int> *polyOrders)
	{
		this->coefficients = gsl_matrix_alloc(inCoefficients->size1, inCoefficients->size2);
		gsl_matrix_memcpy(this->coefficients, inCoefficients);
        this->polyOrders = polyOrders;
        this->numVar = polyOrders->size();
	}
	double RSGISFunctionNDPoly::calcFunction(std::vector<double> *values) throw(RSGISMathException)
	{		
		double outVal = 0.0;
        if(values->size() != this->numVar)
        {
            throw RSGISMathException("Number of values not equal to number of variables coefficients provided for");
        }
        
		double aCoeffPowX = 0.0;
		double bcoeffPowY = 0;
		double cCoeffPowZ = 0;
		double zPow = 0;
		double cCoeff = 0;
		double yPow = 0;
		double xPow = 0;
        
        if (this->numVar == 2) 
        {
            for(int x = 0; x < this->polyOrders->at(0); x ++) 
            {
                double xPow = pow(values->at(0), x); // x^n;
                
                double aCoeff = 0.0; 
                
                for(int y = 0; y < this->polyOrders->at(1) ; y++) // Calculate a_n(density)
                {
                    double yPow = pow(values->at(1),y); // y^n;
                    double bcoeff = gsl_matrix_get(coefficients, x, y); // b_n
                    aCoeff = aCoeff + (bcoeff * yPow);
                }
                
                double acoeffXPow = xPow * aCoeff;
                
                outVal = outVal + acoeffXPow;
            }
        }
        
        else if(this->numVar == 3)
        {
            unsigned int c = 0;
            for(int x = 0; x < this->polyOrders->at(0); x ++) 
            {
                bcoeffPowY = 0.0; 
                for(int y = 0; y < this->polyOrders->at(1); y++)
                {
                    cCoeffPowZ = 0.0;
                    for(int z = 0; z < this->polyOrders->at(2); z++)
                    {     
                        zPow = pow(values->at(2), z); // z^n;
                        //cCoeff = gsl_matrix_get(coefficients, y + (x * this->polyOrders->at(0)), z);
                        cCoeff = gsl_matrix_get(coefficients, c, z);
                        cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
                    }
                    yPow = pow(values->at(1), y); // y^n;
                    bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
                    c++;
                }
                xPow = pow(values->at(0), x); // dielectric^n;
                aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
                //std::cout << "HH - b_" << x << " = " << bcoeffPowY << std::endl;
            }
            outVal = aCoeffPowX;
        }
        else
        {
            throw RSGISMathException("Currently no implamentation for specified number of values");
        }
        
		return outVal;
	}
    int RSGISFunctionNDPoly::numCoefficients() throw(RSGISMathException)
    {
        return 1;
    }
    int RSGISFunctionNDPoly::numVariables() throw(RSGISMathException)
    {
        return this->numVar;
    }
	RSGISFunctionNDPoly::~RSGISFunctionNDPoly()
	{
		gsl_matrix_free(this->coefficients);
	}
	
	double RSGISFunctionRosenbrocksParabolicValley::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		return (100*pow(valueY - (valueX*valueX),2)) + pow(1 - valueX,2);
	}
	
	RSGISFunctionMuParser::RSGISFunctionMuParser(std::string fxExpression, std::string varName, std::string dxExpression, bool usedX)
	{
		unsigned int nVar = 1;
		this->usedX = usedX;
		this->muParserfX = new mu::Parser();
		this->muParserdX = new mu::Parser();
		this->inVals = new mu::value_type[nVar];
		
		this->muParserfX->DefineVar(_T(varName.c_str()), &inVals[0]);
		this->muParserfX->SetExpr(fxExpression.c_str());
		
		if (usedX) 
		{
			this->muParserdX->DefineVar(_T(varName.c_str()), &inVals[0]);
			this->muParserdX->SetExpr(dxExpression.c_str());
		}
		
	}
	double RSGISFunctionMuParser::calcFunction(double value) throw(RSGISMathException)
	{
		double outVal = 0;
		
		try 
		{
			inVals[0] = value;
			outVal = this->muParserfX->Eval();
			
		}
		catch (mu::ParserError &e) // Catch muParser error
		{
			std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
			throw RSGISMathException(message);
		}
		
		return outVal;
	}
	double RSGISFunctionMuParser::dX(double value) throw(RSGISMathException)
	{
		double outVal = 0;
		if (this->usedX) 
		{
			try 
			{
				inVals[0] = value;
				outVal = this->muParserdX->Eval();
				
			}
			catch (mu::ParserError &e) // Catch muParser error
			{
				std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
				throw RSGISMathException(message);
			}
		}
		else 
		{
			throw RSGISMathException("First derivative of function not provided");
		}
		return outVal;

	}
	RSGISFunctionMuParser::~RSGISFunctionMuParser()
	{
		delete[] this->inVals;
		delete this->muParserfX;
		delete this->muParserdX;
	}

	RSGISFunctionMuParser2Var::RSGISFunctionMuParser2Var(std::string fxyExpression, std::string varName1, std::string varName2, std::string dxExpression, std::string dyExpression, bool usediff)
	{
		unsigned int nVar = 2;
		this->usediff = usediff;
		this->muParserfXY = new mu::Parser();
		this->muParserdX = new mu::Parser();
		this->muParserdY = new mu::Parser();
		this->inVals = new mu::value_type[nVar];
		
		this->muParserfXY->DefineVar(_T(varName1.c_str()), &inVals[0]);
		this->muParserfXY->DefineVar(_T(varName2.c_str()), &inVals[1]);
		this->muParserfXY->SetExpr(fxyExpression.c_str());
		
		if (usediff) 
		{
			this->muParserdX->DefineVar(_T(varName1.c_str()), &inVals[0]);
			this->muParserdX->DefineVar(_T(varName2.c_str()), &inVals[1]);
			this->muParserdX->SetExpr(dxExpression.c_str());
			this->muParserdY->DefineVar(_T(varName1.c_str()), &inVals[0]);
			this->muParserdY->DefineVar(_T(varName2.c_str()), &inVals[1]);
			this->muParserdY->SetExpr(dyExpression.c_str());
		}
		
	}
	double RSGISFunctionMuParser2Var::calcFunction(double valueX, double valueY) throw(RSGISMathException)
	{
		double outVal = 0;
		
		try 
		{
			inVals[0] = valueX;
			inVals[1] = valueY;
			outVal = this->muParserfXY->Eval();
			
		}
		catch (mu::ParserError &e) // Catch muParser error
		{
			std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
			throw RSGISMathException(message);
		}
		
		return outVal;
	}
	double RSGISFunctionMuParser2Var::dX(double valueX, double valueY) throw(RSGISMathException)
	{
		double outVal = 0;
		if (this->usediff) 
		{
			try 
			{
				inVals[0] = valueX;
				inVals[1] = valueY;
				outVal = this->muParserdX->Eval();
				
			}
			catch (mu::ParserError &e) // Catch muParser error
			{
				std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
				throw RSGISMathException(message);
			}
		}
		else 
		{
			throw RSGISMathException("First derivative of function with respect to x not provided");
		}
		return outVal;
		
	}
	double RSGISFunctionMuParser2Var::dY(double valueX, double valueY) throw(RSGISMathException)
	{
		double outVal = 0;
		if (this->usediff) 
		{
			try 
			{
				inVals[0] = valueX;
				inVals[1] = valueY;
				outVal = this->muParserdY->Eval();
				
			}
			catch (mu::ParserError &e) // Catch muParser error
			{
				std::string message = std::string("ERROR: ") + std::string(e.GetMsg()) + std::string(":\t \'") + std::string(e.GetExpr()) + std::string("\'");
				throw RSGISMathException(message);
			}
		}
		else 
		{
			throw RSGISMathException("First derivative of function with respect to y not provided");
		}
		return outVal;
		
	}
	RSGISFunctionMuParser2Var::~RSGISFunctionMuParser2Var()
	{
		delete[] this->inVals;
		delete this->muParserfXY;
		delete this->muParserdX;
		delete this->muParserdY;
	}
	
}}

