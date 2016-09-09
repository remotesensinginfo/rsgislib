/*
 *  RSGISGraphData.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/06/2008.
 *  Copyright 2008 Aberystwyth University. All rights reserved.
 *
 */

#include "RSGISGraphData.h"

namespace rsgis{namespace datastruct{
	
	RSGISGraphEdge::RSGISGraphEdge()
	{
		nodeA = NULL;
		nodeB = NULL;
		weight = 0;
		marked = false;
	}
	
	RSGISGraphEdge::RSGISGraphEdge(RSGISGraphVertex *nodeA, RSGISGraphVertex *nodeB, double weight)
	{
		this->nodeA = nodeA;
		this->nodeB = nodeB;
		this->weight = weight;
	}
	
	void RSGISGraphEdge::setNodeA(RSGISGraphVertex *node)
	{
		this->nodeA = node;
	}
	
	void RSGISGraphEdge::setNodeB(RSGISGraphVertex *node)
	{
		this->nodeB = node;
	}
	
	void RSGISGraphEdge::setWeight(double weight)
	{
		this->weight = weight;
	}
	
	RSGISGraphVertex* RSGISGraphEdge::getNodeA()
	{
		return nodeA;
	}
	
	RSGISGraphVertex* RSGISGraphEdge::getNodeB()
	{
		return nodeB;
	}
	
	double RSGISGraphEdge::getWeight()
	{
		return weight;
	}
	
	void RSGISGraphEdge::setMarked(bool marked)
	{
		this->marked = marked;
	}
	
	bool RSGISGraphEdge::getMarked()
	{
		return marked;
	}
	
	RSGISGraphVertex* RSGISGraphEdge::getOppositeNode(RSGISGraphVertex *node)
	{
		if(nodeA == node)
		{
			return nodeB;
		}
		return nodeA;
	}
	
	RSGISGraphVertex* RSGISGraphEdge::getNotMarkedNode()
	{
		if(!nodeA->getVisited())
		{
			return nodeA;
		}
		else if(!nodeB->getVisited())
		{
			return nodeB;
		}
		return NULL;
	}
	
	bool RSGISGraphEdge::operator==(RSGISGraphEdge *edge) const
	{
		if(edge->nodeA->getID() == this->nodeA->getID() &
			edge->nodeB->getID() == this->nodeB->getID())
		{
			return true;
		}
		return false;
	}
	
	RSGISGraphEdge::~RSGISGraphEdge()
	{
		
	}
	
	
	RSGISGraphVertex::RSGISGraphVertex(int id)
	{
		this->id = id;
		name = string("default");
		edges = new list<RSGISGraphEdge*>();
	}
	
	RSGISGraphVertex::RSGISGraphVertex(int id, string name)
	{
		this->id = id;
		this->name = name;
		edges = new list<RSGISGraphEdge*>();
	}
	
	string RSGISGraphVertex::getName()
	{
		return name;
	}
	
	int RSGISGraphVertex::getID()
	{
		return id;
	}
	
	void RSGISGraphVertex::setVisited(bool visited)
	{
		this->visited = visited;
	}
	
	bool RSGISGraphVertex::getVisited()
	{
		return visited;
	}
	
	void RSGISGraphVertex::resetEdges()
	{
		list<RSGISGraphEdge*>::iterator iterEdges;
		for(iterEdges = edges->begin(); iterEdges != edges->end(); iterEdges++)
		{
			(*iterEdges)->setMarked(false);
		}
	}
	
	RSGISGraphEdge* RSGISGraphVertex::findShortestEdgeNoVisit()
	{
		bool first = true;
		double distance = 0;
		RSGISGraphEdge *shortestEdge = NULL;
		RSGISGraphVertex *tmpVertex = NULL;
		list<RSGISGraphEdge*>::iterator iterEdges;
		for(iterEdges = edges->begin(); iterEdges != edges->end(); iterEdges++)
		{
			if(first)
			{
				tmpVertex = (*iterEdges)->getOppositeNode(this);
				if(!tmpVertex->getVisited())
				{
					distance = (*iterEdges)->getWeight();
					shortestEdge = (*iterEdges);
					first = false;
				}
			}
			else
			{
				tmpVertex = (*iterEdges)->getOppositeNode(this);
				if(!tmpVertex->getVisited())
				{
					if((*iterEdges)->getWeight() < distance)
					{
						distance = (*iterEdges)->getWeight();
						shortestEdge = (*iterEdges);
					}
				}
			}
		}
		
		return shortestEdge;
	}
	
