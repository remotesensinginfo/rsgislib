/*
 *  RSGISGenMeanSegImage.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/01/2012.
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

#ifndef RSGISGenMeanSegImage_h
#define RSGISGenMeanSegImage_h

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <math.h>

#include "gdal_priv.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"

namespace rsgis{namespace segment{
    
    using namespace std;
    using namespace rsgis::img;
    
    class RSGISGenMeanSegImage
    {
    public:
        RSGISGenMeanSegImage();
        void generateMeanImage(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *meanImg) throw(RSGISImageCalcException);
        void generateMeanImageUsingClumpTable(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *meanImg) throw(RSGISImageCalcException);
        ~RSGISGenMeanSegImage();
    };
    
}}

#endif
