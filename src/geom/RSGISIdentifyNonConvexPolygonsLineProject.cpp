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
	
	vector<Polygon*>* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygons(list<RSGIS2DPoint*> **clusters, int numClusters) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		vector<Polygon*> *polys = new vector<Polygon*>();
		try
		{
			RSGISPolygon *poly = NULL;
			list<RSGIS2DPoint*>::iterator iterPts;
			vector<LineSegment> *lines = NULL;
		
			for(int n = 0; n < numClusters; n++)
			{
				lines = new vector<LineSegment>();
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
	
	vector<Polygon*>* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygons(list<RSGISPolygon*> **clusters, int numClusters) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		vector<Polygon*> *polys = new vector<Polygon*>();
		try
		{
			RSGISPolygon *poly = NULL;
			list<RSGISPolygon*>::iterator iterPolys;
			vector<LineSegment> *lines = NULL;
			
			for(int n = 0; n < numClusters; n++)
			{
				lines = new vector<LineSegment>();
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
	
	vector<Polygon*>* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygons(list<Polygon*> **clusters, int numClusters) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		vector<Polygon*> *polys = new vector<Polygon*>();
		try
		{
			Polygon *poly = NULL;
			list<Polygon*>::iterator iterPolys;
			vector<LineSegment> *lines = NULL;
			
			for(int n = 0; n < numClusters; n++)
			{
				lines = new vector<LineSegment>();
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
	
	Polygon* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygon(vector<Polygon*> *polygons) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		Polygon *poly = NULL;
		try
		{
			vector<Polygon*>::iterator iterPolys;
			vector<LineSegment> *lines = new vector<LineSegment>();
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
	
	Polygon* RSGISIdentifyNonConvexPolygonsLineProject::retrievePolygon(list<Polygon*> *polygons) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		Polygon *poly = NULL;
		try
		{
			list<Polygon*>::iterator iterPolys;
			vector<LineSegment> *lines = new vector<LineSegment>();
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
	
	Polygon* RSGISIdentifyNonConvexPolygonsLineProject::identifyIrregularBoundaryLineProj(vector<LineSegment> *lines) throw(RSGISGeometryException)
	{
		Polygon *poly = NULL;
		RSGISGeometry geomUtils;
		
		try
		{
			// Find Convex Hull
			Polygon *convexhull = geomUtils.findConvexHull(lines);
			
			
			// Find new boundary within convex hull
			vector<Coordinate*> *coordsLines = this->identifyIrregularBoundaryCoordinatesLineProj(lines, convexhull);
			
			// Create Line representing new boundary
			vector<LineSegment> *newlines = new vector<LineSegment>();
			
			int numPts = coordsLines->size()-1;
			
			for(int i = 0; i < numPts; i++)
			{
				newlines->push_back(LineSegment(*coordsLines->at(i), *coordsLines->at(i+1)));
			}
			newlines->push_back(LineSegment(*coordsLines->at(numPts), *coordsLines->at(0)));
			
			// Find new boundary within convex hull from the line
			vector<Coordinate*> *coordsPolys = this->identifyIrregularBoundaryCoordinatesLineProj(newlines, convexhull);
			
			// Create the polygon representing the new boundary
			GeometryFactory* geomFactory = RSGISGEOSFactoryGenerator::getInstance()->getFactory();
			
			CoordinateArraySequence *coordSeq = new CoordinateArraySequence();
			vector<Coordinate*>::iterator iterCoords;
			Coordinate coord;
			for(iterCoords = coordsPolys->begin(); iterCoords != coordsPolys->end(); iterCoords++)
			{
				coord = Coordinate((*iterCoords)->x, (*iterCoords)->y, (*iterCoords)->z);
				coordSeq->add(coord, false);
			}
			coord = Coordinate((coordsPolys->at(0))->x, (coordsPolys->at(0))->y, (coordsPolys->at(0))->z);
			coordSeq->add(coord, false);
			LinearRing *polyRing = new LinearRing(coordSeq, geomFactory);
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
	
	vector<Coordinate*>* RSGISIdentifyNonConvexPolygonsLineProject::identifyIrregularBoundaryCoordinatesLineProj(vector<LineSegment> *lines, Polygon *convexhull) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		vector<Coordinate*> *coords = new vector<Coordinate*>();
		try
		{
			Envelope *bbox = geomUtils.getEnvelope(convexhull);
			double lineLength = sqrt((bbox->getWidth() * bbox->getWidth()) + (bbox->getHeight() * bbox->getHeight()));
			CoordinateSequence *coordsSeq = convexhull->getExteriorRing()->getCoordinates();
			LineSegment *line = NULL;
			Coordinate start;
			Coordinate end;
			Coordinate *currentS;
			Coordinate *currentE;
			Coordinate *intersect;
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
					currentS = new Coordinate();
					currentE = new Coordinate();
					intersect = new Coordinate();
					gap = resolution*j;
					if(gap < length)
					{
						geomUtils.findPointOnLine(&start, &end, gap, currentS);
						geomUtils.findPoint2Side(&start, currentS, &end, lineLength, currentE);
						
						line = new LineSegment(*currentS, *currentE);
						
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


