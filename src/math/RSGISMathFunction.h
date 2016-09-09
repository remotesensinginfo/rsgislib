/*
 *  RSGISMathFunction.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/05/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISMathFunction_H
#define RSGISMathFunction_H

#include <vector>
#include "math/RSGISMathException.h"

namespace rsgis{namespace math{
        
	class DllExport RSGISMathFunction
	{
		/** RSGISMathFunction
		 * Abstract class for single variable function
		 */
	public: 
		RSGISMathFunction(){};
		virtual double calcFunction(double value) throw(RSGISMathException) = 0;
		virtual double dX(double value) throw(RSGISMathException) = 0;
		virtual int numCoefficients() throw(RSGISMathException) = 0;
		virtual void updateCoefficents(double *coefficients) = 0;
		virtual ~RSGISMathFunction(){};
	};
	
	class DllExport RSGISMathTwoVariableFunction
	{
		/** RSGISMathTwoVariableFunction
		 *  Abstract class for two variable function
		 */
	public: 
		RSGISMathTwoVariableFunction(){};
		virtual double calcFunction(double valueX, double valueY) throw(RSGISMathException) = 0;
		virtual double dX(double valueX, double valueY) throw(RSGISMathException) = 0;
		virtual double dY(double valueX, double valueY) throw(RSGISMathException) = 0;
		virtual int numCoefficients() throw(RSGISMathException) = 0;
		virtual void updateCoefficents(double *coefficients) = 0;
		virtual ~RSGISMathTwoVariableFunction(){};
	};
	
	class DllExport RSGISMathThreeVariableFunction
	{
		/** RSGISMathThreeVariableFunction
		 *  Abstract class two variable function
		 */
	public: 
		RSGISMathThreeVariableFunction(){};
		virtual double calcFunction(double valueX, double valueY, double valueZ) throw(RSGISMathException) = 0;
		virtual double dX(double valueX, double valueY, double valueZ) throw(RSGISMathException) = 0;
		virtual double dY(double valueX, double valueY, double valueZ) throw(RSGISMathException) = 0;
		virtual double dZ(double valueX, double valueY, double valueZ) throw(RSGISMathException) = 0;
		virtual int numCoefficients() throw(RSGISMathException) = 0;
		virtual void updateCoefficents(double *coefficients) = 0;
		virtual ~RSGISMathThreeVariableFunction(){};
	};
    
    class DllExport RSGISMathNVariableFunction
	{
		/** RSGISMathThreeVariableFunction
		 *  Abstract class two variable function
		 */
	public: 
		RSGISMathNVariableFunction(){};
		virtual double calcFunction(std::vector <double> *values) throw(RSGISMathException) = 0;
		virtual int numCoefficients() throw(RSGISMathException) = 0;
        virtual int numVariables() throw(RSGISMathException) = 0;
		virtual void updateCoefficents(double *coefficients) = 0;
		virtual ~RSGISMathNVariableFunction(){};
	};
	
}}

#endif


