/*
 *  RSGISImageBandException.cpp
 *  
 *  RSGIS IMG
 *
 *	A class providing the Exception for errors associated
 *	with image bands the RSGIS library
 *
 *  Created by Pete Bunting on 10/04/2008.
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

#include "RSGISImageBandException.h"

namespace rsgis{namespace img{

	RSGISImageBandException::RSGISImageBandException() : rsgis::RSGISImageException("A RSGISImageBandException has been created..")
	{
		
	}

	RSGISImageBandException::RSGISImageBandException(const char* message) : rsgis::RSGISImageException(message)
	{

	}
	
	RSGISImageBandException::RSGISImageBandException(std::string message) : rsgis::RSGISImageException(message)
	{
		
	}
	
}} //rsgis::img
