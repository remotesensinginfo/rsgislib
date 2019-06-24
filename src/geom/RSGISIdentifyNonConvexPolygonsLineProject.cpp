/*
 *  RSGISIdentifyNonConvexPolygonsLineProject.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/07/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISIdentifyNonConvexPolygonsLineProject.h"

namespace rsgis{namespace geom{
	
	RSGISIdentifyNonConvexPolygonsLineProject::RSGISIdentifyNonConvexPolygonsLineProject(float resolution) : RSGISIdentifyNonConvexPolygons()
	{
		this->resolution = resolution;
	}
	
	std::vector<geos::geom::Polygon*>* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygons(std::list<RSGIS2DPoint*> **clusters, int numClusters)
	{
		RSGISGeometry geomUtils;
		std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
		try
		{
			RSGISPolygon *poly = NULL;
			std::list<RSGIS2DPoint*>::iterator iterPts;
			std::vector<geos::geom::LineSegment> *lines = NULL;
		
			for(int n = 0; n < numClusters; n++)
			{
				lines = new std::vector<geos::geom::LineSegment>();
				for(iterPts = clusters[n]->begin(); iterPts != clusters[n]->end(); iterPts++)
				{
					poly = (RSGISPolygon*) (*iterPts);
					geomUtils.covert2LineSegments(poly->getPolygon(), lines);
				}
				polys->push_back(this->identifyIrregularBoundaryLineProj(lines));
				
				lines->clear();
				delete lines;	
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}	
		return polys;
	}
	
	std::vector<geos::geom::Polygon*>* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygons(std::list<RSGISPolygon*> **clusters, int numClusters)
	{
		RSGISGeometry geomUtils;
		std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
		try
		{
			RSGISPolygon *poly = NULL;
			std::list<RSGISPolygon*>::iterator iterPolys;
			std::vector<geos::geom::LineSegment> *lines = NULL;
			
			for(int n = 0; n < numClusters; n++)
			{
				lines = new std::vector<geos::geom::LineSegment>();
				for(iterPolys = clusters[n]->begin(); iterPolys != clusters[n]->end(); iterPolys++)
				{
					poly = (*iterPolys);
					geomUtils.covert2LineSegments(poly->getPolygon(), lines);
				}
				polys->push_back(this->identifyIrregularBoundaryLineProj(lines));
				
				lines->clear();
				delete lines;	
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		return polys;
	}
	
	std::vector<geos::geom::Polygon*>* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygons(std::list<geos::geom::Polygon*> **clusters, int numClusters)
	{
		RSGISGeometry geomUtils;
		std::vector<geos::geom::Polygon*> *polys = new std::vector<geos::geom::Polygon*>();
		try
		{
			geos::geom::Polygon *poly = NULL;
			std::list<geos::geom::Polygon*>::iterator iterPolys;
			std::vector<geos::geom::LineSegment> *lines = NULL;
			
			for(int n = 0; n < numClusters; n++)
			{
				lines = new std::vector<geos::geom::LineSegment>();
				for(iterPolys = clusters[n]->begin(); iterPolys != clusters[n]->end(); iterPolys++)
				{
					poly = (*iterPolys);
					geomUtils.covert2LineSegments(poly, lines);
				}
				polys->push_back(this->identifyIrregularBoundaryLineProj(lines));
				
				lines->clear();
				delete lines;	
			}
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		return polys;
	}
	
	geos::geom::Polygon* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygon(std::vector<geos::geom::Polygon*> *polygons)
	{
		RSGISGeometry geomUtils;
		geos::geom::Polygon *poly = NULL;
		try
		{
			std::vector<geos::geom::Polygon*>::iterator iterPolys;
			std::vector<geos::geom::LineSegment> *lines = new std::vector<geos::geom::LineSegment>();
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
			{
				geomUtils.covert2LineSegments(*iterPolys, lines);
			}
			poly = this->identifyIrregularBoundaryLineProj(lines);
			
			lines->clear();
			delete lines;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return poly;
	}
	
	geos::geom::Polygon* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygon(std::list<geos::geom::Polygon*> *polygons)
	{
		RSGISGeometry geomUtils;
		geos::geom::Polygon *poly = NULL;
		try
		{
			std::list<geos::geom::Polygon*>::iterator iterPolys;
			std::vector<geos::geom::LineSegment> *lines = new std::vector<geos::geom::LineSegment>();
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
			{
				geomUtils.covert2LineSegments(*iterPolys, lines);
			}
			poly = this->identifyIrregularBoundaryLineProj(lines);
			
			lines->clear();
			delete lines;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return poly;
	}
	
	geos::geom::Polygon* RSGISIdentifyNonConvexPolygonsLineProject::identifyIrregularBoundaryLineProj(std::vector<geos::geom::LineSegment> *lines)
	{
		geos::geom::Polygon *poly = NULL;
		RSGISGeometry geomUtils;
		
		try
		{
			// Find Convex Hull
			geos::geom::Polygon *convexhull = geomUtils.findConvexHull(lines);
			
			
			// Find new boundary within convex hull
			std::vector<geos::geom::Coordinate*> *coordsLines = this->identifyIrregularBoundaryCoordinatesLineProj(lines, convexhull);
			
			// Create Line representing new boundary
			std::vector<geos::geom::LineSegment> *newlines = new std::vector<geos::geom::LineSegment>();
			
			int numPts = coordsLines->size()-1;
			
			for(int i = 0; i < numPts; i++)
			{
				newlines->push_back(geos::geom::LineSegment(*coordsLines->at(i), *coordsLines->at(i+1)));
			}
			newlines->push_back(geos::geom::LineSegment(*coordsLines->at(numPts), *coordsLines->at(0)));
			
			// Find new boundary within convex hull from the line
			std::vector<geos::geom::Coordinate*> *coordsPolys = this->identifyIrregularBoundaryCoordinatesLineProj(newlines, convexhull);
			
			// Create the polygon representing the new boundary
			const geos::geom::GeometryFactory* geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			
			geos::geom::CoordinateArraySequence *coordSeq = new geos::geom::CoordinateArraySequence();
			std::vector<geos::geom::Coordinate*>::iterator iterCoords;
			geos::geom::Coordinate coord;
			for(iterCoords = coordsPolys->begin(); iterCoords != coordsPolys->end(); iterCoords++)
			{
				coord = geos::geom::Coordinate((*iterCoords)->x, (*iterCoords)->y, (*iterCoords)->z);
				coordSeq->add(coord, false);
			}
			coord = geos::geom::Coordinate((coordsPolys->at(0))->x, (coordsPolys->at(0))->y, (coordsPolys->at(0))->z);
			coordSeq->add(coord, false);
			geos::geom::LinearRing *polyRing = new geos::geom::LinearRing(coordSeq, geomFactory);
			poly = geomFactory->createPolygon(polyRing, NULL);
			
			delete newlines;
			for(iterCoords = coordsLines->begin(); iterCoords != coordsLines->end();)
			{
				delete *iterCoords;
				coordsLines->erase(iterCoords);
			}
			delete coordsLines;
			for(iterCoords = coordsPolys->begin(); iterCoords != coordsPolys->end();)
			{
				delete *iterCoords;
				coordsPolys->erase(iterCoords);
			}
			delete coordsPolys;	
			delete convexhull;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return poly;
	}
	
	std::vector<geos::geom::Coordinate*>* RSGISIdentifyNonConvexPolygonsLineProject::identifyIrregularBoundaryCoordinatesLineProj(std::vector<geos::geom::LineSegment> *lines, geos::geom::Polygon *convexhull)
	{
		RSGISGeometry geomUtils;
		std::vector<geos::geom::Coordinate*> *coords = new std::vector<geos::geom::Coordinate*>();
		try
		{
			geos::geom::Envelope *bbox = geomUtils.getEnvelope(convexhull);
			double lineLength = sqrt((bbox->getWidth() * bbox->getWidth()) + (bbox->getHeight() * bbox->getHeight()));
			geos::geom::CoordinateSequence *coordsSeq = convexhull->getExteriorRing()->getCoordinates();
			geos::geom::LineSegment *line = NULL;
			geos::geom::Coordinate start;
			geos::geom::Coordinate end;
			geos::geom::Coordinate *currentS;
			geos::geom::Coordinate *currentE;
			geos::geom::Coordinate *intersect;
			double steps = 0;
			double length = 0;
			double gap = 0;
			int numPts = coordsSeq->getSize();
			int lastIndex = (numPts-1);
			for(int i = 0; i < numPts; i++)
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
				length = start.distance(end);
				steps = length/resolution;
				
				for(int j = 0; j < steps; j++)
				{
					currentS = new geos::geom::Coordinate();
					currentE = new geos::geom::Coordinate();
					intersect = new geos::geom::Coordinate();
					gap = resolution*j;
					if(gap < length)
					{
						geomUtils.findPointOnLine(&start, &end, gap, currentS);
						geomUtils.findPoint2Side(&start, currentS, &end, lineLength, currentE);
						
						line = new geos::geom::LineSegment(*currentS, *currentE);
						
						if(geomUtils.findClosestIntersect(lines, line, currentS, intersect))
						{
							coords->push_back(intersect);
						}
						else
						{
							delete intersect;
						}
						delete line;
					}
					else
					{
						delete intersect;
					}
					delete currentS;
					delete currentE;
				}
				
			}
			
			delete coordsSeq;
			delete bbox;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	
		
		return coords;
	}
	
	RSGISIdentifyNonConvexPolygonsLineProject::~RSGISIdentifyNonConvexPolygonsLineProject()
	{
		
	}
}}


