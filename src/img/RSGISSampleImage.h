/*
 *  RSGISSampleImage.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/04/2013.
 *  Copyright 2013 RSGISLib.
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

#ifndef RSGISSampleImage_H
#define RSGISSampleImage_H

#include <iostream>
#include <fstream>

#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

#include "math/RSGISMathsUtils.h"

#include "gdal_priv.h"

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

    class DllExport RSGISSampleImage
    {
    public:
        RSGISSampleImage();
        void randomSampleImageMask(GDALDataset *inputImage, unsigned int imgBand, GDALDataset *outputImage, std::vector<int> maskVals, unsigned long numSamples);
        void randomSampleImageMaskSmallPxlCount(GDALDataset *inputImage, unsigned int imgBand, GDALDataset *outputImage, std::vector<int> maskVals, unsigned long numSamples, int rndSeed);
        ~RSGISSampleImage();
    };

    
    class DllExport RSGISGetPxlLocAsVec : public RSGISCalcImageValue
    {
    public:
        RSGISGetPxlLocAsVec(std::vector<int> maskVals, std::vector<std::pair<long, long> > *maskPxlLocs, unsigned int imgBand);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, OGREnvelope extent);
        ~RSGISGetPxlLocAsVec();
    private:
        std::vector<int> maskVals;
        std::vector<std::pair<long, long> > *maskPxlLocs;
        unsigned int imgBand;
    };

}}

#endif


