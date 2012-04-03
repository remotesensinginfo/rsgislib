/*
 *  RSGISColour.h
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

#ifndef RSGISColour_H
#define RSGISColour_H

#include <string>
#include <iostream>

using namespace std;

namespace rsgis{namespace utils{

	class RSGISColour
		{
		public:
			RSGISColour();
			RSGISColour(double r, double g, double b, double a);
			double getRed();
			double getGreen();
			double getBlue();
			double getAlpha();
			~RSGISColour();
		protected:
			double red;
			double green;
			double blue;
			double alpha;
		};
}}

#endif
