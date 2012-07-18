/*
 *  RSGISFFTException.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/12/2008.
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

#include "RSGISFFTException.h"

namespace rsgis{namespace img{
	
	RSGISFFTException::RSGISFFTException() : rsgis::RSGISImageException("A RSGISFFTException has been created..")
	{
		
	}
	
	RSGISFFTException::RSGISFFTException(const char* message) : rsgis::RSGISImageException(message)
	{
		
	}
    
    RSGISFFTException::RSGISFFTException(std::string message) : rsgis::RSGISImageException(message)
	{
		
	}
	
}} //rsgis::img

