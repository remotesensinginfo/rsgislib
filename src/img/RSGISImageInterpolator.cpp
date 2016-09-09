/*
 *  RSGISImageInterpolator.cpp
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

#include "RSGISImageInterpolator.h"

namespace rsgis{namespace img{
	
	
	RSGISInterpolator::RSGISInterpolator()
	{
		
	}
	
	RSGISInterpolator::~RSGISInterpolator()
	{
		
	}
	
	
	RSGISCubicInterpolator::RSGISCubicInterpolator() : RSGISInterpolator()
	{
		
	}
	
	double RSGISCubicInterpolator::interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException)
	{
		double pixelValue = 0;
		
		double *newPixels = new double[3];
		double *tmpPixels = new double[3];
		tmpPixels[0] = pixels[0];
		tmpPixels[1] = pixels[3];
		tmpPixels[2] = pixels[6];
		newPixels[0] = this->estimateNewValueFromCurve(tmpPixels, yShift);
		
		tmpPixels[0] = pixels[1];
		tmpPixels[1] = pixels[4];
		tmpPixels[2] = pixels[7];
		newPixels[1] = this->estimateNewValueFromCurve(tmpPixels, yShift);
		
		tmpPixels[0] = pixels[2];
		tmpPixels[1] = pixels[5];
		tmpPixels[2] = pixels[8];
		newPixels[2] = this->estimateNewValueFromCurve(tmpPixels, yShift);
		
		pixelValue = this->estimateNewValueFromCurve(newPixels, xShift);	
		
		if(newPixels != NULL)
		{
			delete[] newPixels;
		}
		if(tmpPixels != NULL)
		{
			delete[] tmpPixels;
		}
		return pixelValue;
	}
	
	double RSGISCubicInterpolator::estimateNewValueFromCurve(double *pixels, double shift)
	{
		double newValue = 0;
		////////////// Fit line /////////////////////
		double ax = 0;
		double bx = 0;
		double cx = 0;
		
		cx = pixels[1];
		bx = ((pixels[2]-pixels[1])-(pixels[0]-pixels[1]))/2;
		ax = pixels[2] - (cx + bx);
		///////////////////////////////////////////////
		
		/////////////// Find new value /////////////
		newValue = (shift*shift)*ax + shift*bx + cx;
		///////////////////////////////////////////
		
		return newValue;
	}
	
	RSGISBilinearAreaInterpolator::RSGISBilinearAreaInterpolator()
	{
		
	}
	
	double RSGISBilinearAreaInterpolator::interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException)
	{
		/************ Calculate New Pixel Value ************/
		double pixelValue = 0;
		
		pixelValue = ((1-xShift) * (1-yShift) * pixels[0]) + 
		(xShift * (1-yShift) * pixels[1]) +
		((1-xShift) * yShift * pixels[2]) +
		(xShift * yShift * pixels[3]);
		/*****************************************************/
		
		return pixelValue;
	}
	
	RSGISBilinearPointInterpolator::RSGISBilinearPointInterpolator()
	{
		
	}
	
	double RSGISBilinearPointInterpolator::interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException)
	{
		double pixelValue = 0;
		double x1Linear = 0;
		double x2Linear = 0;
		
		x1Linear = ((1-xShift)*pixels[0]) + (xShift*pixels[1]);
		x2Linear = ((1-xShift)*pixels[2]) + (xShift*pixels[3]);
		
		pixelValue = ((1-yShift)*x1Linear)+(yShift * x2Linear);
		/*****************************************************/
		
		return pixelValue;
	}
	
	RSGISNearestNeighbourInterpolator::RSGISNearestNeighbourInterpolator()
	{
		
	}
	
	double RSGISNearestNeighbourInterpolator::interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException)
	{		
		/************ Calculate New Pixel Value ************/
		double pixelValue = 0;
		int pixelIndex = 0;
		
		double pixelOverlaps [4];
		
		pixelOverlaps[0] = (1-xShift) * (1-yShift);
		pixelOverlaps[1] = xShift * (1-yShift);
		pixelOverlaps[2] = (1-xShift) * yShift;
		pixelOverlaps[3] = xShift * yShift;
		
		pixelIndex = this->findIndexOfMax(pixelOverlaps, 4);
		pixelValue = pixels[pixelIndex];
		/*****************************************************/
		return pixelValue;
	}
	
	int RSGISNearestNeighbourInterpolator::findIndexOfMax(double *arr, int size)
	{
		double maxValue = arr[0];
		int maxIndex = 0;
		
		for(int i = 0; i < size; i++)
		{
			if(maxValue < arr[i])
			{
				maxIndex = i;
				maxValue = arr[i];
			}
		}
		return maxIndex;
	}
	
	RSGISTriangulationInterpolator::RSGISTriangulationInterpolator()
	{
		
	}
	
	double RSGISTriangulationInterpolator::interpolate(double xShift, double yShift, double *pixels) throw(rsgis::RSGISImageException)
	{
		/********** Calc pixels which create triangle ************/
		double newPixelValue1 = this->triangle(xShift, yShift, pixels, true);
		double newPixelValue2 = this->triangle(xShift, yShift, pixels, false);
		/**********************************************************/
		
		/*********** Calculate final Pixel Value ******************/
		double pixelValue = 0;
		pixelValue = (newPixelValue1 + newPixelValue2) /2;
		/***********************************************************/
		
		return pixelValue;
	}
	
	/***
	 * triangulation: TRUE = ABC and FALSE = BDC
	 */
	double RSGISTriangulationInterpolator::triangle(double xShift, double yShift, double *pixels, bool triangulation)
	{
		/********** Calc pixels which create triangle ************/
		
		double a = 0;
		double b = 0;
		double c = 0;
		
		////////////////////// Triangulation 1 //////////////////////
		double newPixelValue1 = 0;
		
		if( xShift + yShift <= 1)
		{
			c = pixels[0];
			a = pixels[1] - c;
			b = pixels[2] - c;
			
			newPixelValue1 = (a*xShift) + (b*yShift) + c;
		}
		else if( xShift + yShift > 1 )
		{
			a = pixels[3] - pixels[2];
			b = pixels[3] - pixels[1];
			c = pixels[1] - a;
			
			newPixelValue1 = (a*xShift) + (b*yShift) + c;
		}
		//////////////////////////////////////////////////////////////
		
		////////////////////// Triangulation 2 //////////////////////
		double newPixelValue2 = 0;
		a = 0;
		b = 0;
		c = 0;
		
		if( xShift - yShift >= 0 )
		{
			c = pixels[0];
			a = pixels[1] - c;
			b = pixels[3] - (a+c);
			
			newPixelValue2 = (a*xShift) + (b*yShift) + c;
		}
		else if( xShift - yShift < 0 )
		{
			c = pixels[0];
			b = pixels[2] - c;
			a = pixels[3] - (b+c);
			
			newPixelValue2 = (a*xShift) + (b*yShift) + c;
		}
		//////////////////////////////////////////////////////////////
		
		/**********************************************************/
		
		/*********** Calculate final Pixel Value ******************/
		double pixelValue = 0;
		if(triangulation)
		{
			pixelValue = newPixelValue1;
		}
		else
		{
			pixelValue = newPixelValue2;
		}
		/***********************************************************/
		return pixelValue;
	}
	
}}
