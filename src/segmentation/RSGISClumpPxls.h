/*
 *  RSGISClumpPxls.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/01/2012.
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

#ifndef RSGISClumpPxls_h
#define RSGISClumpPxls_h

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <math.h>

#include "gdal_priv.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"

using namespace std;
using namespace rsgis::img;

namespace rsgis{namespace segment{
    
    class RSGISClumpPxls
    {
    public:
        RSGISClumpPxls();
        void performClump(GDALDataset *catagories, GDALDataset *clumps, bool noDataValProvided, unsigned int noDataVal) throw(RSGISImageCalcException);
        ~RSGISClumpPxls();
    };
    
    class RSGISRelabelClumps
    {
    public:
        RSGISRelabelClumps();
        void relabelClumps(GDALDataset *catagories, GDALDataset *clumps) throw(RSGISImageCalcException);
        ~RSGISRelabelClumps();
    };
    
}}

#endif
