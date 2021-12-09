/*
 *  RSGISDropClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/04/2015.
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

#ifndef RSGISDropClumps_H
#define RSGISDropClumps_H

#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>

#include "common/RSGISAttributeTableException.h"

#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISImageStatistics.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISCalcImageStatsAndPyramids.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_segmentation_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace segment{
    
    
    class DllExport RSGISDropClumps
    {
    public:
        RSGISDropClumps();
        void dropSelectedClumps(GDALDataset *clumpsImage, std::string outputImage, std::string selClumps2Drop, std::string gdalFormat, unsigned int ratBand=1);
        ~RSGISDropClumps();
    };
    
    
    class DllExport RSGISDropClumpsRelabelling : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISDropClumpsRelabelling(size_t *newClumpIds, size_t nRows);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        ~RSGISDropClumpsRelabelling();
    protected:
        size_t *newClumpIds;
        size_t nRows;
    };
    
}}

#endif
