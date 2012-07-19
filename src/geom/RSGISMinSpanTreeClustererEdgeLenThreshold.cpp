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
	
	std::list<RSGIS2DPoint*>** RSGISMinSpanTreeClustererEdgeLenThreshold::clusterData(std::vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold) throw(rsgis::math::RSGISClustererException)
	{
		try
		{
			std::cout << "Construct Delaunay Triangulation\n";
			RSGISDelaunayTriangulation *tri = new RSGISDelaunayTriangulation(data);
			ClusterGraph *cg = this->constructGraph(tri, data);
			this->constructMinimumSpanningTree(cg);
			
            boost::property_map<ClusterGraph, boost::edge_weight_t>::type weight = boost::get(boost::edge_weight, *cg);
			boost::graph_traits<ClusterGraph>::edge_iterator edgeIter, e_end;
			
			*threshold = lengththreshold;
			
			// Remove Edges
			for (boost::tie(edgeIter, e_end) = edges(*cg); edgeIter != e_end; ++edgeIter)
			{
				if(boost::get(weight, *edgeIter) > lengththreshold)
				{
					boost::remove_edge(*edgeIter, *cg);
				}
			}
			
			// Find components (The clusters)
            std::vector<int> component(boost::num_vertices(*cg));
			int num_comp = boost::connected_components(*cg, boost::make_iterator_property_map(component.begin(), boost::get(boost::vertex_index, *cg)));
			
            std::list<RSGIS2DPoint*> **outputClusters = new std::list<RSGIS2DPoint*>*[num_comp];
			for(int i = 0; i < num_comp; i++)
			{
				outputClusters[i] = new std::list<RSGIS2DPoint*>();
			}
			
			if(data->size() != component.size())
			{
				throw rsgis::math::RSGISClustererException("Input data and number of graph elements different.");
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
			throw rsgis::math::RSGISClustererException(e.what());
		}
		catch(rsgis::math::RSGISClustererException &e)
		{
			throw e;
		}	
	}
	
	void RSGISMinSpanTreeClustererEdgeLenThreshold::updateLengthThreshold(float lengththreshold)
	{
		this->lengththreshold = lengththreshold;
	}
	
	ClusterGraph* RSGISMinSpanTreeClustererEdgeLenThreshold::constructGraph(RSGISDelaunayTriangulation *tri, std::vector<RSGIS2DPoint*> *data)
	{
        std::vector<RSGIS2DPoint*>::iterator iterData;
		int count = 0;
		for(iterData = data->begin(); iterData != data->end(); iterData++)
		{
			(*iterData)->setIndex(count);
			count++;
		}
		
        std::list<RSGISTriangle*>* triangulation = tri->getTriangulation();
        std::list<RSGISTriangle*>::iterator iterTri;
		
		// Create graph with vertex's
		ClusterGraph *cg = new ClusterGraph(data->size());
		
		// Add edges
		std::cout << "Adding Edges to Graph - may take some time ... ";
		bool edge1Present = false;
		bool edge2Present = false;
		bool edge3Present = false;
		boost::graph_traits<ClusterGraph>::vertex_descriptor u, v;
		
		RSGISTriangle *triangle = NULL;
		ClusterGraph::edge_descriptor e1, e2, e3;
		for(iterTri = triangulation->begin(); iterTri != triangulation->end(); iterTri++)
		{
			triangle = (*iterTri);
			
			// A - B (Edge 1)
			u = boost::vertex(triangle->getPointA()->getIndex(), *cg);
			v = boost::vertex(triangle->getPointB()->getIndex(), *cg);
			tie(e1, edge1Present) = edge(u, v, *cg);
			
			// B - C (Edge 2)
			u = boost::vertex(triangle->getPointB()->getIndex(), *cg);
			v = boost::vertex(triangle->getPointC()->getIndex(), *cg);
			boost::tie(e2, edge2Present) = edge(u, v, *cg);
			
			// A - B (Edge 1)
			u = boost::vertex(triangle->getPointC()->getIndex(), *cg);
			v = boost::vertex(triangle->getPointA()->getIndex(), *cg);
			boost::tie(e3, edge3Present) = edge(u, v, *cg);
			
			
			// Need to check whether edges are already present in the graph...
			if(!edge1Present)
			{
				boost::add_edge(triangle->getPointA()->getIndex(), triangle->getPointB()->getIndex(), triangle->getPointA()->distance(triangle->getPointB()), *cg);
			}
			if(!edge2Present)
			{
				boost::add_edge(triangle->getPointB()->getIndex(), triangle->getPointC()->getIndex(), triangle->getPointB()->distance(triangle->getPointC()), *cg);
			}
			if(!edge3Present)
			{
				boost::add_edge(triangle->getPointC()->getIndex(), triangle->getPointA()->getIndex(), triangle->getPointC()->distance(triangle->getPointA()), *cg);
			}
			edge1Present = false;
			edge2Present = false;
			edge3Present = false;
		}
		std::cout << " Complete.\n";
		return cg;
	}
	
	void RSGISMinSpanTreeClustererEdgeLenThreshold::constructMinimumSpanningTree(ClusterGraph *cg)
	{
		std::cout << "Identifying the Minimum Spanning Tree\n";
        std::vector<boost::graph_traits<ClusterGraph>::edge_descriptor> mst;
		boost::kruskal_minimum_spanning_tree(*cg, back_inserter(mst));
		
		// Remove edges not within the spanning tree
		std::cout << "Remove Edges not within the minimum spanning tree\n";
		bool mstTreeEdge = false;
        std::vector<boost::graph_traits<ClusterGraph>::edge_descriptor>::iterator iterMST;
		boost::graph_traits<ClusterGraph>::edge_iterator edgeIter, e_end;
		
		int i = 0;
		int feedback = num_edges(*cg)/10;
		int feedbackCounter = 0;
		
		std::cout << "Started" << std::flush;		
		for (boost::tie(edgeIter, e_end) = edges(*cg); edgeIter != e_end; ++edgeIter)
		{
			if((boost::num_edges(*cg) > 10) && ((i % feedback) == 0))
			{
				std::cout << ".." << feedbackCounter << ".." << std::flush;
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
		std::cout << " Complete.\n";
	}
	
	RSGISMinSpanTreeClustererEdgeLenThreshold::~RSGISMinSpanTreeClustererEdgeLenThreshold()
	{
		
	}
	
}}


