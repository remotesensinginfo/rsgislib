/*
 *  RSGISGraph.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/06/2008.
 *  Copyright 2008 Aberystwyth University. All rights reserved.
 *
 */

#include "RSGISGraph.h"

namespace rsgis{namespace datastruct{

	RSGISClusteringGraph::RSGISClusteringGraph()
	{
		vertexes = new list<RSGISGraphVertex*>();
		idcounter = 0;
	}
	
	bool RSGISClusteringGraph::uniqueID(RSGISGraphVertex *vertex)
	{
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			if(vertex->getID() == (*iterVertexes)->getID())
			{
				return false;
			}
		}
		return true;
	}
	
	void RSGISClusteringGraph::addVertex(RSGISGraphVertex *vertex) throw(RSGISGraphException)
	{
		if(!this->uniqueID(vertex))
		{
			throw RSGISGraphException("Vertex ID is not unique..");
		}
		idcounter++;
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			if(vertex->createEdge(*iterVertexes))
			{
				RSGISGraphEdge *edge = new RSGISGraphEdge();
				edge->setNodeA(vertex);
				edge->setNodeB(*iterVertexes);
				edge->setWeight(vertex->edgeWeight(*iterVertexes));
				vertex->addEdge(edge);
				(*iterVertexes)->addEdge(edge);
			}
		}
		vertexes->push_back(vertex);
	}
	
	void RSGISClusteringGraph::removeVertex(RSGISGraphVertex *vertex) throw(RSGISGraphException)
	{
		list<RSGISGraphEdge*>::iterator iterEdges;
		for(iterEdges = vertex->getEdges()->begin(); iterEdges != vertex->getEdges()->end(); iterEdges++)
		{
			(*iterEdges)->getOppositeNode(vertex)->removeEdge((*iterEdges));
			vertex->getEdges()->erase(iterEdges);
		}
		vertexes->remove(vertex);
	}
	
	void RSGISClusteringGraph::resetVisitedTags()
	{
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			(*iterVertexes)->setVisited(false);
		}
	}
	
	void RSGISClusteringGraph::resetMarkedEdges()
	{
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			(*iterVertexes)->resetEdges();
		}
	}
	
	void RSGISClusteringGraph::resetMarkedEdgesVertexes()
	{
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			(*iterVertexes)->setVisited(false);
			(*iterVertexes)->resetEdges();
		}
	}
	
	void RSGISClusteringGraph::breathFirstSearch(list<RSGISGraphVertex*> *outVertexes) throw(RSGISGraphException)
	{
		if(!outVertexes->empty())
		{
			throw RSGISGraphException("Output Vector for the breath first search is not empty.");
		}
		
		if(vertexes->size() == 0)
		{
			throw RSGISGraphException("There are no vertexes in this Graph");
		}
		
		this->resetVisitedTags();
		
		list<RSGISGraphVertex*> *queue = new list<RSGISGraphVertex*>();
		list<RSGISGraphEdge*> *tmpEdges = NULL;
		int numEdges = 0;
		list<RSGISGraphVertex*>::iterator iterQueue;
		list<RSGISGraphEdge*>::iterator iterEdges;
		queue->push_back(vertexes->front());
		
		RSGISGraphVertex *tmpVertex = NULL;
		while(queue->size() > 0)
		{
			iterQueue = queue->begin();
			tmpVertex = *iterQueue;
			queue->erase(iterQueue);
			
			if(!tmpVertex->getVisited())
			{
				tmpVertex->setVisited(true);
				tmpEdges = tmpVertex->getEdges();
				numEdges = tmpEdges->size();
				
				for(iterEdges = tmpEdges->begin(); iterEdges != tmpEdges->end(); iterEdges++)
				{
					if(!(*iterEdges)->getOppositeNode(tmpVertex)->getVisited())
					{
						queue->push_back((*iterEdges)->getOppositeNode(tmpVertex));
					}
				}
				outVertexes->push_back(tmpVertex);		
			}
		}
	}
	
	void RSGISClusteringGraph::findMinimumSpanningTree() throw(RSGISGraphException)
	{
		this->resetMarkedEdgesVertexes();
		list<RSGISGraphVertex*> *markedVertexes = new list<RSGISGraphVertex*>();
		list<RSGISGraphVertex*>::iterator iterMarked;
		vertexes->front()->setVisited(true);
		markedVertexes->push_back(vertexes->front());
		RSGISGraphEdge *shortestEdge = NULL;
		RSGISGraphEdge *tmpEdge = NULL;
		RSGISGraphVertex *tmpVertex = NULL;
		bool first = true;
		
		while(markedVertexes->size() != vertexes->size())
		{
			shortestEdge = NULL;
			tmpVertex = NULL;
			first = true;
			
			for(iterMarked = markedVertexes->begin(); iterMarked != markedVertexes->end(); iterMarked++)
			{
				tmpEdge = (*iterMarked)->findShortestEdgeNoVisit();
				if(first)
				{
					shortestEdge = tmpEdge;
				}
				else
				{
					if(tmpEdge->getWeight() < shortestEdge->getWeight())
					{
						shortestEdge = tmpEdge;
					}
				}
			}
			
			tmpVertex = shortestEdge->getNotMarkedNode();
			if(tmpVertex == NULL)
			{
				throw RSGISGraphException("Both nodes are marked!!");
			}
			tmpVertex->setVisited(true);
			shortestEdge->setMarked(true);
			markedVertexes->push_back(tmpVertex);
		}
	}
	
	void RSGISClusteringGraph::removeUnmarkedEdges() throw(RSGISGraphException)
	{
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			//cout << "Removing unmarked edges from vertex " << (*iterVertexes)->getID() << endl;
			(*iterVertexes)->removeUnmarkedEdges();
		}
	}
	
	void RSGISClusteringGraph::removeMarkedEdges() throw(RSGISGraphException)
	{
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			//cout << "Removing marked edges from vertex " << (*iterVertexes)->getID() << endl;
			(*iterVertexes)->removeMarkedEdges();
		}
	}
	
	double RSGISClusteringGraph::meanEdgeWeight()
	{
		if(vertexes->size() == 1)
		{
			return 0;
		}
		
		this->resetMarkedEdgesVertexes();
		double sum = 0;
		int numEdges = 0;
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			(*iterVertexes)->getSumEdgeWeights(&sum, &numEdges);
		}
		return (sum/numEdges);
	}
	
	double RSGISClusteringGraph::stddevEdgeWeight(double mean)
	{
		if(vertexes->size() == 1)
		{
			return 0;
		}
		this->resetMarkedEdgesVertexes();
		double sum = 0;
		int numEdges = 0;
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			(*iterVertexes)->setSumSqDiffEdgeWeights(&sum, &numEdges, mean);
		}
		return sqrt(sum/numEdges);
		
	}
	
	void RSGISClusteringGraph::markEdgesGtThreshold(double threshold)
	{
		this->resetMarkedEdgesVertexes();

		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			(*iterVertexes)->markEdgesGtThreshold(threshold);
		}
		
	}
	
	int RSGISClusteringGraph::nextID()
	{
		return idcounter;
	}
	
	int RSGISClusteringGraph::getSize()
	{
		return vertexes->size();
	}
	
	void RSGISClusteringGraph::printGraph()
	{
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			(*iterVertexes)->printVertex();
		}
	}
	
	RSGISClusteringGraph::~RSGISClusteringGraph()
	{
		
	}
	
	RSGISSpatialClusteringGraph::RSGISSpatialClusteringGraph(RSGISAbstractOutlinePolygon *outlineGenerator) : RSGISClusteringGraph()
	{
		this->outlineGenerator = outlineGenerator;
	}
	
	void RSGISSpatialClusteringGraph::addVertex(RSGISSpatialGraphVertex *vertex)throw(RSGISGraphException)
	{
		RSGISClusteringGraph::addVertex(vertex);
	}
	
	void RSGISSpatialClusteringGraph::removeVertex(RSGISSpatialGraphVertex *vertex)throw(RSGISGraphException)
	{
		RSGISClusteringGraph::removeVertex(vertex);
	}

	Envelope* RSGISSpatialClusteringGraph::getGraphExtent()
	{
		Envelope *env = new Envelope();
		RSGISSpatialGraphVertex *spatialVertex = NULL;
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			spatialVertex = dynamic_cast<RSGISSpatialGraphVertex*> (*iterVertexes);
			if(spatialVertex != NULL)
			{
				spatialVertex->getGeometry()->getCoordinates()->expandEnvelope(*env);
			}
			
		}
		return env;
	}
	
	vector<Geometry*>* RSGISSpatialClusteringGraph::getGeometries()
	{
		vector<Geometry*> *geometries = new vector<Geometry*>();
		RSGISSpatialGraphVertex *spatialVertex = NULL;
		list<RSGISGraphVertex*>::iterator iterVertexes;
		for(iterVertexes = vertexes->begin(); iterVertexes != vertexes->end(); iterVertexes++)
		{
			spatialVertex = dynamic_cast<RSGISSpatialGraphVertex*> (*iterVertexes);
			if(spatialVertex != NULL)
			{
				geometries->push_back(spatialVertex->getGeometry());
			}
			
		}
		return geometries;
	}
	
	MultiPolygon* RSGISSpatialClusteringGraph::getMultiPolygon()
	{
		GeometryFactory *geosGeomFactory = new GeometryFactory();
		vector<Geometry*>* geoms = this->getGeometries();
		return geosGeomFactory->createMultiPolygon(*geoms);
	}
	
	Polygon* RSGISSpatialClusteringGraph::getGraphOutline()
	{
		return outlineGenerator->createOutline(this->getMultiPolygon());
	}
	
	RSGISAbstractOutlinePolygon* RSGISSpatialClusteringGraph::getOutlineGenerator()
	{
		return this->outlineGenerator;
	}
	
}}
