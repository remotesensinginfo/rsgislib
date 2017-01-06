/*
 *  RSGISGraphException.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/06/2008.
 *  Copyright 2008 Aberystwyth University. All rights reserved.
 *
 */

#ifndef RSGISGraphException_H
#define RSGISGraphException_H

#include "common/RSGISException.h"

using namespace std;

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

namespace rsgis 
{
	namespace datastruct
	{
		class DllExport RSGISGraphException : public RSGISException
			{
			public:
				RSGISGraphException();
				RSGISGraphException(const char* message);
				virtual const char* what();
			};
	}
}

#endif

