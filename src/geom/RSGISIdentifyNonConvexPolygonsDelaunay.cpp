/*
 *  RSGISIdentifyNonConvexPolygonsDelaunay.cpp
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

#include "RSGISIdentifyNonConvexPolygonsDelaunay.h"
namespace rsgis{namespace geom{
	
	RSGISIdentifyNonConvexPolygonsDelaunay::RSGISIdentifyNonConvexPolygonsDelaunay() : RSGISIdentifyNonConvexPolygons()
	{

	}

	vector<Polygon*>* RSGISIdentifyNonConvexPolygonsDelaunay::retrievePolygons(list<RSGIS2DPoint*> **clusters, int numClusters) throw(RSGISGeometryException)
	{
		/*RSGISGeometry geomUtils;
		RSGISMathsUtils mathUtils;
		vector<Polygon*> *polys = new vector<Polygon*>();
		vector<Polygon*> **polysClusters = new vector<Polygon*>*[numClusters];
		try
		{
			list<RSGIS2DPoint*> *pts = new list<RSGIS2DPoint*>();
			list<RSGIS2DPoint*>::iterator iterPts;
			RSGISPolygon *poly = NULL;
			for(int n = 0; n < numClusters; n++)
			{
				polysClusters[n] = new vector<Polygon*>();
				for(iterPts = clusters[n]->begin(); iterPts != clusters[n]->end(); iterPts++)
				{
					poly = (RSGISPolygon*) (*iterPts);
					poly->getPoints(pts, n);
					polysClusters[n]->push_back(poly->getPolygon());
				}
				
			}
			
			pts->sort(greater<RSGIS2DPoint*>());
			cout << "Number of points before removing duplicates = " << pts->size() << endl;
			bool first = true;
			RSGIS2DPoint *pt_previous = NULL;
			for(iterPts = pts->begin(); iterPts != pts->end(); iterPts++)
			{
				if(first)
				{
					pt_previous = *iterPts;
					first = false;
				}
				else if(*pt_previous == **iterPts)
				{
					pts->erase(iterPts);
				}
				else
				{
					pt_previous = *iterPts;
				}
			}
			
			cout << "Number of points after removing duplicates = " << pts->size() << endl;
			
			int numPts = pts->size();
			float *x = new float[numPts];
			float *y = new float[numPts];
			float *c = new float[numPts];
			int i = 0;
			for(iterPts = pts->begin(); iterPts != pts->end(); iterPts++)
			{
				x[i] = (*iterPts)->getX();
				y[i] = (*iterPts)->getY();
				c[i] = (*iterPts)->getClassID();
				
				i++;
			}
			
			//RSGISExportForPlotting::getInstance()->export2DColourScatter("polygon_points_sorted", x, y, c, numPts);
			
			cout << "Create Delaunay Triangulation\n";
			RSGISDelaunayTriangulation *delaunayTriangulation = new RSGISDelaunayTriangulation(pts);
			delaunayTriangulation->plotTriangulationAsEdges("triangulation");
			
			list<RSGISTriangle*> *triangulation = delaunayTriangulation->getTriangulation();
			list<RSGISTriangle*>::iterator iterTri;
			RSGISTriangle *tri = NULL;
			for(iterTri = triangulation->begin(); iterTri != triangulation->end(); iterTri++)
			{
				tri = *iterTri;
				if(tri->getPointA()->getClassID() != tri->getPointB()->getClassID() |
				   tri->getPointB()->getClassID() != tri->getPointC()->getClassID() |
				   tri->getPointC()->getClassID() != tri->getPointA()->getClassID() )
				{
					triangulation->erase(iterTri);
				}
			}
			
			//delaunayTriangulation->plotTriangulationAsEdges("triangulation_classsplit");
			
			list<RSGISTriangle*> **clusterTriangles = new list<RSGISTriangle*>*[numClusters];
			for(int i = 0; i < numClusters; i++)
			{
				clusterTriangles[i] = new list<RSGISTriangle*>();
			}
			
			unsigned int zero = 0;
			unsigned int uIntNumClusters = numClusters;
			
			for(iterTri = triangulation->begin(); iterTri != triangulation->end(); iterTri++)
			{
				tri = *iterTri;
				if(tri->getPointA()->getClassID() >= zero & tri->getPointA()->getClassID() < uIntNumClusters)
				{
					clusterTriangles[tri->getPointA()->getClassID()]->push_back(tri);
				}
				else
				{
					throw RSGISGeometryException("Error classID out of numCluster bounds.");
				}
			}
			
			// Loop through clusters and send to function to have polygon produced.
			cout << "Merging Triangles to create polygons:\n";
			for(int i = 0; i < numClusters; i++)
			{
				string triangulationPlotString = string("triangulation_") + mathUtils.inttostring(i);
				
				this->plotTriangulationAsEdges(triangulationPlotString, clusterTriangles[i]);
				
				cout << "Cluster " << i << " .. (" << triangulationPlotString << ")\n";
				try
				{
					polys->push_back(this->findPolygonFromTriangles(clusterTriangles[i], polysClusters[i], i));
				}
				catch(RSGISGeometryException e)
				{
					cout << "ERROR OCCURED!!: " << e.what() << " IGNORING!" << endl;
				}
				polysClusters[i]->clear();
				cout << " Complete\n";
			}
			delete[] polysClusters;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}	*/
		throw RSGISGeometryException("NOT IMPLEMENT!");
		return NULL;
	}

	vector<Polygon*>* RSGISIdentifyNonConvexPolygonsDelaunay::retrievePolygons(list<RSGISPolygon*> **clusters, int numClusters) throw(RSGISGeometryException)
	{
		throw RSGISGeometryException("NOT IMPLEMENT!");
		return NULL;
	}
	
	vector<Polygon*>* RSGISIdentifyNonConvexPolygonsDelaunay::retrievePolygons(list<Polygon*> **clusters, int numClusters) throw(RSGISGeometryException)
	{
		vector<Polygon*> *polys = NULL;
		
		try
		{
			list<RSGIS2DPoint*> *pts = new list<RSGIS2DPoint*>();
			list<Polygon*>::iterator iterPolys;
			for(int i = 0; i < numClusters; ++i)
			{
				for(iterPolys = clusters[i]->begin(); iterPolys != clusters[i]->end(); ++iterPolys)
				{
					this->getPoints(*iterPolys, pts, i);
				}
			}
			
			polys = this->findPolygonsFromCoordinates(pts, numClusters);
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return polys;
	}

	Polygon* RSGISIdentifyNonConvexPolygonsDelaunay::retrievePolygon(vector<Polygon*> *polygons) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		Polygon *poly = NULL;
		try
		{
			if(polygons->size() == 0)
			{
				throw RSGISGeometryException("There are no polygons from which to find a new polygon.");
			}
			
			Polygon *convexHull = geomUtils.findConvexHull(polygons);
			Envelope *env = geomUtils.getEnvelope(convexHull);
			float buffer = 0;
			if(env->getWidth() > env->getHeight())
			{
				buffer = env->getWidth();
			}
			else
			{
				buffer = env->getHeight();
			}
			delete env;
			Polygon *bufferedCHBuffer = dynamic_cast<Polygon*>(convexHull->buffer(buffer));
			Polygon *bufferedCH5 = dynamic_cast<Polygon*>(convexHull->buffer(5));
			
			CoordinateSequence *coords = bufferedCHBuffer->getCoordinates();
			Coordinate coord = Coordinate(0,0,0);
			Coordinate coordN = Coordinate(0,0,0);
			float distance = 0;
			list<RSGIS2DPoint*> *pts_edge = new list<RSGIS2DPoint*>();
			for(unsigned int i = 0; i < coords->size()-1; i++)
			{
				coord = coords->getAt(i);
				coordN = coords->getAt(i+1);
				distance = coord.distance(coordN);
				if(distance > 10)
				{
					geomUtils.findPointOnLine(&coord, &coordN, 10, pts_edge);
				}
				pts_edge->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
			}
			
			coord = coords->getAt(coords->size()-1);
			coordN = coords->getAt(0);
			distance = coord.distance(coordN);
			if(distance > 10)
			{
				geomUtils.findPointOnLine(&coord, &coordN, 10, pts_edge);
			}
			pts_edge->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
			delete coords;
			
			coords = bufferedCH5->getCoordinates();
			list<RSGIS2DPoint*> *pts_edge2 = new list<RSGIS2DPoint*>();
			for(unsigned int i = 0; i < coords->size()-1; i++)
			{
				coord = coords->getAt(i);
				coordN = coords->getAt(i+1);
				distance = coord.distance(coordN);
				if(distance > 5)
				{
					geomUtils.findPointOnLine(&coord, &coordN, 5, pts_edge2);
				}
				pts_edge2->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
			}
			
			coord = coords->getAt(coords->size()-1);
			coordN = coords->getAt(0);
			distance = coord.distance(coordN);
			if(distance > 5)
			{
				geomUtils.findPointOnLine(&coord, &coordN, 5, pts_edge2);
			}
			pts_edge2->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
			delete coords;
			
			list<RSGIS2DPoint*> *pts = new list<RSGIS2DPoint*>();
			vector<Polygon*>::iterator iterPolys;
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
			{
				coords = (*iterPolys)->getCoordinates();
				for(unsigned int i = 0; i < coords->size(); i++)
				{
					coord = coords->getAt(i);
					pts->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
				}
				delete coords;
			}
			
			list<RSGIS2DPoint*>::iterator iterPTS; 
			list<RSGIS2DPoint*> *pts_edge_all = new list<RSGIS2DPoint*>();
			for(iterPTS = pts_edge->begin(); iterPTS != pts_edge->end(); ++iterPTS)
			{
				pts_edge_all->push_back(*iterPTS);
			}
			for(iterPTS = pts_edge2->begin(); iterPTS != pts_edge2->end(); ++iterPTS)
			{
				pts_edge_all->push_back(*iterPTS);
			}
			
			cout << "Create delaunay triangulation\n";
			RSGISTriangle *boundTriangle = geomUtils.findBoundingTriangle(pts_edge);
			RSGISDelaunayTriangulation *delaunayTriangulation = new RSGISDelaunayTriangulation(boundTriangle);
			delaunayTriangulation->createDelaunayTriangulation(pts_edge);
			//delaunayTriangulation->plotTriangulationAsEdges("pts1_delaunay_tri");
			delaunayTriangulation->createDelaunayTriangulation(pts);
			//delaunayTriangulation->plotTriangulationAsEdges("pts2_delaunay_tri");
			delaunayTriangulation->createDelaunayTriangulation(pts_edge2);
			//delaunayTriangulation->plotTriangulationAsEdges("pts3_delaunay_tri");
			delaunayTriangulation->finaliseTriangulation(pts_edge_all);
			//delaunayTriangulation->plotTriangulationAsEdges("final_delaunay_tri");
			poly = this->findPolygonFromTriangles(delaunayTriangulation->getTriangulation(), 0, false);
			delete delaunayTriangulation;

			pts_edge_all->clear();
			delete pts_edge_all;
			for(iterPTS = pts_edge->begin(); iterPTS != pts_edge->end(); ++iterPTS)
			{
				delete *iterPTS;
				pts_edge->erase(iterPTS);
			}
			delete pts_edge;
			for(iterPTS = pts_edge2->begin(); iterPTS != pts_edge2->end(); ++iterPTS)
			{
				delete *iterPTS;
				pts_edge2->erase(iterPTS);
			}
			delete pts_edge2;
			for(iterPTS = pts->begin(); iterPTS != pts->end(); ++iterPTS)
			{
				delete *iterPTS;
				pts->erase(iterPTS);
			}
			delete pts;
			delete convexHull;
			delete bufferedCHBuffer;
			delete bufferedCH5;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return poly;
	}
	
	Polygon* RSGISIdentifyNonConvexPolygonsDelaunay::retrievePolygon(list<Polygon*> *polygons) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		Polygon *poly = NULL;
		try
		{
			if(polygons->size() == 0)
			{
				throw RSGISGeometryException("There are no polygons from which to find a new polygon.");
			}
			
			Polygon *convexHull = geomUtils.findConvexHull(polygons);
			Envelope *env = geomUtils.getEnvelope(convexHull);
			float buffer = 0;
			if(env->getWidth() > env->getHeight())
			{
				buffer = env->getWidth();
			}
			else
			{
				buffer = env->getHeight();
			}
			delete env;
			Polygon *bufferedCHBuffer = dynamic_cast<Polygon*>(convexHull->buffer(buffer));
			Polygon *bufferedCH5 = dynamic_cast<Polygon*>(convexHull->buffer(5));
			
			CoordinateSequence *coords = bufferedCHBuffer->getCoordinates();
			Coordinate coord = Coordinate(0,0,0);
			Coordinate coordN = Coordinate(0,0,0);
			float distance = 0;
			list<RSGIS2DPoint*> *pts_edge = new list<RSGIS2DPoint*>();
			for(unsigned int i = 0; i < coords->size()-1; i++)
			{
				coord = coords->getAt(i);
				coordN = coords->getAt(i+1);
				distance = coord.distance(coordN);
				if(distance > 10)
				{
					geomUtils.findPointOnLine(&coord, &coordN, 10, pts_edge);
				}
				pts_edge->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
			}
			
			coord = coords->getAt(coords->size()-1);
			coordN = coords->getAt(0);
			distance = coord.distance(coordN);
			if(distance > 10)
			{
				geomUtils.findPointOnLine(&coord, &coordN, 10, pts_edge);
			}
			pts_edge->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
			delete coords;
			
			coords = bufferedCH5->getCoordinates();
			list<RSGIS2DPoint*> *pts_edge2 = new list<RSGIS2DPoint*>();
			for(unsigned int i = 0; i < coords->size()-1; i++)
			{
				coord = coords->getAt(i);
				coordN = coords->getAt(i+1);
				distance = coord.distance(coordN);
				if(distance > 5)
				{
					geomUtils.findPointOnLine(&coord, &coordN, 5, pts_edge2);
				}
				pts_edge2->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
			}
			
			coord = coords->getAt(coords->size()-1);
			coordN = coords->getAt(0);
			distance = coord.distance(coordN);
			if(distance > 5)
			{
				geomUtils.findPointOnLine(&coord, &coordN, 5, pts_edge2);
			}
			pts_edge2->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
			delete coords;
			
			list<RSGIS2DPoint*> *pts = new list<RSGIS2DPoint*>();
			list<Polygon*>::iterator iterPolys;
			for(iterPolys = polygons->begin(); iterPolys != polygons->end(); iterPolys++)
			{
				coords = (*iterPolys)->getCoordinates();
				for(unsigned int i = 0; i < coords->size(); i++)
				{
					coord = coords->getAt(i);
					pts->push_back(new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z)));
				}
				delete coords;
			}
			
			list<RSGIS2DPoint*>::iterator iterPTS; 
			list<RSGIS2DPoint*> *pts_edge_all = new list<RSGIS2DPoint*>();
			for(iterPTS = pts_edge->begin(); iterPTS != pts_edge->end(); ++iterPTS)
			{
				pts_edge_all->push_back(*iterPTS);
			}
			for(iterPTS = pts_edge2->begin(); iterPTS != pts_edge2->end(); ++iterPTS)
			{
				pts_edge_all->push_back(*iterPTS);
			}
			
			cout << "Create delaunay triangulation\n";
			RSGISTriangle *boundTriangle = geomUtils.findBoundingTriangle(pts_edge);
			RSGISDelaunayTriangulation *delaunayTriangulation = new RSGISDelaunayTriangulation(boundTriangle);
			delaunayTriangulation->createDelaunayTriangulation(pts_edge);
			//delaunayTriangulation->plotTriangulationAsEdges("pts1_delaunay_tri");
			delaunayTriangulation->createDelaunayTriangulation(pts);
			//delaunayTriangulation->plotTriangulationAsEdges("pts2_delaunay_tri");
			delaunayTriangulation->createDelaunayTriangulation(pts_edge2);
			//delaunayTriangulation->plotTriangulationAsEdges("pts3_delaunay_tri");
			delaunayTriangulation->finaliseTriangulation(pts_edge_all);
			//delaunayTriangulation->plotTriangulationAsEdges("final_delaunay_tri");
			poly = this->findPolygonFromTriangles(delaunayTriangulation->getTriangulation(), 0, false);
			delete delaunayTriangulation;
			
			pts_edge_all->clear();
			delete pts_edge_all;
			for(iterPTS = pts_edge->begin(); iterPTS != pts_edge->end(); ++iterPTS)
			{
				delete *iterPTS;
				pts_edge->erase(iterPTS);
			}
			delete pts_edge;
			for(iterPTS = pts_edge2->begin(); iterPTS != pts_edge2->end(); ++iterPTS)
			{
				delete *iterPTS;
				pts_edge2->erase(iterPTS);
			}
			delete pts_edge2;
			for(iterPTS = pts->begin(); iterPTS != pts->end(); ++iterPTS)
			{
				delete *iterPTS;
				pts->erase(iterPTS);
			}
			delete pts;
			delete convexHull;
			delete bufferedCHBuffer;
			delete bufferedCH5;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return poly;
	}

	vector<Polygon*>* RSGISIdentifyNonConvexPolygonsDelaunay::findPolygonsFromCoordinates(list<RSGIS2DPoint*> *pts, int numClusters) throw(RSGISGeometryException)
	{
		vector<Polygon*> *polys = new vector<Polygon*>();
		try
		{
			cout << "Create delaunay triangulation\n";
			RSGISDelaunayTriangulation *delaunayTriangulation = new RSGISDelaunayTriangulation(pts);
			
			cout << "Remove triangles across class/cluster border\n";
			list<RSGISTriangle*> *triangulation = delaunayTriangulation->getTriangulation();
			list<RSGISTriangle*>::iterator iterTri;
			RSGISTriangle *tri = NULL;
			for(iterTri = triangulation->begin(); iterTri != triangulation->end(); iterTri++)
			{
				tri = *iterTri;
				if((tri->getPointA()->getClassID() != tri->getPointB()->getClassID()) |
				   (tri->getPointB()->getClassID() != tri->getPointC()->getClassID()) |
				   (tri->getPointC()->getClassID() != tri->getPointA()->getClassID()) )
				{
					triangulation->erase(iterTri);
				}
			}
			
			list<RSGISTriangle*> **clusterTriangles = new list<RSGISTriangle*>*[numClusters];
			for(int i = 0; i < numClusters; i++)
			{
				clusterTriangles[i] = new list<RSGISTriangle*>();
			}
			
			unsigned int zero = 0;
			unsigned int uIntNumClusters = numClusters;
			
			for(iterTri = triangulation->begin(); iterTri != triangulation->end(); iterTri++)
			{
				tri = *iterTri;
				if((tri->getPointA()->getClassID() >= zero) & (tri->getPointA()->getClassID() < uIntNumClusters))
				{
					clusterTriangles[tri->getPointA()->getClassID()]->push_back(tri);
				}
				else
				{
					throw RSGISGeometryException("Error classID out of numCluster bounds.");
				}
			}
			
			cout << "Merging Triangles to create polygons:\n";
			for(int i = 0; i < numClusters; i++)
			{
				try
				{
					if(clusterTriangles[i]->size() > 0)
					{
						polys->push_back(this->findPolygonFromTriangles(clusterTriangles[i], i, true));
					}
				}
				catch(RSGISGeometryException e)
				{
					cout << "ERROR OCCURED!!: " << e.what() << " IGNORING!" << endl;
				}
			}			
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
		
		return polys;
	}
	
	Polygon* RSGISIdentifyNonConvexPolygonsDelaunay::findPolygonFromTriangles(list<RSGISTriangle*> *cluster, int id, bool tryAgain) throw(RSGISGeometryException)
	{
		Polygon *outPoly = NULL;
		RSGISGeometry geomUtils;
		RSGISMathsUtils mathUtils;
		try
		{
			// Loop through and merge all neighboring triangles.
			vector<Polygon*> *polys = new vector<Polygon*>();
			list<RSGISTriangle*>::iterator iterTri;

			for(iterTri = cluster->begin(); iterTri != cluster->end(); iterTri++)
			{
				polys->push_back((*iterTri)->createPolygon());
			}
			
			//RSGISGeomTestExport vecIO;
			//vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/alltris.shp", true, polys);
			
			geomUtils.mergeTouchingPolygons(polys);
			
			//vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/mergedtris.shp", true, polys);
			
			if(polys->size() == 1)
			{
				outPoly = polys->at(0);
				polys->erase(remove(polys->begin(), polys->end(), outPoly));
			}
			else if(polys->size() == 0)
			{
				throw RSGISGeometryException("There are no polygons from which to create a polygon.");
			}
			else
			{
				float totalarea = 0;
				float maxPolyAreaValue = 0;
				float area = 0;
				Polygon *maxAreaPoly = NULL;
				bool first = true;
				vector<Polygon*>::iterator iterPolys;
				for(iterPolys = polys->begin(); iterPolys != polys->end(); ++iterPolys )
				{
					if(first)
					{
						maxPolyAreaValue = (*iterPolys)->getArea();
						maxAreaPoly = *iterPolys;
						first = false;
						totalarea += maxPolyAreaValue;
					}
					else
					{
						area = (*iterPolys)->getArea();
						totalarea += area;
						if(area > maxPolyAreaValue)
						{
							maxAreaPoly = *iterPolys;
							maxPolyAreaValue = area;
						}
					}
				}
				
				if( maxPolyAreaValue/totalarea > 0.9)
				{
					outPoly = maxAreaPoly;
					polys->erase(remove(polys->begin(), polys->end(), outPoly));
				}
				else if(tryAgain)
				{
					try
					{
						outPoly = this->retrievePolygon(polys);
					}
					catch(RSGISGeometryException &e)
					{
						string message = string("WARNING: polys->size() = ") + mathUtils.inttostring(polys->size());
						
						for(iterPolys = polys->begin(); iterPolys != polys->end(); )
						{
							area = (*iterPolys)->getArea();
							//cout << "Polygon has area " << area << " which is " << area/totalarea << " of the total area " << totalarea << endl;
							delete *iterPolys;
							polys->erase(iterPolys);
						}
						delete polys;
						
						
						throw RSGISGeometryException(message);
					}
				}	
				else
				{
					outPoly = maxAreaPoly;
					polys->erase(remove(polys->begin(), polys->end(), outPoly));
				}
			}
			
			vector<Polygon*>::iterator iterPolys;
			for(iterPolys = polys->begin(); iterPolys != polys->end(); )
			{
				delete *iterPolys;
				polys->erase(iterPolys);
			}
			delete polys;
		}
		catch(RSGISGeometryException e)
		{
			throw e;
		}
		
		return outPoly;
	}
	
	void RSGISIdentifyNonConvexPolygonsDelaunay::plotTriangulationAsEdges(string filename, list<RSGISTriangle*> *triangleList)
	{
		list<LineSegment> *lines = new list<LineSegment>();
		list<RSGISTriangle*>::iterator iterTriangles;
		RSGISTriangle *tri = NULL;
		for(iterTriangles = triangleList->begin(); iterTriangles != triangleList->end(); iterTriangles++)
		{
			tri = *iterTriangles;
			lines->push_back(LineSegment(*tri->getPointA()->getPoint(), *tri->getPointB()->getPoint()));
			lines->push_back(LineSegment(*tri->getPointB()->getPoint(), *tri->getPointC()->getPoint()));
			lines->push_back(LineSegment(*tri->getPointC()->getPoint(), *tri->getPointA()->getPoint()));
		}
		RSGISExportForPlotting::getInstance()->export2DLines(filename, lines);
		lines->clear();
		delete lines;
	}
	
	void RSGISIdentifyNonConvexPolygonsDelaunay::getPoints(Polygon *poly, list<RSGIS2DPoint*> *pts, unsigned int classID)
	{
		if(poly != NULL)
		{
			CoordinateSequence *coords = poly->getCoordinates();
			Coordinate coord;
			RSGIS2DPoint *pt = NULL;
			for(unsigned int i = 0; i < coords->getSize(); i++)
			{
				coord = coords->getAt(i);
				pt = new RSGIS2DPoint(new Coordinate(coord.x, coord.y, coord.z));
				pt->setClassID(classID);
				pts->push_back(pt);
			}
			delete coords;
		}
	}
	
	
	RSGISIdentifyNonConvexPolygonsDelaunay::~RSGISIdentifyNonConvexPolygonsDelaunay()
	{
		
	}
}}


