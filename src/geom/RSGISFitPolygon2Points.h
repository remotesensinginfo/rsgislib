/*
 *  RSGISFitPolygon2Points.h
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

#ifndef RSGISFitPolygon2Points_H
#define RSGISFitPolygon2Points_H

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "geom/RSGISPolygon.h"
#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISGeometryException.h"
#include "geom/RSGISGeomTestExport.h"

#include "geos/geom/Polygon.h"
#include "geos/geom/LineSegment.h"

#include "utils/RSGISGEOSFactoryGenerator.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/algorithm.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Alpha_shape_2.h>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/connected_components.hpp>

namespace rsgis{namespace geom{

    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef K::FT FT;
    //typedef K::Point_2  Point;
    //typedef K::Segment_2  Segment;
    typedef CGAL::Alpha_shape_vertex_base_2<K> Vb;
    typedef CGAL::Alpha_shape_face_base_2<K>  Fb;
    typedef CGAL::Triangulation_data_structure_2<Vb,Fb> Tds;
    typedef CGAL::Delaunay_triangulation_2<K,Tds> Triangulation_2;
    typedef CGAL::Alpha_shape_2<Triangulation_2>  Alpha_shape_2;
    typedef Alpha_shape_2::Alpha_shape_edges_iterator Alpha_shape_edges_iterator;
    
    typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS, boost::property< boost::vertex_name_t, std::string >, boost::property< boost::edge_weight_t, float > > EdgesGraph;
    typedef boost::graph_traits<EdgesGraph>::vertex_descriptor vertexDescr;
    typedef boost::graph_traits<EdgesGraph>::edge_descriptor edgeDescr;
    
    class DllExport RSGISFitAlphaShapesPolygonToPoints
    {
    public:
        RSGISFitAlphaShapesPolygonToPoints();
        geos::geom::Polygon* fitPolygon(std::vector<OGRPoint*> *pts, double alphaVal) throw(RSGISGeometryException);
        ~RSGISFitAlphaShapesPolygonToPoints();
    protected:
        geos::geom::Polygon* extractPolygonFromEdges(std::vector<geos::geom::LineSegment *> *lines) throw(RSGISGeometryException);
        void createCoordSequence(EdgesGraph *edgesGraph, vertexDescr currV, std::vector<int> *mark, std::vector<RSGIS2DPoint*> *ptVerts, geos::geom::CoordinateSequence *coords, std::vector<geos::geom::CoordinateSequence*> *loops);
    };
    
}}

#endif


