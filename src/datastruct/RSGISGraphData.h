/*
 *  RSGISGraphData.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/06/2008.
 *  Copyright 2008 Aberystwyth University. All rights reserved.
 *
 */

#ifndef RSGISGraphData_H
#define RSGISGraphData_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "datastruct/RSGISGraphException.h"
#include "geos/geom/Geometry.h"

using namespace std;
using namespace geos::geom;

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
	
	class RSGISGraphVertex;
	class DllExport RSGISGraphEdge
	{
	public:
		RSGISGraphEdge();
		RSGISGraphEdge(RSGISGraphVertex *nodeA, RSGISGraphVertex *nodeB, double weight);
		void setNodeA(RSGISGraphVertex *node);
		void setNodeB(RSGISGraphVertex *node);
		void setWeight(double weight);
		RSGISGraphVertex* getNodeA();
		RSGISGraphVertex* getNodeB();
		RSGISGraphVertex* getOppositeNode(RSGISGraphVertex *node);
		RSGISGraphVertex* getNotMarkedNode();
		double getWeight();
		void setMarked(bool marked);
		bool getMarked();
		~RSGISGraphEdge();
		bool operator==(RSGISGraphEdge *edge) const;
	protected:
		RSGISGraphVertex *nodeA;
		RSGISGraphVertex *nodeB;
		double weight;
		bool marked;
	};
	
	class DllExport RSGISGraphVertex
		{
		public:
			RSGISGraphVertex(int id);
			RSGISGraphVertex(int id, string name);
			virtual double edgeWeight(RSGISGraphVertex *node) throw(RSGISGraphException)=0;
			virtual bool createEdge(RSGISGraphVertex *node) throw(RSGISGraphException)=0;
			void addEdge(RSGISGraphEdge *edge);
			void removeEdge(RSGISGraphEdge *edge);
			list<RSGISGraphEdge*>* getEdges();
			RSGISGraphEdge* findShortestEdgeNoVisit();
			void removeUnmarkedEdges() throw(RSGISGraphException);
			void removeMarkedEdges() throw(RSGISGraphException);
			void setVisited(bool visited);
			bool getVisited();
			void resetEdges();
			void getSumEdgeWeights(double *sum, int *numEdges);
			void setSumSqDiffEdgeWeights(double *sum, int *numEdges, double mean);
			void markEdgesGtThreshold(double threshold);
			string getName();
			int getID();
			void printVertex();
			virtual ~RSGISGraphVertex();
			bool operator==(RSGISGraphVertex *vertex) const;
		protected:
			int id;
			string name;
			bool visited;
			list<RSGISGraphEdge*> *edges;
		};
	
	class DllExport RSGISSpatialGraphVertex : public RSGISGraphVertex
	{
	public:
		RSGISSpatialGraphVertex(int id) : RSGISGraphVertex(id){};
		virtual double edgeWeight(RSGISGraphVertex *node) throw(RSGISGraphException)=0;
		virtual bool createEdge(RSGISGraphVertex *node) throw(RSGISGraphException)=0;
		virtual Geometry* getGeometry()=0;
	};
}}

#endif