	void RSGISGraphVertex::removeUnmarkedEdges() throw(RSGISGraphException)
	{
		RSGISGraphVertex *tmpVertex = NULL;
		list<RSGISGraphEdge*>::iterator iterEdges;
		for(iterEdges = edges->begin(); iterEdges != edges->end(); iterEdges++)
		{
			
			if(!(*iterEdges)->getMarked())
			{
				tmpVertex = (*iterEdges)->getOppositeNode(this);
				//cout << "Removing edge connecting " << tmpVertex->getID() << " and " << this->id << endl;
				tmpVertex->removeEdge((*iterEdges));
				edges->erase(iterEdges);
			}
				
		}
	}
	
	void RSGISGraphVertex::removeMarkedEdges() throw(RSGISGraphException)
	{
		RSGISGraphVertex *tmpVertex = NULL;
		list<RSGISGraphEdge*>::iterator iterEdges;
		for(iterEdges = edges->begin(); iterEdges != edges->end(); iterEdges++)
		{
			
			if((*iterEdges)->getMarked())
			{
				tmpVertex = (*iterEdges)->getOppositeNode(this);
				//cout << "Removing edge connecting " << tmpVertex->getID() << " and " << this->id << endl;
				tmpVertex->removeEdge((*iterEdges));
				edges->erase(iterEdges);
			}
			
		}
	}
	
	void RSGISGraphVertex::addEdge(RSGISGraphEdge *edge)
	{
		edges->push_back(edge);
	}
	
	void RSGISGraphVertex::removeEdge(RSGISGraphEdge *edge)
	{
		edges->remove(edge);
	}
	
	list<RSGISGraphEdge*>* RSGISGraphVertex::getEdges()
	{
		return edges;
	}
	
	void RSGISGraphVertex::printVertex()
	{
		cout << "Vertex " << id << endl;
		cout << "Edges:\n";
		list<RSGISGraphEdge*>::iterator iterEdges;
		for(iterEdges = edges->begin(); iterEdges != edges->end(); iterEdges++)
		{
			cout << id << "->" << (*iterEdges)->getOppositeNode(this)->getID() << "(" << (*iterEdges)->getWeight() << ")" << endl;
		}
	}
	
	void RSGISGraphVertex::getSumEdgeWeights(double *sum, int *numEdges)
	{
		list<RSGISGraphEdge*>::iterator iterEdges;
		for(iterEdges = edges->begin(); iterEdges != edges->end(); iterEdges++)
		{
			if(!(*iterEdges)->getMarked())
			{
				*sum += (*iterEdges)->getWeight();
				*numEdges += 1;
				(*iterEdges)->setMarked(true);
			}
		}
	}
	
	void RSGISGraphVertex::setSumSqDiffEdgeWeights(double *sum, int *numEdges, double mean)
	{
		double diff = 0;
		double sqDiff = 0;
		list<RSGISGraphEdge*>::iterator iterEdges;
		for(iterEdges = edges->begin(); iterEdges != edges->end(); iterEdges++)
		{
			if(!(*iterEdges)->getMarked())
			{
				diff = mean - (*iterEdges)->getWeight();
				sqDiff = diff * diff;
				*sum += sqDiff;
				*numEdges += 1;
				(*iterEdges)->setMarked(true);
			}
		}
	}
	
	void RSGISGraphVertex::markEdgesGtThreshold(double threshold)
	{
		list<RSGISGraphEdge*>::iterator iterEdges;
		for(iterEdges = edges->begin(); iterEdges != edges->end(); iterEdges++)
		{
			if((*iterEdges)->getWeight() > threshold)
			{
				(*iterEdges)->setMarked(true);
			}
		}
	}
	
	bool RSGISGraphVertex::operator==(RSGISGraphVertex *vertex) const
	{
		if(vertex->id == this->id)
		{
			return true;
		}
		return false;
	}
	
	RSGISGraphVertex::~RSGISGraphVertex()
	{
		
	}
	
}}
