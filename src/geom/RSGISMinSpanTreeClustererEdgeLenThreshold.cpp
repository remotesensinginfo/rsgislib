/*
 *  RSGISMinSpanTreeClustererEdgeLenThreshold.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/08/2009.
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

#include "RSGISMinSpanTreeClustererEdgeLenThreshold.h"

namespace rsgis{namespace geom{
	
	
	RSGISMinSpanTreeClustererEdgeLenThreshold::RSGISMinSpanTreeClustererEdgeLenThreshold(float lengththreshold) : RSGISSpatialClustererInterface()
	{
		this->lengththreshold = lengththreshold;
	}
	
	list<RSGIS2DPoint*>** RSGISMinSpanTreeClustererEdgeLenThreshold::clusterData(vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold) throw(RSGISClustererException)
	{
		try
		{
			cout << "Construct Delaunay Triangulation\n";
			RSGISDelaunayTriangulation *tri = new RSGISDelaunayTriangulation(data);
			ClusterGraph *cg = this->constructGraph(tri, data);
			this->constructMinimumSpanningTree(cg);
			
			property_map<ClusterGraph, edge_weight_t>::type weight = get(edge_weight, *cg);
			graph_traits<ClusterGraph>::edge_iterator edgeIter, e_end;
			
			*threshold = lengththreshold;
			
			// Remove Edges
			for (tie(edgeIter, e_end) = edges(*cg); edgeIter != e_end; ++edgeIter)
			{
				if(get(weight, *edgeIter) > lengththreshold)
				{
					remove_edge(*edgeIter, *cg);
				}
			}
			
			// Find components (The clusters)
			vector<int> component(num_vertices(*cg));
			int num_comp = connected_components(*cg, make_iterator_property_map(component.begin(), get(vertex_index, *cg)));
			
			list<RSGIS2DPoint*> **outputClusters = new list<RSGIS2DPoint*>*[num_comp];
			for(int i = 0; i < num_comp; i++)
			{
				outputClusters[i] = new list<RSGIS2DPoint*>();
			}
			
			if(data->size() != component.size())
			{
				throw RSGISClustererException("Input data and number of graph elements different.");
			}
			
			for(unsigned int i = 0; i < data->size(); i++)
			{
				outputClusters[component.at(i)]->push_back(data->at(i));
			}
			
			delete tri;
			
			*numclusters = num_comp;
			return outputClusters;
		}
		catch(RSGISGeometryException &e)
		{
			throw RSGISClustererException(e.what());
		}
		catch(RSGISClustererException &e)
		{
			throw e;
		}	
	}
	
	void RSGISMinSpanTreeClustererEdgeLenThreshold::updateLengthThreshold(float lengththreshold)
	{
		this->lengththreshold = lengththreshold;
	}
	
	ClusterGraph* RSGISMinSpanTreeClustererEdgeLenThreshold::constructGraph(RSGISDelaunayTriangulation *tri, vector<RSGIS2DPoint*> *data)
	{
		vector<RSGIS2DPoint*>::iterator iterData;
		int count = 0;
		for(iterData = data->begin(); iterData != data->end(); iterData++)
		{
			(*iterData)->setIndex(count);
			count++;
		}
		
		list<RSGISTriangle*>* triangulation = tri->getTriangulation();
		list<RSGISTriangle*>::iterator iterTri;
		
		// Create graph with vertex's
		ClusterGraph *cg = new ClusterGraph(data->size());
		
		// Add edges
		cout << "Adding Edges to Graph - may take some time ... ";
		bool edge1Present = false;
		bool edge2Present = false;
		bool edge3Present = false;
		graph_traits<ClusterGraph>::vertex_descriptor u, v;
		
		RSGISTriangle *triangle = NULL;
		ClusterGraph::edge_descriptor e1, e2, e3;
		for(iterTri = triangulation->begin(); iterTri != triangulation->end(); iterTri++)
		{
			triangle = (*iterTri);
			
			// A - B (Edge 1)
			u = vertex(triangle->getPointA()->getIndex(), *cg);
			v = vertex(triangle->getPointB()->getIndex(), *cg);
			tie(e1, edge1Present) = edge(u, v, *cg);
			
			// B - C (Edge 2)
			u = vertex(triangle->getPointB()->getIndex(), *cg);
			v = vertex(triangle->getPointC()->getIndex(), *cg);
			tie(e2, edge2Present) = edge(u, v, *cg);
			
			// A - B (Edge 1)
			u = vertex(triangle->getPointC()->getIndex(), *cg);
			v = vertex(triangle->getPointA()->getIndex(), *cg);
			tie(e3, edge3Present) = edge(u, v, *cg);
			
			
			// Need to check whether edges are already present in the graph...
			if(!edge1Present)
			{
				add_edge(triangle->getPointA()->getIndex(), triangle->getPointB()->getIndex(), triangle->getPointA()->distance(triangle->getPointB()), *cg);
			}
			if(!edge2Present)
			{
				add_edge(triangle->getPointB()->getIndex(), triangle->getPointC()->getIndex(), triangle->getPointB()->distance(triangle->getPointC()), *cg);
			}
			if(!edge3Present)
			{
				add_edge(triangle->getPointC()->getIndex(), triangle->getPointA()->getIndex(), triangle->getPointC()->distance(triangle->getPointA()), *cg);
			}
			edge1Present = false;
			edge2Present = false;
			edge3Present = false;
		}
		cout << " Complete.\n";
		return cg;
	}
	
	void RSGISMinSpanTreeClustererEdgeLenThreshold::constructMinimumSpanningTree(ClusterGraph *cg)
	{
		cout << "Identifying the Minimum Spanning Tree\n";
		vector<graph_traits<ClusterGraph>::edge_descriptor> mst;
		kruskal_minimum_spanning_tree(*cg, back_inserter(mst));
		
		// Remove edges not within the spanning tree
		cout << "Remove Edges not within the minimum spanning tree\n";
		bool mstTreeEdge = false;
		vector<graph_traits<ClusterGraph>::edge_descriptor>::iterator iterMST;
		graph_traits<ClusterGraph>::edge_iterator edgeIter, e_end;
		
		int i = 0;
		int feedback = num_edges(*cg)/10;
		int feedbackCounter = 0;
		
		cout << "Started" << flush;		
		for (tie(edgeIter, e_end) = edges(*cg); edgeIter != e_end; ++edgeIter)
		{
			if((num_edges(*cg) > 10) && ((i % feedback) == 0))
			{
				cout << ".." << feedbackCounter << ".." << flush;
				feedbackCounter = feedbackCounter + 10;
			}
			
			mstTreeEdge = false;
			for(iterMST = mst.begin(); iterMST != mst.end(); iterMST++)
			{
				if(*iterMST == *edgeIter)
				{
					mstTreeEdge = true;
				}
			}
			if(!mstTreeEdge)
			{
				remove_edge(*edgeIter, *cg);
			}
			i++;
		}
		cout << " Complete.\n";
	}
	
	RSGISMinSpanTreeClustererEdgeLenThreshold::~RSGISMinSpanTreeClustererEdgeLenThreshold()
	{
		
	}
	
}}


