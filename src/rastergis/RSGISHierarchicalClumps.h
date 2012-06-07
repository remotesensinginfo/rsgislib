/*
 *  RSGISHierarchicalClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/02/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISHierarchicalClumps_H
#define RSGISHierarchicalClumps_H

#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <list>

#include "common/RSGISImageException.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

namespace rsgis{namespace rastergis{
    
    using namespace std;
    using namespace rsgis;
    
    struct RSGISSubClumps
    {
        double area;
        double tlX;
        double tlY;
        double brX;
        double brY;
        unsigned int minPxlX;
        unsigned int minPxlY;
        unsigned int maxPxlX;
        unsigned int maxPxlY;
        list<unsigned int> subclumps;
    };
    
    class RSGISHierarchicalClumps
    {
    public:
        RSGISHierarchicalClumps();
        vector<RSGISSubClumps*>* findSubClumps(GDALDataset *catagories, GDALDataset *clumps, bool noDataValProvided, unsigned int noDataVal) throw(RSGISImageException);
        ~RSGISHierarchicalClumps();
    };
    
}}

#endif




