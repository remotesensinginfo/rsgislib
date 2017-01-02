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
	
    geos::geom::Polygon** RSGISFFTProcessing::findDominateFreq(rsgis::math::Matrix *magnitude, int startCircle, int endCircle, int *numPolys) throw(RSGISFFTException)
	{
		rsgis::math::RSGISMatrices matrixUtils;
		rsgis::geom::RSGISGeometry geomUtils;
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
		
		geos::geom::Polygon **circles = NULL;
		geos::geom::Polygon **bboxes = NULL;
		
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
			
			std::cout << "Centre Point [" << halfWidth << "," << halfHeight << "](" << ((halfHeight) * magnitude->m)+halfWidth << ") = " << magnitude->matrix[((halfHeight) * magnitude->m)+halfWidth] << std::endl;
			
			if(startCircle > endCircle)
			{
				throw RSGISFFTException("StartCircle is greater than endCircle.");
			}
			if(endCircle > maxCircleRadius)
			{
				std::cout << "endCircle was larger than the image. Defaulting to image size " << maxCircleRadius << std::endl;
				endCircle = maxCircleRadius;
			}
			
			numCircles = endCircle - startCircle;
			circles = new geos::geom::Polygon*[numCircles];
			circleCounter = 0;
			
			for(int i = startCircle; i < endCircle; i++)
			{
				circles[circleCounter++] = geomUtils.createCircle(0, 0, i, 0.5);
			}
			
			std::cout << "Created " << circleCounter << " circles\n";
			
			bboxes = new geos::geom::Polygon*[length];
			
			std::cout << "Matrix size = [" << magnitude->m << "," << magnitude->n << "] centre = ["  << halfWidth << "," << halfHeight << "]\n";
			
			row = 0;
			column = 0;
			step = 1.0;
			startX = (halfWidth + 0.5) * (-1);
			leftX = startX;
			rightX = startX + step;
			topY = halfHeight + 0.5;
			bottomY = topY - step;
			
			std::cout << "TL [" << leftX << "," << topY << "]\n";
			
			for(int i = 0; i < length; i++)
			{
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
						
			
			
			int maxCircle = 0;
			double maxValue = 0;
			for(int i = 0; i < numCircles; i++)
			{
				std::cout << "Circle " << i << ":" << std::endl;
            }
			
			distFromCentre = startCircle + maxCircle;
			distPixels = ((float)distFromCentre)/2;
			
			std::cout << "\nCircle " << maxCircle << " is the circle with the distance on it\n";
			std::cout << "Distance value = " << maxValue << std::endl;
			std::cout << "Distance from centre = " << distFromCentre << " therefore width is " << distPixels << " pixels." << std::endl;
			
			int numPts = 0;
			geos::geom::Polygon **ptPolys = geomUtils.findMaxPointsOnPolyEdge(magnitude, bboxes, circles[maxCircle], &numPts);
						
			geos::geom::Coordinate centre;
			std::cout << "number of points = " << numPts << std::endl;
			float angle = 0;
			if(numPts == 2)
			{
				geos::geom::Coordinate left;
				geos::geom::Coordinate right;
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
				std::cout << "Polygon = " << ptPolys[0]->toString() << std::endl;
				ptPolys[0]->getCentroid(centre);
				std::cout << "Centre = [" << centre.x << "," << centre.y << "]" << std::endl;
				angle = atan(centre.y/centre.x);
			}
			else if(numPts > 2)
			{
				std::cout << "Do not know how to deal with more points...\n";
			}
			else
			{
				std::cout << "No point found!\n";
			}
			
			std::cout << "Angle: " << angle << " radians = " << (angle / M_PI)*180 << " degrees." << std::endl;
			
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


