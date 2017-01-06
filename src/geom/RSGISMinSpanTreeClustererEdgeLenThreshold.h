/*
 *  RSGISMinSpanTreeClustererEdgeLenThreshold.h
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

#ifndef RSGISMinSpanTreeClustererEdgeLenThreshold_H
#define RSGISMinSpanTreeClustererEdgeLenThreshold_H

#include <iostream>
#include <string>
#include <list>

#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISSpatialClustererInterface.h"
#include "geom/RSGISDelaunayTriangulation.h"

#include "math/RSGISClustererException.h"

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/connected_components.hpp>

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
	
	/// Class that implments the abstract interface RSGISSpatialClustererInterface.
	/// The clusterer calculates the minimum spanning tree from a delaunay triangulation 
	/// and removes the edges above a length threshold.
	
	typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS, boost::property< boost::vertex_name_t, std::string >, boost::property< boost::edge_weight_t, float > > ClusterGraph;
	
	class DllExport RSGISMinSpanTreeClustererEdgeLenThreshold : public RSGISSpatialClustererInterface
		{
		public:
			RSGISMinSpanTreeClustererEdgeLenThreshold(float lengththreshold);
			virtual std::list<RSGIS2DPoint*>** clusterData(std::vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold) throw(rsgis::math::RSGISClustererException);
			void updateLengthThreshold(float lengththreshold);
			virtual ~RSGISMinSpanTreeClustererEdgeLenThreshold();
		protected:
			float lengththreshold;
			ClusterGraph* constructGraph(RSGISDelaunayTriangulation *tri, std::vector<RSGIS2DPoint*> *data);
			void constructMinimumSpanningTree(ClusterGraph *cg);
		};
}}

#endif



