/*
 *  RSGISImgSummaryStatsFromMultiResImgs.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/01/2018.
 *  Copyright 2018 RSGISLib.
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

#ifndef RSGISImgSummaryStatsFromMultiResImgs_H
#define RSGISImgSummaryStatsFromMultiResImgs_H

#include <iostream>
#include <string>
#include <math.h>

#include "common/RSGISImageException.h"

#include "math/RSGISMathsUtils.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
#ifdef rsgis_img_EXPORTS
#define DllExport   __declspec( dllexport )
#else
#define DllExport   __declspec( dllimport )
#endif
#else
#define DllExport
#endif

namespace rsgis{namespace img{
    
    class DllExport RSGISImgSummaryStatsFromMultiResImgs
    {
        public:
            RSGISImgSummaryStatsFromMultiResImgs();
            void calcSummaryStats4LowResPxlsFromHighResImg(GDALDataset *refDataset, GDALDataset *statsDataset, unsigned int statsImgBand, std::string outImg, std::string gdalFormat, GDALDataType gdalDataType, bool useNoData, std::vector<rsgis::math::rsgissummarytype> sumStats, unsigned int xIOGrid, unsigned int yIOGrid);
            ~RSGISImgSummaryStatsFromMultiResImgs();
    };
    
    
    
    
    
    class DllExport RSGISCalcHighResImgSummaryStats : public rsgis::img::RSGISCalcValuesFromMultiResInputs
    {
    public:
        RSGISCalcHighResImgSummaryStats(int numberOutBands, std::vector<rsgis::math::rsgissummarytype> sumStats);
        void calcImageValue(float *bandValues, int numInVals, bool useNoData, float noDataVal, double *output);
        ~RSGISCalcHighResImgSummaryStats();
    protected:
        std::vector<rsgis::math::rsgissummarytype> sumStats;
    };
    
    
}}

#endif

