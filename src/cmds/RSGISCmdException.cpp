/*
 *  RSGISCmdException.cpp
 *
 *  Created by Pete Bunting on 29/04/2013.
 *  Copyright 2013 RSGISLib.
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

#include "RSGISCmdException.h"

namespace rsgis { namespace cmds {
    
	RSGISCmdException::RSGISCmdException() : std::exception()
	{
		msgs = "A RSGISCmdException has been created..";
	}
    
    RSGISCmdException::RSGISCmdException(const char* message) : std::exception()
	{
		msgs = std::string(message);
	}
	
	RSGISCmdException::RSGISCmdException(std::string message) : std::exception()
	{
		msgs = message;
	}
    
	const char* RSGISCmdException::what()
	{
		return msgs.c_str();
	}
    
	RSGISCmdException::~RSGISCmdException()
	{
        
	}
    
}}
