/*
 *  RSGISDelaunayTriangulation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2009.
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

#include "RSGISDelaunayTriangulation.h"

namespace rsgis{namespace geom{
	
	RSGISDelaunayTriangulation::RSGISDelaunayTriangulation(RSGIS2DPoint *a, RSGIS2DPoint *b, RSGIS2DPoint *c)
	{
		this->triangleList = new list<RSGISTriangle*>();
		RSGISTriangle *tri = new RSGISTriangle(a, b, c);
		triangleList->push_back(tri);
		this->bbox = new Envelope();
		this->bbox->expandToInclude(tri->getBBox());
		this->aOuter = a;
		this->bOuter = b;
		this->cOuter = c;
	}
	
	RSGISDelaunayTriangulation::RSGISDelaunayTriangulation(RSGISTriangle *tri)
	{
		this->triangleList = new list<RSGISTriangle*>();
		triangleList->push_back(tri);
		this->bbox = new Envelope();
		this->bbox->expandToInclude(tri->getBBox());
		this->aOuter = tri->getPointA();
		this->bOuter = tri->getPointB();
		this->cOuter = tri->getPointC();
	}
	
	RSGISDelaunayTriangulation::RSGISDelaunayTriangulation(list<RSGIS2DPoint*> *data) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		RSGISTriangle *tri = geomUtils.findBoundingTriangle(data);
		
		this->triangleList = new list<RSGISTriangle*>();
		triangleList->push_back(tri);
		
		this->bbox = new Envelope();
		this->bbox->expandToInclude(tri->getBBox());
		this->aOuter = tri->getPointA();
		this->bOuter = tri->getPointB();
		this->cOuter = tri->getPointC();
				
		int i = 0;
		int feedback = data->size()/10;
		int feedbackCounter = 0;
		
		cout << "Started" << flush;
		list<RSGIS2DPoint*>::iterator iterData;
		for(iterData = data->begin(); iterData != data->end(); ++iterData)
		{
			if((i % feedback) == 0)
			{
				cout << ".." << feedbackCounter << ".." << flush;
				feedbackCounter = feedbackCounter + 10;
			}
			this->addVertex(*iterData);
			++i;
		}
		cout << " Complete.\n";
		this->finaliseTriangulation();
	}
	
	RSGISDelaunayTriangulation::RSGISDelaunayTriangulation(vector<RSGIS2DPoint*> *data) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		RSGISTriangle *tri = geomUtils.findBoundingTriangle(data);
		
		this->triangleList = new list<RSGISTriangle*>();
		triangleList->push_back(tri);
		
		this->bbox = new Envelope();
		this->bbox->expandToInclude(tri->getBBox());
		this->aOuter = tri->getPointA();
		this->bOuter = tri->getPointB();
		this->cOuter = tri->getPointC();
		
		int i = 0;
		int feedback = data->size()/10;
		int feedbackCounter = 0;
		
		cout << "Started" << flush;
		vector<RSGIS2DPoint*>::iterator iterData;
		for(iterData = data->begin(); iterData != data->end(); ++iterData)
		{
			if((data->size() > 10) && ((i % feedback) == 0))
			{
				cout << ".." << feedbackCounter << ".." << flush;
				feedbackCounter = feedbackCounter + 10;
			}
			this->addVertex(*iterData);
			++i;
		}
		cout << " Complete.\n";
		this->finaliseTriangulation();
	}
	
	void RSGISDelaunayTriangulation::createDelaunayTriangulation(list<RSGIS2DPoint*> *data) throw(RSGISGeometryException)
	{
		RSGISMathsUtils mathsUtils;
		int i = 0;
		int feedback = data->size()/10;
		int feedbackCounter = 0;
		
		cout << "Started (" << data->size() << " nodes)" << flush;
		list<RSGIS2DPoint*>::iterator iterData;
		for(iterData = data->begin(); iterData != data->end(); ++iterData)
		{
			if((data->size() > 10) && ((i % feedback) == 0))
			{
				cout << ".." << feedbackCounter << ".." << flush;
				feedbackCounter = feedbackCounter + 10;
			}
			//cout << "Adding " << i << endl;
			//string filename = string("triangles_before_") + mathsUtils.inttostring(i);
			//this->plotTriangulationAsEdges(filename);
			this->addVertex(*iterData);
			//filename = string("triangles_after_") + mathsUtils.inttostring(i);
			//this->plotTriangulationAsEdges(filename);
			++i;
		}
		cout << " Complete.\n";
	}
	
	void RSGISDelaunayTriangulation::addVertex(RSGIS2DPoint *pt) throw(RSGISGeometryException)
	{
		RSGISGeometry geomUtils;
		try
		{
			list<RSGISTriangle*>::iterator iterTriangles;
			
			list<RSGISTriangle*> *tmpTriangles = new list<RSGISTriangle*>();
			
			RSGISTriangle *tri = NULL;
			bool foundTri = false;
			bool duplicate = false;
			
			iterTriangles = triangleList->begin();
			for(iterTriangles = triangleList->begin(); iterTriangles != triangleList->end(); )
			{
				tri = *iterTriangles;
				
				if((pt->distance(tri->getPointA()) < 0.1) | 
				   (pt->distance(tri->getPointB()) < 0.1) | 
				   (pt->distance(tri->getPointC()) < 0.1))
				{
					foundTri = false;
					duplicate = true;
					break;
				}
				else if(tri->containedCircle(pt))
				{
					foundTri = true;
					tmpTriangles->push_back(tri);
					triangleList->erase(iterTriangles++);
				}
				else 
				{
					++iterTriangles;
				}
			}
						
			if(foundTri)
			{
				list<RSGIS2DPoint*> *pts = this->getPtsClockwise(tmpTriangles, pt);
				list<RSGIS2DPoint*>::iterator iterPTS;
				/*
				cout << "PT: " << *pt << endl;
				for(iterPTS = pts->begin(); iterPTS != pts->end(); ++iterPTS)
				{
					cout << **iterPTS << endl;
				}
				*/
				bool first = true;
				RSGIS2DPoint *tmpPt = NULL;
				for(iterPTS = pts->begin(); iterPTS != pts->end(); ++iterPTS)
				{
					if(first)
					{
						first = false;
						tmpPt = *iterPTS;
					}
					else
					{
						try
						{
							if(!geomUtils.straightLine(tmpPt, *iterPTS, pt))
							{
								triangleList->push_back(new RSGISTriangle(tmpPt, *iterPTS, pt));
							}
							tmpPt = *iterPTS;
						}
						catch(RSGISGeometryException &e)
						{
							throw e;
						}
					}
				}
				try
				{
					if(!geomUtils.straightLine(pts->front(), pts->back(), pt))
					{
						triangleList->push_back(new RSGISTriangle(pts->front(),  pts->back(), pt));
					}
				}
				catch(RSGISGeometryException &e)
				{
					throw e;
				}
				delete pts;
			}
			else if(duplicate)
			{
				for(iterTriangles = tmpTriangles->begin(); iterTriangles != tmpTriangles->end(); )
				{
					triangleList->push_back(*iterTriangles);
					tmpTriangles->erase(iterTriangles++);
				}
			}
			else
			{
				//throw RSGISGeometryException("Cannot find triangle for inserted point");
			}
			
			for(iterTriangles = tmpTriangles->begin(); iterTriangles != tmpTriangles->end(); )
			{
				delete *iterTriangles;
				tmpTriangles->erase(iterTriangles++);
			}
			delete tmpTriangles;
		}
		catch(RSGISGeometryException &e)
		{
			throw e;
		}
	}
	
	void RSGISDelaunayTriangulation::finaliseTriangulation(list<RSGIS2DPoint*> *data)
	{
		list<RSGISTriangle*>::iterator iterTriangles;
		list<RSGIS2DPoint*>::iterator iterPts;
		RSGISTriangle *tri = NULL;
		RSGIS2DPoint *pt = NULL;
		Envelope *env = new Envelope();
		bool found = false;
		
		for(iterTriangles = triangleList->begin(); iterTriangles != triangleList->end(); ++iterTriangles)
		{
			tri = *iterTriangles;
			found = false;
			
			for(iterPts = data->begin(); iterPts != data->end(); iterPts++)
			{
				pt = *iterPts;
				if(tri->getPointA() == pt || 
				   tri->getPointB() == pt ||
				   tri->getPointC() == pt)
				{
					found = true;
					break;
				}
				
			}
			
			if(found)
			{
				delete tri;
				triangleList->erase(iterTriangles);
			}
			else if(tri->getPointA() == this->aOuter || 
			   tri->getPointB() == this->aOuter ||
			   tri->getPointC() == this->aOuter)
			{
				delete tri;
				triangleList->erase(iterTriangles);
			}
			else if(tri->getPointA() == this->bOuter || 
					tri->getPointB() == this->bOuter ||
					tri->getPointC() == this->bOuter)
			{
				delete tri;
				triangleList->erase(iterTriangles);
			}
			else if(tri->getPointA() == this->cOuter || 
					tri->getPointB() == this->cOuter ||
					tri->getPointC() == this->cOuter)
			{
				delete tri;
				triangleList->erase(iterTriangles);
			}
			else
			{
				env->expandToInclude(tri->getBBox());
			}
		}
		delete bbox;
		bbox = env;
	}
	
	void RSGISDelaunayTriangulation::finaliseTriangulation()
	{
		list<RSGISTriangle*>::iterator iterTriangles;
		RSGISTriangle *tri = NULL;
		Envelope *env = new Envelope();
		
		for(iterTriangles = triangleList->begin(); iterTriangles != triangleList->end(); )
		{
			tri = *iterTriangles;
			if(tri->getPointA() == this->aOuter || 
			   tri->getPointB() == this->aOuter ||
			   tri->getPointC() == this->aOuter)
			{
				delete tri;
				triangleList->erase(iterTriangles++);
			}
			else if(tri->getPointA() == this->bOuter || 
					tri->getPointB() == this->bOuter ||
					tri->getPointC() == this->bOuter)
			{
				delete tri;
				triangleList->erase(iterTriangles++);
			}
			else if(tri->getPointA() == this->cOuter || 
					tri->getPointB() == this->cOuter ||
					tri->getPointC() == this->cOuter)
			{
				delete tri;
				triangleList->erase(iterTriangles++);
			}
			else
			{
				env->expandToInclude(tri->getBBox());
				++iterTriangles;
			}
		}
		delete bbox;
		bbox = env;
	}
	
	list<RSGISTriangle*>* RSGISDelaunayTriangulation::getTriangulation()
	{
		return triangleList;
	}
	
	list<RSGIS2DPoint*>* RSGISDelaunayTriangulation::getPtsClockwise(list<RSGISTriangle*> *tris, RSGIS2DPoint *pt)
	{
		RSGISGeometry geomUtils;
		list<RSGIS2DPoint*> *pts = new list<RSGIS2DPoint*>();
		list<RSGISTriangle*>::iterator iterTriangles;	
		list<RSGIS2DPoint*>::iterator iterPTS;
		RSGISTriangle *tri = NULL;
		RSGIS2DPoint *tmpPt = NULL;
		
		double angleBaseListPt = 0;
		double angleBaseIterPt = 0;
		bool ptAdded = false;
		
		for(iterTriangles = tris->begin(); iterTriangles != tris->end(); ++iterTriangles)
		{
			tri = *iterTriangles;
			for(int i = 0; i < 3; i++)
			{
				if(i == 0)
				{
					// vertex a
					tmpPt = tri->getPointA();
				}
				else if(i == 1)
				{
					// vertex b
					tmpPt = tri->getPointB();
				}
				else if(i == 2)
				{
					// vertex c
					tmpPt = tri->getPointC();
				}
				
				if(pts->size() == 0)
				{
					pts->push_back(tmpPt);
				}
				else
				{
					//cout << "finding location in list to add point\n";
					angleBaseListPt = geomUtils.calcAngle(pt, tmpPt);
					ptAdded = false;
					for(iterPTS = pts->begin(); iterPTS != pts->end(); iterPTS++)
					{
						if(*iterPTS == tmpPt)
						{
							ptAdded = true;
							break;
						}
						else
						{
							angleBaseIterPt = geomUtils.calcAngle(pt, *iterPTS);
							if(angleBaseListPt < angleBaseIterPt)
							{
								pts->insert(iterPTS, tmpPt);
								ptAdded = true;
								break;
							}
						}
					}
					if(!ptAdded)
					{
						pts->push_back(tmpPt);
					}
				}
			}
			
		}
		return pts;
	}
	
	void RSGISDelaunayTriangulation::plotTriangulationAsEdges(string filename)
	{
		list<LineSegment> *lines = new list<LineSegment>();
		list<RSGISTriangle*>::iterator iterTriangles;
		RSGISTriangle *tri = NULL;
		for(iterTriangles = triangleList->begin(); iterTriangles != triangleList->end(); ++iterTriangles)
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
	
	RSGISDelaunayTriangulation::~RSGISDelaunayTriangulation()
	{
		list<RSGISTriangle*>::iterator iterTriangles;
		for(iterTriangles = triangleList->begin(); iterTriangles != triangleList->end(); )
		{
			delete *iterTriangles;
			triangleList->erase(iterTriangles++);
		}
		delete triangleList;
		delete bbox;
		delete aOuter;
		delete bOuter;
		delete cOuter;
	}
}}

