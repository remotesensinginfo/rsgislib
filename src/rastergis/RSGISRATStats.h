/*
 *  RSGISRATStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/08/2015.
 *  Copyright 2015 RSGISLib.
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

#ifndef RSGISRATStats_H
#define RSGISRATStats_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "common/RSGISAttributeTableException.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "math/RSGISMathsUtils.h"


namespace rsgis{namespace rastergis{
    
    class DllExport RSGISRATStats
    {
    public:
        RSGISRATStats();
        float calc1DJMDistance(GDALDataset *clumpsImage, std::string varCol, float binWidth, std::string classColumn, std::string class1Val, std::string class2Val, unsigned int ratBand) throw(RSGISAttributeTableException);
        float calc2DJMDistance(GDALDataset *clumpsImage, std::string var1Col, std::string var2Col, float var1binWidth, float var2binWidth, std::string classColumn, std::string class1Val, std::string class2Val, unsigned int ratBand) throw(RSGISAttributeTableException);
        float calcBhattacharyyaDistance(GDALDataset *clumpsImage, std::string varCol, std::string classColumn, std::string class1Val, std::string class2Val, unsigned int ratBand) throw(RSGISAttributeTableException);
        ~RSGISRATStats();
    };
    
}}

#endif


