/*
 *  RSGISSharpenLowResImagery.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/05/2017.
 *  Copyright 2017 RSGISLib. All rights reserved.
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

#ifndef RSGISSharpenLowResImagery_H
#define RSGISSharpenLowResImagery_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISImageException.h"
#include "img/RSGISImageCalcException.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

#include "math/RSGISMathsUtils.h"

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


namespace rsgis { namespace img {

    enum RSGISSharpenBandStatus
    {
        rsgis_sharp_band_ignore = 0,
        rsgis_sharp_band_lowres = 1,
        rsgis_sharp_band_highres = 2
    };
    
    struct DllExport RSGISSharpenBandInfo
    {
        unsigned int band;
        RSGISSharpenBandStatus status;
        std::string bandName;
    };
    
    class DllExport RSGISSharpenLowResBands : public RSGISCalcImageValue
    {
    public:
        RSGISSharpenLowResBands(int numberOutBands, RSGISSharpenBandInfo *bandInfo, unsigned int nBandInfo, unsigned int nLowResBands, unsigned int nHighResBands, unsigned int defWinSize, int noDataVal, double *imgMinVal, double *imgMaxVal);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        ~RSGISSharpenLowResBands();
    private:
        rsgis::math::RSGISMathsUtils mathUtils;
        RSGISSharpenBandInfo *bandInfo;
        unsigned int nBandInfo;
        unsigned int nLowResBands;
        unsigned int nHighResBands;
        unsigned int defWinSize;
        unsigned int nWinPxls;
        double **lowResPxlVals;
        double **highResPxlVals;
        int noDataVal;
        double *imgMinVal;
        double *imgMaxVal;
        unsigned int *lowResBandIdxs;
        unsigned int *highResBandIdxs;
        rsgis::math::RSGISLinearFitVals **linFits;
    };

}}

#endif
