 /*
 *  RSGISFunctions.h
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

#ifndef RSGISFunctions_H
#define RSGISFunctions_H

#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "muParser.h"
#include "math/RSGISMathFunction.h"
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

namespace rsgis{namespace math{
	
	enum functionType 
	{
		polynomial,
		polynomialGSL,
		polynomial2D,
		ln,
		ln2Var,
		ln2VarQuadratic,
		linXfLinY,
		muparser
	};
	
	class DllExport RSGISFunctionPolynomial : public RSGISMathFunction
	{
		/// Polynomial function
	public: 
		RSGISFunctionPolynomial(double *coefficients, int order);
		virtual double calcFunction(double value);
		virtual double dX(double value);
		virtual int numCoefficients() {return order;}
		virtual void updateCoefficents(double *newCoefficents){this->coefficients = newCoefficents;}
		virtual ~RSGISFunctionPolynomial();
	private:
		double *coefficients;
		int order;
	};
	class DllExport RSGISFunctionPolynomialGSL : public RSGISMathFunction
	{
		/// Polynomial function, with coefficients read as GSL vector
	public: 
		RSGISFunctionPolynomialGSL(gsl_vector *coefficients, int order);
		virtual double calcFunction(double value);
		virtual double dX(double value);
		virtual int numCoefficients() {return order;}
		virtual void updateCoefficents(double *newCoefficents);
		virtual ~RSGISFunctionPolynomialGSL();
	private:
		gsl_vector *coefficients;
		int order;
	};
	class DllExport RSGISFunctionLn : public RSGISMathFunction
	{
		/// a + b Ln(x)
	public: 
		RSGISFunctionLn(double coeffA, double coeffB);
		virtual double calcFunction(double value);
		virtual double dX(double value);
		virtual int numCoefficients() {return 2;}
		virtual void updateCoefficents(double *newCoefficents){this->coeffA = newCoefficents[0]; this->coeffB = newCoefficents[1];}
		virtual ~RSGISFunctionLn();
	private:
		double coeffA;
		double coeffB;
	};
	
	class DllExport RSGISFunction2VarLn : public RSGISMathTwoVariableFunction
 	{
		/// a + (b * y) + (c * y * ln(x))
	public: 
		RSGISFunction2VarLn(double coeffA, double coeffB, double coeffC);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY);
		virtual double dY(double valueX, double valueY);
		virtual int numCoefficients() {return 3;};
		virtual void updateCoefficents(double *newCoefficents){this->coeffA = newCoefficents[0]; this->coeffB = newCoefficents[1];this->coeffC = newCoefficents[2];};
		virtual ~RSGISFunction2VarLn();
	private:
		double coeffA;
		double coeffB;
		double coeffC;
	};
	
	class DllExport RSGISFunctionLinXfLinY : public RSGISMathTwoVariableFunction
 	{
		/// (b0 + b1 y) + (c0 + c1 y) * x
	public: 
		RSGISFunctionLinXfLinY(double coeffB0, double coeffB1, double coeffC0, double coeffC1);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY);
		virtual double dY(double valueX, double valueY);
		virtual int numCoefficients() {return 3;};
		virtual void updateCoefficents(double *newCoefficents){this->coeffB0 = newCoefficents[0]; this->coeffB1 = newCoefficents[1];this->coeffC0 = newCoefficents[2];this->coeffC1 = newCoefficents[3];};
		virtual ~RSGISFunctionLinXfLinY();
	private:
		double coeffB0;
		double coeffB1;
		double coeffC0;
		double coeffC1;

	};
	
	class DllExport RSGISFunction2VarLnQuadratic : public RSGISMathTwoVariableFunction
 	{
		/// a + b ln(x) + c y ln(x^2)  
	public: 
		RSGISFunction2VarLnQuadratic(double coeffA, double coeffB, double coeffC);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY);
		virtual double dY(double valueX, double valueY);
		virtual int numCoefficients() {return 3;};
		virtual void updateCoefficents(double *newCoefficents){this->coeffA = newCoefficents[0]; this->coeffB = newCoefficents[1];this->coeffC = newCoefficents[2];};
		virtual ~RSGISFunction2VarLnQuadratic();
	private:
		double coeffA;
		double coeffB;
		double coeffC;
	};
	
	class DllExport RSGISFunction2Var2DataLeastSquares : public RSGISMathTwoVariableFunction
 	{
		/// Least squares
	public: 
		RSGISFunction2Var2DataLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, double dataA, double dataB);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY){throw RSGISMathException("Not implemented");};
		virtual double dY(double valueX, double valueY){throw RSGISMathException("Not implemented");};
		virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunction2Var2DataLeastSquares();
	private:
		RSGISMathTwoVariableFunction *functionA;
		RSGISMathTwoVariableFunction *functionB;
		double dataA;
		double dataB;
	};
	
	class DllExport RSGISFunction2Var3DataLeastSquares : public RSGISMathTwoVariableFunction
 	{
		/// Least squares
	public: 
		RSGISFunction2Var3DataLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, RSGISMathTwoVariableFunction *functionC, double dataA, double dataB, double dataC);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY){throw RSGISMathException("Not implemented");};
		virtual double dY(double valueX, double valueY){throw RSGISMathException("Not implemented");};
		virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunction2Var3DataLeastSquares();
	private:
		RSGISMathTwoVariableFunction *functionA;
		RSGISMathTwoVariableFunction *functionB;
		RSGISMathTwoVariableFunction *functionC;
		double dataA;
		double dataB;
		double dataC;
	};
	
	class DllExport RSGISFunction2Var2DataPreconditionedLeastSquares : public RSGISMathTwoVariableFunction
 	{
		/// Least squares with preconditioning
		/** L(X) = 1/2 { || f(X) - d0 || ^2 + || X - Xap || ^2 } */
	public: 
		RSGISFunction2Var2DataPreconditionedLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, 
														 double dataA, double dataB, double apX1, double apX2, gsl_matrix *invCovMatrixP, gsl_matrix *invCovMatrixD);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY) {throw RSGISMathException("Not implemented");};
		virtual double dY(double valueX, double valueY) {throw RSGISMathException("Not implemented");};
	    virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunction2Var2DataPreconditionedLeastSquares();
	private:
		RSGISMathTwoVariableFunction *functionA;
		RSGISMathTwoVariableFunction *functionB;
		gsl_matrix *invCovMatrixP;
		gsl_matrix *invCovMatrixD;
		double dataA;
		double dataB;
		double apX1;
		double apX2;
		gsl_vector *deltaD;
		gsl_vector *deltaX;
		gsl_vector *tempA;
		bool useAP;
	};
	
	class DllExport RSGISFunction2Var3DataPreconditionedLeastSquares : public RSGISMathTwoVariableFunction
 	{
		/// Least squares with preconditioning
		/** L(X) = 1/2 { || f(X) - d0 || ^2 + || X - Xap || ^2 } */
	public: 
		RSGISFunction2Var3DataPreconditionedLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, RSGISMathTwoVariableFunction *functionC, 
														 double dataA, double dataB, double dataC, double apX1, double apX2, gsl_matrix *invCovMatrixP, gsl_matrix *invCovMatrixD);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY) {throw RSGISMathException("Not implemented");};
		virtual double dY(double valueX, double valueY) {throw RSGISMathException("Not implemented");};
	    virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunction2Var3DataPreconditionedLeastSquares();
	private:
		RSGISMathTwoVariableFunction *functionA;
		RSGISMathTwoVariableFunction *functionB;
		RSGISMathTwoVariableFunction *functionC;
		gsl_matrix *invCovMatrixP;
		gsl_matrix *invCovMatrixD;
		double dataA;
		double dataB;
		double dataC;
		double apX1;
		double apX2;
		gsl_vector *deltaD;
		gsl_vector *deltaX;
		gsl_vector *tempA;
		gsl_vector *tempB;
		bool useAP;
	};
	
	class DllExport RSGISFunction3Var3DataPreconditionedLeastSquares : public RSGISMathThreeVariableFunction
 	{
		/// Least squares with preconditioning
		/** L(X) = 1/2 { || f(X) - d0 || ^2 + || X - Xap || ^2 } */
	public: 
		RSGISFunction3Var3DataPreconditionedLeastSquares(RSGISMathThreeVariableFunction *functionA, RSGISMathThreeVariableFunction *functionB, RSGISMathThreeVariableFunction *functionC, 
														 double dataA, double dataB, double dataC, double apX1, double apX2, double apX3, gsl_matrix *invCovMatrixP, gsl_matrix *invCovMatrixD);
		virtual double calcFunction(double valueX, double valueY, double valueZ);
		virtual double dX(double valueX, double valueY, double valueZ) {throw RSGISMathException("Not implemented");};
		virtual double dY(double valueX, double valueY, double valueZ) {throw RSGISMathException("Not implemented");};
		virtual double dZ(double valueX, double valueY, double valueZ) {throw RSGISMathException("Not implemented");};
	    virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunction3Var3DataPreconditionedLeastSquares();
	private:
		RSGISMathThreeVariableFunction *functionA;
		RSGISMathThreeVariableFunction *functionB;
		RSGISMathThreeVariableFunction *functionC;
		gsl_matrix *invCovMatrixP;
		gsl_matrix *invCovMatrixD;
		double dataA;
		double dataB;
		double dataC;
		double apX1;
		double apX2;
		double apX3;
		gsl_vector *deltaD;
		gsl_vector *deltaX;
		gsl_vector *tempA;
		gsl_vector *tempB;
		bool useAP;
	};

	class DllExport RSGISFunction3Var4DataPreconditionedLeastSquares : public RSGISMathThreeVariableFunction
 	{
		/// Least squares with preconditioning
		/** L(X) = 1/2 { || f(X) - d0 || ^2 + || X - Xap || ^2 } */
	public: 
		RSGISFunction3Var4DataPreconditionedLeastSquares(RSGISMathThreeVariableFunction *functionA, RSGISMathThreeVariableFunction *functionB, RSGISMathThreeVariableFunction *functionC, RSGISMathThreeVariableFunction *functionD, 
														 double dataA, double dataB, double dataC, double dataD,double apX1, double apX2, double apX3, gsl_matrix *invCovMatrixP, gsl_matrix *invCovMatrixD);
		virtual double calcFunction(double valueX, double valueY, double valueZ);
		virtual double dX(double valueX, double valueY, double valueZ) {throw RSGISMathException("Not implemented");};
		virtual double dY(double valueX, double valueY, double valueZ) {throw RSGISMathException("Not implemented");};
		virtual double dZ(double valueX, double valueY, double valueZ) {throw RSGISMathException("Not implemented");};
	    virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunction3Var4DataPreconditionedLeastSquares();
	private:
		RSGISMathThreeVariableFunction *functionA;
		RSGISMathThreeVariableFunction *functionB;
		RSGISMathThreeVariableFunction *functionC;
		RSGISMathThreeVariableFunction *functionD;
		gsl_matrix *invCovMatrixP;
		gsl_matrix *invCovMatrixD;
		double dataA;
		double dataB;
		double dataC;
		double dataD;
		double apX1;
		double apX2;
		double apX3;
		gsl_vector *deltaD;
		gsl_vector *deltaX;
		gsl_vector *tempA;
		gsl_vector *tempB;
		bool useAP;
	};
	
	
	class DllExport RSGISFunctionEstimationLeastSquares : public RSGISMathTwoVariableFunction
 	{
		/// Least squares
	public: 
		RSGISFunctionEstimationLeastSquares(RSGISMathTwoVariableFunction *functionA, RSGISMathTwoVariableFunction *functionB, double dataA, double dataB);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY){throw RSGISMathException("Not implemented");}
		virtual double dY(double valueX, double valueY){throw RSGISMathException("Not implemented");}
		virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunctionEstimationLeastSquares();
	private:
		RSGISMathTwoVariableFunction *functionA;
		RSGISMathTwoVariableFunction *functionB;
		double dataA;
		double dataB;
	};
	
	class DllExport RSGISFunction2DPoly : public RSGISMathTwoVariableFunction
 	{
		/// Two dimensional polynomial function
		/** z = a0(y) + a1(y)b + a2(y)b^2
		 *   aN(y) = b0 + b1y + b2y^2
		 *   The function and differentials are calculated seperately.
		 */
		
	public: 
		RSGISFunction2DPoly(gsl_matrix *inCoefficients);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY);
		virtual double dY(double valueX, double valueY);
		virtual int numCoefficients() {return orderX * orderY;}
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunction2DPoly();
	private:
		gsl_matrix *coefficients;
		int orderX;
		int orderY;
	};
	
	class DllExport RSGISFunction3DPoly : public RSGISMathThreeVariableFunction
 	{
		/// Two dimensional polynomial function
		/** z = a0(y) + a1(y)b + a2(y)b^2
		 *   aN(y) = b0 + b1y + b2y^2
		 *   The function and differentials are calculated seperately.
		 */
		 // TODO: Tidy up differential functions to only calculate neccesary values (not all)
	public: 
		RSGISFunction3DPoly(gsl_matrix *inCoefficients, int orderX, int orderY, int orderZ);
		virtual double calcFunction(double valueX, double valueY, double valueZ);
		virtual double dX(double valueX, double valueY, double valueZ);
		virtual double dY(double valueX, double valueY, double valueZ);
		virtual double dZ(double valueX, double valueY, double valueZ);
		virtual int numCoefficients() {return orderX * orderY * orderZ;}
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunction3DPoly();
	private:
		gsl_matrix *coefficients;
		int orderX;
		int orderY;
		int orderZ;
	};
    
	class DllExport RSGISFunctionNDPoly : public RSGISMathNVariableFunction
 	{
		/// N dimensional polynomial function
		/** z = a0(y) + a1(y)b + a2(y)b^2
		 *   aN(y) = b0 + b1y + b2y^2
		 *   The function and differentials are calculated seperately.
		 */
	public: 
		RSGISFunctionNDPoly(gsl_matrix *inCoefficients, std::vector<int> *polyOrders);
		virtual double calcFunction(std::vector<double> *values);
		virtual int numCoefficients();
        virtual int numVariables();
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunctionNDPoly();
	private:
		gsl_matrix *coefficients;
		std::vector<int> *polyOrders;
        int numVar;
	};
    
	
	class DllExport RSGISFunctionRosenbrocksParabolicValley : public RSGISMathTwoVariableFunction
 	{
		/// Rosenbrock's Parabolic Valley (Rosenbrock, 1960).
	public: 
		RSGISFunctionRosenbrocksParabolicValley(){};
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY){throw RSGISMathException("Not implemented");}
		virtual double dY(double valueX, double valueY){throw RSGISMathException("Not implemented");}
		virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunctionRosenbrocksParabolicValley(){};
	};
	
	class DllExport RSGISFunctionReturnZero : public RSGISMathFunction
	{
		/// Returns zero
	public: 
		RSGISFunctionReturnZero(){};
		virtual double calcFunction(double value){return 0;};
		virtual double dX(double value){throw RSGISMathException("Not implemented");};
		virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunctionReturnZero(){};
	private:
	};
	
	class DllExport RSGISFunctionMuParser : public RSGISMathFunction
	{
		/// Uses muParser to define function and optionally first derivative
		/** The function is passes in as an mu parser string.
		  * A number of functions are supported - see http://muparser.sourceforge.net/ for more details.
		  * The default variable name is 'x' unless specified using 'varName'
		  * The derivative of the function must be defined if it is too be used.
		  * If no derivative is supplied then an exception will be thrown if 'dX' is called'
		  */
	public: 
		RSGISFunctionMuParser(std::string fxExpression, std::string varName = "x", std::string dxExpression = "", bool usedX = false);
		virtual double calcFunction(double value);
		virtual double dX(double value);
		virtual int numCoefficients() {throw RSGISMathException("Not known for muparser expression");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunctionMuParser();
	private:
        mu::Parser *muParserfX; // muParser to hold function
        mu::Parser *muParserdX; // muParser to hold first derivative
        mu::value_type *inVals;
		bool usedX; // Check if using derivative information
	};
	
	class DllExport RSGISFunctionMuParser2Var : public RSGISMathTwoVariableFunction
	{
		/// Uses muParser to define function and optionally first derivative
		/** The function is passes in as an mu parser string.
		 * A number of functions are supported - see http://muparser.sourceforge.net/ for more details.
		 * The default variable names are 'x' and 'y' unless specified using 'varName1' and 'varName2'
		 * The derivative of the function must be defined if it is too be used.
		 * If no derivative is supplied then an exception will be thrown if 'dX' is called'
		 */
	public: 
		RSGISFunctionMuParser2Var(std::string fxyExpression, std::string varName1 = "x", std::string varName2 = "y", std::string dxExpression = "", std::string dyExpression = "", bool usediff = false);
		virtual double calcFunction(double valueX, double valueY);
		virtual double dX(double valueX, double valueY);
		virtual double dY(double valueX, double valueY);
		virtual int numCoefficients() {throw RSGISMathException("Not known for muparser expression");};
		virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		virtual ~RSGISFunctionMuParser2Var();
	private:
        mu::Parser *muParserfXY; // muParser to hold function
        mu::Parser *muParserdX; // muParser to hold first derivative with respect to x
        mu::Parser *muParserdY; // muParser to hold first derivative with respect to y
        mu::value_type *inVals;
		bool usediff; // Check if using derivative information
	};
	
}}

#endif
