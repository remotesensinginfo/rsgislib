/*
 *  RSGISVectorException.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/04/2008.
 *  Copyright 2008 RSGISLib.
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

#include "RSGISVectorException.h"

namespace rsgis {
	
	RSGISVectorException::RSGISVectorException() : RSGISException("A RSGISVectorException has been created..")
	{

	}
	
	
	RSGISVectorException::RSGISVectorException(const char* message) : RSGISException(message)
	{
		
	}
	
	RSGISVectorException::RSGISVectorException(std::string message) : RSGISException(message)
	{
		
	}
	
} //rsgis

