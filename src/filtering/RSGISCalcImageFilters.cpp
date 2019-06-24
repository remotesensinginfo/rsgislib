/*
 *  RSGISCalcImageFilters.cpp
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

#include "RSGISCalcImageFilters.h"


namespace rsgis{namespace filter{
	
	RSGISCalcLapacianFilter::RSGISCalcLapacianFilter(float stddev) : RSGISCalcFilter()
	{
		this->stddev = stddev;
	}
	
	float RSGISCalcLapacianFilter::calcFilterValue(int x, int y)
	{
		float xSq = x * x;
		float ySq = y * y;
		float var = stddev * stddev;
		
		float part1 = xSq + ySq - (2 * var);
		float part2 = ((xSq + ySq)*(-1))/(2 * var);
		float part3 = pow(stddev,4);
		
		float lap = part3 * exp(part2) * part1;
		return lap;
	}
	
	RSGISCalcGaussianSmoothFilter::RSGISCalcGaussianSmoothFilter(float stddevX, float stddevY, float angle) : RSGISCalcFilter()
	{
		this->stddevX = stddevX;
		this->stddevY = stddevY;
		this->angle = angle;
	}
	
	float RSGISCalcGaussianSmoothFilter::calcFilterValue(int x, int y)
	{
		float xVar = stddevX * stddevX;
		float yVar = stddevY * stddevY;
		float a1 = (cos(angle)*cos(angle))/xVar;
		float a2 = (sin(angle)*sin(angle))/yVar;
		float a = a1 + a2;
		
		float b1 = (sin(2*angle)*(-1))/xVar;
		float b2 = sin(2*angle)/yVar;
		float b = b1 + b2;
		
		float c1 = (sin(angle)*sin(angle))/xVar;
		float c2 = (cos(angle)*cos(angle))/yVar;
		float c = c1 + c2;
		
		float constNorm = 2 * M_PI * stddevX * stddevY;
		
		float partA = (a*(-1)) * (x*x);
		float partB = b * x * y;
		float partC = c * (y*y);
		
		float g = exp(partA - partB - partC)/constNorm;
		
		return g;
	}
	
	
	RSGISCalcGaussianFirstDerivativeFilter::RSGISCalcGaussianFirstDerivativeFilter(float stddevX, float stddevY, float angle) : RSGISCalcFilter()
	{
		this->stddevX = stddevX;
		this->stddevY = stddevY;
		this->angle = angle;
	}
	
	float RSGISCalcGaussianFirstDerivativeFilter::calcFilterValue(int x, int y)
	{
		float xVar = stddevX * stddevX;
		float yVar = stddevY * stddevY;
		float a1 = (cos(angle)*cos(angle))/xVar;
		float a2 = (sin(angle)*sin(angle))/yVar;
		float a = a1 + a2;
		
		float b1 = (sin(2*angle)*(-1))/xVar;
		float b2 = sin(2*angle)/yVar;
		float b = b1 + b2;
		
		float c1 = (sin(angle)*sin(angle))/xVar;
		float c2 = (cos(angle)*cos(angle))/yVar;
		float c = c1 + c2;
		
		float constNorm = 2 * M_PI * stddevX * stddevY;
		
		float partA = (a*(-1)) * (x*x);
		float partB = b * x * y;
		float partC = c * (y*y);
		
		float g = exp(partA - partB - partC)/constNorm;
		
		float gx = g * (((-2)*a*x) - (b*y));
		float gy = g * (((b*(-1))*x) - (2*c*y));
		
		float f = (gx * sin(angle)) + (gy * cos(angle));
		
		return f;
	}
	
	
	RSGISCalcGaussianSecondDerivativeFilter::RSGISCalcGaussianSecondDerivativeFilter(float stddevX, float stddevY, float angle) : RSGISCalcFilter()
	{
		this->stddevX = stddevX;
		this->stddevY = stddevY;
		this->angle = angle;
	}
	
	float RSGISCalcGaussianSecondDerivativeFilter::calcFilterValue(int x, int y)
	{
		float xVar = stddevX * stddevX;
		float yVar = stddevY * stddevY;
		float a1 = (cos(angle)*cos(angle))/xVar;
		float a2 = (sin(angle)*sin(angle))/yVar;
		float a = a1 + a2;
		
		float b1 = (sin(2*angle)*(-1))/xVar;
		float b2 = sin(2*angle)/yVar;
		float b = b1 + b2;
		
		float c1 = (sin(angle)*sin(angle))/xVar;
		float c2 = (cos(angle)*cos(angle))/yVar;
		float c = c1 + c2;
		
		float constNorm = 2 * M_PI * stddevX * stddevY;
		
		float partA = (a*(-1)) * (x*x);
		float partB = b * x * y;
		float partC = c * (y*y);
		
		float g = exp(partA - partB - partC)/constNorm;
		
		float gxxP1 = (-2)*a;
		float gxxP2 = ((-2)*a*x) - (b*y);
		float gxx = g * (gxxP1 + (gxxP2 * gxxP2));
		
		float gyyP1 = (-2)*c;
		float gyyP2 = ((b*(-1))*x) - (2*c*y);
		float gyy = g * (gyyP1 + (gyyP2 * gyyP2));
		
		float gxyP1 = (-2)*b;
		float gxyP2 = ((-2)*a*x) - (b*y);
		float gxyP3 = ((b*(-1))*x) - (2*c*y);
		float gxy =  g * (gxyP1 + gxyP2) * gxyP3;
		
		float fP1 = gxx * (sin(angle)*sin(angle));
		float fP2 = 2*gxy*sin(angle)*cos(angle);
		float fP3 = gyy * (cos(angle)*cos(angle));
		float f = fP1 + fP2 + fP3;
		return f;
	}	

}}
