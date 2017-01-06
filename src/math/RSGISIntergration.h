/*
 *  RSGISIntergration.h
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

#ifndef RSGISIntergration_H
#define RSGISIntergration_H

#include <iostream>
#include <string>
#include <math.h>
#include "math/RSGISMathFunction.h"
#include "math/RSGISMathException.h"

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
	    
	struct DllExport point2D
	{
		double x;
		double y;
	};
	
	class DllExport RSGISIntergration
		{
		public:
			RSGISIntergration(RSGISMathFunction *function);
			virtual double calcArea(double min, double max, bool total) throw(RSGISMathException)=0;
			virtual double calcValue4Area(double area) throw(RSGISMathException)=0;
			virtual double calcValue4ProportionArea(double propArea) throw(RSGISMathException)=0;
			virtual ~RSGISIntergration(){};
		protected:
			RSGISMathFunction *function;
			bool totalCalulated;
			double totalArea;
			double areaMin;
			double areaMax;
		};
	
	class TrapeziumIntegration : public RSGISIntergration
		{
		public: 
			TrapeziumIntegration(RSGISMathFunction *function, double division);
			virtual double calcArea(double min, double max, bool total) throw(RSGISMathException);
			virtual double calcValue4Area(double area) throw(RSGISMathException);
			virtual double calcValue4ProportionArea(double propArea) throw(RSGISMathException);
			double calcMaxValue()throw(RSGISMathException);
			void getUpperLowerValues(double lower, double upper, double prob)throw(RSGISMathException);
			virtual ~TrapeziumIntegration();
		protected:
			double calcTrapziumArea(point2D bl, point2D tl, point2D tr, point2D br) throw(RSGISMathException);
			double division;
			double *segmentAreas;
			double *segmentY;
			int numSegments;
		};
}}

#endif

