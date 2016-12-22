/*
 *  RSGISFitPolygon2Points.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/06/2015.
 *  Copyright 2015 RSGISLib.
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

#include "RSGISFitPolygon2Points.h"

namespace rsgis{namespace geom{
    
    
    RSGISFitAlphaShapesPolygonToPoints::RSGISFitAlphaShapesPolygonToPoints()
    {
        
    }
    
    geos::geom::Polygon* RSGISFitAlphaShapesPolygonToPoints::fitPolygon(std::vector<OGRPoint*> *pts, double alphaVal) throw(RSGISGeometryException)
    {
        geos::geom::Polygon *poly = NULL;
        try
        {
            std::cout << "Create CGAL Points\n";
            std::vector<K::Point_2> points;
            for(std::vector<OGRPoint*>::iterator iterPts = pts->begin(); iterPts != pts->end(); ++iterPts)
            {
                K::Point_2 cgalPt((*iterPts)->getX(),(*iterPts)->getY());
                points.push_back(cgalPt);
            }
            std::cout << "Computer Alpha Shape\n";
            Alpha_shape_2 A(points.begin(), points.end(), FT(0), Alpha_shape_2::REGULARIZED);
            
            if(alphaVal < 0)
            {
                alphaVal = *A.find_optimal_alpha(1);
                std::cout << "Optimal Alpha = " << alphaVal << std::endl;
            }
            
            A.set_alpha(alphaVal);
            
            std::cout << "Export Segments\n";
            std::vector<K::Segment_2> segments;
            std::cout.precision(15);
            std::vector<geos::geom::LineSegment *> *lines = new std::vector<geos::geom::LineSegment *>();
            for(Alpha_shape_edges_iterator it =  A.alpha_shape_edges_begin(); it != A.alpha_shape_edges_end(); ++it)
            {
                Alpha_shape_2::Classification_type edgeType = A.classify(*it, alphaVal);
                
                K::Point_2 ptSource = A.segment(*it).source();
                K::Point_2 ptTarget = A.segment(*it).target();
                
                if(edgeType == Alpha_shape_2::REGULAR)
                {
                    lines->push_back(new geos::geom::LineSegment(geos::geom::Coordinate(ptSource.x(), ptSource.y()), geos::geom::Coordinate(ptTarget.x(), ptTarget.y())));
                }
    
            }
            
            poly = this->extractPolygonFromEdges(lines);
            
            delete lines;
            
        }
        catch (RSGISGeometryException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISGeometryException(e.what());
        }
        
        return poly;
    }
    
    geos::geom::Polygon* RSGISFitAlphaShapesPolygonToPoints::extractPolygonFromEdges(std::vector<geos::geom::LineSegment *> *lines) throw(RSGISGeometryException)
    {
        geos::geom::Polygon *poly = NULL;
        try
        {
            size_t numLines = lines->size();
            std::cout << "Num of Lines = " << numLines << std::endl;
            
            //RSGISGeomTestExport geomExport;
            //geomExport.exportGEOSLineSegments2SHP("./AlphaShapesLines.shp", true, lines);
                        
            RSGIS2DPoint *vertexA = NULL;
            bool foundA = false;
            RSGIS2DPoint *vertexB = NULL;
            bool foundB = false;
            
            std::vector<RSGIS2DPoint*> ptVerts;
            
            std::cout << "Create list of vertexes\n";
            // Create a list of vertexes with
            int count = 0;
            int feedback = numLines/10;
            int feedbackCounter = 0;
            std::cout << "Started " << std::flush;
            for (std::vector<geos::geom::LineSegment *>::iterator iterLines = lines->begin(); iterLines != lines->end(); ++iterLines)
            {
                if((numLines > 10) && ((count % feedback) == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                foundA = false;
                vertexA = new RSGIS2DPoint(new geos::geom::Coordinate((*iterLines)->p0.x, (*iterLines)->p0.y));
                foundB = false;
                vertexB = new RSGIS2DPoint(new geos::geom::Coordinate((*iterLines)->p1.x, (*iterLines)->p1.y));
                
                if(!ptVerts.empty())
                {
                    for(std::vector<RSGIS2DPoint*>::iterator iterPts = ptVerts.begin(); iterPts != ptVerts.end(); ++iterPts)
                    {
                        if((!foundA) && ((*iterPts)->equals(vertexA)))
                        {
                            delete vertexA;
                            vertexA = *iterPts;
                            foundA = true;
                        }
                        
                        if((!foundB) && ((*iterPts)->equals(vertexB)))
                        {
                            delete vertexB;
                            vertexB = *iterPts;
                            foundB = true;
                        }
                        
                        if(foundA & foundB)
                        {
                            break;
                        }
                    }
                }
                
                if(!foundA)
                {
                    vertexA->setIndex(ptVerts.size());
                    ptVerts.push_back(vertexA);
                }
                
                if(!foundB)
                {
                    vertexB->setIndex(ptVerts.size());
                    ptVerts.push_back(vertexB);
                }
                
                ++count;
            }
            std::cout << " Complete.\n";
            
            size_t numVerts = ptVerts.size();
            std::cout << "There are " << numVerts << " vertexs\n";
            
            std::cout << "Create and populate graph\n";
            // Create Graph
            EdgesGraph edgesGraph = EdgesGraph(numVerts);
            
            geos::geom::Coordinate coordA;
            geos::geom::Coordinate coordB;
            
            size_t numEdgesAdded = 0;
            count = 0;
            feedback = numLines/10;
            feedbackCounter = 0;
            std::cout << "Started " << std::flush;
            vertexDescr vA, vB;
            EdgesGraph::edge_descriptor eDescript;
            bool edgePresent = false;
            for (std::vector<geos::geom::LineSegment *>::iterator iterLines = lines->begin(); iterLines != lines->end(); ++iterLines)
            {
                if((numLines > 10) && ((count % feedback) == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                coordA = geos::geom::Coordinate((*iterLines)->p0.x, (*iterLines)->p0.y);
                coordB = geos::geom::Coordinate((*iterLines)->p1.x, (*iterLines)->p1.y);
                
                foundA = false;
                foundB = false;
                
                for(std::vector<RSGIS2DPoint*>::iterator iterPts = ptVerts.begin(); iterPts != ptVerts.end(); ++iterPts)
                {
                    if((!foundA) && ((*iterPts)->equals(&coordA)))
                    {
                        vertexA = *iterPts;
                        foundA = true;
                    }
                    
                    if((!foundB) && ((*iterPts)->equals(&coordB)))
                    {
                        vertexB = *iterPts;
                        foundB = true;
                    }
                    
                    if(foundA & foundB)
                    {
                        break;
                    }
                }
                
                if(!foundA)
                {
                    std::cout << "Vertex A: [" << coordA.x << ", " << coordA.y << "]\n";
                    throw RSGISGeometryException("Could not find vertex A in list");
                }
                
                if(!foundB)
                {
                    std::cout << "Vertex B: [" << coordB.x << ", " << coordB.y << "]\n";
                    throw RSGISGeometryException("Could not find vertex B in list");
                }
                
                edgePresent = false;
                vA = boost::vertex(vertexA->getIndex(), edgesGraph);
                vB = boost::vertex(vertexB->getIndex(), edgesGraph);
                boost::tie(eDescript, edgePresent) = boost::edge(vA, vB, edgesGraph);
                
                if(!edgePresent)
                {
                    boost::add_edge(vertexA->getIndex(), vertexB->getIndex(), vertexA->distance(vertexB), edgesGraph);
                    ++numEdgesAdded;
                }
                
                ++count;
            }
            std::cout << " Complete.\n";
            
            std::cout << numEdgesAdded << " edges were added to the graph\n";
            
            // Find Connected Components
            std::vector<int> component(boost::num_vertices(edgesGraph));
            int num_comp = boost::connected_components(edgesGraph, boost::make_iterator_property_map(component.begin(), boost::get(boost::vertex_index, edgesGraph)));
            std::cout << "There are " << num_comp << " components (i.e., boundary + holes)\n";
            
            
            
            if(ptVerts.size() != component.size())
            {
                std::cout << "ptVerts.size() = " << ptVerts.size() << std::endl;
                std::cout << "component.size() = " << component.size() << std::endl;
                throw RSGISGeometryException("Number of vertexs and graph elements are different.");
            }
            
            // Create geos::geom::CoordinateSequence objects for each component
            std::vector<geos::geom::CoordinateSequence*> compCoords;
            
            long *compIdxs = new long[num_comp];
            long *compNumGt2Edges = new long[num_comp];
            for(size_t i = 0; i < num_comp; ++i)
            {
                compIdxs[i] = -1;
                compNumGt2Edges[i] = 0;
                compCoords.push_back(new geos::geom::CoordinateArraySequence());
            }
            
            // iterate using numbers ++i etc...
            for(size_t i = 0; i < ptVerts.size(); ++i)
            {
                ptVerts.at(i)->setClassID(component.at(i));
                if(compIdxs[component.at(i)] == -1)
                {
                    compIdxs[component.at(i)] = i;
                }
                if(edgesGraph.m_vertices.at(i).m_out_edges.size() > 2)
                {
                    ++compNumGt2Edges[component.at(i)];
                }
            }
            
            bool skip = false;
            std::vector<geos::geom::CoordinateSequence*> *loops = new std::vector<geos::geom::CoordinateSequence*>();
            boost::graph_traits<EdgesGraph>::adjacency_iterator vi, vi_end;
            for(size_t i = 0; i < num_comp; ++i)
            {
                if(compNumGt2Edges[i] == 0)
                {
                    vertexDescr startV = boost::vertex(compIdxs[i], edgesGraph);
                    std::vector<int> mark(ptVerts.size(), 0);
                    mark.at(startV) = 1;
                    compCoords.at(i)->add(ptVerts.at(startV)->getCoordPoint(), true);
                    loops->clear();
                    
                    for(tie(vi,vi_end) = boost::adjacent_vertices(startV, edgesGraph); vi != vi_end; ++vi)
                    {
                        if(mark.at(*vi) == 0)
                        {
                            mark.at(*vi) = 1;
                            compCoords.at(i)->add(ptVerts.at(*vi)->getCoordPoint(), true);
                            this->createCoordSequence(&edgesGraph, *vi, &mark, &ptVerts, compCoords.at(i), loops);
                            break;
                        }
                    }
                    compCoords.at(i)->add(ptVerts.at(startV)->getCoordPoint(), true); // Close the ring...
                    
                    if(loops->size() > 0)
                    {
                        for(size_t j = 0; j < loops->size(); ++j)
                        {
                            compCoords.push_back(loops->at(j));
                        }
                        loops->clear();
                    }

                }
                else if(compNumGt2Edges[i] == 1)
                {
                    for(size_t j = 0; j < ptVerts.size(); ++j)
                    {
                        if((component.at(j) == i) && (edgesGraph.m_vertices.at(j).m_out_edges.size() == 4))
                        {
                            compIdxs[i] = j;
                            break;
                        }
                    }
                    
                    vertexDescr startV = boost::vertex(compIdxs[i], edgesGraph);
                    std::vector<int> mark(ptVerts.size(), 0);
                    mark.at(startV) = 1;
                    compCoords.at(i)->add(ptVerts.at(startV)->getCoordPoint(), true);
                    loops->clear();
                    
                    for(tie(vi,vi_end) = boost::adjacent_vertices(startV, edgesGraph); vi != vi_end; ++vi)
                    {
                        if(mark.at(*vi) == 0)
                        {
                            mark.at(*vi) = 1;
                            compCoords.at(i)->add(ptVerts.at(*vi)->getCoordPoint(), true);
                            this->createCoordSequence(&edgesGraph, *vi, &mark, &ptVerts, compCoords.at(i), loops);
                            compCoords.at(i)->add(ptVerts.at(startV)->getCoordPoint(), true); // Close the ring...
                            loops->push_back(compCoords.at(i));
                            compCoords.at(i) = new geos::geom::CoordinateArraySequence();
                            compCoords.at(i)->add(ptVerts.at(startV)->getCoordPoint(), true);
                        }
                    }
                    
                    
                    if(loops->size() > 0)
                    {
                        for(size_t j = 0; j < loops->size(); ++j)
                        {
                            compCoords.push_back(loops->at(j));
                        }
                        loops->clear();
                    }
                }
                else if(compNumGt2Edges[i] > 1)
                {
                    std::cerr << "WARNING: The number of component edges with more than 2 edges is greater than 1 - an implementation for this has not yet been done...\n";
                    skip = true;
                    //throw RSGISGeometryException("The number of component edges with more than 2 edges is greater than 1 - an implementation for this has not yet been done...");
                }
            }
            
            if(!skip)
            {
                geos::geom::GeometryFactory *geomFactory = rsgis::utils::RSGISGEOSFactoryGenerator::getInstance()->getFactory();
                if( compCoords.size() > 1)
                {
                    int maxIdx = -1;
                    geos::geom::Envelope maxEnv;
                    geos::geom::Envelope curEnv;
                    for(size_t i = 0; i < compCoords.size(); ++i)
                    {
                        if(i == 0)
                        {
                            maxIdx = i;
                            maxEnv.setToNull();
                            compCoords.at(i)->expandEnvelope(maxEnv);
                        }
                        else
                        {
                            curEnv.setToNull();
                            compCoords.at(i)->expandEnvelope(curEnv);
                            if(curEnv.covers(maxEnv))
                            {
                                maxIdx = i;
                                maxEnv = curEnv;
                            }
                        }
                    }
                    
                    std::cout << "Component " << maxIdx << " is the polygon boundary the rest are holes\n";
                    
                    geos::geom::LinearRing *ring = NULL;
                    std::vector<geos::geom::Geometry*> *holes = new std::vector<geos::geom::Geometry*>();
                    for(size_t i = 0; i < compCoords.size(); ++i)
                    {
                        if(i == maxIdx)
                        {
                            ring = geomFactory->createLinearRing(compCoords.at(i));
                        }
                        else
                        {
                            if(compCoords.at(i)->size() > 3)
                            {
                                holes->push_back(geomFactory->createLinearRing(compCoords.at(i)));
                            }
                        }
                    }
                    poly = geomFactory->createPolygon(ring, holes);
                }
                else
                {
                    geos::geom::LinearRing *ring = geomFactory->createLinearRing(compCoords.at(0));
                    std::vector<geos::geom::Geometry*> *holes = new std::vector<geos::geom::Geometry*>();
                    poly = geomFactory->createPolygon(ring, holes);
                }
            }
            
            // Clean Up...
            for(std::vector<RSGIS2DPoint*>::iterator iterVerts = ptVerts.begin(); iterVerts != ptVerts.end(); ++iterVerts)
            {
                delete (*iterVerts);
            }
        }
        catch (RSGISGeometryException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISGeometryException(e.what());
        }
        
        return poly;
    }
    
    void RSGISFitAlphaShapesPolygonToPoints::createCoordSequence(EdgesGraph *edgesGraph, vertexDescr currV, std::vector<int> *mark, std::vector<RSGIS2DPoint*> *ptVerts, geos::geom::CoordinateSequence *coords, std::vector<geos::geom::CoordinateSequence*> *loops)
    {
        try
        {
            boost::graph_traits<EdgesGraph>::adjacency_iterator vi, vi_end;
            for(tie(vi,vi_end) = boost::adjacent_vertices(currV, *edgesGraph); vi != vi_end; ++vi)
            {
                if(mark->at(*vi) == 0)
                {
                    mark->at(*vi) = 1;
                    coords->add(ptVerts->at(*vi)->getCoordPoint(), true);
                    this->createCoordSequence(edgesGraph, *vi, mark, ptVerts, coords, loops);
                    break; // Test to process just the first vertex (if there is more than one...)
                }
            }
        }
        catch (RSGISGeometryException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISGeometryException(e.what());
        }
    }
    
    RSGISFitAlphaShapesPolygonToPoints::~RSGISFitAlphaShapesPolygonToPoints()
    {
        
    }
    
}}



