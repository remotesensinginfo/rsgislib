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
				std::cout << "Construct Delaunay Triangulation\n";
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
				
				for(boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; )
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
				outputClusters = new std::list<RSGIS2DPoint*>*[1];
				outputClusters[0] = new std::list<RSGIS2DPoint*>();
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
		std::cout << " Complete.\n";
		return cg;
	}

	void RSGISMinSpanTreeClustererStdDevThreshold::constructMinimumSpanningTree(ClusterGraph *cg)
	{
        std::cout << "Identifying the Minimum Spanning Tree\n";
        std::vector<boost::graph_traits<ClusterGraph>::edge_descriptor> mst;
		boost::kruskal_minimum_spanning_tree(*cg, std::back_inserter(mst));
		
		// Remove edges not within the spanning tree
		std::cout << "Remove Edges not within the minimum spanning tree\n";
		bool mstTreeEdge = false;
        std::vector<boost::graph_traits<ClusterGraph>::edge_descriptor>::iterator iterMST;
		boost::graph_traits<ClusterGraph>::edge_iterator edgeIter, e_end;
		
		int i = 0;
		int feedback = num_edges(*cg)/10;
		int feedbackCounter = 0;
		
		std::cout << "Started" << std::flush;		
		for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; )
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
				boost::remove_edge(*edgeIter++, *cg);
			}
			else 
			{
				++edgeIter;
			}

			i++;
		}
		std::cout << " Complete.\n";
	}
	
	RSGISMinSpanTreeClustererStdDevThreshold::~RSGISMinSpanTreeClustererStdDevThreshold()
	{
		
	}
    
    
    
    
    RSGISGraphGeomClusterer::RSGISGraphGeomClusterer(bool useMinSpanTree, float stddevthreshold, double maxEdgeLength, std::string shpFileEdges, bool outShpEdges, std::string h5EdgeLengths, bool outH5EdgeLens)
    {
        this->stddevthreshold = stddevthreshold;
        this->maxEdgeLength = maxEdgeLength;
        this->shpFileEdges = shpFileEdges;
        this->outShpEdges = outShpEdges;
        this->h5EdgeLengths = h5EdgeLengths;
        this->outH5EdgeLens = outH5EdgeLens;
        this->useMinSpanTree = useMinSpanTree;
    }
    
    std::list<RSGIS2DPoint*>** RSGISGraphGeomClusterer::clusterData(std::vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold) throw(rsgis::math::RSGISClustererException)
    {
        std::list<RSGIS2DPoint*> **outputClusters = NULL;
        *numclusters = 0;
        *threshold = 0.0;
        try
        {
            size_t numGeoms = data->size();

            if(numGeoms > 3)
            {
                std::cout << "Construct Delaunay Triangulation\n";
                
                DelaunayTriangulation *dt = new DelaunayTriangulation();
                PointValueMap *values = new PointValueMap();
                int count = 0;
                int feedback = numGeoms/10;
                int feedbackCounter = 0;
                
                std::cout << "Started " << std::flush;
                for(std::vector<RSGIS2DPoint*>::iterator iterPts = data->begin(); iterPts != data->end(); ++iterPts)
                {
                    if((numGeoms > 10) && ((count % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    (*iterPts)->setIndex(count);
                    K::Point_2 cgalPt((*iterPts)->getX(),(*iterPts)->getY());
                    dt->insert(cgalPt);
                    CGALCoordType value = count;
                    values->insert(std::make_pair(cgalPt, value));
                    
                    count++;
                }
                std::cout << " Complete.\n";
                
                ClusterGraph *cg = this->constructGraph(dt, values, data);
                
                delete dt;
                delete values;
                
                if(this->useMinSpanTree)
                {
                    this->constructMinimumSpanningTree(cg);
                }
                
                boost::property_map<ClusterGraph, boost::edge_weight_t>::type weight = boost::get(boost::edge_weight, *cg);
                boost::graph_traits<ClusterGraph>::edge_iterator edgeIter, e_end;
                
                if(this->outH5EdgeLens)
                {
                    rsgis::utils::RSGISExportColumnData2HDF export2HDF = rsgis::utils::RSGISExportColumnData2HDF();
                    export2HDF.createFile(h5EdgeLengths, 1, "Edge Lengths in Minimum Spanning Tree", H5::PredType::IEEE_F64LE);
                    double *edgeWeight = new double[1];
                    for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; ++edgeIter)
                    {
                        edgeWeight[0] = boost::get(weight, *edgeIter);
                        export2HDF.addDataRow(edgeWeight, H5::PredType::NATIVE_DOUBLE);
                    }
                    delete[] edgeWeight;
                    export2HDF.close();
                }
                
                if(this->outShpEdges)
                {
                    std::vector<geos::geom::LineSegment *> *lines = new std::vector<geos::geom::LineSegment *>();
                    std::vector<double> *weights = new std::vector<double>();
                    int targetV = 0;
                    int sourceV = 0;
                    
                    for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; ++edgeIter)
                    {
                        targetV = boost::target(*edgeIter, *cg);
                        sourceV = boost::source(*edgeIter, *cg);
                        
                        //std::cout << "Target = " << targetV << std::endl;
                        //std::cout << "Source = " << sourceV << std::endl;
                        
                        lines->push_back(new geos::geom::LineSegment(data->at(sourceV)->getCoordPoint(), data->at(targetV)->getCoordPoint()));
                        weights->push_back(boost::get(weight, *edgeIter));
                    }
                    
                    RSGISGeomTestExport geomExport;
                    geomExport.exportGEOSLineSegments2SHP(shpFileEdges, true, lines, weights);
                    delete lines;
                }
                
                // Calculate mean edge weight
                double totalEdgeWeight = 0;
                int countEdges = 0;
                for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; ++edgeIter)
                {
                    totalEdgeWeight += boost::get(weight, *edgeIter);
                    countEdges++;
                }
                double meanEdgeWeight = totalEdgeWeight/countEdges;
                std::cout << "Mean Edge Weight = " << meanEdgeWeight << std::endl;
                
                //Calculate Standard Deviation of edge weights
                double total_sq = 0;
                for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; ++edgeIter)
                {
                    total_sq +=  ((boost::get(weight, *edgeIter) - meanEdgeWeight) * (boost::get(weight, *edgeIter) - meanEdgeWeight));
                }
                double stddev = sqrt(total_sq/countEdges);
                std::cout << "Std Dev Edge Weight = " << stddev << std::endl;
                
                // Remove Edges
                double removalThreshold = meanEdgeWeight + (stddev * stddevthreshold);
                if(removalThreshold > this->maxEdgeLength)
                {
                    removalThreshold = this->maxEdgeLength;
                    std::cout << "Max Edge Length overriding stddev threshold...\n";
                }
                
                *threshold = removalThreshold;
                std::cout << "Edge Removal Threshold = " << removalThreshold << std::endl;
                
                double edgeWeight = 0;
                
                for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; )
                {
                    edgeWeight = boost::get(weight, *edgeIter);
                    if(edgeWeight > removalThreshold)
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
                std::cout << "Num Comps = " << num_comp << std::endl;
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
                
                delete cg;
                
                *numclusters = num_comp;
            }
            else
            {
                throw rsgis::math::RSGISClustererException("Need at least 3 geometries to cluster.");
            }
        }
        catch (std::exception &e)
        {
            rsgis::math::RSGISClustererException(e.what());
        }
        
        return outputClusters;
    }
    
    ClusterGraph* RSGISGraphGeomClusterer::constructGraph(DelaunayTriangulation *dt, PointValueMap *values, std::vector<RSGIS2DPoint*> *data) throw(rsgis::math::RSGISClustererException)
    {
        // Create graph with vertex's
        ClusterGraph *cg = new ClusterGraph(data->size());
        try
        {
            // Add edges
            std::cout << "Adding Edges to Graph (may take some time ...) \n";
            bool edge1Present = false;
            bool edge2Present = false;
            bool edge3Present = false;
            boost::graph_traits<ClusterGraph>::vertex_descriptor u, v;
            
            RSGIS2DPoint *vertexA = NULL;
            RSGIS2DPoint *vertexB = NULL;
            RSGIS2DPoint *vertexC = NULL;
            
            size_t numFaces = dt->number_of_faces();
            int count = 0;
            int feedback = numFaces/10;
            int feedbackCounter = 0;
            std::cout << "Started " << std::flush;
            
            ClusterGraph::edge_descriptor e1, e2, e3;
            for (DelaunayTriangulation::Finite_faces_iterator it = dt->finite_faces_begin(); it != dt->finite_faces_end(); it++)
            {
                if((numFaces > 10) && ((count % feedback) == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                vertexA = data->at((*values->find(dt->triangle(it)[0])).second);
                vertexB = data->at((*values->find(dt->triangle(it)[1])).second);
                vertexC = data->at((*values->find(dt->triangle(it)[2])).second);
                
                // A - B (Edge 1)
                u = boost::vertex(vertexA->getIndex(), *cg);
                v = boost::vertex(vertexB->getIndex(), *cg);
                boost::tie(e1, edge1Present) = boost::edge(u, v, *cg);
                
                // B - C (Edge 2)
                u = boost::vertex(vertexB->getIndex(), *cg);
                v = boost::vertex(vertexC->getIndex(), *cg);
                boost::tie(e2, edge2Present) = boost::edge(u, v, *cg);
                
                // A - B (Edge 1)
                u = vertex(vertexC->getIndex(), *cg);
                v = vertex(vertexA->getIndex(), *cg);
                boost::tie(e3, edge3Present) = boost::edge(u, v, *cg);
                
                
                // Need to check whether edges are already present in the graph...
                if(!edge1Present)
                {
                    boost::add_edge(vertexA->getIndex(), vertexB->getIndex(), vertexA->distance(vertexB), *cg);
                }
                if(!edge2Present)
                {
                    boost::add_edge(vertexB->getIndex(), vertexC->getIndex(), vertexB->distance(vertexC), *cg);
                }
                if(!edge3Present)
                {
                    boost::add_edge(vertexC->getIndex(), vertexA->getIndex(), vertexC->distance(vertexA), *cg);
                }
                edge1Present = false;
                edge2Present = false;
                edge3Present = false;
                ++count;
            }
            std::cout << ". Complete.\n";
            return cg;
            
        }
        catch (std::exception &e)
        {
            rsgis::math::RSGISClustererException(e.what());
        }
        
        return cg;
    }
    
    void RSGISGraphGeomClusterer::constructMinimumSpanningTree(ClusterGraph *cg) throw(rsgis::math::RSGISClustererException)
    {
        try
        {
            std::cout << "Identifying the Minimum Spanning Tree\n";
            std::vector<boost::graph_traits<ClusterGraph>::edge_descriptor> mst;
            boost::kruskal_minimum_spanning_tree(*cg, std::back_inserter(mst));
            
            // Remove edges not within the spanning tree
            bool mstTreeEdge = false;
            std::vector<boost::graph_traits<ClusterGraph>::edge_descriptor>::iterator iterMST;
            boost::graph_traits<ClusterGraph>::edge_iterator edgeIter, e_end;
            
            int i = 0;
            int feedback = num_edges(*cg)/10;
            int feedbackCounter = 0;
            
            std::cout << "Started " << std::flush;
            for (boost::tie(edgeIter, e_end) = boost::edges(*cg); edgeIter != e_end; )
            {
                if((boost::num_edges(*cg) > 10) && ((i % feedback) == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
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
            std::cout << " Complete.\n";
        }
        catch (std::exception &e)
        {
            rsgis::math::RSGISClustererException(e.what());
        }
    }
    
    RSGISGraphGeomClusterer::~RSGISGraphGeomClusterer()
    {
        
    }
    
    
    
    
    
    
    
    
    
    
	
}}

