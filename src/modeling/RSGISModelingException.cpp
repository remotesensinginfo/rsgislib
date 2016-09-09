/*
 *  RSGISModelingException.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 17/11/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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


#include "RSGISModelingException.h"

namespace rsgis{namespace modeling{
	
	RSGISModelingException::RSGISModelingException() : rsgis::RSGISException("A RSGISClustererException has been created..")
	{
		
	}
	
	RSGISModelingException::RSGISModelingException(const char* message) : rsgis::RSGISException(message)
	{
		
	}
	
	RSGISModelingException::RSGISModelingException(std::string message) : rsgis::RSGISException(message)
	{
		
	}
	
}}

