/*
 *  RSGISGeometry.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/06/2008.
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

#include "RSGISGeometry.h"

namespace rsgis{namespace geom{

	RSGISGeometry::RSGISGeometry()
	{
		
	}
	
	void RSGISGeometry::fitPlane2Points(point3D *pts, int numPts, double *a, double *b, double *c)
	{
		rsgis::math::RSGISMatrices matrices;
		rsgis::math::Matrix *matrixA = NULL;
		rsgis::math::Matrix *matrixB = NULL;
		rsgis::math::Matrix *matrixCoFactors = NULL;
		rsgis::math::Matrix *matrixCoFactorsT = NULL;
		rsgis::math::Matrix *outputs = NULL;
		try
		{
			double sXY = 0;
			double sX = 0;
			double sXSqu = 0;
			double sY = 0;
			double sYSqu = 0;
			double sXZ = 0;
			double sYZ = 0;
			double sZ = 0;
			
			for(int i = 0; i < numPts; i++)
			{
				sXY += (pts[i].x * pts[i].y);
				sX += pts[i].x;
				sXSqu += (pts[i].x * pts[i].x);
				sY += pts[i].y;
				sYSqu += (pts[i].y * pts[i].y);
				sXZ += (pts[i].x * pts[i].z);
				sYZ += (pts[i].y * pts[i].z);
				sZ += pts[i].z;
			}
			
			matrixA = matrices.createMatrix(3, 3);
			matrixA->matrix[0] = sXSqu;
			matrixA->matrix[1] = sXY;
			matrixA->matrix[2] = sX;
			matrixA->matrix[3] = sXY;
			matrixA->matrix[4] = sYSqu;
			matrixA->matrix[5] = sY;
			matrixA->matrix[6] = sX;
			matrixA->matrix[7] = sY;
			matrixA->matrix[8] = numPts;
			matrixB = matrices.createMatrix(1, 3);
			matrixB->matrix[0] = sXZ;
			matrixB->matrix[1] = sYZ;
			matrixB->matrix[2] = sZ;
			
			double determinantA = matrices.determinant(matrixA);
			matrixCoFactors = matrices.cofactors(matrixA);
			matrixCoFactorsT = matrices.transpose(matrixCoFactors);
			double multiplier = 1/determinantA;
			matrices.multipleSingle(matrixCoFactorsT, multiplier);
			outputs = matrices.multiplication(matrixCoFactorsT, matrixB);
			*a = outputs->matrix[0];
			*b = outputs->matrix[1];
			*c = outputs->matrix[2];
		}
		catch(rsgis::math::RSGISMatricesException &e)
		{
			if(matrixA != NULL)
			{
                matrices.freeMatrix(matrixA);
			}
			if(matrixB != NULL)
			{
				matrices.freeMatrix(matrixB);
			}
			if(matrixCoFactors != NULL)
			{
				matrices.freeMatrix(matrixCoFactors);
			}
			if(matrixCoFactorsT != NULL)
			{
				matrices.freeMatrix(matrixCoFactorsT);
			}
			if(outputs != NULL)
			{
				matrices.freeMatrix(outputs);
			}
			throw rsgis::math::RSGISMathException(e.what());
		}
		
        matrices.freeMatrix(matrixA);
        matrices.freeMatrix(matrixB);
        matrices.freeMatrix(matrixCoFactors);
        matrices.freeMatrix(matrixCoFactorsT);
        matrices.freeMatrix(outputs);

	}
	
	double RSGISGeometry::calcPlaneFit(point3D *pts, int numPts, double a, double b, double c)
	{
		double planeZ = 0;
		double diff = 0;
		double sqdiff = 0;
		for(int i = 0; i < numPts; i++)
		{
			planeZ = (a * pts[i].x) + (b * pts[i].y) + c;
			diff = planeZ - pts[i].z;
			sqdiff += (diff * diff);
		}
		return sqrt(sqdiff);
	}
	
	double RSGISGeometry::calcPlaneZAt(double a, double b, double c, double x, double y)
	{
		return ((a * x) + (b * y) + c);
	}
	
	double RSGISGeometry::distance2D(point3D *pt1, point2D *pt2)
	{
		double diffX = pt1->x - pt2->x;
		double diffY = pt1->y - pt2->y;
		double sqdiff = (diffX * diffX) + (diffY * diffY);
		return sqrt(sqdiff);
	}
	
	geos::geom::Polygon* RSGISGeometry::createCircle(float a, float b, float r, float resolution)
	{
		const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		float c = 2 * M_PI * r;
		int numPts = floor((c/resolution)+0.5);
		
		if(numPts < 3)
		{
			throw RSGISGeometryException("You need to decrease your resolution as there are too few points to create a circle.");
		}
		
		double x = 0;
		double circleX;
		double y = 0;
		double circleY;
		float step = (2 * M_PI)/numPts;
		float t = 0;
		
		geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence();
		std::cout.precision(15);
		for(int i = 0; i < numPts; i++)
		{
			circleX = (r * cos(t));
			circleY = (r * sin(t));
						
			x = a + circleX;
			y = b + circleY;
						
			coordSeq->add(geos::geom::Coordinate(x, y, 0));
			t += step;
		}
		//Close loop.
		x = (r * cos(0.0)) + a;
		y = (r * sin(0.0)) + b;
		coordSeq->add(geos::geom::Coordinate(x, y, 0));
		
		geos::geom::LinearRing *linearRing = geomFactory->createLinearRing(coordSeq);
		geos::geom::Polygon *circle = geomFactory->createPolygon(linearRing, NULL);
		return circle;
	}
	
	geos::geom::Polygon* RSGISGeometry::createPolygon(float xTL, float yTL, float xBR, float yBR)
	{
		const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence();
		coordSeq->add(geos::geom::Coordinate(xTL, yTL, 0));
		coordSeq->add(geos::geom::Coordinate(xTL, yBR, 0));
		coordSeq->add(geos::geom::Coordinate(xBR, yBR, 0));
		coordSeq->add(geos::geom::Coordinate(xBR, yTL, 0));
		coordSeq->add(geos::geom::Coordinate(xTL, yTL, 0));
		
		geos::geom::LinearRing *linearRing = geomFactory->createLinearRing(coordSeq);
		geos::geom::Polygon *rectangle = geomFactory->createPolygon(linearRing, NULL);
		return rectangle;		
	}
	
	geos::geom::Polygon** RSGISGeometry::findMaxPointsOnPolyEdge(rsgis::math::Matrix *matrix, geos::geom::Polygon **bboxes, geos::geom::Polygon *poly, int *numPolys)
	{
		int length = matrix->m * matrix->n;
		std::vector<double> *values = new std::vector<double>();
		std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
		for(int i = 0; i < length; i++)
		{
			if(poly->overlaps(bboxes[i]))
			{
				values->push_back(matrix->matrix[i]);
				polys->push_back(bboxes[i]);
			}
		}
		
		bool first = true;
		double maxValue = 0;
		for(unsigned int i = 0; i < values->size(); i++)
		{
			std::cout << values->at(i) << std::endl;
			if(first)
			{
				maxValue = values->at(i);
				first = false;
			}
			else if(values->at(i) > maxValue)
			{
				maxValue = values->at(i);
			}
		}
		
		std::cout << "Max = " << maxValue << std::endl;
		
		*numPolys = 0;
		for(unsigned int i = 0; i < values->size(); i++)
		{
			if(values->at(i) == maxValue)
			{
				*numPolys = *numPolys + 1;
			}
		}
		
		std::cout << "There are " << *numPolys << " polygons = " << maxValue << std::endl;
		geos::geom::Polygon **outPolys = new geos::geom::Polygon*[*numPolys];
		int polyCounter = 0;
		for(unsigned int i = 0; i < values->size(); i++)
		{
			if(values->at(i) == maxValue)
			{
				outPolys[polyCounter++] = polys->at(i);
			}
		}
		return outPolys;
	}
	
	geos::geom::Envelope* RSGISGeometry::findBBox(std::list<RSGIS2DPoint*> *pts)
	{
		if(pts->size() < 2)
		{
			throw RSGISGeometryException("Insufficent points with which to create the bounding box\n");
		}
		
		const geos::geom::Coordinate *coord;
		std::list<RSGIS2DPoint*>::iterator iterPts;
		
		double minX = 0;
		double minY = 0;
		double maxX = 0;
		double maxY = 0;
		
		bool first = true;
		for(iterPts = pts->begin(); iterPts != pts->end(); iterPts++)
		{
			coord = (*iterPts)->getPoint();
			
			if(first)
			{
				minX = coord->x;
				minY = coord->y;
				maxX = coord->x;
				maxY = coord->y;
				first = false;
			}
			else
			{
				if(coord->x < minX)
				{
					minX = coord->x;
				}
				else if(coord->x > maxX)
				{
					maxX = coord->x;
				}
				
				if(coord->y < minY)
				{
					minY = coord->y;
				}
				else if(coord->y > maxY)
				{
					maxY = coord->y;
				}
			}
		}
		
		return new geos::geom::Envelope(minX, maxX, minY, maxY);
	}

	geos::geom::Envelope* RSGISGeometry::findBBox(std::vector<RSGIS2DPoint*> *pts)
	{
		if(pts->size() < 2)
		{
			throw RSGISGeometryException("Insufficent points with which to create the bounding box\n");
		}
		
		const geos::geom::Coordinate *coord;
		std::vector<RSGIS2DPoint*>::iterator iterPts;
		
		double minX = 0;
		double minY = 0;
		double maxX = 0;
		double maxY = 0;
		
		bool first = true;
		for(iterPts = pts->begin(); iterPts != pts->end(); iterPts++)
		{
			coord = (*iterPts)->getPoint();
			
			if(first)
			{
				minX = coord->x;
				minY = coord->y;
				maxX = coord->x;
				maxY = coord->y;
				first = false;
			}
			else
			{
				if(coord->x < minX)
				{
					minX = coord->x;
				}
				else if(coord->x > maxX)
				{
					maxX = coord->x;
				}
				
				if(coord->y < minY)
				{
					minY = coord->y;
				}
				else if(coord->y > maxY)
				{
					maxY = coord->y;
				}
			}
		}
		
		return new geos::geom::Envelope(minX, maxX, minY, maxY);
	}
	
	
	RSGISTriangle* RSGISGeometry::findBoundingTriangle(std::list<RSGIS2DPoint*> *pts)
	{
		geos::geom::Envelope *env = NULL;
		try
		{
			env = this->findBBox(pts);
		}
		catch(RSGISGeometryException& e)
		{
			throw e;
		}
		
		geos::geom::Coordinate centre;
		env->centre(centre);
		double width = env->getWidth();
		double height = env->getHeight();
		double maxValue = 0;
		if(width > height)
		{
			maxValue = width;
		}
		else
		{
			maxValue = height;
		}
		double triangleValue = maxValue*3;
		
		RSGIS2DPoint *a = new RSGIS2DPoint(new geos::geom::Coordinate(centre.x, (centre.y + triangleValue), 0));
		RSGIS2DPoint *b = new RSGIS2DPoint(new geos::geom::Coordinate((centre.x + triangleValue), centre.y, 0));
		RSGIS2DPoint *c = new RSGIS2DPoint(new geos::geom::Coordinate((centre.x - triangleValue),(centre.y - triangleValue), 0));
		RSGISTriangle *tri = new RSGISTriangle(a, b, c);
		
		delete env;
		return tri;
	}
	
	RSGISTriangle* RSGISGeometry::findBoundingTriangle(std::vector<RSGIS2DPoint*> *pts)
	{
		geos::geom::Envelope *env = NULL;
		try
		{
			env = this->findBBox(pts);
		}
		catch(RSGISGeometryException& e)
		{
			throw e;
		}
		
		geos::geom::Coordinate centre;
		env->centre(centre);
		double width = env->getWidth();
		double height = env->getHeight();
		double maxValue = 0;
		if(width > height)
		{
			maxValue = width;
		}
		else
		{
			maxValue = height;
		}
		double triangleValue = maxValue*3;

		RSGIS2DPoint *a = new RSGIS2DPoint(new geos::geom::Coordinate(centre.x, (centre.y + triangleValue), 0));
		RSGIS2DPoint *b = new RSGIS2DPoint(new geos::geom::Coordinate((centre.x + triangleValue), centre.y, 0));
		RSGIS2DPoint *c = new RSGIS2DPoint(new geos::geom::Coordinate((centre.x - triangleValue),(centre.y - triangleValue), 0));
		RSGISTriangle *tri = new RSGISTriangle(a, b, c);
		
		delete env;
		return tri;
	}
	
	double RSGISGeometry::calcAngle(RSGIS2DPoint *pbase, RSGIS2DPoint *pt)
	{
		rsgis::math::RSGISMathsUtils mathUtils;
		double dX = pbase->getX() - pt->getX();
		double dY = pbase->getY() - pt->getY();
		double r = sqrt((dX*dX) + (dY*dY));
		double angle = 0;
		
		if((dX >= 0) & (dY >= 0))
		{
			// TOP RIGHT
			angle = 0;
			angle = angle + asin(dX/r);
		}
		else if((dX >=0) & (dY < 0))
		{
			// BOTTOM RIGHT
			angle = mathUtils.degreesToRadians(90);
			angle = angle + acos(dX/r);
		}
		else if((dX < 0)  & (dY < 0))
		{
			//BOTTOM LEFT
			angle = mathUtils.degreesToRadians(180);
			angle = angle + asin((dX*(-1))/r);
		}
		else
		{
			// TOP RIGHT
			angle = mathUtils.degreesToRadians(270);
			angle = angle + acos((dX*(-1))/r);
		}
        
		return angle;
	}
	
	geos::geom::MultiPolygon* RSGISGeometry::createMultiPolygon(const std::vector<geos::geom::Geometry*> *polygons)
	{
		const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		return geomFactory->createMultiPolygon(*polygons);
	}
	
	geos::geom::Polygon* RSGISGeometry::findConvexHull(std::vector<geos::geom::Polygon*> *geoms)
	{
		std::vector<geos::geom::Coordinate> *coordinates  = new std::vector<geos::geom::Coordinate>();
		geos::geom::CoordinateSequence *geomCoords = NULL;
		std::vector<geos::geom::Polygon*>::iterator iterGeom;
		for(iterGeom = geoms->begin(); iterGeom != geoms->end(); iterGeom++)
		{
			geomCoords = (*iterGeom)->getCoordinates();
			
			for(unsigned int i = 0; i < geomCoords->size(); i++)
			{
				coordinates->push_back(geomCoords->getAt(i));
			}
			delete geomCoords;
		}
		
		geos::geom::Polygon *poly = this->findConvexHull(coordinates);
		delete coordinates;
		
		return poly;
	}
	
	geos::geom::Polygon* RSGISGeometry::findConvexHull(std::list<geos::geom::Polygon*> *geoms)
	{
		std::vector<geos::geom::Coordinate> *coordinates  = new std::vector<geos::geom::Coordinate>();
		geos::geom::CoordinateSequence *geomCoords = NULL;
		std::list<geos::geom::Polygon*>::iterator iterGeom;
		for(iterGeom = geoms->begin(); iterGeom != geoms->end(); iterGeom++)
		{
			geomCoords = (*iterGeom)->getCoordinates();
			
			for(unsigned int i = 0; i < geomCoords->size(); i++)
			{
				coordinates->push_back(geomCoords->getAt(i));
			}
			delete geomCoords;
		}
		
		geos::geom::Polygon *poly = this->findConvexHull(coordinates);
		delete coordinates;
		
		return poly;
	}
	
	geos::geom::Polygon* RSGISGeometry::findConvexHull(const std::vector<geos::geom::Geometry*> *geometries)
	{
		std::vector<geos::geom::Geometry*> *geoms = const_cast<std::vector<geos::geom::Geometry*>*>(geometries);
		std::vector<geos::geom::Coordinate> *coordinates  = new std::vector<geos::geom::Coordinate>();
		geos::geom::CoordinateSequence *geomCoords = NULL;
		std::vector<geos::geom::Geometry*>::iterator iterGeom;
		for(iterGeom = geoms->begin(); iterGeom != geoms->end(); iterGeom++)
		{
			geomCoords = (*iterGeom)->getCoordinates();
			
			for(unsigned int i = 0; i < geomCoords->size(); i++)
			{
				coordinates->push_back(geomCoords->getAt(i));
			}
			delete geomCoords;
		}
		
		geos::geom::Polygon *poly = this->findConvexHull(coordinates);
		delete coordinates;
		
		return poly;
	}
	
	geos::geom::Polygon* RSGISGeometry::findConvexHull(std::vector<geos::geom::LineSegment> *lines)
	{
		std::vector<geos::geom::Coordinate> *coordinates  = new std::vector<geos::geom::Coordinate>();
		std::vector<geos::geom::LineSegment>::iterator iterLines;
		for(iterLines = lines->begin(); iterLines != lines->end(); iterLines++)
		{
			coordinates->push_back((*iterLines).p0);
			coordinates->push_back((*iterLines).p1);
		}
		geos::geom::Polygon *poly = this->findConvexHull(coordinates);
		coordinates->clear();
		delete coordinates;
		return poly;
	}
	
	geos::geom::Polygon* RSGISGeometry::findConvexHull(std::vector<geos::geom::Coordinate> *coordinates)
	{
        geos::geom::Polygon *poly = NULL;
        try
        {
            std::sort(coordinates->begin(), coordinates->end(), SortCoordinates(geos::geom::Coordinate(0,0,0)));
            
            if(coordinates->size() > 2)
            {
                bool upEdgeLoopBack = true;
                
                // Upper Edge
                std::vector<geos::geom::Coordinate> *upperEdge = new std::vector<geos::geom::Coordinate>();
                upperEdge->push_back(coordinates->at(0));
                upperEdge->push_back(coordinates->at(1));
                
                for(unsigned int i = 2; i < coordinates->size(); i++)
                {
                    upEdgeLoopBack = true;
                    
                    upperEdge->push_back(coordinates->at(i));
                    
                    while(upEdgeLoopBack)
                    {
                        int currentIndex = upperEdge->size() -1;
                        
                        if(upperEdge->size() <= 2)
                        {
                            upEdgeLoopBack = false;
                        }
                        else if(this->turnDirections(&(upperEdge->at(currentIndex-2)),
                                                     &(upperEdge->at(currentIndex-1)),
                                                     &(upperEdge->at(currentIndex))) != 1)
                        {
                            this->removeItem(upperEdge, upperEdge->at(currentIndex-1));
                        }
                        else
                        {
                            upEdgeLoopBack = false;
                        }
                    }
                }
                
                // Lower Edge
                std::vector<geos::geom::Coordinate> *lowerEdge = new std::vector<geos::geom::Coordinate>();
                int nodeIndex = coordinates->size() -1;
                lowerEdge->push_back(coordinates->at(nodeIndex--));
                lowerEdge->push_back(coordinates->at(nodeIndex--));
                
                for(int i = nodeIndex; i >= 0; i--)
                {
                    upEdgeLoopBack = true;
                    lowerEdge->push_back(coordinates->at(i));
                    while(upEdgeLoopBack)
                    {
                        int currentIndex = lowerEdge->size() -1;
                        if(lowerEdge->size() <= 2)
                        {
                            upEdgeLoopBack = false;
                        }
                        else if(this->turnDirections(&(lowerEdge->at(currentIndex)),
                                                     &(lowerEdge->at(currentIndex-1)),
                                                     &(lowerEdge->at(currentIndex-2))) != -1)
                        {
                            this->removeItem(lowerEdge, lowerEdge->at(currentIndex-1));
                        }
                        else
                        {
                            upEdgeLoopBack = false;
                        }
                    }
                }

                // Create Polygon...
                geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence();
                std::vector<geos::geom::Coordinate>::iterator iterCoords;
                for(iterCoords = upperEdge->begin(); iterCoords != upperEdge->end(); ++iterCoords)
                {
                    coordSeq->add((*iterCoords), true);
                }
                for(iterCoords = lowerEdge->begin(); iterCoords != lowerEdge->end(); ++iterCoords)
                {
                    coordSeq->add((*iterCoords), true);
                }
                
                coordSeq->add(upperEdge->front(), true);
                
                const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
                geos::geom::LinearRing *polyRing = new geos::geom::LinearRing(coordSeq, geomFactory);
                poly = geomFactory->createPolygon(polyRing, NULL);
                
                delete upperEdge;
                delete lowerEdge;
            }
            else 
            {
                throw RSGISGeometryException("Too few geos::geom::Coordinate to create convert hull.");
            }
        }
        catch (RSGISGeometryException &e)
        {
            throw e;
        }
		catch (RSGISException &e)
        {
            throw RSGISGeometryException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISGeometryException(e.what());
        }
		
		return poly;
	}
    
    geos::geom::Polygon* RSGISGeometry::findBoundingBox(std::vector<geos::geom::Coordinate> *coordinates)
    {
        geos::geom::Polygon *poly = NULL;
        try
        {
            // Do something else.
            double xMin = 0;
            double xMax = 0;
            double yMin = 0;
            double yMax = 0;
            bool first = true;
            for(std::vector<geos::geom::Coordinate>::iterator iterCoords = coordinates->begin(); iterCoords != coordinates->end(); ++iterCoords)
            {
                if(first)
                {
                    xMin = (*iterCoords).x;
                    xMax = (*iterCoords).x;
                    yMin = (*iterCoords).y;
                    yMax = (*iterCoords).y;
                    first = false;
                }
                else
                {
                    if((*iterCoords).x < xMin)
                    {
                        xMin = (*iterCoords).x;
                    }
                    else if((*iterCoords).x > xMax)
                    {
                        xMax = (*iterCoords).x;
                    }
                    
                    if((*iterCoords).y < yMin)
                    {
                        yMin = (*iterCoords).y;
                    }
                    else if((*iterCoords).y > yMax)
                    {
                        yMax = (*iterCoords).y;
                    }
                }
            }
            
            if((xMax - xMin) < 0.1)
            {
                xMax += 1;
                xMin -= 1;
            }
            
            if((yMax - yMin) < 0.1)
            {
                yMax += 1;
                yMin -= 1;
            }
            
            
            // Create Polygon...
            geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence();
            coordSeq->add(geos::geom::Coordinate(xMin, yMax), true);
            coordSeq->add(geos::geom::Coordinate(xMax, yMax), true);
            coordSeq->add(geos::geom::Coordinate(xMax, yMin), true);
            coordSeq->add(geos::geom::Coordinate(xMin, yMin), true);
            coordSeq->add(geos::geom::Coordinate(xMin, yMax), true);
            
            const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
            geos::geom::LinearRing *polyRing = new geos::geom::LinearRing(coordSeq, geomFactory);
            poly = geomFactory->createPolygon(polyRing, NULL);
        }
        catch (RSGISGeometryException &e)
        {
            throw e;
        }
		catch (RSGISException &e)
        {
            throw RSGISGeometryException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISGeometryException(e.what());
        }
		
		return poly;
    }
	
    int RSGISGeometry::turnDirections(geos::geom::Coordinate *a, geos::geom::Coordinate *b, geos::geom::Coordinate *c)
    {
		double ax = a->x - 0.1;
		double bx = b->x;
		double cx = c->x + 0.1;
		double ay = a->y;
		double by = b->y;
		double cy = c->y;
        int returnValue = 0;
        // y = (dy/dx)x - n
        double dydx = (cy - ay)/(cx - ax);
        double n = ay - (dydx * ax);
        double bYPos = (dydx*bx) + n;

        if(bYPos < by)
        {
            returnValue = 1;
        }
        else if(bYPos > by)
        {
            returnValue = -1;
        }
        else
        {
            returnValue = 0;
        }
        return returnValue;
    }
	
	bool RSGISGeometry::removeItem(std::vector<geos::geom::Coordinate> *coordinates, geos::geom::Coordinate coord)
	{
		std::vector<geos::geom::Coordinate>::iterator iterCoords;
		for(iterCoords = coordinates->begin(); iterCoords != coordinates->end(); iterCoords++)
		{
			if(coord == (*iterCoords))
			{
				coordinates->erase(iterCoords);
				return true;
			}
		}
		return false;
	}
	
	void RSGISGeometry::findLine(geos::geom::Coordinate *p1, geos::geom::Coordinate *p2, double *m, double *c)
	{
		*m = (p2->y - p1->y)/(p2->x - p1->x);
        *c = p1->y - ((*m) * p1->x);
	}
	
	void RSGISGeometry::findPointOnLine(geos::geom::Coordinate *p1, geos::geom::Coordinate *p2, float distance, geos::geom::Coordinate *p3)
	{
		if(distance == 0)
		{
			p3->x = p1->x;
			p3->y = p1->y;
		}
		else
		{
			double dx = p2->x - p1->x;
			double dy = p2->y - p1->y;
			double theta = atan(dy/dx);
			double y1 = distance * sin(theta);
			double x1 = distance * cos(theta);

            if((dx >= 0) & (dy > 0))
			{
				p3->x = p1->x + x1;
				p3->y = p1->y + y1;
			}
			else if((dx >= 0) & (dy <= 0))
			{
				p3->x = p1->x + x1;
				p3->y = p1->y + y1;
			}
			else if((dx < 0) & (dy > 0))
			{
				p3->x = p1->x - x1;
				p3->y = p1->y - y1;
			}
			else if((dx < 0) & (dy <= 0))
			{
				p3->x = p1->x - x1;
				p3->y = p1->y - y1;
			}
		}
		p3->z = 0;
	}
	
	void RSGISGeometry::findPointOnLine(geos::geom::Coordinate *p1, geos::geom::Coordinate *p2, float distance, std::list<RSGIS2DPoint*> *pts)
	{
		
		if(distance != 0)
		{
			float length = p1->distance(*p2);
			int numSegments = length/distance;
			
			double dx = p2->x - p1->x;
			double dy = p2->y - p1->y;
			double theta = atan(dy/dx);
						
			double y1 = 0;
			double x1 = 0;
			double stepDist = 0;
			
			for(int i = 0; i < numSegments; i++)
			{
				stepDist = distance * i;

				y1 = stepDist * sin(theta);
				x1 = stepDist * cos(theta);
				
				if((dx >= 0) & (dy > 0))
				{
					pts->push_back(new RSGIS2DPoint(new geos::geom::Coordinate((p1->x + x1), (p1->y + y1), 0)));
				}
				else if((dx >= 0) & (dy <= 0))
				{
					pts->push_back(new RSGIS2DPoint(new geos::geom::Coordinate((p1->x + x1), (p1->y + y1), 0)));
				}
				else if((dx < 0) & (dy > 0))
				{
					pts->push_back(new RSGIS2DPoint(new geos::geom::Coordinate((p1->x - x1), (p1->y - y1), 0)));
				}
				else if((dx < 0) & (dy <= 0))
				{
					pts->push_back(new RSGIS2DPoint(new geos::geom::Coordinate((p1->x - x1), (p1->y - y1), 0)));
				}
			}
		}
	}
	
	void RSGISGeometry::findPointOnLine(geos::geom::Coordinate *p1, geos::geom::Coordinate *p2, float distance, geos::geom::CoordinateSequence *coords)
	{
		
		if(distance > 0)
		{
			float length = p1->distance(*p2);
			int numSegments = length/distance;
			
			double dx = p2->x - p1->x;
			double dy = p2->y - p1->y;
			double theta = atan(dy/dx);
			
			double y1 = 0;
			double x1 = 0;
			double stepDist = 0;
			
			for(int i = 0; i < numSegments; i++)
			{
				stepDist = distance * i;
				
				y1 = stepDist * sin(theta);
				x1 = stepDist * cos(theta);
				
				if((dx >= 0) & (dy > 0))
				{
					coords->add(geos::geom::Coordinate((p1->x + x1), (p1->y + y1), 0));
				}
				else if((dx >= 0) & (dy <= 0))
				{
					coords->add(geos::geom::Coordinate((p1->x + x1), (p1->y + y1), 0));
				}
				else if((dx < 0) & (dy > 0))
				{
					coords->add(geos::geom::Coordinate((p1->x - x1), (p1->y - y1), 0));
				}
				else if((dx < 0) & (dy <= 0))
				{
					coords->add(geos::geom::Coordinate((p1->x - x1), (p1->y - y1), 0));
				}
			}
		}
	}
	
	void RSGISGeometry::findPoint2Side(geos::geom::Coordinate *pStart, geos::geom::Coordinate *p2, geos::geom::Coordinate *pEnd, float distance, geos::geom::Coordinate *p3)
	{
		double dx = pEnd->x - pStart->x;
		double dy = pEnd->y - pStart->y;
		double beta = atan(dy/dx);
				
		double distanceP1P2 = pStart->distance(*p2);
		double distanceP1P3 = sqrt((distance * distance) + (distanceP1P2 * distanceP1P2));
		double theta = atan(distance/distanceP1P2);
		double alpha = M_PI - (theta - beta);
        
		double localX = distanceP1P3 * cos(alpha);
		double localY = distanceP1P3 * sin(alpha);
		
		if((dx >= 0) & (dy > 0))
		{
			p3->x = pStart->x - localX;
			p3->y = pStart->y - localY;
		}
		else if((dx >= 0) & (dy <= 0))
		{
			p3->x = pStart->x - localX;
			p3->y = pStart->y - localY;
		}
		else if((dx < 0) & (dy > 0))
		{
			p3->x = pStart->x + localX;
			p3->y = pStart->y + localY;
		}
		else if((dx < 0) & (dy <= 0))
		{
			p3->x = pStart->x + localX;
			p3->y = pStart->y + localY;
		}
		else
		{
			std::cout << "ERROR DO NOT HAVE A SOLUTION\n";
		}
		p3->z = 0;
	}
	
	geos::geom::Envelope* RSGISGeometry::getEnvelope(geos::geom::Geometry *geom)
	{
		geos::geom::Envelope *env = new geos::geom::Envelope();
		geos::geom::CoordinateSequence *coordSeq = geom->getCoordinates();
		
		int numPts = coordSeq->getSize();
		for(int i = 0; i < numPts; i++)
		{
			env->expandToInclude(coordSeq->getAt(i));
		}
		
		delete coordSeq;		
		return env;
	}
	
	bool RSGISGeometry::findClosestIntersect(std::vector<geos::geom::LineSegment> *lines, geos::geom::LineSegment *line, geos::geom::Coordinate *pt, geos::geom::Coordinate *coord)
	{
		bool returnValue = false;
		
		geos::geom::Coordinate minCoord;
		double minDistance = 0;
		double distance = 0;
		geos::geom::Coordinate tmpCoord;
		bool first = true;
		std::vector<geos::geom::LineSegment>::iterator iterLines;
		for(iterLines = lines->begin(); iterLines != lines->end(); iterLines++)
		{
			if(line->intersection((*iterLines), tmpCoord))
			{
				if(first)
				{
					first = false;
					minCoord = tmpCoord;
					minDistance = pt->distance(tmpCoord);
				}
				else
				{
					distance = pt->distance(tmpCoord);
					if(distance < minDistance)
					{
						minCoord = tmpCoord;
						minDistance = distance;
					}
				}
				returnValue = true;
			}
		}
		
		*coord = minCoord;
		return returnValue;
	}
	
	void RSGISGeometry::covert2LineSegments(geos::geom::Polygon *poly, std::vector<geos::geom::LineSegment> *lines)
	{
		geos::geom::CoordinateSequence *coords = poly->getExteriorRing()->getCoordinates();
		int numPts = coords->size()-1;
		for(int i = 0; i < numPts; ++i)
		{
			lines->push_back(geos::geom::LineSegment(coords->getAt(i), coords->getAt(i+1)));
		}
		delete coords;
		
		int numHoles = poly->getNumInteriorRing();
		for(int n = 0; n < numHoles; ++n)
		{
			coords = poly->getInteriorRingN(n)->getCoordinates();
			numPts = coords->size()-1;
			for(int i = 0; i < numPts; ++i)
			{
				lines->push_back(geos::geom::LineSegment(coords->getAt(i), coords->getAt(i+1)));
			}
			delete coords;
			
		}
	}
	
	void RSGISGeometry::dissolveMergeSmallPolygons(std::vector<geos::geom::Polygon*> *polygons, std::vector<geos::geom::Polygon*> *smallPolygons, RSGISIdentifyNonConvexPolygons *nonconvexoutline)
	{
		try
		{
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			std::vector<geos::geom::Polygon*>::iterator iterSmallPolys;
			geos::geom::Polygon *poly = NULL;
			geos::geom::Polygon *overlapPolygon = NULL;
			geos::geom::Polygon *containedPolygon = NULL;
			geos::geom::Polygon *areaErrorPolygon = NULL;
			bool change = true;
			bool overlap = false;
			bool contains = false;
			bool areaError = false;
			int count = 0;
			std::cout << "Merging neighboring polygons, this may take some time: \nStarted .." << std::flush;
			while(change)
			{
				change = false;
				iterPolys = polygons->begin();
				while(iterPolys != polygons->end())
				{
					if(count % 20 == 0)
					{
						std::cout << "." << std::flush;
					}
					poly = *iterPolys;
					if(poly->getArea() < 0.1)
					{
						polygons->erase(iterPolys);
						delete poly;
					}
					else
					{
						overlap = false;
						contains = false;
						areaError = false;
						for(iterSmallPolys = smallPolygons->begin(); iterSmallPolys != smallPolygons->end(); iterSmallPolys++)
						{
							if((*iterSmallPolys)->getArea() < 0.1)
							{
								areaError = true;
								areaErrorPolygon = *iterSmallPolys;
								break;
							}
							else if(poly->contains((*iterSmallPolys)))
							{
								contains = true;
								containedPolygon = *iterSmallPolys;
								break;
							}
							else if(poly->overlaps((*iterSmallPolys)))
							{
								overlap = true;
								overlapPolygon = *iterSmallPolys;
								break;
							}
						}
						
						if(areaError)
						{
							change = true;
							smallPolygons->erase(std::remove(smallPolygons->begin(), smallPolygons->end(), areaErrorPolygon));
							delete areaErrorPolygon;
							iterPolys++;
						}
						else if(contains)
						{
							change = true;
							smallPolygons->erase(std::remove(smallPolygons->begin(), smallPolygons->end(), containedPolygon));
							delete containedPolygon;
							iterPolys++;
						}
						else if(overlap)
						{
							change = true;
							smallPolygons->erase(std::remove(smallPolygons->begin(), smallPolygons->end(), overlapPolygon));
							polygons->erase(iterPolys);
							geos::geom::Polygon *newPoly = this->mergePolygons(poly, overlapPolygon, nonconvexoutline);
							polygons->push_back(newPoly);

							iterPolys = polygons->begin();
							
							delete poly;
							delete overlapPolygon;
						}
						else
						{
							iterPolys++;
						}
					}
					count++;
				}
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	void RSGISGeometry::mergeWithNearest(std::vector<geos::geom::Polygon*> *polygons, std::vector<geos::geom::Polygon*> *polygonsToMerge, RSGISIdentifyNonConvexPolygons *identifyNonConvexPolygon)
	{
		int numPolygons = polygons->size();
		int numPolygonsToMerge = polygonsToMerge->size();
		geos::geom::Polygon *poly = NULL;
		geos::geom::Polygon *tmpPoly = NULL;
		
		std::vector<geos::geom::Polygon*> *polygonsMerge = new std::vector<geos::geom::Polygon*>();
		
		float distance = 0;
		float minDistance = 0;
		int minIndex = 0;
		bool first = true;
		for(int i = 0; i < numPolygonsToMerge; i++)
		{
			poly = polygonsToMerge->at(i);
			first = true;
			for(int j = 0; j < numPolygons; j++)
			{
				tmpPoly = polygons->at(j);
				distance = poly->distance(tmpPoly);
				if(first)
				{
					minIndex = j;
					minDistance = distance;
					first = false;
				}
				else if(distance < minDistance)
				{
					minIndex = j;
					minDistance = distance;
				}
			}
			polygonsMerge->push_back(polygonsToMerge->at(i));
			polygonsMerge->push_back(polygons->at(minIndex));
			polygons->erase((polygons->begin()+minIndex));
			polygons->push_back(identifyNonConvexPolygon->retrievePolygon(polygonsMerge));
			polygonsMerge->clear();
		}
		polygonsToMerge->clear();
		delete polygonsMerge;
	}
	
	void RSGISGeometry::mergeWithNeighbor(std::vector<geos::geom::Polygon*> *polygons, std::vector<geos::geom::Polygon*> *polygonsToMerge, float relBorderThreshold, RSGISIdentifyNonConvexPolygons *nonconvexoutline)
	{
		try
		{
			int count = 0;
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			std::vector<geos::geom::Polygon*>::iterator iterPolysToMerge;
			geos::geom::Polygon *poly = NULL;
			geos::geom::Polygon *maxRelBorderPoly = NULL;
			float relBorder = 0;
			float maxRelBorder = 0;
			bool change = true;
			bool first = true;
			std::cout << "Merging neighboring polygons, this may take some time: \nStarted .." << std::flush;
			while(change)
			{
				std::cout << ".." << std::flush;
				relBorder = 0;
				maxRelBorder = 0;
				change = false;
				std::cout << "Number to merge = " << polygonsToMerge->size() << std::endl;
				for(iterPolysToMerge = polygonsToMerge->begin(); iterPolysToMerge != polygonsToMerge->end(); )
				{
					poly = *iterPolysToMerge;
					first = true;
					std::cout << "Number to polygons = " << polygons->size() << " and number to merge = " << polygonsToMerge->size() << std::endl;
					for(iterPolys = polygons->begin(); iterPolys != polygons->end(); ++iterPolys)
					{
						std::cout << count << std::flush;
						++count;
						std::cout << "1 area = " << poly->getArea() << " 2 area = " << (*iterPolys)->getArea() << std::endl;
						relBorder = this->calcRelativeBorder(poly, *iterPolys);
						std::cout << " = " << relBorder << std::endl;
						if(first)
						{
							maxRelBorderPoly = *iterPolys;
							maxRelBorder = relBorder;
							first = false;
						}
						else if(relBorder > maxRelBorder)
						{
							maxRelBorderPoly = *iterPolys;
							maxRelBorder = relBorder;
						}
					}
					
					std::cout << "Rel. border = " << maxRelBorder << std::endl;
					
					if(maxRelBorder > relBorderThreshold)
					{
						change = true;
						polygons->erase(std::remove(polygons->begin(), polygons->end(), maxRelBorderPoly));
						polygonsToMerge->erase(iterPolysToMerge);
						geos::geom::Polygon *newPoly = this->mergePolygons(poly, maxRelBorderPoly, nonconvexoutline);
						polygons->push_back(newPoly);
						delete poly;
						delete maxRelBorderPoly;
						
						if(polygonsToMerge->size() == 0)
						{
							break;
						}
					}
					else
					{
						iterPolysToMerge++;
					}
				}
			}
			std::cout << " Complete.\n";
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
	}
	
	float RSGISGeometry::calcRelativeBorder(geos::geom::Polygon *poly1in, geos::geom::Polygon *poly2in)
	{
		float relBorder = 0;
		try
		{
			geos::geom::Polygon *poly1 = dynamic_cast<geos::geom::Polygon*>(rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createGeometry(poly1in));
			geos::geom::Polygon *poly2 = dynamic_cast<geos::geom::Polygon*>(rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createGeometry(poly2in));
			
			geos::geom::Geometry *geom = this->getIntersection(poly1, poly2);
						
			if((geom != NULL) & (geom->getNumGeometries() > 0))
			{
			
				std::vector<geos::geom::LineString*> *lines = new std::vector<geos::geom::LineString*>();
				this->retrieveLines(geom, lines);
				std::vector<geos::geom::LineString*>::iterator iterLines;
				float sharedBorderLength = 0;
				for(iterLines = lines->begin(); iterLines != lines->end(); iterLines++)
				{
					sharedBorderLength += (*iterLines)->getLength();
					delete *iterLines;
				}
				lines->clear();
				delete lines;
				
				const geos::geom::CoordinateSequence *poly1Coords = poly1->getExteriorRing()->getCoordinatesRO();
				int poly1CoordsSize = poly1Coords->getSize()-1;
				float poly1BorderLength = 0;
				
				geos::geom::Coordinate poly1P1;
				geos::geom::Coordinate poly1P2;
				for(int i = 0; i < poly1CoordsSize; i++)
				{
					poly1P1 = poly1Coords->getAt(i);
					poly1P2 = poly1Coords->getAt(i+1);
					poly1BorderLength += poly1P1.distance(poly1P2);
				}
				poly1P1 = poly1Coords->getAt(poly1CoordsSize);
				poly1P2 = poly1Coords->getAt(0);
				poly1BorderLength += poly1P1.distance(poly1P2);
				
				relBorder = sharedBorderLength/poly1BorderLength;
			}
			else
			{
				// No Relative Border...
				relBorder = 0;
			}	
			rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(poly1);
			rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->destroyGeometry(poly2);
		}
		catch(geos::util::TopologyException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return relBorder;
	}
	
	geos::geom::Polygon* RSGISGeometry::mergePolygons(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2, RSGISIdentifyNonConvexPolygons *nonconvexoutline)
	{
		float tolerance = 0.25;
		geos::geom::Polygon *poly = NULL;
		try
		{
			bool areaError = false;
			bool error = false;	
			for(int i = 0; i < 4; i++)
			{
				try
				{
					geos::geom::Geometry *geom = poly1->Union(poly2);
					std::cout << "Union successful\n";
					if(geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON)
					{
						std::cout << "Type POLYGON\n";
						poly = dynamic_cast<geos::geom::Polygon*>(geom);
                        std::cout << "Area = " << poly->getArea() << std::endl;
					}
					else
					{
						std::cout << "Type OTHER\n";
						std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
						this->retrievePolygons(geom, polys);
						
						std::vector<geos::geom::Polygon*>::iterator iterPolys = polys->begin();
						while(iterPolys != polys->end())
						{
							if((*iterPolys)->getArea() < 0.1)
							{
								polys->erase(iterPolys);
							}
							else
							{
								iterPolys++;
							}
						}
						
						if(polys->size() == 0)
						{
							std::string message = std::string("Union of polygons did not produce a polygon: ") + std::string(geom->getGeometryType()) + std::string(" - could not resolve.");
							throw RSGISGeometryException(message.c_str());
						}
						else if(polys->size() == 1)
						{
							poly = polys->at(0);
						}
						else
						{
							poly = nonconvexoutline->retrievePolygon(polys);
						}
						delete polys;
					}
					
				}
				catch(geos::util::TopologyException &e)
				{
					error = true;
					std::cout << "i == " << i << std::endl;
					if(i == 0)
					{						
						std::cout << "WARNING: " << e.what() << std::endl;
						
						std::vector<geos::geom::Polygon*> *poly1Vec = new std::vector<geos::geom::Polygon*>();
						poly1Vec->push_back(poly1);
						std::vector<geos::geom::Polygon*> *poly2Vec = new std::vector<geos::geom::Polygon*>();
						poly2Vec->push_back(poly2);
						
						this->performMorphologicalOperation(poly1Vec, closing, 0, 30);
						this->performMorphologicalOperation(poly2Vec, closing, 0, 30);
						
						std::vector<geos::geom::Polygon*>::iterator iterClosedPolys = poly1Vec->begin();
						while(iterClosedPolys != poly1Vec->end())
						{
							if((*iterClosedPolys)->getArea() < 0.1)
							{
								poly1Vec->erase(iterClosedPolys);
							}
							else
							{
								iterClosedPolys++;
							}
						}
						
						iterClosedPolys = poly2Vec->begin();
						while(iterClosedPolys != poly2Vec->end())
						{
							if((*iterClosedPolys)->getArea() < 0.1)
							{
								poly2Vec->erase(iterClosedPolys);
							}
							else
							{
								iterClosedPolys++;
							}
						}
						
						
						if(poly1Vec->size() > 1)
						{
							throw RSGISGeometryException("too many polygons... (poly1)");
						}
						else
						{
							poly1 = poly1Vec->at(0);
						}
						
						if(poly2Vec->size() > 1)
						{
							throw RSGISGeometryException("too many polygons... (poly2)");
						}
						else
						{
							poly2 = poly2Vec->at(0);
						}
						
						poly1Vec->clear();
						delete poly1Vec;
						poly2Vec->clear();
						delete poly2Vec;
												
						if((poly1->getArea() == 0) | (poly2->getArea() == 0))
						{
							areaError = true;
							break;
						}				
					}
					else if(i == 1)
					{						
						geos::geom::Polygon *snapPolygon1 = NULL;
						geos::geom::Polygon *snapPolygon2 = NULL;
						bool snapPoly1Error = false;
						bool snapPoly2Error = false;
						geos::geom::Envelope *env = this->getEnvelope(poly1);
						geos::geom::Envelope *env2 = this->getEnvelope(poly2);
						env->expandToInclude(env2);
						delete env2;
						try
						{
							snapPolygon1 = this->snapToXYGrid(poly1, tolerance, false, env);
						}
						catch(RSGISGeometryException &e)
						{
							snapPoly1Error = true;
						}
						try
						{
							snapPolygon2 = this->snapToXYGrid(poly2, tolerance, false, env);
						}
						catch(RSGISGeometryException &e)
						{
							snapPoly2Error = true;
						}
						
						delete env;
						
						if(!snapPoly1Error & !snapPoly2Error)
						{
							delete poly1;
							delete poly2;
							poly1 = snapPolygon1;
							poly2 = snapPolygon2;
						}
						if(snapPoly1Error & snapPoly2Error)
						{
							throw RSGISGeometryException(e.what());
						}
						else if(snapPoly1Error)
						{
							delete poly2;
							poly2 = snapPolygon2;
						}
						else if(snapPoly2Error)
						{
							delete poly1;
							poly1 = snapPolygon1;
						}
						else
						{
							// do nothing..
						}
						
						if((poly1->getArea() == 0) | (poly2->getArea() == 0))
						{
							areaError = true;
							break;
						}
					}
					else if(i == 2)
					{
						geos::geom::Polygon *nodedPolygon1 = NULL;
						geos::geom::Polygon *nodedPolygon2 = NULL;
						bool nodedPoly1Error = false;
						bool nodedPoly2Error = false;
						try
						{
							nodedPolygon1 = this->insertIntersectNodes(poly1, poly2);
						}
						catch(RSGISGeometryException &e)
						{
							nodedPoly1Error = true;
						}
						try
						{
							nodedPolygon2 = this->insertIntersectNodes(poly2, poly1);
						}
						catch(RSGISGeometryException &e)
						{
							nodedPoly2Error = true;
						}
						
						if(!nodedPoly1Error & !nodedPoly2Error)
						{
							delete poly1;
							delete poly2;
							poly1 = nodedPolygon1;
							poly2 = nodedPolygon2;
						}
						if(nodedPoly1Error & nodedPoly2Error)
						{
							throw RSGISGeometryException(e.what());
						}
						else if(nodedPoly1Error)
						{
							delete poly2;
							poly2 = nodedPolygon2;
						}
						else if(nodedPoly2Error)
						{
							delete poly1;
							poly1 = nodedPolygon1;
						}
						else
						{
							// do nothing..
						}
						
						if((poly1->getArea() == 0) | (poly2->getArea() == 0))
						{
							areaError = true;
							break;
						}
					}
					else if(i == 3)
					{
						throw RSGISGeometryException(e.what());
					}
				}
				catch(RSGISGeometryException &e)
				{
					throw e;
				}
				if(!error)
				{
					break;
				}
			}
			
			if(areaError)
			{
				if(poly1->getArea() > 0)
				{
					poly = poly1;
				}
				else if(poly2->getArea() > 0)
				{
					poly = poly2;
				}
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return poly;
	}
	
	geos::geom::Polygon* RSGISGeometry::mergePolygons(geos::geom::Polygon *poly1, std::vector<geos::geom::Polygon*> *polygons, RSGISIdentifyNonConvexPolygons *nonconvexoutline)
	{
		geos::geom::Polygon *poly = poly1;
		geos::geom::Polygon *tmpPoly = NULL;
		std::vector<geos::geom::Polygon*>::iterator iterPolys;
		try
		{
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
			{
				tmpPoly = this->mergePolygons(poly, *iterPolys, nonconvexoutline);
				delete poly;
				poly = tmpPoly;
			}	
		}
		catch(geos::util::TopologyException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return poly;
	}
	
	void RSGISGeometry::findContaining(geos::geom::Polygon *outline, std::vector<geos::geom::Polygon*> *polygons, std::vector<geos::geom::Polygon*> *outPolygons)
	{
		// Find the polygons from 'polygons' that are contained within 'outline'
		// Save polygons to 'outPolygons'
		
		std::vector<geos::geom::Polygon*>::iterator iterPolys;
		try
		{
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
			{
				if (outline->contains(*iterPolys) == true)
				{
					outPolygons->push_back(*iterPolys);
				}
			}	
		}
		catch(geos::util::TopologyException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	void RSGISGeometry::removeOverlaps(std::vector<geos::geom::Polygon*> *polygons, geos::geom::Envelope *env, float tolerance, float dissolveThreshold)
	{
		rsgis::math::RSGISMathsUtils mathUtils;
		try
		{
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			std::vector<geos::geom::Polygon*>::iterator iterPolys2;
			std::vector<geos::geom::Polygon*> *intersections = NULL;
			geos::geom::Polygon *polygon1 = NULL;
			geos::geom::Polygon *polygon2 = NULL;
			bool intersects = false;
			bool polygon1Contains = false;
			bool polygon2Contains = false;
			bool areaError = false;
			int count = 0;
			std::cout << "Started ." << std::flush;
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
			{
				if((count % 20) == 0)
				{
					std::cout << "." << std::flush;
				}
				count++;
				
				
				polygon1 = *iterPolys;
				if(polygon1->getArea() < 0.1)
				{
					polygons->erase(iterPolys);
					delete polygon1;
				}
				else
				{
					areaError = false;
					intersects = false;
					polygon1Contains = false;
					polygon2Contains = false;
					for(iterPolys2 = polygons->begin(); iterPolys2 != polygons->end(); iterPolys2++)
					{
						polygon2 = *iterPolys2;
						try
						{
							if(polygon2->getArea() < 0.1)
							{
								areaError = true;
								break;
							}
							else if((polygon1 != polygon2) && (polygon1->contains(polygon2) | polygon1->within(polygon2)))
							{
								polygon1Contains = true;
								break;
							}
							else if((polygon1 != polygon2) && (polygon2->contains(polygon1) | polygon2->within(polygon1)))
							{
								polygon2Contains = true;
								break;
							}
							else if((polygon1 != polygon2) && !polygon1->touches(polygon2) && polygon1->intersects(polygon2))
							{
								intersects = true;
								break;
							}
						}
						catch(geos::util::TopologyException &e)
						{
							std::cout << "WARNING: " << e.what() << std::endl;
						}
					}
					
					if(areaError)
					{
						polygons->erase(std::remove(polygons->begin(), polygons->end(), polygon2));
						delete polygon2;
					}
					else if(polygon1Contains)
					{
						if(polygon2->getArea() < dissolveThreshold)
						{
							polygons->erase(std::remove(polygons->begin(), polygons->end(), polygon2));
						}
						else
						{
							polygons->erase(std::remove(polygons->begin(), polygons->end(), polygon1));
							geos::geom::Polygon *newPoly = this->createHole(polygon1, polygon2);
							polygons->push_back(newPoly);
						}
						iterPolys = polygons->begin();
					}
					else if(polygon2Contains)
					{
						if(polygon1->getArea() < dissolveThreshold)
						{
							polygons->erase(std::remove(polygons->begin(), polygons->end(), polygon1));
						}
						else
						{
							polygons->erase(std::remove(polygons->begin(), polygons->end(), polygon2));
							geos::geom::Polygon *newPoly = this->createHole(polygon2, polygon1);
							polygons->push_back(newPoly);
						}
						iterPolys = polygons->begin();
					}
					else if(intersects)
					{
						polygons->erase(std::remove(polygons->begin(), polygons->end(), polygon1));
						polygons->erase(std::remove(polygons->begin(), polygons->end(), polygon2));
						
						geos::geom::Geometry *geomIntersection = NULL;
						geos::geom::Geometry *geomDifference1to2 = NULL;
						geos::geom::Geometry *geomDifference2to1 = NULL;
						bool error = false;
						bool snapError = false;
						
						for(int i = 0; i < 5; i++)
						{
							error = false;
							areaError = false;
							snapError = false;
							try
							{
								geomIntersection = NULL;
								geomDifference1to2 = NULL;
								geomDifference2to1 = NULL;
								geomIntersection = polygon1->intersection(polygon2);
								geomDifference1to2 = polygon1->difference(polygon2);
								geomDifference2to1 = polygon2->difference(polygon1);
							}
							catch(geos::util::TopologyException &e)
							{
								if(geomIntersection != NULL)
								{
									delete geomIntersection;
								}
								if(geomDifference1to2 != NULL)
								{
									delete geomDifference1to2;
								}
								if(geomDifference2to1 != NULL)
								{
									delete geomDifference2to1;
								}
								
								error = true;
								if(i == 0)
								{
									std::cout << "\nWARNING ERROR (" << e.what() << "), trying to resolve" << std::endl;
									
									
									std::vector<geos::geom::Polygon*> *poly1Vec = new std::vector<geos::geom::Polygon*>();
									poly1Vec->push_back(polygon1);
									std::vector<geos::geom::Polygon*> *poly2Vec = new std::vector<geos::geom::Polygon*>();
									poly2Vec->push_back(polygon2);
									
									this->performMorphologicalOperation(poly1Vec, closing, 0.01, 30);
									this->performMorphologicalOperation(poly2Vec, closing, 0.01, 30);
									
									std::vector<geos::geom::Polygon*>::iterator iterClosedPolys = poly1Vec->begin();
									while(iterClosedPolys != poly1Vec->end())
									{
										if((*iterClosedPolys)->getArea() < 0.1)
										{
											poly1Vec->erase(iterClosedPolys);
										}
										else
										{
											iterClosedPolys++;
										}
									}
									
									iterClosedPolys = poly2Vec->begin();
									while(iterClosedPolys != poly2Vec->end())
									{
										if((*iterClosedPolys)->getArea() < 0.1)
										{
											poly2Vec->erase(iterClosedPolys);
										}
										else
										{
											iterClosedPolys++;
										}
									}
																		
									if(poly1Vec->size() > 1)
									{
										std::string message = std::string("too many polygons... (polygon1): ") + mathUtils.inttostring(poly1Vec->size());
										throw RSGISGeometryException(message);
									}
									else
									{
										polygon1 = poly1Vec->at(0);
									}
									
									if(poly2Vec->size() > 1)
									{
										std::string message = std::string("too many polygons... (polygon2): ") + mathUtils.inttostring(poly2Vec->size());
										throw RSGISGeometryException(message);
									}
									else
									{
										polygon2 = poly2Vec->at(0);
									}
									
									poly1Vec->clear();
									delete poly1Vec;
									poly2Vec->clear();
									delete poly2Vec;
									
									if((polygon1->getArea() == 0) | (polygon2->getArea() == 0))
									{
										areaError = true;
										break;
									}
								}
								else if(i == 1)
								{
									geos::geom::Polygon *snapPolygon1 = NULL;
									geos::geom::Polygon *snapPolygon2 = NULL;
									bool snapPoly1Error = false;
									bool snapPoly2Error = false;
									try
									{
										snapPolygon1 = this->snapToXYGrid(polygon1, tolerance, false, env);
									}
									catch(RSGISGeometryException &e)
									{
										snapPoly1Error = true;
									}
									try
									{
										snapPolygon2 = this->snapToXYGrid(polygon2, tolerance, false, env);
									}
									catch(RSGISGeometryException &e)
									{
										snapPoly2Error = true;
									}
									if(!snapPoly1Error & !snapPoly2Error)
									{
										delete polygon1;
										delete polygon2;
										polygon1 = snapPolygon1;
										polygon2 = snapPolygon2;
									}
									if(snapPoly1Error & snapPoly2Error)
									{
										snapError = true;
										break;
									}
									else if(snapPoly1Error)
									{
										delete polygon2;
										polygon2 = snapPolygon2;
									}
									else if(snapPoly2Error)
									{
										delete polygon1;
										polygon1 = snapPolygon1;
									}
									else
									{
										// do nothing..
									}
									
									if((polygon1->getArea() == 0) | (polygon2->getArea() == 0))
									{
										areaError = true;
										break;
									}
								}
								else if(i == 2)
								{
									
									geos::geom::Polygon *nodedPolygon1 = NULL;
									geos::geom::Polygon *nodedPolygon2 = NULL;
									bool nodedPoly1Error = false;
									bool nodedPoly2Error = false;
									try
									{
										nodedPolygon1 = this->insertIntersectNodes(polygon1, polygon2);
									}
									catch(RSGISGeometryException &e)
									{
										nodedPoly1Error = true;
									}
									try
									{
										nodedPolygon2 = this->insertIntersectNodes(polygon2, polygon1);
									}
									catch(RSGISGeometryException &e)
									{
										nodedPoly2Error = true;
									}
									
									if(!nodedPoly1Error & !nodedPoly2Error)
									{
										delete polygon1;
										delete polygon2;
										polygon1 = nodedPolygon1;
										polygon2 = nodedPolygon2;
									}
									if(nodedPoly1Error & nodedPoly2Error)
									{
										snapError = true;
										break;
									}
									else if(nodedPoly1Error)
									{
										delete polygon2;
										polygon2 = nodedPolygon2;
									}
									else if(nodedPoly2Error)
									{
										delete polygon1;
										polygon1 = nodedPolygon1;
									}
									else
									{
										// do nothing..
									}
									
									if((polygon1->getArea() == 0) | (polygon2->getArea() == 0))
									{
										areaError = true;
										break;
									}
								}
								else if(i == 3)
								{
									std::vector<geos::geom::Polygon*> *poly1Vec = new std::vector<geos::geom::Polygon*>();
									poly1Vec->push_back(polygon1);
									std::vector<geos::geom::Polygon*> *poly2Vec = new std::vector<geos::geom::Polygon*>();
									poly2Vec->push_back(polygon2);
									
									this->performMorphologicalOperation(poly1Vec, closing, 0.01, 30);
									this->performMorphologicalOperation(poly2Vec, closing, 0.01, 30);
									
									std::vector<geos::geom::Polygon*>::iterator iterClosedPolys = poly1Vec->begin();
									while(iterClosedPolys != poly1Vec->end())
									{
										if((*iterClosedPolys)->getArea() < 0.1)
										{
											poly1Vec->erase(iterClosedPolys);
										}
										else
										{
											iterClosedPolys++;
										}
									}
									
									iterClosedPolys = poly2Vec->begin();
									while(iterClosedPolys != poly2Vec->end())
									{
										if((*iterClosedPolys)->getArea() < 0.1)
										{
											poly2Vec->erase(iterClosedPolys);
										}
										else
										{
											iterClosedPolys++;
										}
									}
									
									if(poly1Vec->size() > 1)
									{
										std::string message = std::string("too many polygons... (polygon1): ") + mathUtils.inttostring(poly1Vec->size());
										//geomExport.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/errors/poly1Vec.shp", true, poly1Vec);
										throw RSGISGeometryException(message);
									}
									else
									{
										polygon1 = poly1Vec->at(0);
									}
									
									if(poly2Vec->size() > 1)
									{
										std::string message = std::string("too many polygons... (polygon2): ") + mathUtils.inttostring(poly2Vec->size());
										throw RSGISGeometryException(message);
									}
									else
									{
										polygon2 = poly2Vec->at(0);
									}
									
									poly1Vec->clear();
									delete poly1Vec;
									poly2Vec->clear();
									delete poly2Vec;
									
									if((polygon1->getArea() == 0) | (polygon2->getArea() == 0))
									{
										areaError = true;
										break;
									}
								}
								else if(i == 4)
								{
									throw RSGISGeometryException(e.what());
								}
							}
							if(!error)
							{
								break;
							}
						}
						
						if(!areaError & !snapError)
						{
							intersections = new std::vector<geos::geom::Polygon*>();
							this->retrievePolygons(geomIntersection, intersections);
							this->retrievePolygons(geomDifference1to2, intersections);
							this->retrievePolygons(geomDifference2to1, intersections);
							
							for(iterPolys2 = intersections->begin(); iterPolys2 != intersections->end(); iterPolys2++)
							{
								if(((*iterPolys2)->getArea() > 0) & ((*iterPolys2)->getNumPoints() > 3))
								{
									polygons->push_back(*iterPolys2);
								}
							}
							delete intersections;
							
							iterPolys = polygons->begin();
						}
						else if(areaError)
						{
							if(polygon1->getArea() > 0)
							{
								polygons->push_back(polygon1);
							}
							if(polygon2->getArea() > 0)
							{
								polygons->push_back(polygon2);
							}
						}
						else if(snapError)
						{
							// IGNORE Polygons.
						}
					}
					else
					{
						iterPolys++;
					}
				}
			}
		}
		catch(geos::util::TopologyException &e)
		{
			std::cout << " - Incomplete.\n";
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISGeometryException &e)
		{
			std::cout << " - Incomplete.\n";
			throw e;
		}
		
		std::cout << " - Complete.\n";
	}
	
	geos::geom::Polygon* RSGISGeometry::createHole(const geos::geom::Polygon *poly, const geos::geom::Polygon *hole)
	{
		const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		
		geos::geom::CoordinateSequence *polyCoords = poly->getExteriorRing()->getCoordinates();
		geos::geom::LinearRing *shell = geomFactory->createLinearRing(polyCoords);
		
		std::vector<geos::geom::Geometry*> *holes = new std::vector<geos::geom::Geometry*>();
		polyCoords = hole->getExteriorRing()->getCoordinates();
		holes->push_back(geomFactory->createLinearRing(polyCoords));
		
		int numHoles = poly->getNumInteriorRing();
		for(int i = 0; i < numHoles; i++)
		{
			polyCoords = poly->getInteriorRingN(i)->getCoordinates();
			holes->push_back(geomFactory->createLinearRing(polyCoords));
		}
		
		return geomFactory->createPolygon(shell, holes); 
	}
	
	geos::geom::Polygon* RSGISGeometry::insertIntersectNodes(geos::geom::Polygon *poly, geos::geom::Polygon *intersect)
	{
		geos::geom::Polygon *polygon = NULL;
		try
		{
			std::vector<geos::geom::Coordinate> *newPolyCoords = new std::vector<geos::geom::Coordinate>();
			geos::geom::CoordinateSequence *polyCoords = poly->getExteriorRing()->getCoordinates();
			geos::geom::CoordinateSequence *intersectCoords = intersect->getExteriorRing()->getCoordinates();
			std::vector<geos::geom::LineSegment*> *polyLines = new std::vector<geos::geom::LineSegment*>();
			std::vector<geos::geom::LineSegment*> *intersectLines = new std::vector<geos::geom::LineSegment*>();
			
			int numCoords = polyCoords->getSize()-1;
			for(int i = 0; i < numCoords; i++)
			{
				polyLines->push_back(new geos::geom::LineSegment(polyCoords->getAt(i), polyCoords->getAt(i+1)));
			}
			
			numCoords = intersectCoords->getSize()-1;
			for(int i = 0; i < numCoords; i++)
			{
				intersectLines->push_back(new geos::geom::LineSegment(intersectCoords->getAt(i), intersectCoords->getAt(i+1)));
			}
			
			std::vector<geos::geom::LineSegment*>::iterator iterPolyLines;
			std::vector<geos::geom::LineSegment*>::iterator iterIntersectLines;
			geos::geom::LineSegment *polyLine = NULL;
			geos::geom::LineSegment *intersectLine = NULL;
			geos::geom::Coordinate intersection;
			for(iterPolyLines = polyLines->begin(); iterPolyLines != polyLines->end(); iterPolyLines++)
			{
				polyLine = (*iterPolyLines);
				newPolyCoords->push_back(geos::geom::Coordinate(polyLine->p0.x, polyLine->p0.y, polyLine->p0.z));

                for(iterIntersectLines = intersectLines->begin(); iterIntersectLines != intersectLines->end(); iterIntersectLines++)
				{
					intersectLine = (*iterIntersectLines);
					if(polyLine->intersection(*intersectLine, intersection))
					{
						if(((intersection) != polyLine->p0) && ((intersection) != polyLine->p1))
						{
							newPolyCoords->push_back( geos::geom::Coordinate(intersection.x, intersection.y, intersection.z));
						}
					}
				}
			}
			geos::geom::Coordinate coord;
			coord = newPolyCoords->front();
			newPolyCoords->push_back(geos::geom::Coordinate(coord.x, coord.y, coord.z));

            delete polyCoords;
			delete intersectCoords;
			delete polyLines;
			delete intersectLines;
			
			const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			geos::geom::CoordinateArraySequenceFactory coordSeqFactory;

			geos::geom::CoordinateSequence *outerShellCoords = coordSeqFactory.create(newPolyCoords);
			geos::geom::LinearRing *shell = geomFactory->createLinearRing(outerShellCoords);
						
			int numHoles = poly->getNumInteriorRing();
			geos::geom::Polygon *newPolygon = NULL;

            if(numHoles > 0)
			{
				std::vector<geos::geom::Geometry*> *holes = new std::vector<geos::geom::Geometry*>();
				std::vector<geos::geom::Coordinate> *holeCoords = NULL;
				geos::geom::LinearRing *holeShell = NULL;
				
				for(int i = 0; i < numHoles; i++)
				{
					polyCoords = poly->getInteriorRingN(i)->getCoordinates();
					holeCoords = new std::vector<geos::geom::Coordinate>();
					for(unsigned int j = 0; j < polyCoords->getSize(); j++)
					{
						coord = polyCoords->getAt(j);
						holeCoords->push_back(geos::geom::Coordinate(coord.x, coord.y, coord.z));
					}
					holeShell = geomFactory->createLinearRing(coordSeqFactory.create(holeCoords));
					holes->push_back(geomFactory->createPolygon(holeShell, NULL));
				}
				newPolygon = geomFactory->createPolygon(shell, holes);
			}
			else
			{
				newPolygon = geomFactory->createPolygon(shell, NULL);
			}
			
			polygon = dynamic_cast<geos::geom::Polygon*>(newPolygon->buffer(0));
		}
		catch(geos::util::TopologyException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		return polygon;
	}
	
	double RSGISGeometry::minDistanceBetweenPoints(geos::geom::Polygon *poly)
	{
		geos::geom::CoordinateSequence *polyCoords = poly->getExteriorRing()->getCoordinates();
		int numCoords = polyCoords->size()-1;
		if(numCoords < 3)
		{
			throw RSGISGeometryException("RSGISGeometryException: Not enough coordinates to form a polygon.");
		}
		double minDistance = polyCoords->getAt(0).distance(polyCoords->getAt(numCoords));
		double distance = 0;
		for(int i = 0; i < numCoords; i++)
		{
			distance = polyCoords->getAt(i).distance(polyCoords->getAt(i+1));
			if(distance < minDistance)
			{
				minDistance = distance;
			}
		}
		return distance;
	}
	
	geos::geom::Polygon* RSGISGeometry::snapToXYGrid(geos::geom::Polygon *poly, double tolerance, bool calcTolerance, geos::geom::Envelope *env)
	{
		geos::geom::Polygon *polygon = NULL;
		try
		{
			const geos::geom::PrecisionModel *pmIn = poly->getPrecisionModel();
						
			float xMin = pmIn->makePrecise(env->getMinX());
			float yMin = pmIn->makePrecise(env->getMinY());
									
			double polyMinDistance = this->minDistanceBetweenPoints(poly);
			
			if(calcTolerance)
			{
				tolerance = polyMinDistance/2;
			}
			else if(tolerance > polyMinDistance/2)
			{
				std::cout << "WARNING: tolerance is high and might change polygon topology\n";
			}
			
			geos::geom::CoordinateArraySequence *newPolyCoords = new geos::geom::CoordinateArraySequence();
			geos::geom::CoordinateSequence *polyCoords = poly->getExteriorRing()->getCoordinates();
			int numCoords = polyCoords->size()-1;
			if(numCoords < 3)
			{
				throw RSGISGeometryException("RSGISGeometryException: Not enough coordinates to form a polygon.");
			}
			geos::geom::Coordinate coord;
			geos::geom::Coordinate newCoord;
			float x = 0;
			float y = 0;
			int xCell = 0;
			int yCell = 0;
			float invTolerance = 1/tolerance;
			for(int i = 0; i < numCoords; i++)
			{
				coord = polyCoords->getAt(i);
				x = coord.x - xMin;
				y = coord.y - yMin;
				xCell = (int)(x/tolerance);
				yCell = (int)(y/tolerance);
				
				newCoord = geos::geom::Coordinate((xMin + (xCell/invTolerance)), (yMin + (yCell/invTolerance)), coord.z);
				newPolyCoords->add(newCoord, false);				
			}
			coord = newPolyCoords->getAt(0);
			newPolyCoords->add(geos::geom::Coordinate(coord.x, coord.y, coord.z), false);
			
			const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			
			if(newPolyCoords->size() <= 3)
			{
				throw RSGISGeometryException("Too few points to create Polygon\n");
			}
			
			geos::geom::LinearRing *shellRing = geomFactory->createLinearRing(newPolyCoords);
			
			polygon = geomFactory->createPolygon(shellRing, new std::vector<geos::geom::Geometry*>());
		}
		catch(geos::util::TopologyException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		catch(geos::util::IllegalArgumentException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return polygon;
	}
	
	void RSGISGeometry::printPolygonCoords2File(geos::geom::Polygon *poly, std::string filename)
	{
        std::ofstream outTxtFile;
		outTxtFile.open(filename.c_str(), std::ios::out | std::ios::trunc);
		
		if(outTxtFile.is_open())
		{
			outTxtFile.precision(12);
			geos::geom::CoordinateSequence *coords = poly->getExteriorRing()->getCoordinates();
			outTxtFile << "POLYGON (" << coords->getSize() << ")\n";
			geos::geom::Coordinate coord;
			for(unsigned int i = 0; i < coords->getSize(); i++)
			{
				coord = coords->getAt(i);
				outTxtFile << coord.x << "," << coord.y << std::endl;
			}
		}
		outTxtFile.flush();
		outTxtFile.close();
	}
	
	void RSGISGeometry::retrievePolygons(geos::geom::Geometry *geom, std::vector<geos::geom::Polygon*> *polygons)
	{
		try
		{
			if( geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON)
			{
				geos::geom::Polygon *poly = dynamic_cast<geos::geom::Polygon*>(geom->clone());
				polygons->push_back(poly);
			}
			else if(geom->getGeometryTypeId() == geos::geom::GEOS_GEOMETRYCOLLECTION)
			{				
				geos::geom::GeometryCollection *geomCollect = dynamic_cast<geos::geom::GeometryCollection*>(geom);
				int numGeoms = geomCollect->getNumGeometries();
				geos::geom::Geometry *geom = NULL;
				for(int i = 0; i < numGeoms; i++)
				{
					geom = const_cast<geos::geom::Geometry*>(geomCollect->getGeometryN(i));
					
					this->retrievePolygons(geom, polygons);
				}
			}
			else if(geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON)
			{
				geos::geom::MultiPolygon *mPolys = dynamic_cast<geos::geom::MultiPolygon*>(geom);
				int numGeoms = mPolys->getNumGeometries();

                geos::geom::Polygon *poly = NULL;
				for(int i = 0; i < numGeoms; i++)
				{
					poly = dynamic_cast<geos::geom::Polygon*>(mPolys->getGeometryN(i)->clone());
					polygons->push_back(poly);
				}
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	void RSGISGeometry::retrieveLines(geos::geom::Geometry *geom, std::vector<geos::geom::LineString*> *lines)
	{
		try
		{			
			if( geom->getGeometryTypeId() == geos::geom::GEOS_LINESTRING)
			{
				geos::geom::LineString *line = dynamic_cast<geos::geom::LineString*>(geom->clone());
				lines->push_back(line);
			}
			else if((geom->getGeometryTypeId() == geos::geom::GEOS_GEOMETRYCOLLECTION) | 
					(geom->getGeometryTypeId() == geos::geom::GEOS_MULTILINESTRING))
			{				
				geos::geom::GeometryCollection *geomCollect = dynamic_cast<geos::geom::GeometryCollection*>(geom);
				int numGeoms = geomCollect->getNumGeometries();
				geos::geom::Geometry *geom = NULL;
				for(int i = 0; i < numGeoms; i++)
				{
					geom = const_cast<geos::geom::Geometry*>(geomCollect->getGeometryN(i));
					
					this->retrieveLines(geom, lines);
				}
			}
			else
			{
				// IGNORE.
				delete geom;
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	void RSGISGeometry::performMorphologicalOperation(std::vector<geos::geom::Polygon*> *polygons, Morphology morphology, float buffer, int curveSegments)
	{
		try
		{
			float negBuffer = buffer * (-1);
			if(morphology == opening)
			{
				// dilation and then erosion
				std::vector<geos::geom::Polygon*> *tmpPolygons = new std::vector<geos::geom::Polygon*>();
				std::vector<geos::geom::Polygon*>::iterator iterPolys;
				geos::geom::Geometry *geom = NULL;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					if((*iterPolys)->getArea() > 0.1)
					{
						try
						{
							geom = (*iterPolys)->buffer(negBuffer);
							this->retrievePolygons(geom, tmpPolygons);
							delete geom;
						}
						catch(geos::util::TopologyException &e)
						{
							// IGNORE
							std::cout << "WARNING: Possible data lose error being ignored. " << e.what() << std::endl;
						}
					}
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				polygons->clear();
				
				for(iterPolys = tmpPolygons->begin(); iterPolys != tmpPolygons->end(); )
				{
					if((*iterPolys)->getArea() > 0.1)
					{
						try
						{
							geom = (*iterPolys)->buffer(buffer);
							this->retrievePolygons(geom, polygons);
							delete geom;
						}
						catch(geos::util::TopologyException &e)
						{
							// IGNORE
							std::cout << "WARNING: Possible data lose error being ignored. " << e.what() << std::endl;
						}
					}
					delete *iterPolys;
					tmpPolygons->erase(iterPolys);
				}
				delete tmpPolygons;
			}
			else if(morphology == closing)
			{
				// erosion and then dilation
				std::vector<geos::geom::Polygon*> *tmpPolygons = new std::vector<geos::geom::Polygon*>();
				std::vector<geos::geom::Polygon*>::iterator iterPolys;
				geos::geom::Geometry *geom = NULL;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					if((*iterPolys)->getArea() > 0.1)
					{
						try
						{
							geom = (*iterPolys)->buffer(buffer);
							this->retrievePolygons(geom, tmpPolygons);
							delete geom;
						}
						catch(geos::util::TopologyException &e)
						{
							// IGNORE
							std::cout << "WARNING: Possible data lose error being ignored. " << e.what() << std::endl;
						}
					}
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				for(iterPolys = tmpPolygons->begin(); iterPolys != tmpPolygons->end(); )
				{
					if((*iterPolys)->getArea() > 0.1)
					{
						try
						{
							geom = (*iterPolys)->buffer(negBuffer);
							this->retrievePolygons(geom, polygons);
							delete geom;
						}
						catch(geos::util::TopologyException &e)
						{
							// IGNORE
							std::cout << "WARNING: Possible data lose error being ignored. " << e.what() << std::endl;
						}
					}
					delete *iterPolys;
					tmpPolygons->erase(iterPolys);
				}
				delete tmpPolygons;
			}
			else if(morphology == dilation)
			{
				std::vector<geos::geom::Polygon*> *tmpPolygons = new std::vector<geos::geom::Polygon*>();
				std::vector<geos::geom::Polygon*>::iterator iterPolys;
				geos::geom::Geometry *geom = NULL;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					if((*iterPolys)->getArea() > 0.1)
					{
						try
						{
							geom = (*iterPolys)->buffer(buffer);
							this->retrievePolygons(geom, tmpPolygons);
							delete geom;
						}
						catch(geos::util::TopologyException &e)
						{
							// IGNORE
							std::cout << "WARNING: Possible data lose error being ignored. " << e.what() << std::endl;
						}
					}
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				
				for(iterPolys = tmpPolygons->begin(); iterPolys != tmpPolygons->end(); )
				{
					polygons->push_back(*iterPolys);
					tmpPolygons->erase(iterPolys);
				}
				delete tmpPolygons;
			}
			else if(morphology == erosion)
			{
				std::vector<geos::geom::Polygon*> *tmpPolygons = new std::vector<geos::geom::Polygon*>();
				std::vector<geos::geom::Polygon*>::iterator iterPolys;
				geos::geom::Geometry *geom = NULL;
				for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
				{
					if((*iterPolys)->getArea() > 0.1)
					{
						try
						{
							geom = (*iterPolys)->buffer(negBuffer);
							this->retrievePolygons(geom, tmpPolygons);
							delete geom;
						}
						catch(geos::util::TopologyException &e)
						{
							// IGNORE
							std::cout << "WARNING: Possible data lose error being ignored. " << e.what() << std::endl;
						}
					}
					delete *iterPolys;
					polygons->erase(iterPolys);
				}
				
				for(iterPolys = tmpPolygons->begin(); iterPolys != tmpPolygons->end(); )
				{
					polygons->push_back(*iterPolys);
					tmpPolygons->erase(iterPolys);
				}
				delete tmpPolygons;
			}
			else
			{
				throw RSGISGeometryException("Unknown Morphological Operation");
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}	
	}
	
	void RSGISGeometry::performMorphologicalOperation(std::vector<geos::geom::Geometry*> *geometries, Morphology morphology, float buffer, int curveSegments)
	{
		try
		{
			float negBuffer = buffer * (-1);
			if(morphology == opening)
			{
				// dilation and then erosion
				std::vector<geos::geom::Geometry*> *tmpGeometries = new std::vector<geos::geom::Geometry*>();
				std::vector<geos::geom::Geometry*>::iterator iterGeom;
				for(iterGeom = geometries->begin(); iterGeom != geometries->end(); )
				{
					tmpGeometries->push_back((*iterGeom)->buffer(negBuffer));
					delete *iterGeom;
					geometries->erase(iterGeom);
				}
				for(iterGeom = tmpGeometries->begin(); iterGeom != tmpGeometries->end(); )
				{
					geometries->push_back((*iterGeom)->buffer(buffer));
					delete *iterGeom;
					tmpGeometries->erase(iterGeom);
				}
				delete tmpGeometries;
			}
			else if(morphology == closing)
			{
				// erosion and then dilation
				std::vector<geos::geom::Geometry*> *tmpGeometries = new std::vector<geos::geom::Geometry*>();
				std::vector<geos::geom::Geometry*>::iterator iterGeom;
				for(iterGeom = geometries->begin(); iterGeom != geometries->end(); )
				{
					tmpGeometries->push_back((*iterGeom)->buffer(buffer));
					delete *iterGeom;
					geometries->erase(iterGeom);
				}
				for(iterGeom = tmpGeometries->begin(); iterGeom != tmpGeometries->end(); )
				{
					geometries->push_back((*iterGeom)->buffer(negBuffer));
					delete *iterGeom;
					tmpGeometries->erase(iterGeom);
				}
				delete tmpGeometries;
			}
			else if(morphology == dilation)
			{
				std::vector<geos::geom::Geometry*> *tmpGeometries = new std::vector<geos::geom::Geometry*>();
				std::vector<geos::geom::Geometry*>::iterator iterGeom;
				for(iterGeom = geometries->begin(); iterGeom != geometries->end(); )
				{
					tmpGeometries->push_back((*iterGeom)->buffer(buffer));
					delete *iterGeom;
					geometries->erase(iterGeom);
				}
				for(iterGeom = tmpGeometries->begin(); iterGeom != tmpGeometries->end(); )
				{
					geometries->push_back(*iterGeom);
					tmpGeometries->erase(iterGeom);
				}
				delete tmpGeometries;
			}
			else if(morphology == erosion)
			{
				std::vector<geos::geom::Geometry*> *tmpGeometries = new std::vector<geos::geom::Geometry*>();
				std::vector<geos::geom::Geometry*>::iterator iterGeom;
				for(iterGeom = geometries->begin(); iterGeom != geometries->end(); )
				{
					tmpGeometries->push_back((*iterGeom)->buffer(negBuffer));
					delete *iterGeom;
					geometries->erase(iterGeom);
				}
				for(iterGeom = tmpGeometries->begin(); iterGeom != tmpGeometries->end(); )
				{
					geometries->push_back(*iterGeom);
					tmpGeometries->erase(iterGeom);
				}
				delete tmpGeometries;
			}
			else
			{
				throw RSGISGeometryException("Unknown Morphological Operation");
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}	
	}
	
	double RSGISGeometry::get3DlineLenght(geos::geom::LineSegment *inLine)
	{
		geos::geom::Coordinate *startCoord = new geos::geom::Coordinate(inLine->p0);
		geos::geom::Coordinate *endCoord = new geos::geom::Coordinate(inLine->p1);
		
		double deltaX = endCoord->x - startCoord->x;
		double deltaY = endCoord->y - startCoord->y;
		double deltaZ = endCoord->z - startCoord->z;		
		double lineLenght = sqrt((deltaX*deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));
		
		delete startCoord;
		delete endCoord;
		
		return lineLenght;
	}
	
	double RSGISGeometry::get3DLineZenithAngle(geos::geom::LineSegment *inLine)
	{
		geos::geom::Coordinate *startCoord = new geos::geom::Coordinate(inLine->p0);
		geos::geom::Coordinate *endCoord = new geos::geom::Coordinate(inLine->p1);
		
		double deltaX = endCoord->x - startCoord->x;
		double deltaY = endCoord->y - startCoord->y;
		double deltaZ = endCoord->z - startCoord->z;		
		double r = sqrt((deltaX*deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));
		double phi = acos (deltaZ / r);

		delete startCoord;
		delete endCoord;
		
		return phi;
	}
	
	double RSGISGeometry::get3DlineAzimuthAngle(geos::geom::LineSegment *inLine)
	{
		geos::geom::Coordinate *startCoord = new geos::geom::Coordinate(inLine->p0);
		geos::geom::Coordinate *endCoord = new geos::geom::Coordinate(inLine->p1);
		
		double deltaX = endCoord->x - startCoord->x;
		double deltaY = endCoord->y - startCoord->y;
		double theta = atan(deltaY / deltaX);
		
		delete startCoord;
		delete endCoord;
		
		return theta;
	}
	
	double RSGISGeometry::get3DLineZenithAngleDeg(geos::geom::LineSegment *inLine)
	{
		double pi = 3.141592653589793;
		geos::geom::Coordinate *startCoord = new geos::geom::Coordinate(inLine->p0);
		geos::geom::Coordinate *endCoord = new geos::geom::Coordinate(inLine->p1);
		
		double deltaX = endCoord->x - startCoord->x;
		double deltaY = endCoord->y - startCoord->y;
		double deltaZ = endCoord->z - startCoord->z;		
		double r = sqrt((deltaX*deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));
		double phi = acos (deltaZ / r);
		double phiDeg = (phi / pi) * 180;
		delete startCoord;
		delete endCoord;
		
		return phiDeg;
	}
	
	double RSGISGeometry::get3DlineAzimuthAngleDeg(geos::geom::LineSegment *inLine)
	{
		double pi = 3.141592653589793;
		geos::geom::Coordinate *startCoord = new geos::geom::Coordinate(inLine->p0);
		geos::geom::Coordinate *endCoord = new geos::geom::Coordinate(inLine->p1);
		
		double deltaX = endCoord->x - startCoord->x;
		double deltaY = endCoord->y - startCoord->y;
		double theta = atan(deltaY / deltaX);
		double thetaDeg = (theta / pi) * 180;
		
		
		delete startCoord;
		delete endCoord;
		
		return thetaDeg;
	}
	
	std::vector<geos::geom::Polygon*>* RSGISGeometry::polygonsBasicClean(std::vector<geos::geom::Polygon*> *polygons)
	{
		std::vector<geos::geom::Polygon*> *outPolygons = new std::vector<geos::geom::Polygon*>();
		std::vector<geos::geom::Polygon*> *tmpPolys = new std::vector<geos::geom::Polygon*>();
		try
		{
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			geos::geom::Geometry *geom = NULL;
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); )
			{
				if((*iterPolys)->getArea() < 0.1)
				{
					polygons->erase(iterPolys);
				}
				else
				{
					try
					{
						geom = (*iterPolys)->buffer(0.001);
						this->retrievePolygons(geom, tmpPolys);
					}
					catch(geos::util::TopologyException &e)
					{
						// IGNORE
						std::cout << "WARNING: Possible data lose error being ignored. " << e.what() << std::endl;
					}
					iterPolys++;
				}
			}
			for(iterPolys = tmpPolys->begin(); iterPolys != tmpPolys->end(); )
			{
				if((*iterPolys)->getArea() < 0.1)
				{
					tmpPolys->erase(iterPolys);
				}
				else
				{
					try
					{
						geom = (*iterPolys)->buffer(-0.001);
						this->retrievePolygons(geom, outPolygons);
					}
					catch(geos::util::TopologyException &e)
					{
						// IGNORE
						std::cout << "WARNING: Possible data lose error being ignored. " << e.what() << std::endl;
					}
					iterPolys++;
				}
			}
			
			tmpPolys->clear();
			delete tmpPolys;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return outPolygons;
	}
	
	std::vector<geos::geom::Polygon*>* RSGISGeometry::polygonsSnapToXYGrid(std::vector<geos::geom::Polygon*> *polygons, float tolerance)
	{
		std::vector<geos::geom::Polygon*> *snappedPolygons = new std::vector<geos::geom::Polygon*>();
		std::vector<geos::geom::Polygon*> *outPolygons = NULL;
		try
		{
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			geos::geom::Envelope *env = new geos::geom::Envelope();
			geos::geom::Envelope *envTmp = NULL;
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
			{
				envTmp = this->getEnvelope(*iterPolys);
				env->expandToInclude(envTmp);
				delete envTmp;
			}
			
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
			{
				snappedPolygons->push_back(this->snapToXYGrid(*iterPolys, tolerance, false, env));
			}
			
			outPolygons = this->polygonsBasicClean(snappedPolygons);
			snappedPolygons->clear();
			delete snappedPolygons;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return outPolygons;
	}
	
	geos::geom::Geometry* RSGISGeometry::getIntersection(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2)
	{
		if(poly1 == NULL)
		{
			throw RSGISGeometryException("Polygon 1 is NULL.");
		}
		
		if(poly2 == NULL)
		{
			throw RSGISGeometryException("Polygon 2 is NULL.");
		}
		
		float tolerance = 0.25;
		geos::geom::Geometry *geom = NULL;
		try
		{
			bool areaError = false;
			bool error = false;	
			for(int i = 0; i < 4; i++)
			{
				try
				{
					geom = poly1->intersection(poly2);
				}
				catch(geos::util::TopologyException &e)
				{
					error = true;
					
					if(i == 0)
					{						
						std::cout << "WARNING: " << e.what() << std::endl;
						
						std::vector<geos::geom::Polygon*> *poly1Vec = new std::vector<geos::geom::Polygon*>();
						poly1Vec->push_back(poly1);
						std::vector<geos::geom::Polygon*> *poly2Vec = new std::vector<geos::geom::Polygon*>();
						poly2Vec->push_back(poly2);
						
						this->performMorphologicalOperation(poly1Vec, closing, 0, 30);
						this->performMorphologicalOperation(poly2Vec, closing, 0, 30);
						
						std::vector<geos::geom::Polygon*>::iterator iterClosedPolys = poly1Vec->begin();
						while(iterClosedPolys != poly1Vec->end())
						{
							if((*iterClosedPolys)->getArea() < 0.1)
							{
								poly1Vec->erase(iterClosedPolys);
							}
							else
							{
								iterClosedPolys++;
							}
						}
						
						iterClosedPolys = poly2Vec->begin();
						while(iterClosedPolys != poly2Vec->end())
						{
							if((*iterClosedPolys)->getArea() < 0.1)
							{
								poly2Vec->erase(iterClosedPolys);
							}
							else
							{
								iterClosedPolys++;
							}
						}
						
						
						if(poly1Vec->size() > 1)
						{
							throw RSGISGeometryException("too many polygons... (poly1)");
						}
						else
						{
							poly1 = poly1Vec->at(0);
						}
						
						if(poly2Vec->size() > 1)
						{
							throw RSGISGeometryException("too many polygons... (poly2)");
						}
						else
						{
							poly2 = poly2Vec->at(0);
						}
						
						poly1Vec->clear();
						delete poly1Vec;
						poly2Vec->clear();
						delete poly2Vec;
						
						if((poly1->getArea() == 0) | (poly2->getArea() == 0))
						{
							areaError = true;
							break;
						}				
					}
					else if(i == 1)
					{						
						geos::geom::Polygon *snapPolygon1 = NULL;
						geos::geom::Polygon *snapPolygon2 = NULL;
						bool snapPoly1Error = false;
						bool snapPoly2Error = false;
						geos::geom::Envelope *env = this->getEnvelope(poly1);
						geos::geom::Envelope *env2 = this->getEnvelope(poly2);
						env->expandToInclude(env2);
						delete env2;
						try
						{
							snapPolygon1 = this->snapToXYGrid(poly1, tolerance, false, env);
						}
						catch(RSGISGeometryException &e)
						{
							snapPoly1Error = true;
						}
						try
						{
							snapPolygon2 = this->snapToXYGrid(poly2, tolerance, false, env);
						}
						catch(RSGISGeometryException &e)
						{
							snapPoly2Error = true;
						}
						
						delete env;
						
						if(!snapPoly1Error & !snapPoly2Error)
						{
							delete poly1;
							delete poly2;
							poly1 = snapPolygon1;
							poly2 = snapPolygon2;
						}
						else if(snapPoly1Error & snapPoly2Error)
						{
							throw RSGISGeometryException(e.what());
						}
						else if(snapPoly1Error)
						{
							delete poly2;
							poly2 = snapPolygon2;
						}
						else if(snapPoly2Error)
						{
							delete poly1;
							poly1 = snapPolygon1;
						}
						else
						{
							// do nothing..
						}
						
						if((poly1->getArea() == 0) | (poly2->getArea() == 0))
						{
							areaError = true;
							break;
						}
					}
					else if(i == 2)
					{
						geos::geom::Polygon *nodedPolygon1 = NULL;
						geos::geom::Polygon *nodedPolygon2 = NULL;
						bool nodedPoly1Error = false;
						bool nodedPoly2Error = false;
						try
						{
							nodedPolygon1 = this->insertIntersectNodes(poly1, poly2);
						}
						catch(RSGISGeometryException &e)
						{
							nodedPoly1Error = true;
						}
						try
						{
							nodedPolygon2 = this->insertIntersectNodes(poly2, poly1);
						}
						catch(RSGISGeometryException &e)
						{
							nodedPoly2Error = true;
						}
						
						if(!nodedPoly1Error & !nodedPoly2Error)
						{
							delete poly1;
							delete poly2;
							poly1 = nodedPolygon1;
							poly2 = nodedPolygon2;
						}
						if(nodedPoly1Error & nodedPoly2Error)
						{
							throw RSGISGeometryException(e.what());
						}
						else if(nodedPoly1Error)
						{
							delete poly2;
							poly2 = nodedPolygon2;
						}
						else if(nodedPoly2Error)
						{
							delete poly1;
							poly1 = nodedPolygon1;
						}
						else
						{
							// do nothing..
						}
						
						if((poly1->getArea() == 0) | (poly2->getArea() == 0))
						{
							areaError = true;
							break;
						}
					}
					else if(i == 3)
					{
						throw RSGISGeometryException(e.what());
					}
				}
				catch(RSGISGeometryException &e)
				{
					throw e;
				}
				if(!error)
				{
					break;
				}
			}
			
			if(areaError)
			{
				if(poly1->getArea() > 0)
				{
					geom = poly1;
				}
				else if(poly2->getArea() > 0)
				{
					geom = poly2;
				}
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return geom;
	}
	
	geos::geom::Geometry* RSGISGeometry::getIntersection(geos::geom::Geometry *geom1in, geos::geom::Geometry *geom2in)
	{
		if(geom1in == NULL)
		{
			throw RSGISGeometryException("Geometry 1 is NULL.");
		}
		
		if(geom2in == NULL)
		{
			throw RSGISGeometryException("Geometry 2 is NULL.");
		}
		
		geos::geom::Geometry *geom1 = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createGeometry(geom1in);
		geos::geom::Geometry *geom2 = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory()->createGeometry(geom2in);
		
		geos::geom::Geometry *geom = NULL;
		try
		{
			bool areaError = false;
			bool error = false;	
			for(int i = 0; i < 2; i++)
			{
				try
				{
					geom = geom1->intersection(geom2);
				}
				catch(geos::util::TopologyException &e)
				{
					error = true;
					
					if(i == 0)
					{						
						std::cout << "WARNING: " << e.what() << std::endl;
						
						std::vector<geos::geom::Geometry*> *geom1Vec = new std::vector<geos::geom::Geometry*>();
						geom1Vec->push_back(geom1);
						std::vector<geos::geom::Geometry*> *geom2Vec = new std::vector<geos::geom::Geometry*>();
						geom2Vec->push_back(geom2);
						
						this->performMorphologicalOperation(geom1Vec, closing, 0, 30);
						this->performMorphologicalOperation(geom2Vec, closing, 0, 30);
						
						std::vector<geos::geom::Geometry*>::iterator iterClosedGeom = geom1Vec->begin();
						while(iterClosedGeom != geom1Vec->end())
						{
							if((*iterClosedGeom)->getArea() < 0.1)
							{
								geom1Vec->erase(iterClosedGeom);
							}
							else
							{
								iterClosedGeom++;
							}
						}
						
						iterClosedGeom = geom2Vec->begin();
						while(iterClosedGeom != geom2Vec->end())
						{
							if((*iterClosedGeom)->getArea() < 0.1)
							{
								geom2Vec->erase(iterClosedGeom);
							}
							else
							{
								iterClosedGeom++;
							}
						}
						
						if(geom1Vec->size() > 1)
						{
							throw RSGISGeometryException("too many geometries... (geom1)");
						}
						else
						{
							geom1 = geom1Vec->at(0);
						}
						
						if(geom2Vec->size() > 1)
						{
							throw RSGISGeometryException("too many geometeries... (geom2)");
						}
						else
						{
							geom2 = geom2Vec->at(0);
						}
						
						geom1Vec->clear();
						delete geom1Vec;
						geom2Vec->clear();
						delete geom2Vec;
						
						if((geom1->getArea() == 0) | (geom2->getArea() == 0))
						{
							areaError = true;
							break;
						}				
					}
					else if(i == 1)
					{
						throw RSGISGeometryException(e.what());
					}
				}
				catch(RSGISGeometryException &e)
				{
					throw e;
				}
				if(!error)
				{
					break;
				}
			}
			
			if(areaError)
			{
				if(geom1->getArea() > 0)
				{
					geom = geom1;
				}
				else if(geom2->getArea() > 0)
				{
					geom = geom2;
				}
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		delete geom1;
		delete geom2;
		
		return geom;
	}
	
	geos::geom::Geometry* RSGISGeometry::getDifference(geos::geom::Geometry *geom1in, geos::geom::Geometry *geom2in)
	{
		if(geom1in == NULL)
		{
			throw RSGISGeometryException("Geometry 1 is NULL.");
		}
		
		if(geom2in == NULL)
		{
			throw RSGISGeometryException("Geometry 2 is NULL.");
		}
		
		geos::geom::Geometry *geom1 = geom1in->clone();
		geos::geom::Geometry *geom2 = geom2in->clone();
		
		geos::geom::Geometry *geom = NULL;
		try
		{
			bool areaError = false;
			bool error = false;	
			for(int i = 0; i < 2; i++)
			{
				try
				{
					geom = geom1->difference(geom2);
				}
				catch(geos::util::TopologyException &e)
				{
					error = true;
					
					if(i == 0)
					{						
						std::cout << "WARNING: " << e.what() << std::endl;
						
						std::vector<geos::geom::Geometry*> *geom1Vec = new std::vector<geos::geom::Geometry*>();
						geom1Vec->push_back(geom1);
						std::vector<geos::geom::Geometry*> *geom2Vec = new std::vector<geos::geom::Geometry*>();
						geom2Vec->push_back(geom2);
						
						this->performMorphologicalOperation(geom1Vec, closing, 0, 30);
						this->performMorphologicalOperation(geom2Vec, closing, 0, 30);
						
						std::vector<geos::geom::Geometry*>::iterator iterClosedGeom = geom1Vec->begin();
						while(iterClosedGeom != geom1Vec->end())
						{
							if((*iterClosedGeom)->getArea() < 0.1)
							{
								geom1Vec->erase(iterClosedGeom);
							}
							else
							{
								iterClosedGeom++;
							}
						}
						
						iterClosedGeom = geom2Vec->begin();
						while(iterClosedGeom != geom2Vec->end())
						{
							if((*iterClosedGeom)->getArea() < 0.1)
							{
								geom2Vec->erase(iterClosedGeom);
							}
							else
							{
								iterClosedGeom++;
							}
						}
						
						if(geom1Vec->size() > 1)
						{
							throw RSGISGeometryException("too many geometries... (geom1)");
						}
						else
						{
							geom1 = geom1Vec->at(0);
						}
						
						if(geom2Vec->size() > 1)
						{
							throw RSGISGeometryException("too many geometeries... (geom2)");
						}
						else
						{
							geom2 = geom2Vec->at(0);
						}
						
						geom1Vec->clear();
						delete geom1Vec;
						geom2Vec->clear();
						delete geom2Vec;
						
						if((geom1->getArea() == 0) | (geom2->getArea() == 0))
						{
							areaError = true;
							break;
						}				
					}
					else if(i == 1)
					{
						throw RSGISGeometryException(e.what());
					}
				}
				catch(RSGISGeometryException &e)
				{
					throw e;
				}
				if(!error)
				{
					break;
				}
			}
			
			if(areaError)
			{
				if(geom1->getArea() > 0)
				{
					geom = geom1;
				}
				else if(geom2->getArea() > 0)
				{
					geom = geom2;
				}
			}
			
			delete geom1;
			delete geom2;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return geom;
	}
	
	geos::geom::Polygon* RSGISGeometry::polygonUnion(geos::geom::Polygon *poly1in, geos::geom::Polygon *poly2in)
	{
		float tolerance = 0.05;
		geos::geom::Polygon *poly = NULL;
		try
		{
			geos::geom::Polygon *poly1 = dynamic_cast<geos::geom::Polygon*>(poly1in->clone());
			geos::geom::Polygon *poly2 = dynamic_cast<geos::geom::Polygon*>(poly2in->clone());
			
			geos::geom::Geometry *geom = NULL;
			
			bool areaError = false;
			bool error = false;	
			for(int i = 0; i < 4; i++)
			{
				try
				{
					geom = poly1->Union(poly2);
					if(geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON)
					{
						poly = dynamic_cast<geos::geom::Polygon*>(geom);
					}
					else
					{
						std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
						this->retrievePolygons(geom, polys);
						
						std::vector<geos::geom::Polygon*>::iterator iterPolys = polys->begin();
						while(iterPolys != polys->end())
						{
							if((*iterPolys)->getArea() < 0.1)
							{
								delete *iterPolys;
								polys->erase(iterPolys);
							}
							else
							{
								iterPolys++;
							}
						}
												
						if(polys->size() == 0)
						{
							std::string message = std::string("Union of polygons did not produce a polygon: ") + std::string(geom->getGeometryType()) + std::string(" - could not resolve.");
							throw RSGISGeometryException(message.c_str());
						}
						else if(polys->size() == 1)
						{
							poly = polys->at(0);
						}
						else
						{
							//RSGISGeomTestExport vecIO;
							//vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/error_polys.shp", true, polys);
							std::vector<geos::geom::Polygon*>::iterator iterPolys;
							for(iterPolys = polys->begin(); iterPolys != polys->end(); iterPolys++)
							{
								delete *iterPolys;
								polys->erase(iterPolys);
							}
							delete polys;
							
							std::string message = std::string("Union of polygons produced multiple polygon: ") + std::string(geom->getGeometryType()) + std::string(" - could not resolve.");
							throw RSGISGeometryException(message.c_str());
						}
						polys->clear();
						delete polys;
					}
					
				}
				catch(geos::util::TopologyException &e)
				{
					error = true;

                    if(i == 0)
					{
						std::cout << "WARNING: " << e.what() << std::endl;
						
						std::vector<geos::geom::Polygon*> *poly1Vec = new std::vector<geos::geom::Polygon*>();
						poly1Vec->push_back(poly1);
						std::vector<geos::geom::Polygon*> *poly2Vec = new std::vector<geos::geom::Polygon*>();
						poly2Vec->push_back(poly2);
						
						this->performMorphologicalOperation(poly1Vec, closing, 0, 30);
						this->performMorphologicalOperation(poly2Vec, closing, 0, 30);
						
						std::vector<geos::geom::Polygon*>::iterator iterClosedPolys = poly1Vec->begin();
						while(iterClosedPolys != poly1Vec->end())
						{
							if((*iterClosedPolys)->getArea() < 0.1)
							{
								delete *iterClosedPolys;
								poly1Vec->erase(iterClosedPolys);
							}
							else
							{
								iterClosedPolys++;
							}
						}
						
						iterClosedPolys = poly2Vec->begin();
						while(iterClosedPolys != poly2Vec->end())
						{
							if((*iterClosedPolys)->getArea() < 0.1)
							{
								delete *iterClosedPolys;
								poly2Vec->erase(iterClosedPolys);
							}
							else
							{
								iterClosedPolys++;
							}
						}
						
						
						if(poly1Vec->size() > 1)
						{
							std::vector<geos::geom::Polygon*>::iterator iterPolys;
							for(iterPolys = poly1Vec->begin(); iterPolys != poly1Vec->end(); iterPolys++)
							{
								delete *iterPolys;
								poly1Vec->erase(iterPolys);
							}
							delete poly1Vec;
							
							throw RSGISGeometryException("too many polygons... (poly1)");
						}
						else
						{
							poly1 = poly1Vec->at(0);
						}
						
						if(poly2Vec->size() > 1)
						{
							//RSGISGeomTestExport geomExport;
							//geomExport.exportGEOSPolygons2SHP("/Users/pete/Temp/output/polys2.shp", true, poly2Vec);
							
							std::vector<geos::geom::Polygon*>::iterator iterPolys;
							for(iterPolys = poly2Vec->begin(); iterPolys != poly2Vec->end(); iterPolys++)
							{
								delete *iterPolys;
								poly2Vec->erase(iterPolys);
							}
							delete poly2Vec;
							throw RSGISGeometryException("too many polygons... (poly2)");
						}
						else
						{
							poly2 = poly2Vec->at(0);
						}
						
						poly1Vec->clear();
						delete poly1Vec;
						poly2Vec->clear();
						delete poly2Vec;
						
						if((poly1->getArea() == 0) | (poly2->getArea() == 0))
						{
							areaError = true;
							break;
						}				
					}
					else if(i == 1)
					{
						geos::geom::Polygon *snapPolygon1 = NULL;
						geos::geom::Polygon *snapPolygon2 = NULL;
						bool snapPoly1Error = false;
						bool snapPoly2Error = false;
						geos::geom::Envelope *env = this->getEnvelope(poly1);
						geos::geom::Envelope *env2 = this->getEnvelope(poly2);
						env->expandToInclude(env2);
						delete env2;
						try
						{
							snapPolygon1 = this->snapToXYGrid(poly1, tolerance, false, env);
						}
						catch(RSGISGeometryException &e)
						{
							snapPoly1Error = true;
						}
						try
						{
							snapPolygon2 = this->snapToXYGrid(poly2, tolerance, false, env);
						}
						catch(RSGISGeometryException &e)
						{
							snapPoly2Error = true;
						}
						
						delete env;
						
						if(!snapPoly1Error & !snapPoly2Error)
						{
							delete poly1;
							delete poly2;
							poly1 = snapPolygon1;
							poly2 = snapPolygon2;
						}
						if(snapPoly1Error & snapPoly2Error)
						{
							throw RSGISGeometryException(e.what());
						}
						else if(snapPoly1Error)
						{
							delete poly2;
							poly2 = snapPolygon2;
						}
						else if(snapPoly2Error)
						{
							delete poly1;
							poly1 = snapPolygon1;
						}
						else
						{
							// do nothing..
						}
						
						if((poly1->getArea() == 0) | (poly2->getArea() == 0))
						{
							areaError = true;
							break;
						}
					}
					else if(i == 2)
					{
						geos::geom::Polygon *nodedPolygon1 = NULL;
						geos::geom::Polygon *nodedPolygon2 = NULL;
						bool nodedPoly1Error = false;
						bool nodedPoly2Error = false;
						try
						{
							nodedPolygon1 = this->insertIntersectNodes(poly1, poly2);
						}
						catch(RSGISGeometryException &e)
						{
							nodedPoly1Error = true;
						}
						try
						{
							nodedPolygon2 = this->insertIntersectNodes(poly2, poly1);
						}
						catch(RSGISGeometryException &e)
						{
							nodedPoly2Error = true;
						}
						
						if(!nodedPoly1Error & !nodedPoly2Error)
						{
							delete poly1;
							delete poly2;
							poly1 = nodedPolygon1;
							poly2 = nodedPolygon2;
						}
						if(nodedPoly1Error & nodedPoly2Error)
						{
							throw RSGISGeometryException(e.what());
						}
						else if(nodedPoly1Error)
						{
							delete poly2;
							poly2 = nodedPolygon2;
						}
						else if(nodedPoly2Error)
						{
							delete poly1;
							poly1 = nodedPolygon1;
						}
						else
						{
							// do nothing..
						}
						
						if((poly1->getArea() == 0) | (poly2->getArea() == 0))
						{
							areaError = true;
							break;
						}
					}
					else if(i == 3)
					{
						throw RSGISGeometryException(e.what());
					}
				}
				catch(RSGISGeometryException &e)
				{
					throw e;
				}
				if(!error)
				{
					break;
				}
			}
			
			if(areaError)
			{
				if(poly1->getArea() > 0)
				{
					poly = poly1;
				}
				else if(poly2->getArea() > 0)
				{
					poly = poly2;
				}
			}
			
			delete poly1;
			delete poly2;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return poly;
	}
	
	void RSGISGeometry::mergeTouchingPolygons(std::vector<geos::geom::Polygon*> *polys)
	{
		try
		{
			std::vector<geos::geom::Polygon*>::iterator iterPolys1;
			std::vector<geos::geom::Polygon*>::iterator iterPolys2;
			
			geos::geom::Polygon *poly1 = NULL;
			geos::geom::Polygon *poly2 = NULL;
			geos::geom::Polygon *poly = NULL;
			
			bool merge = false;
			std::cout << "There are " << polys->size() << " polygons to merge\n";
			int feedback = 0;
			bool change = true;
			std::cout << "Started " << std::flush;
			while(change)
			{
				change = false;
				for(iterPolys1 = polys->begin(); iterPolys1 != polys->end();)
				{
					if(feedback == 100)
					{
						std::cout << "." << std::flush;
						feedback = 0;
					}
					++feedback;
					poly1 = *iterPolys1;
					merge = false;
					for(iterPolys2 = polys->begin(); iterPolys2 != polys->end(); ++iterPolys2)
					{
						poly2 = *iterPolys2;
						if((poly1 != poly2) && (this->overlap(poly1, poly2) > 0))//poly1->overlaps(poly2))
						{
							merge = true;
							break;
						}
					}
					
					if(merge)
					{
						poly = this->polygonUnion(poly1, poly2);
						polys->erase(iterPolys1);
						polys->erase(std::remove(polys->begin(), polys->end(), poly2));
						delete poly1;
						delete poly2;
						polys->push_back(poly);
						iterPolys1 = polys->begin();
						change = true;
					}
					else
					{
						++iterPolys1;
					}
				}
			}
			std::cout << " Complete\n";
			std::cout << "There are " << polys->size() << " following merge\n";
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	void RSGISGeometry::mergeTouchingPolygonsWithIndex(std::vector<geos::geom::Polygon*> *polys)
	{
		try
		{
			geos::index::quadtree::Quadtree *spatIdx = new geos::index::quadtree::Quadtree();
			
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
			{
				spatIdx->insert(this->getEnvelope((*iterPolys)), (*iterPolys));
			}
			
			std::vector<void*> tmpVec = std::vector<void*>();
			geos::geom::Envelope *tmpEnv = NULL;
			std::vector<geos::geom::Polygon*>::iterator iterPolys1;
			std::vector<void*>::iterator iterPolys2;
			geos::geom::Polygon *poly1 = NULL;
			geos::geom::Polygon *poly2 = NULL;
			geos::geom::Polygon *poly = NULL;
			bool merge = false;
			std::cout << "There are " << polys->size() << " polygons to merge\n";
			unsigned long feedbackCounter = 0;
			unsigned int feedback = 1000;
			bool giveFeedback;
			if(polys->size() > 10000)
			{
				feedback = polys->size()/100;
				giveFeedback = true;
			}
			else if(polys->size() > 1000)
			{
				feedback = polys->size()/10;
				giveFeedback = true;
			}
			else
			{
				giveFeedback = false;
			}
			bool change = true;
			std::cout << "Started " << std::flush;
			while(change)
			{
				change = false;
				for(iterPolys1 = polys->begin(); iterPolys1 != polys->end();)
				{
					if((giveFeedback) && (feedbackCounter > feedback))
					{
						std::cout << "." << polys->size() << "." << std::flush;
						feedbackCounter = 0;
					}
					
					poly1 = *iterPolys1;
					merge = false;
					
					tmpEnv = this->getEnvelope(poly1);
					spatIdx->query(tmpEnv, tmpVec);
					
					for(iterPolys2 = tmpVec.begin(); iterPolys2 != tmpVec.end(); ++iterPolys2)
					{
						poly2 = static_cast<geos::geom::Polygon*>((*iterPolys2));
						if((poly1 != poly2) && (this->overlap(poly1, poly2) > 0))
						{
							merge = true;
							break;
						}
					}
					tmpVec.clear();
					
					if(merge)
					{
						if(!spatIdx->remove(this->getEnvelope(poly1), poly1))
						{
							throw RSGISGeometryException("Polygon could not be removed from index");
						}
						if(!spatIdx->remove(this->getEnvelope(poly2), poly2))
						{
							throw RSGISGeometryException("Polygon could not be removed from index");
						}
						
						poly = this->polygonUnion(poly1, poly2);
						polys->erase(iterPolys1);
						polys->erase(std::remove(polys->begin(), polys->end(), poly2));
						delete poly1;
						delete poly2;
						polys->push_back(poly);
						spatIdx->insert(this->getEnvelope(poly), poly);
						//iterPolys1 = polys->begin();
						change = true;
						++feedbackCounter;
					}
					else
					{
						++iterPolys1;
					}
				}
			}
			std::cout << " Complete\n";
			std::cout << "There are " << polys->size() << " following merge\n";
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	void RSGISGeometry::mergeTouchingPolygonsForce(std::vector<geos::geom::Polygon*> *polys)
	{
		try
		{
			std::vector<geos::geom::Polygon*>::iterator iterPolys1;
			std::vector<geos::geom::Polygon*>::iterator iterPolys2;
			
			geos::geom::Polygon *poly1 = NULL;
			geos::geom::Polygon *poly2 = NULL;
			geos::geom::Polygon *poly = NULL;
			
			bool merge = false;
			bool change = true;
			while(change)
			{
				change = false;
				for(iterPolys1 = polys->begin(); iterPolys1 != polys->end();)
				{
					poly1 = *iterPolys1;
					merge = false;
					for(iterPolys2 = polys->begin(); iterPolys2 != polys->end(); ++iterPolys2)
					{
						poly2 = *iterPolys2;
						if((poly1 != poly2) && (this->overlapForce(poly1, poly2) > 0))//poly1->overlaps(poly2))
						{
							merge = true;
							break;
						}
					}
					
					if(merge)
					{
						poly = this->polygonUnion(poly1, poly2);
						polys->erase(iterPolys1);
						polys->erase(std::remove(polys->begin(), polys->end(), poly2));
						delete poly1;
						delete poly2;
						polys->push_back(poly);
						iterPolys1 = polys->begin();
						change = true;
					}
					else
					{
						++iterPolys1;
					}
				}
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	bool RSGISGeometry::shareBorder(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2)
	{
		bool shareborder = false;
		try
		{
			std::vector<geos::geom::LineSegment> *lines1 = new std::vector<geos::geom::LineSegment>();
			this->covert2LineSegments(poly1, lines1);
			
			std::vector<geos::geom::LineSegment> *lines2 = new std::vector<geos::geom::LineSegment>();
			this->covert2LineSegments(poly2, lines2);
			
			std::vector<geos::geom::LineSegment>::iterator iterLines1;
			std::vector<geos::geom::LineSegment>::iterator iterLines2;

            for(iterLines1 = lines1->begin(); iterLines1 != lines1->end(); ++iterLines1)
			{
				for(iterLines2 = lines2->begin(); iterLines2 != lines2->end(); ++iterLines2)
				{
					if( this->equalLineSegments(&(*iterLines1), &(*iterLines2), 0.1) && ((*iterLines1).getLength() != 0))
					{
						shareborder = true;
						break;
					}
				}
				if(shareborder)
				{
					break;
				}
			}
			
			
			lines1->clear();
			delete lines1;
			lines2->clear();
			delete lines2;
		}
		catch(RSGISGeometryException &e)
		{
			std::cerr << "WARNING: " << e.what() << std::endl;
			shareborder = false;
		}
		return shareborder;
	}
	
	bool RSGISGeometry::equalLineSegments(geos::geom::LineSegment *line1, geos::geom::LineSegment *line2, float threshold)
	{
		bool linesequal = false;
		try
		{
			if((line1->p0.distance(line2->p0) < threshold) &
			   (line1->p1.distance(line2->p1) < threshold))
			{
				linesequal = true;
			}
			else if((line1->p0.distance(line2->p1) < threshold) &
					(line1->p1.distance(line2->p0) < threshold))
			{
				linesequal = true;
			}
			else
			{
				linesequal = false;
			}
		}
		catch(std::exception &e)
		{
			std::cout << "WARNING: " << e.what() << std::endl;
			linesequal = false;
		}
		return linesequal;
	}
	
	bool RSGISGeometry::straightLine(RSGIS2DPoint *p1, RSGIS2DPoint *p2, RSGIS2DPoint *p3)
	{
		bool returnValue = false;
		try
		{
			if(((p1->getX() == p2->getX()) & (p2->getX() == p3->getX())) |
			   ((p1->getY() == p2->getY()) & (p2->getY() == p3->getY())))
			{
				returnValue = true;
			}
			else
			{
				// p1 and p2 y = mx + c
				
				double deltaX = p2->getX() - p1->getX();
				double deltaY = p2->getY() - p1->getY();
				
				double m = deltaY/deltaX;
				double c = p1->getY() - (p1->getX() * m);
				
				// does p3 fit on the line y = mx + c
				double y3 = (p3->getX() * m) + c;
				
				if(y3 == p3->getY())
				{
					float y1 = (p1->getX() * m) + c;
					float y2 = (p2->getX() * m) + c;
					
					std::cout << "P1: [" << p1->getX() << "," << p1->getY() << "] P2: [" << p2->getX() << "," << p2->getY() << "] P3: [" << p3->getX() << "," << p3->getY() << "]\n";
					
					std::cout << "deltaX = " << deltaX << std::endl;
					std::cout << "deltaY = " << deltaY << std::endl;
					
					std::cout << "m = " << m << std::endl;
					std::cout << "c = " << c << std::endl;
					
					std::cout << "y1 = " << y1 << std::endl;
					std::cout << "y2 = " << y2 << std::endl;
					std::cout << "y3 = " << y3 << std::endl;
					returnValue = true;
				}
				else
				{
					returnValue = false;
				}
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		return returnValue;
	}
	
	float RSGISGeometry::amountOfOverlap(geos::geom::Polygon *poly, std::vector<geos::geom::Polygon*> *polys)
	{
		bool first = true;
		float maxOverlap = 0;
		float overlap = 0;
		std::vector<geos::geom::Polygon*>::iterator iterPolys;
		for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys)
		{
			if(poly->within(*iterPolys))
			{
				maxOverlap = 1;
				break;
			}
			else if(poly->overlaps(*iterPolys))
			{
				geos::geom::Geometry *diffGeom = this->getDifference(poly, *iterPolys);
				overlap = diffGeom->getArea()/poly->getArea();
				if(overlap > 1)
				{
					throw RSGISGeometryException("Overlap is greater than 1. Polygon probably has area = 0.");
				}
				delete diffGeom;
			}
			else
			{
				overlap = 0;
			}
			
			if(first)
			{
				maxOverlap = overlap;
				first = false;
			}
			else if(overlap > maxOverlap)
			{
				maxOverlap = overlap;
			}
		}
		
		
		return maxOverlap;
	}
	
	void RSGISGeometry::mergeTouchingPolygons(std::vector<geos::geom::Polygon*> *polys, float sizeThreshold)
	{
		try
		{
			std::vector<geos::geom::Polygon*>::iterator iterPolys1;
			std::vector<geos::geom::Polygon*>::iterator iterPolys2;
			
			geos::geom::Polygon *poly1 = NULL;
			geos::geom::Polygon *poly2 = NULL;
			geos::geom::Polygon *poly = NULL;
			
			bool merge = false;
			std::cout << "There are " << polys->size() << " polygons to merge\n";
			int feedback = 0;
			bool change = true;
			std::cout << "Started " << std::flush;
			while(change)
			{
				change = false;
				for(iterPolys1 = polys->begin(); iterPolys1 != polys->end();)
				{
					if(feedback == 100)
					{
						std::cout << "." << std::flush;
						feedback = 0;
					}
					++feedback;
					poly1 = *iterPolys1;
					merge = false;
					if(poly1->getArea() < sizeThreshold)
					{
						for(iterPolys2 = polys->begin(); iterPolys2 != polys->end(); ++iterPolys2)
						{
							poly2 = *iterPolys2;
							if((poly1 != poly2) && (this->shareBorder(poly1, poly2)))
							{
								merge = true;
								break;
							}
						}
					}
					
					
					if(merge)
					{
						poly = this->polygonUnion(poly1, poly2);
						polys->erase(iterPolys1);
						polys->erase(std::remove(polys->begin(), polys->end(), poly2));
						delete poly1;
						delete poly2;
						polys->push_back(poly);
						iterPolys1 = polys->begin();
						change = true;
					}
					else
					{
						++iterPolys1;
					}
				}
			}
			std::cout << " Complete\n";
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	bool RSGISGeometry::geometryBetweenFast(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2, geos::geom::Geometry *master)
	{
		bool returnVal = false;
		try
		{
			std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
			polys->push_back(poly1);
			polys->push_back(poly2);
			
			geos::geom::Polygon *ch = this->findConvexHull(polys);

			if(ch->intersects(master))
			{
				returnVal = true;
			}
			else
			{
				returnVal = false;
			}
			
			polys->clear();
			delete polys;
			delete ch;
			
		}
		catch(geos::util::TopologyException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return returnVal;
	}
	
	bool RSGISGeometry::geometryBetweenAccurate(geos::geom::Polygon *poly1, geos::geom::Polygon *poly2, geos::geom::Geometry *master)
	{
		bool returnVal = false;
		try
		{
			std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
			polys->push_back(poly1);
			polys->push_back(poly2);
			
			geos::geom::Polygon *ch = this->findConvexHull(polys);
			
			if(ch->intersects(master))
			{
				returnVal = true;
			}
			else
			{
				returnVal = false;
			}
			
			delete ch;
			
		}
		catch(geos::util::TopologyException &e)
		{
			returnVal = true;
			std::cout << "WARNING: " << e.what() << std::endl;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return returnVal;
	}
	
	geos::geom::Polygon* RSGISGeometry::addNodes(geos::geom::Polygon *poly, int nodeStep)
	{
		RSGISGeometry geomUtils;
		geos::geom::Polygon *outPoly = NULL;
		try
		{
			geos::geom::CoordinateSequence *inCoords = poly->getExteriorRing()->getCoordinates();
			geos::geom::CoordinateSequence *coords = new geos::geom::CoordinateArraySequence();
			geos::geom::Coordinate coord = geos::geom::Coordinate(0,0,0);
			geos::geom::Coordinate coordN = geos::geom::Coordinate(0,0,0);
			float distance = 0;
			for(unsigned int i = 0; i < inCoords->size()-1; i++)
			{
				coord = inCoords->getAt(i);
				coordN = inCoords->getAt(i+1);
				distance = coord.distance(coordN);
				coords->add(geos::geom::Coordinate(coord.x, coord.y, coord.z));
				if(distance > nodeStep)
				{
					geomUtils.findPointOnLine(&coord, &coordN, nodeStep, coords);
				}
				coords->add(geos::geom::Coordinate(coordN.x, coordN.y, coordN.z));
			}
			delete inCoords;
			
			const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			geos::geom::LinearRing *ring = geomFactory->createLinearRing(coords);
			outPoly = geomFactory->createPolygon(ring, NULL);
		}
		catch(RSGISGeometryException &e)
		{
		      throw e;
		}
		return outPoly;
	}
	
	std::vector<geos::geom::LineSegment*>* RSGISGeometry::findLineProj(geos::geom::Polygon *poly)
	{
		std::vector<geos::geom::LineSegment*> *lines = new std::vector<geos::geom::LineSegment*>();
		
		try
		{
			geos::geom::CoordinateSequence *coordsSeq = poly->getExteriorRing()->getCoordinates();
			geos::geom::Coordinate start;
			geos::geom::Coordinate end;
			geos::geom::Coordinate *currentE = NULL;
			float lineLength = poly->getLength();
			int numPts = coordsSeq->getSize();
			int lastIndex = (numPts-1);
			
			for(int i = 0; i < numPts; ++i)
			{
				if(i == lastIndex)
				{
					coordsSeq->getAt(i,start);
					coordsSeq->getAt(0, end);
				}
				else
				{
					coordsSeq->getAt(i,start);
					coordsSeq->getAt(i+1, end);
				}
				this->findPoint2Side(&start, &start, &end, lineLength, currentE);
								
				lines->push_back(new geos::geom::LineSegment(geos::geom::Coordinate(start.x, start.y, start.z), *currentE));
			}
			
			delete coordsSeq;			
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return lines;
	}
	
	std::vector<geos::geom::LineSegment*>* RSGISGeometry::findLineProj(std::vector<geos::geom::Coordinate*> *coords, float lineLength)
	{
		std::vector<geos::geom::LineSegment*> *lines = new std::vector<geos::geom::LineSegment*>();
		
		try
		{
			geos::geom::Coordinate *start;
			geos::geom::Coordinate *end;
			geos::geom::Coordinate *currentE;
			int numPts = coords->size();
			int lastIndex = (numPts-1);
			
			for(int i = 0; i < numPts; ++i)
			{
				if(i == lastIndex)
				{
					start = coords->at(i);
					end = coords->at(0);
				}
				else
				{
					start = coords->at(i);
					end = coords->at(i+1);
				}
				
				currentE = new geos::geom::Coordinate();
				this->findPoint2Side(start, start, end, lineLength, currentE);
				lines->push_back(new geos::geom::LineSegment(geos::geom::Coordinate(start->x, start->y, start->z), *currentE));
				delete currentE;
			}
			
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return lines;
	}
	
	void RSGISGeometry::removeNeighborDuplicates(std::vector<geos::geom::Coordinate*> *coords)
	{
		geos::geom::Coordinate *prev = NULL;
		geos::geom::Coordinate *current = NULL;
		std::vector<geos::geom::Coordinate*>::iterator iterCoords;
		double dx = 0;
		double dy = 0;
		bool first = true;
		for(iterCoords = coords->begin(); iterCoords != coords->end();)
		{
			current = *iterCoords;
			if(first)
			{
				prev = coords->back();
				first = false;
			}
			
			
			dx = prev->x - current->x;
			dy = prev->y - current->y;
			
			if((dx == 0) & (dy == 0))
			{
				delete *iterCoords;
				coords->erase(iterCoords);
			}
			else
			{
				prev = *iterCoords;
				++iterCoords;
			}
		}
	}
	
	float RSGISGeometry::overlap(geos::geom::Geometry *geom1, geos::geom::Geometry *geom2)
	{
		float areaOverlap = 0;
		try 
		{
			if(geom1->overlaps(geom2))
			{
				geos::geom::Geometry *geom = this->getIntersection(geom1, geom2);
				areaOverlap = geom->getArea();
				delete geom;
			}
			else 
			{
				areaOverlap = 0;
			}

		}
		catch (RSGISGeometryException &e) 
		{
			throw e;
		}

        return areaOverlap;
	}
	
	float RSGISGeometry::overlapForce(geos::geom::Geometry *geom1, geos::geom::Geometry *geom2)
	{
		float areaOverlap = 0;
		try 
		{
			if(geom1->intersects(geom2))
			{
				geos::geom::Geometry *geom = this->getIntersection(geom1, geom2);
				areaOverlap = geom->getArea();
				delete geom;
			}
			else 
			{
				areaOverlap = 0;
			}
			
		}
		catch (RSGISGeometryException &e) 
		{
			// If an exception is caught, continue and set areaOverlap to 1
			areaOverlap = 1;
		}
		return areaOverlap;
	}
	
	geos::geom::Polygon* RSGISGeometry::removeHoles(geos::geom::Polygon* poly)
	{
		const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
		const geos::geom::LineString *exteriorRing = poly->getExteriorRing();			
		geos::geom::CoordinateSequence *coordSeq = exteriorRing->getCoordinates();
		geos::geom::LinearRing *linearRingShell = new geos::geom::LinearRing(coordSeq, geosGeomFactory);
		geos::geom::Polygon *polygonGeom = geosGeomFactory->createPolygon(linearRingShell, NULL); 
		return polygonGeom;
	}
    
    std::vector<geos::geom::Polygon*>* RSGISGeometry::removeHoles(std::vector<geos::geom::Polygon*> *polys)
    {
        std::vector<geos::geom::Polygon*> *polysOut = new std::vector<geos::geom::Polygon*>();
        
        geos::geom::Polygon *poly;
        for(std::vector<geos::geom::Polygon*>::iterator iterPoly = polys->begin(); iterPoly != polys->end(); ++iterPoly)
        {
            poly = this->removeHoles(*iterPoly);
            if(poly != NULL)
            {
                polysOut->push_back(poly);
            }
        }
        
        return polysOut;
    }
    
    void RSGISGeometry::convertGeometryCollection2Lines(OGRGeometryCollection *geomCollectionIn, OGRGeometryCollection *geomCollectionOut)
    {
        try 
        {
            OGRGeometry *geom = NULL;
            for(int i = 0; i < geomCollectionIn->getNumGeometries(); ++i)
            {
                geom = geomCollectionIn->getGeometryRef(i);
                OGRwkbGeometryType geomType = geom->getGeometryType();
                if(geomType == wkbPolygon)
                {
                    this->convertPolygons2Lines((OGRPolygon*)geom, geomCollectionOut);
                }
                else if(geomType == wkbMultiPolygon)
                {
                    OGRMultiPolygon *mutliPoly = (OGRMultiPolygon*)geom;
                    for(int j = 0; j < mutliPoly->getNumGeometries(); ++j)
                    {
                        this->convertPolygons2Lines((OGRPolygon*)mutliPoly->getGeometryRef(j), geomCollectionOut);
                    }
                }
                else
                {
                    geomCollectionOut->addGeometry(geom);
                }
            }
        }
        catch (RSGISGeometryException &e) 
        {
            throw e;
        }
        catch (std::exception &e) 
        {
            throw RSGISGeometryException(e.what());
        }
    }
    
    void RSGISGeometry::convertPolygons2Lines(OGRPolygon *poly, OGRGeometryCollection *geomCollectionOut)
    {
        try 
        {
            OGRPoint *pt1 = new OGRPoint();
            OGRPoint *pt2 = new OGRPoint();
            OGRLinearRing *ring = poly->getExteriorRing();
            for(int i = 1; i < ring->getNumPoints(); ++i)
            {
                if(i == ring->getNumPoints()-1)
                {
                    ring->getPoint(i, pt1);
                    ring->getPoint(0, pt2);
                }
                else
                {
                    ring->getPoint(i, pt1);
                    ring->getPoint(i+1, pt2);
                }
                
                OGRLineString *line = new OGRLineString();
                line->addPoint(pt1->getX(), pt1->getY(), 0);
                line->addPoint(pt2->getX(), pt2->getY(), 0);
                geomCollectionOut->addGeometryDirectly(line);
            }
            
            delete pt1;
            delete pt2;
        }
        catch (RSGISGeometryException &e) 
        {
            throw e;
        }
        catch (std::exception &e) 
        {
            throw RSGISGeometryException(e.what());
        }
    }
    
    std::vector<geos::geom::Polygon*>* RSGISGeometry::simplifyPolygons(std::vector<geos::geom::Polygon*> *polys, double distTol)
    {
        std::vector<geos::geom::Geometry*> *polyAsGeoms = new std::vector<geos::geom::Geometry*>();
        
        for(std::vector<geos::geom::Polygon*>::iterator iterPoly = polys->begin(); iterPoly != polys->end(); ++iterPoly)
        {
            polyAsGeoms->push_back(*iterPoly);
        }
        
        const geos::geom::GeometryFactory* geosGeomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
        geos::geom::MultiPolygon* mPoly = geosGeomFactory->createMultiPolygon(polyAsGeoms);
        
        geos::simplify::DouglasPeuckerSimplifier simplifier(mPoly);
        simplifier.setDistanceTolerance(distTol);
        geos::geom::Geometry *outGeom = simplifier.getResultGeometry().get();
        
        std::vector<geos::geom::Polygon*> *outPolys = new std::vector<geos::geom::Polygon*>();
        this->retrievePolygons(outGeom, outPolys);
        
        return outPolys;
    }
    
    geos::geom::Polygon* RSGISGeometry::applyOffset2Polygon(geos::geom::Polygon *poly, double xOff, double yOff)
    {
        geos::geom::Polygon *polygon = NULL;
		try
		{
			geos::geom::CoordinateArraySequence *newPolyCoords = new geos::geom::CoordinateArraySequence();
			geos::geom::CoordinateSequence *polyCoords = poly->getExteriorRing()->getCoordinates();
			int numCoords = polyCoords->size()-1;
			if(numCoords < 3)
			{
				throw RSGISGeometryException("RSGISGeometryException: Not enough coordinates to form a polygon.");
			}
			geos::geom::Coordinate coord;
			geos::geom::Coordinate newCoord;

			for(int i = 0; i < numCoords; i++)
			{
				coord = polyCoords->getAt(i);
				
				newCoord = geos::geom::Coordinate(coord.x + xOff, coord.y + yOff, coord.z);
				newPolyCoords->add(newCoord, false);				
			}
			coord = newPolyCoords->getAt(0);
			newPolyCoords->add(geos::geom::Coordinate(coord.x, coord.y, coord.z), false);
			
			const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			
			geos::geom::LinearRing *shellRing = geomFactory->createLinearRing(newPolyCoords);
			
			polygon = geomFactory->createPolygon(shellRing, new std::vector<geos::geom::Geometry*>());
		}
		catch(geos::util::TopologyException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		catch(geos::util::IllegalArgumentException &e)
		{
			throw RSGISGeometryException(e.what());
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return polygon;
    }
    
    std::vector<geos::geom::Polygon*>* RSGISGeometry::applyOffset2Polygons(std::vector<geos::geom::Polygon*> *polys, double xOff, double yOff)
    {
        std::vector<geos::geom::Polygon*> *polysOut = new std::vector<geos::geom::Polygon*>();
        
        geos::geom::Polygon *poly;
        for(std::vector<geos::geom::Polygon*>::iterator iterPoly = polys->begin(); iterPoly != polys->end(); ++iterPoly)
        {
            poly = this->applyOffset2Polygon(*iterPoly, xOff, yOff);
            if(poly != NULL)
            {
                polysOut->push_back(poly);
            }
        }
        
        return polysOut;
    }
	
	RSGISGeometry::~RSGISGeometry()
	{
		
	}
}}
