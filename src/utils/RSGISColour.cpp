/*
 *  RSGISColour.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/08/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#include "RSGISColour.h"

namespace rsgis{namespace utils{
	
	RSGISColour::RSGISColour()
	{
		this->red = 0;
		this->green = 0;
		this->blue = 0;
		this->alpha = 0;
	}
	
	RSGISColour::RSGISColour(double r, double g, double b, double a)
	{
		this->red = r;
		this->green = g;
		this->blue = b;
		this->alpha = a;
	}
	
	double RSGISColour::getRed()
	{
		return red;
	}
	
	double RSGISColour::getGreen()
	{
		return green;
	}
	
	double RSGISColour::getBlue()
	{
		return blue;
	}
	
	double RSGISColour::getAlpha()
	{
		return alpha;
	}
	
	RSGISColour::~RSGISColour()
	{
		
	}
    
    
    RSGISColourInt::RSGISColourInt()
	{
		this->red = 0;
		this->green = 0;
		this->blue = 0;
		this->alpha = 0;
	}
	
	RSGISColourInt::RSGISColourInt(int r, int g, int b, int a)
	{
		this->red = r;
		this->green = g;
		this->blue = b;
		this->alpha = a;
	}
	
	int RSGISColourInt::getRed()
	{
		return red;
	}
	
	int RSGISColourInt::getGreen()
	{
		return green;
	}
	
	int RSGISColourInt::getBlue()
	{
		return blue;
	}
	
	int RSGISColourInt::getAlpha()
	{
		return alpha;
	}
	
	RSGISColourInt::~RSGISColourInt()
	{
		
	}
    
}}

