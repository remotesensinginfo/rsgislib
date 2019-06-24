/*
 *  RSGISBaysianIntergrateFunctionNoPrior.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 12/12/2008.
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


#ifndef RSGISBaysianIntergrateFunctionPrior_H
#define RSGISBaysianIntergrateFunctionPrior_H

#include "math/RSGISMathFunction.h"
#include "math/RSGISProbDistro.h"

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
	    
	class DllExport RSGISBaysianIntergrateFunctionPrior : public RSGISMathFunction
		{
		public:
			RSGISBaysianIntergrateFunctionPrior(RSGISMathFunction *function, RSGISProbDistro *probDistro, double variance);
			void updateValue(float value);
			virtual double dX(double value){throw RSGISMathException("Not implemented");};
			virtual double calcFunction(double predictVal);
			virtual int numCoefficients(){throw RSGISMathException("Not implemented");};
			virtual void updateCoefficents(double *newCoefficents){throw RSGISMathException("Not implemented");};
		protected:
			RSGISMathFunction *function;
			RSGISProbDistro *probDistro;
			double variance; 
			float value;
		};
	
}}


#endif
