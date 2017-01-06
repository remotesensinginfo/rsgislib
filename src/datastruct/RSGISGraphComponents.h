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

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_datastruct_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis 
{
	namespace datastruct
	{
		class DllExport RSGISGraphComponents
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
		
		class DllExport RSGISSpatialGraphComponents
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



