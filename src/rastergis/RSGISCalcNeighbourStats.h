/*
 *  RSGISCalcNeighbourStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/08/2014.
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

#ifndef RSGISCalcNeighbourStats_H
#define RSGISCalcNeighbourStats_H

#include <string>
#include <vector>
#include <math.h>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathsUtils.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_rastergis_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace rastergis{

    struct DllExport RSGISFieldAttStats
    {
        std::string field;
        unsigned int fieldIdx;
        bool calcMin;
        std::string minField;
        unsigned int minFieldIdx;
        bool calcMax;
        std::string maxField;
        unsigned int maxFieldIdx;
        bool calcMean;
        std::string meanField;
        unsigned int meanFieldIdx;
        bool calcStdDev;
        std::string stdDevField;
        unsigned int stdDevFieldIdx;
        bool calcSum;
        std::string sumField;
        unsigned int sumFieldIdx;
    };
    
    class DllExport RSGISCalcNeighbourStats
    {
    public:
        RSGISCalcNeighbourStats();
        void populateStatsDiff2Neighbours(GDALDataset *inputClumps, RSGISFieldAttStats *fieldStats, bool useAbsDiff, unsigned int ratBand)throw(RSGISAttributeTableException);
        ~RSGISCalcNeighbourStats();
    };
    
}}

#endif

