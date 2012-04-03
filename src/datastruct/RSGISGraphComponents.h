/*
 *  RSGISGraphComponents.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/06/2008.
 *  Copyright 2008 Aberystwyth University. All rights reserved.
 *
 */


#ifndef RSGISGraphComponents_H
#define RSGISGraphComponents_H

#include <iostream>
#include <string>
#include <list>
#include "datastruct/RSGISGraph.h"

using namespace std;

namespace rsgis 
{
	namespace datastruct
	{
		class RSGISGraphComponents
			{
			public:
				RSGISGraphComponents();
				void generateComponents(RSGISClusteringGraph *graph);
				void generateComponents();
				list<RSGISClusteringGraph*>* getComponents();
				~RSGISGraphComponents();
			protected:
				list<RSGISClusteringGraph*> *components;
			};
		
		class RSGISSpatialGraphComponents
			{
			public:
				RSGISSpatialGraphComponents();
				void generateComponents(RSGISSpatialClusteringGraph *graph);
				void generateComponents();
				list<RSGISSpatialClusteringGraph*>* getComponents();
				~RSGISSpatialGraphComponents();
			protected:
				list<RSGISSpatialClusteringGraph*> *components;
			};
	}
}

#endif



