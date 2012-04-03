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

namespace rsgis 
{
	namespace datastruct
	{
		class RSGISGraphException : public RSGISException
			{
			public:
				RSGISGraphException();
				RSGISGraphException(const char* message);
				virtual const char* what();
			};
	}
}

#endif

