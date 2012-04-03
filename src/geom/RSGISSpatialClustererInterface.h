/*
 *  RSGISSpatialClustererInterface.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/08/2009.
 *  Copyright 2009 RSGISLib.
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

#ifndef RSGISSpatialClustererInterface_H
#define RSGISSpatialClustererInterface_H

#include <iostream>
#include <string>
#include <list>

#include "geom/RSGIS2DPoint.h"

#include "math/RSGISClustererException.h"

using namespace std;
using namespace rsgis::math;

namespace rsgis{namespace geom{
	/// Abstract interface to implement Spatial Clusterers
	class RSGISSpatialClustererInterface
		{
		public:
			virtual list<RSGIS2DPoint*>** clusterData(vector<RSGIS2DPoint*> *data, int *numclusters, double *threshold) throw(RSGISClustererException) = 0;
			virtual ~RSGISSpatialClustererInterface(){};
		};
}}

#endif




