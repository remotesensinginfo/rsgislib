/*
 *  RSGISGraphComponents.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/06/2008.
 *  Copyright 2008 Aberystwyth University. All rights reserved.
 *
 */

#include "RSGISGraphComponents.h"

namespace rsgis{namespace datastruct{

	
	RSGISGraphComponents::RSGISGraphComponents()
	{
		components = new list<RSGISClusteringGraph*>();
	}
	
	void RSGISGraphComponents::generateComponents(RSGISClusteringGraph *graph)
	{
		list<RSGISGraphVertex*> *newGraphVertexes = new list<RSGISGraphVertex*>();
		list<RSGISGraphVertex*>::iterator iterVertexes;
		RSGISGraphVertex *tmpVertex = NULL;
		int counter = 0;
		while(graph->getSize() > 0)
		{
			cout << "Generating component " << counter++ << " graph has size " << graph->getSize() << endl;
			RSGISClusteringGraph *newGraph = new RSGISClusteringGraph();
			graph->breathFirstSearch(newGraphVertexes);
			//cout << "Finished breath first search\n";
			for(iterVertexes = newGraphVertexes->begin(); iterVertexes != newGraphVertexes->end(); iterVertexes++)
			{
				tmpVertex = (*iterVertexes);
				//cout << "Graph size (before remove): " << graph->getSize() << endl;
				graph->removeVertex(tmpVertex);
				//cout << "Graph size (after remove): " << graph->getSize() << endl;
				newGraph->addVertex(tmpVertex);
			}
			newGraphVertexes->clear();
			components->push_back(newGraph);
		}
	}
	
	void RSGISGraphComponents::generateComponents()
	{
		list<RSGISClusteringGraph*> *oldComponents = new list<RSGISClusteringGraph*>();
		list<RSGISClusteringGraph*>::iterator iterComponents;
		for(iterComponents = components->begin(); iterComponents != components->end(); iterComponents++)
		{
			oldComponents->push_back((*iterComponents));
			components->erase(iterComponents);
		}
		
		int counter = 0;
		for(iterComponents = oldComponents->begin(); iterComponents != oldComponents->end(); iterComponents++)
		{
			cout << "Generating components for component " << counter++ << endl;
			this->generateComponents((*iterComponents));
		}
	}
	
	list<RSGISClusteringGraph*>* RSGISGraphComponents::getComponents()
	{
		return components;
	}
	
	RSGISGraphComponents::~RSGISGraphComponents()
	{
		
	}
	
	RSGISSpatialGraphComponents::RSGISSpatialGraphComponents()
	{
		components = new list<RSGISSpatialClusteringGraph*>();
	}
	
	void RSGISSpatialGraphComponents::generateComponents(RSGISSpatialClusteringGraph *graph)
	{
		list<RSGISGraphVertex*> *newGraphVertexes = new list<RSGISGraphVertex*>();
		list<RSGISGraphVertex*>::iterator iterVertexes;
		RSGISSpatialGraphVertex *tmpVertex = NULL;
		int counter = 0;
		while(graph->getSize() > 0)
		{
			cout << "Generating component " << counter++ << " graph has size " << graph->getSize() << endl;
			RSGISSpatialClusteringGraph *newGraph = new RSGISSpatialClusteringGraph(graph->getOutlineGenerator());
			graph->breathFirstSearch(newGraphVertexes);
			//cout << "Finished breath first search\n";
			for(iterVertexes = newGraphVertexes->begin(); iterVertexes != newGraphVertexes->end(); iterVertexes++)
			{
				tmpVertex = dynamic_cast<RSGISSpatialGraphVertex*> ((*iterVertexes));
				//cout << "Graph size (before remove): " << graph->getSize() << endl;
				graph->removeVertex(tmpVertex);
				//cout << "Graph size (after remove): " << graph->getSize() << endl;
				newGraph->addVertex(tmpVertex);
			}
			newGraphVertexes->clear();
			components->push_back(newGraph);
		}
	}
	
	void RSGISSpatialGraphComponents::generateComponents()
	{
		list<RSGISSpatialClusteringGraph*> *oldComponents = new list<RSGISSpatialClusteringGraph*>();
		list<RSGISSpatialClusteringGraph*>::iterator iterComponents;
		for(iterComponents = components->begin(); iterComponents != components->end(); iterComponents++)
		{
			oldComponents->push_back((*iterComponents));
			components->erase(iterComponents);
		}
		
		int counter = 0;
		for(iterComponents = oldComponents->begin(); iterComponents != oldComponents->end(); iterComponents++)
		{
			cout << "Generating components for component " << counter++ << endl;
			this->generateComponents((*iterComponents));
		}
	}
	
	list<RSGISSpatialClusteringGraph*>* RSGISSpatialGraphComponents::getComponents()
	{
		return components;
	}
	
	RSGISSpatialGraphComponents::~RSGISSpatialGraphComponents()
	{
		
	}

}}

