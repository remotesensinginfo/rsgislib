/*
 *  RSGISClumpRegionGrowing.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/08/2014.
 *  Copyright 2014 RSGISLib.
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

#ifndef RSGISClumpRegionGrowing_H
#define RSGISClumpRegionGrowing_H

#include <string>
#include <vector>
#include <math.h>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISLogicExpEvaluation.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISBinaryClassifyClumps.h"


namespace rsgis{namespace rastergis{
    
    class DllExport RSGISClumpRegionGrowing
    {
    public:
        RSGISClumpRegionGrowing();
        void growClassRegion(GDALDataset *inputClumps, std::string classColumn, std::string classVal, int maxIter, unsigned int ratBand, std::string xmlBlock)throw(RSGISAttributeTableException);
        void growClassRegionNeighCriteria(GDALDataset *inputClumps, std::string classColumn, std::string classVal, int maxIter, unsigned int ratBand, std::string xmlBlockCriteria, std::string xmlBlockNeighCriteria)throw(RSGISAttributeTableException);
        ~RSGISClumpRegionGrowing();
    };
    
}}

#endif

