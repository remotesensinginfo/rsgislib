/*
 *  RSGISCalcImageFilters.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/05/2008.
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

#ifndef RSGISCalcImageFilters_H
#define RSGISCalcImageFilters_H

#include <math.h>
#include <iostream>
#include "filtering/RSGISImageFilterException.h"
#include "filtering/RSGISCalcFilter.h"

using namespace std;

namespace rsgis{namespace filter{
	
	class RSGISCalcLapacianFilter : public RSGISCalcFilter
		{
		public: 
			RSGISCalcLapacianFilter(float stddev);
			virtual float calcFilterValue(int x, int y) throw(RSGISImageFilterException);
			virtual ~RSGISCalcLapacianFilter(){};
		protected:
			float stddev;
		};
	
	class RSGISCalcGaussianSmoothFilter : public RSGISCalcFilter
		{
		public: 
			RSGISCalcGaussianSmoothFilter(float stddevX, float stddevY, float angle);
			virtual float calcFilterValue(int x, int y) throw(RSGISImageFilterException);
			virtual ~RSGISCalcGaussianSmoothFilter(){};
		protected:
			float stddevX;
			float stddevY;
			float angle;
		};
	
	class RSGISCalcGaussianFirstDerivativeFilter : public RSGISCalcFilter
		{
		public: 
			RSGISCalcGaussianFirstDerivativeFilter(float stddevX, float stddevY, float angle);
			virtual float calcFilterValue(int x, int y) throw(RSGISImageFilterException);
			virtual ~RSGISCalcGaussianFirstDerivativeFilter(){};
		protected:
			float stddevX;
			float stddevY;
			float angle;
		};
	
	class RSGISCalcGaussianSecondDerivativeFilter : public RSGISCalcFilter
		{
		public: 
			RSGISCalcGaussianSecondDerivativeFilter(float stddevX, float stddevY, float angle);
			virtual float calcFilterValue(int x, int y) throw(RSGISImageFilterException);
			virtual ~RSGISCalcGaussianSecondDerivativeFilter(){};
		protected:
			float stddevX;
			float stddevY;
			float angle;
		};	
	
	
}}
#endif

