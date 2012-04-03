/*
 *  RSGISFFTProcessing.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 28/11/2008.
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

#include "RSGISFFTProcessing.h"


namespace rsgis{namespace img{


	RSGISFFTProcessing::RSGISFFTProcessing()
	{
		
	}
	
	Polygon** RSGISFFTProcessing::findDominateFreq(Matrix *magnitude, int startCircle, int endCircle, int *numPolys) throw(RSGISFFTException)
	{
		RSGISMatrices matrixUtils;
		RSGISGeometry geomUtils;
		int length = magnitude->m * magnitude->n;
		int halfWidth = (magnitude->m/2);
		int halfHeight = (magnitude->n/2);
		int maxCircleRadius = 0;
		int numCircles = 0;
		int circleCounter = 0;
		int row = 0;
		int column = 0;
		float step = 0;
		float startX = 0;
		float leftX = 0;
		float rightX = 0;
		float topY = 0;
		float bottomY = 0;
		int distFromCentre = 0;
		float distPixels = 0;
		
		Polygon **circles = NULL;
		Polygon **bboxes = NULL;
		
		try
		{
			if(halfWidth < halfHeight)
			{
				maxCircleRadius = halfWidth;
			}
			else
			{
				maxCircleRadius = halfHeight;
			}
			
			//matrixUtils.printMatrix(magnitude);
			//cout << "DC = " << dc << " value = " << magnitude->matrix[dc] << endl;
			cout << "Centre Point [" << halfWidth << "," << halfHeight << "](" << ((halfHeight) * magnitude->m)+halfWidth << ") = " << magnitude->matrix[((halfHeight) * magnitude->m)+halfWidth] << endl;
			
			if(startCircle > endCircle)
			{
				throw RSGISFFTException("StartCircle is greater than endCircle.");
			}
			if(endCircle > maxCircleRadius)
			{
				cout << "endCircle was larger than the image. Defaulting to image size " << maxCircleRadius << endl;
				endCircle = maxCircleRadius;
			}
			
			numCircles = endCircle - startCircle;
			circles = new Polygon*[numCircles];
			circleCounter = 0;
			
			for(int i = startCircle; i < endCircle; i++)
			{
				circles[circleCounter++] = geomUtils.createCircle(0, 0, i, 0.5);
			}
			
			cout << "Created " << circleCounter << " circles\n";
			
			bboxes = new Polygon*[length];
			
			cout << "Matrix size = [" << magnitude->m << "," << magnitude->n << "] centre = ["  << halfWidth << "," << halfHeight << "]\n";
			
			row = 0;
			column = 0;
			step = 1.0;
			startX = (halfWidth + 0.5) * (-1);
			leftX = startX;
			rightX = startX + step;
			topY = halfHeight + 0.5;
			bottomY = topY - step;
			
			cout << "TL [" << leftX << "," << topY << "]\n";
			
			for(int i = 0; i < length; i++)
			{
				//cout << "[column,row]: [" << column << "," << row << "]" << endl;
				bboxes[i] = geomUtils.createPolygon(leftX, topY, rightX, bottomY);
				
				leftX = leftX + step;
				rightX = rightX + step;
				
				column++;
				if(column == magnitude->m)
				{
					leftX = startX;
					rightX = startX + step;
					topY = topY - step;
					bottomY = bottomY - step;
					row++;
					column = 0;
				}
			}
			*numPolys = length;
			
			//cout << "DC Polygon = " << bboxes[dc]->toString() << endl;
			
			
			
			int maxCircle = 0;
			double maxValue = 0;
			//bool first = true;
			//SortedGenericList<double> *values = NULL;
			for(int i = 0; i < numCircles; i++)
			{
				cout << "Circle " << i << ":" << endl;
				/*
				values = geomUtils.findValuesOnEdge(magnitude, bboxes, circles[i]);
				cout << "max: " << *values->peekTop() << endl;
				cout << "min: " << *values->peekBottom() << endl;
				
				if(first)
				{
					maxCircle = i;
					maxValue = *values->peekTop();
					first = false;
				}
				else if(*values->peekTop() > maxValue)
				{
					maxCircle = i;
					maxValue = *values->peekTop();
				}
				
				delete values;
				*/
			}
			
			distFromCentre = startCircle + maxCircle;
			distPixels = ((float)distFromCentre)/2;
			
			cout << "\nCircle " << maxCircle << " is the circle with the distance on it\n";
			cout << "Distance value = " << maxValue << endl;
			cout << "Distance from centre = " << distFromCentre << " therefore width is " << distPixels << " pixels." << endl;
			
			int numPts = 0;
			Polygon **ptPolys = geomUtils.findMaxPointsOnPolyEdge(magnitude, bboxes, circles[maxCircle], &numPts);
						
			Coordinate centre;
			cout << "number of points = " << numPts << endl;
			float angle = 0;
			if(numPts == 2)
			{
				Coordinate left;
				Coordinate right;
				ptPolys[0]->getCentroid(centre);
				if(centre.x < 0)
				{
					ptPolys[0]->getCentroid(left);
					ptPolys[1]->getCentroid(right);
				}
				else
				{
					ptPolys[0]->getCentroid(right);
					ptPolys[1]->getCentroid(left);
				}
				
				angle = atan(left.y/left.x);
			}
			else if(numPts == 1)
			{
				cout << "Polygon = " << ptPolys[0]->toString() << endl;
				ptPolys[0]->getCentroid(centre);
				cout << "Centre = [" << centre.x << "," << centre.y << "]" << endl;
				angle = atan(centre.y/centre.x);
			}
			else if(numPts > 2)
			{
				cout << "Do not know how to deal with more points...\n";
			}
			else
			{
				cout << "No point found!\n";
			}
			
			cout << "Angle: " << angle << " radians = " << (angle / M_PI)*180 << " degrees." << endl;
			
			*numPolys = numPts;
			return ptPolys;
		}
		catch(RSGISFFTException e)
		{
			
			
			throw e;
		}
		
	}	
	
	RSGISFFTProcessing::~RSGISFFTProcessing()
	{
		
	}
}}


