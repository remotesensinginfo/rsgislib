/*
 *  RSGISException.cpp
 *
 *  RSGIS Common
 *
 *	A class providing the base Exception for the 
 *	the RSGIS library
 *
 *  Created by Pete Bunting on 04/02/2008.
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

#include "RSGISException.h"

namespace rsgis
{

	RSGISException::RSGISException() : std::exception()
	{
		msgs = "A RSGISException has been created..";
	}

	RSGISException::RSGISException(const char* message) : exception()
	{
		msgs = std::string(message);
	}
	
	RSGISException::RSGISException(std::string message) : std::exception()
	{
		msgs = message;
	}

	const char* RSGISException::what() const throw()
	{
		return msgs.c_str();
	}

	RSGISException::~RSGISException() throw()
	{
		// do nothing
	}

}
