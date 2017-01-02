/*
 *  RSGISIntergration.cpp
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

#include "RSGISIntergration.h"

namespace rsgis{namespace math{

	RSGISIntergration::RSGISIntergration(RSGISMathFunction *function)
	{
		this->function = function;
		totalCalulated = false;
		totalArea = 0;
	}
	
	
	TrapeziumIntegration::TrapeziumIntegration(RSGISMathFunction *function, double division) : RSGISIntergration(function)
	{
		this->division = division;
		segmentAreas = NULL;
		segmentY = NULL;
		numSegments = 0;
	}
	
	/***
	 *
	 * Area calculating only works with positive y values!
	 *
	 */
	double TrapeziumIntegration::calcArea(double min, double max, bool total) throw(RSGISMathException)
	{
		if(segmentAreas != NULL)
		{
			delete[] segmentAreas;
		}
		
		if(segmentY != NULL)
		{
			delete[] segmentY;
		}
		
		numSegments = ceil((max-min)/division);
		if(numSegments <= 0)
		{
			throw RSGISMathException("Incorrect min, max or division values provided");
		}
		segmentAreas = new double[numSegments];
		segmentY = new double[numSegments+1];
		
		double lowBoundX = min;
		double upperBoundX = min + division;
		double lowBoundY = 0;
		double upperBoundY = 0;
		double areaSum = 0;
		point2D bl;
		point2D tl;
		point2D tr;
		point2D br;
		bool first = true;
		
		for(int i = 0; i < numSegments; i++)
		{
			lowBoundY = function->calcFunction(lowBoundX);
			upperBoundY = function->calcFunction(upperBoundX);
						
			if(first)
			{
				segmentY[i] = lowBoundY;
				segmentY[i+1] = upperBoundY;
				first = false;
			}
			else
			{
				segmentY[i+1] = upperBoundY;
			}
			
			bl.x = lowBoundX;
			bl.y = 0;
			tl.x = lowBoundX;
			tl.y = lowBoundY;
			tr.x = upperBoundX;
			tr.y = upperBoundY;
			br.x = upperBoundX;
			br.y = 0;
			
			segmentAreas[i] = this->calcTrapziumArea(bl, tl, tr, br);
			areaSum += segmentAreas[i];
			lowBoundX += division;
			upperBoundX += division;
		}
		
		if(total)
		{
			totalCalulated = true;
			totalArea = areaSum;
			areaMin = min;
			areaMax = max;
		}
		
		return areaSum;
	}
	
	/***
	 *
	 * Area calculating only works with positive y values!
	 *
	 */
	double TrapeziumIntegration::calcTrapziumArea(point2D bl, point2D tl, point2D tr, point2D br) throw(RSGISMathException)
	{
		if(bl.y < 0 | tl.y < 0 | br.y < 0 | tr.y < 0)
		{
			throw RSGISMathException("TrapeziumIntegration::calcTrapziumArea can only calculate areas for positive values of y.");
		}
		
		double width = br.x - bl.x;
		double minHeight = 0;
		double maxHeight = 0;
		double diffHeight = 0;
		bool equalHeight = false;
		double area = 0;
		
		if(tl.y > tr.y)
		{
			minHeight = tr.y;
			maxHeight = tl.y;
		}
		else if(tr.y > tl.y)
		{
			minHeight = tl.y;
			maxHeight = tr.y;
		}
		else 
		{
			equalHeight = true;
		}
		
		area = minHeight * width;
		
		if(!equalHeight)
		{
			diffHeight = maxHeight - minHeight;
			area += ((diffHeight * width)/2);
		}
		return area;
	}
	
	double TrapeziumIntegration::calcValue4Area(double area) throw(RSGISMathException)
	{
		if(!totalCalulated)
		{
			throw RSGISMathException("The total area under the curve first needs to be calculated.");
		}
		double sum = 0;
		double diffPrev = 10000000000.0;
		double diffCurrent = 10000000000.0;
		int index = 0;
		
		for(int i = 0; i < numSegments; i++)
		{
			sum += segmentAreas[i];
			diffPrev = diffCurrent;
			diffCurrent = area - sum;
						
			if(diffCurrent < 0)
			{
				index = i-1;
				break;
			}
		}
		
		return (areaMin + (index * division));
	}
	
	double TrapeziumIntegration::calcValue4ProportionArea(double propArea) throw(RSGISMathException)
	{
		if(!totalCalulated)
		{
			throw RSGISMathException("The total area under the curve first needs to be calculated.");
		}
		
		double propOfTotalArea = totalArea * propArea;
				
		return this->calcValue4Area(propOfTotalArea);
		
	}
	
	double TrapeziumIntegration::calcMaxValue()throw(RSGISMathException)
	{
		if(!totalCalulated)
		{
			throw RSGISMathException("The total area under the curve first needs to be calculated.");
		}
		
		bool first = true;
		int maxSegment = 0;
		for(int i = 0; i < numSegments; i++)
		{
			if(first)
			{
				maxSegment = i;
				first = false;
			}
			else
			{
				if(segmentAreas[i] > segmentAreas[maxSegment])
				{
					maxSegment = i;
				}
			}
		}
		return (areaMin + (maxSegment * division));
	}
	
	void TrapeziumIntegration::getUpperLowerValues(double lower, double upper, double prob)throw(RSGISMathException)
	{
		if(!totalCalulated)
		{
			throw RSGISMathException("The total area under the curve first needs to be calculated.");
		}
		
		bool first = true;
		int maxIndex = 0;
		for(int i = 0; i < (numSegments+1); i++)
		{
			if(first)
			{
				maxIndex = i;
				first = false;
			}
			else
			{
				if(segmentY[i] > segmentY[maxIndex])
				{
					maxIndex = i;
				}
			}
		}
				
		double threshold = segmentY[maxIndex]*prob;
		
		double diffLower = 0;
		double diffSquLower = 0;
		double diffUpper = 0;
		double diffSquUpper = 0;
		double minLowerDiff = 0;
		int lowerIndex = 0;
		double minUpperDiff = 0;
		int upperIndex = 0;
		
		bool firstLower = true;
		bool firstUpper = true;
		
		for(int i = 0; i < (numSegments+1); i++)
		{
			if(i < maxIndex)
			{
				if(firstLower)
				{
					lowerIndex = i;
					diffLower = threshold - segmentY[i];
					minLowerDiff = diffLower * diffLower;
					firstLower = false;
				}
				else
				{
					diffLower = threshold - segmentY[i];
					diffSquLower = diffLower * diffLower;
					if(diffSquLower < minLowerDiff)
					{
						lowerIndex = i;
						minLowerDiff = diffSquLower;
					}
				}
			}
			else
			{
				if(firstUpper)
				{
					upperIndex = i;
					diffUpper = threshold - segmentY[i];
					minUpperDiff = diffUpper * diffUpper;
					firstUpper = false;
				}
				else
				{
					diffUpper = threshold - segmentY[i];
					diffSquUpper = diffUpper * diffUpper;
 					if(diffSquUpper < minUpperDiff)
					{
						upperIndex = i;
						minUpperDiff = diffSquUpper;
					}
				}
			}
		}
		
		if(lowerIndex == 0)
		{
			lower = 0;
		}
		else
		{
			lower = (areaMin + ((lowerIndex-1) * division));
		}
		
		if(upperIndex == 0)
		{
			upper = 0;
		}
		else
		{
			upper = (areaMin + ((upperIndex-1) * division));
		}
		
	}
	
	TrapeziumIntegration::~TrapeziumIntegration()
	{
		if(segmentAreas != NULL)
		{
			delete[] segmentAreas;
		}
		if(segmentY != NULL)
		{
			delete[] segmentY;
		}
	}
	
	
}}

