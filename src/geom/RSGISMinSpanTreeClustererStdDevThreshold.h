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

#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISSpatialClustererInterface.h"
#include "geom/RSGISDelaunayTriangulation.h"

#include "math/RSGISClustererException.h"

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/connected_components.hpp>

using namespace std;
using namespace rsgis::math;
using namespace boost;

namespace rsgis{namespace geom{
	
	/// Class that implments the abstract interface RSGISSpatialClustererInterface.
	/// The clusterer calculates the minimum spanning tree from a delaunay triangulation 
	/// and removes the edges above a given number of standard deviations.
	
	typedef adjacency_list< vecS, vecS, undirectedS, property< vertex_name_t, string >, property< edge_weight_t, float > > ClusterGraph;
	
	class RSGISMinSpanTreeClustererStdDevThreshold : public RSGISSpatialClustererInterface
		{
		public:
			RSGISMinSpanTreeClustererStdDevThreshold(float stddevthreshold, float maxEdgeLength);
			virtual list<RSGIS2DPoint*>** clusterData(vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold) throw(RSGISClustererException);
			virtual ~RSGISMinSpanTreeClustererStdDevThreshold();
		protected:
			float stddevthreshold;
			float maxEdgeLength;
			ClusterGraph* constructGraph(RSGISDelaunayTriangulation *tri, vector<RSGIS2DPoint*> *data);
			void constructMinimumSpanningTree(ClusterGraph *cg);
		};
}}

#endif



