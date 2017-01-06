/*
 *  RSGISGraph.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/06/2008.
 *  Copyright 2008 Aberystwyth University. All rights reserved.
 *
 */

#ifndef RSGISGraph_H
#define RSGISGraph_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <math.h>
#include "datastruct/RSGISGraphData.h"
#include "datastruct/RSGISGraphException.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/Envelope.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/CoordinateSequence.h"
#include "utils/RSGISAbstractOutlinePolygon.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis::utils;

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_datastruct_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace datastruct{	
	
	class DllExport RSGISClusteringGraph
		{
		public:
			RSGISClusteringGraph();
			void addVertex(RSGISGraphVertex *vertex)throw(RSGISGraphException);
			void removeVertex(RSGISGraphVertex *vertex) throw(RSGISGraphException);
			void breathFirstSearch(list<RSGISGraphVertex*> *outVertexes) throw(RSGISGraphException);
			void findMinimumSpanningTree() throw(RSGISGraphException);
			void removeUnmarkedEdges() throw(RSGISGraphException);
			void removeMarkedEdges() throw(RSGISGraphException);
			void resetVisitedTags();
			void resetMarkedEdges();
			void resetMarkedEdgesVertexes();
			void markEdgesGtThreshold(double threshold);
			double meanEdgeWeight();
			double stddevEdgeWeight(double mean);
			void printGraph();
			int getSize();
			int nextID();
			~RSGISClusteringGraph();
		protected:
			bool uniqueID(RSGISGraphVertex *vertex);
			list<RSGISGraphVertex*> *vertexes;
			int idcounter;
		};
	
	class DllExport RSGISSpatialClusteringGraph : public RSGISClusteringGraph
	{
	public: 
		RSGISSpatialClusteringGraph(RSGISAbstractOutlinePolygon *outlineGenerator);
		void addVertex(RSGISSpatialGraphVertex *vertex)throw(RSGISGraphException);
		void removeVertex(RSGISSpatialGraphVertex *vertex)throw(RSGISGraphException);
		Envelope* getGraphExtent();
		vector<Geometry*>* getGeometries();
		MultiPolygon* getMultiPolygon();
		Polygon* getGraphOutline();
		RSGISAbstractOutlinePolygon* getOutlineGenerator();
	protected:
		RSGISAbstractOutlinePolygon *outlineGenerator;
	};
}}

#endif
