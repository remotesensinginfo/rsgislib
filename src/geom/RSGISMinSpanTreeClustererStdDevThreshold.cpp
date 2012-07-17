/*
 *  RSGISMinSpanTreeClustererStdDevThreshold.cpp
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

#include "RSGISMinSpanTreeClustererStdDevThreshold.h"

namespace rsgis{namespace geom{

	
	RSGISMinSpanTreeClustererStdDevThreshold::RSGISMinSpanTreeClustererStdDevThreshold(float stddevthreshold, float maxEdgeLength) : RSGISSpatialClustererInterface()
	{
		this->stddevthreshold = stddevthreshold;
		this->maxEdgeLength = maxEdgeLength;
	}
	
    std::list<RSGIS2DPoint*>** RSGISMinSpanTreeClustererStdDevThreshold::clusterData(std::vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold) throw(rsgis::math::RSGISClustererException)
	{
		try
		{
			std::list<RSGIS2DPoint*> **outputClusters = NULL;
			if(data->size() > 1)
			{
				cout << "Construct Delaunay Triangulation\n";
				RSGISDelaunayTriangulation *tri = new RSGISDelaunayTriangulation(data);
				ClusterGraph *cg = this->constructGraph(tri, data);
				this->constructMinimumSpanningTree(cg);
				
                boost::property_map<ClusterGraph, boost::edge_weight_t>::type weight = boost::get(boost::edge_weight, *cg);
				boost::graph_traits<ClusterGraph>::edge_iterator edgeIter, e_end;
				
				// Calculate mean edge weight
				double totalEdgeWeight = 0;
				int countEdges = 0;
				for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; ++edgeIter)
				{
					totalEdgeWeight += boost::get(weight, *edgeIter);
					countEdges++;
				}
				double meanEdgeWeight = totalEdgeWeight/countEdges;
				
				//Calculate Standard Deviation of edge weights
				double total_sq = 0;
				for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; ++edgeIter)
				{
					total_sq +=  ((boost::get(weight, *edgeIter) - meanEdgeWeight) * (boost::get(weight, *edgeIter) - meanEdgeWeight));
				}
				double stddev = sqrt(total_sq/countEdges);
				
				// Remove Edges
				double removal_threshold = meanEdgeWeight + (stddev * stddevthreshold);
				*threshold = removal_threshold;
				double edgeWeight = 0;
				
				for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; )
				{
					edgeWeight = boost::get(weight, *edgeIter);
					if( (edgeWeight > removal_threshold) | (edgeWeight > this->maxEdgeLength))
					{
						boost::remove_edge(*edgeIter++, *cg);
					}
					else 
					{
						++edgeIter;
					}
					
				}
				
				// Find components (The clusters)
                std::vector<int> component(boost::num_vertices(*cg));
				int num_comp = boost::connected_components(*cg, boost::make_iterator_property_map(component.begin(), boost::get(boost::vertex_index, *cg)));
				
				outputClusters = new std::list<RSGIS2DPoint*>*[num_comp];
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
				delete cg;
				
				*numclusters = num_comp;
				return outputClusters;
			}
			else if(data->size() == 1)
			{
				*numclusters = 1;
				outputClusters = new list<RSGIS2DPoint*>*[1];
				outputClusters[0] = new list<RSGIS2DPoint*>();
				outputClusters[0]->push_back(data->at(0));
			}
			else 
			{
				throw rsgis::math::RSGISClustererException("No data was provided for clustering.");
			}

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
	
	ClusterGraph* RSGISMinSpanTreeClustererStdDevThreshold::constructGraph(RSGISDelaunayTriangulation *tri, std::vector<RSGIS2DPoint*> *data)
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
		cout << "Adding Edges to Graph - may take some time ... ";
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
			boost::tie(e1, edge1Present) = boost::edge(u, v, *cg);
			
			// B - C (Edge 2)
			u = boost::vertex(triangle->getPointB()->getIndex(), *cg);
			v = boost::vertex(triangle->getPointC()->getIndex(), *cg);
			boost::tie(e2, edge2Present) = boost::edge(u, v, *cg);
			
			// A - B (Edge 1)
			u = vertex(triangle->getPointC()->getIndex(), *cg);
			v = vertex(triangle->getPointA()->getIndex(), *cg);
			boost::tie(e3, edge3Present) = boost::edge(u, v, *cg);
			
			
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
		cout << " Complete.\n";
		return cg;
	}

	void RSGISMinSpanTreeClustererStdDevThreshold::constructMinimumSpanningTree(ClusterGraph *cg)
	{
        std::cout << "Identifying the Minimum Spanning Tree\n";
        std::vector<boost::graph_traits<ClusterGraph>::edge_descriptor> mst;
		boost::kruskal_minimum_spanning_tree(*cg, std::back_inserter(mst));
		
		// Remove edges not within the spanning tree
		cout << "Remove Edges not within the minimum spanning tree\n";
		bool mstTreeEdge = false;
        std::vector<boost::graph_traits<ClusterGraph>::edge_descriptor>::iterator iterMST;
		boost::graph_traits<ClusterGraph>::edge_iterator edgeIter, e_end;
		
		int i = 0;
		int feedback = num_edges(*cg)/10;
		int feedbackCounter = 0;
		
		cout << "Started" << flush;		
		for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; )
		{
			if((boost::num_edges(*cg) > 10) && ((i % feedback) == 0))
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
				boost::remove_edge(*edgeIter++, *cg);
			}
			else 
			{
				++edgeIter;
			}

			i++;
		}
		cout << " Complete.\n";
	}
	
	RSGISMinSpanTreeClustererStdDevThreshold::~RSGISMinSpanTreeClustererStdDevThreshold()
	{
		
	}
	
}}

