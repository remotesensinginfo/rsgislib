/*
 *  RSGISMinSpanTreeClustererStdDevThreshold.h
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

#ifndef RSGISMinSpanTreeClustererStdDevThreshold_H
#define RSGISMinSpanTreeClustererStdDevThreshold_H

#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <iterator>

#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISSpatialClustererInterface.h"
#include "geom/RSGISDelaunayTriangulation.h"

#include "math/RSGISClustererException.h"

#include "utils/RSGISExportData2HDF.h"

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/connected_components.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Interpolation_traits_2.h>
#include <CGAL/natural_neighbor_coordinates_2.h>
#include <CGAL/interpolation_functions.h>
#include <CGAL/algorithm.h>
#include <CGAL/Origin.h>
#include <CGAL/squared_distance_2.h>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_geom_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace geom{

    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef K::FT                                         CGALCoordType;
    typedef K::Vector_2                                   CGALVector;
    typedef K::Point_2                                    CGALPoint;
    
    typedef CGAL::Delaunay_triangulation_2<K>             DelaunayTriangulation;
    typedef CGAL::Interpolation_traits_2<K>               InterpTraits;
    typedef CGAL::Delaunay_triangulation_2<K>::Vertex_handle    Vertex_handle;
    
    typedef std::vector< std::pair<CGALPoint, CGALCoordType> >   CoordinateVector;
    typedef std::map<CGALPoint, CGALCoordType, K::Less_xy_2>     PointValueMap;
    
	/// Class that implments the abstract interface RSGISSpatialClustererInterface.
	/// The clusterer calculates the minimum spanning tree from a delaunay triangulation 
	/// and removes the edges above a given number of standard deviations.
	
	typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS, boost::property< boost::vertex_name_t, std::string >, boost::property< boost::edge_weight_t, float > > ClusterGraph;
	
	class DllExport RSGISMinSpanTreeClustererStdDevThreshold : public RSGISSpatialClustererInterface
		{
		public:
			RSGISMinSpanTreeClustererStdDevThreshold(float stddevthreshold, float maxEdgeLength);
			virtual std::list<RSGIS2DPoint*>** clusterData(std::vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold);
			virtual ~RSGISMinSpanTreeClustererStdDevThreshold();
		protected:
			float stddevthreshold;
			float maxEdgeLength;
			ClusterGraph* constructGraph(RSGISDelaunayTriangulation *tri, std::vector<RSGIS2DPoint*> *data);
			void constructMinimumSpanningTree(ClusterGraph *cg);
		};
    
    
    class DllExport RSGISGraphGeomClusterer : public RSGISSpatialClustererInterface
    {
    public:
        RSGISGraphGeomClusterer(bool useMinSpanTree, float stddevthreshold, double maxEdgeLength, std::string shpFileEdges, bool outShpEdges, std::string h5EdgeLengths, bool outH5EdgeLens);
        virtual std::list<RSGIS2DPoint*>** clusterData(std::vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold);
        virtual ~RSGISGraphGeomClusterer();
    protected:
        bool useMinSpanTree;
        float stddevthreshold;
        double maxEdgeLength;
        std::string shpFileEdges;
        bool outShpEdges;
        std::string h5EdgeLengths;
        bool outH5EdgeLens;
        ClusterGraph* constructGraph(DelaunayTriangulation *dt, PointValueMap *values, std::vector<RSGIS2DPoint*> *data);
        void constructMinimumSpanningTree(ClusterGraph *cg);
    };
    
    
}}

#endif



