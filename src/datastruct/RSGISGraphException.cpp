/*
 *  RSGISGraphException.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/06/2008.
 *  Copyright 2008 Aberystwyth University. All rights reserved.
 *
 */

#include "RSGISGraphException.h"

namespace rsgis {namespace datastruct{
	
	RSGISGraphException::RSGISGraphException() : RSGISException("A RSGISGraphException has been created..")
	{
		
	}
	
	
	RSGISGraphException::RSGISGraphException(const char* message) : RSGISException(message)
	{
		
	}
	
	const char* RSGISGraphException::what()
	{
		return msgs;
	}
	
}} //rsgis::datastruct
