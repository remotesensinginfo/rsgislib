/*
 *  RSGISFindImageOffset.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/07/2021.
 *  Copyright 2021 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 *
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef RSGISFindImageOffset_H
#define RSGISFindImageOffset_H

#include <iostream>
#include <string>
#include <cmath>
#include <list>

#include "gdal_priv.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImage.h"
#include "registration/RSGISImageSimilarityMetric.h"
#include "math/RSGISPolyFit.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "boost/math/special_functions/fpclassify.hpp"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
#ifdef rsgis_registration_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
#define DllExport
#endif

namespace rsgis{namespace reg{

    class DllExport RSGISFindImageOffset
    {
    public:
        RSGISFindImageOffset();
        std::pair<double, double> findImageOffset(GDALDataset *refDataset, GDALDataset *fltDataset,
                                                  unsigned int xSearch, unsigned int ySearch,
                                                  RSGISImageCalcSimilarityMetric *metric,
                                                  bool calcSubPixelRes=false, unsigned int subPixelRes=0);
        float findExtreme(bool findMin, gsl_vector *coefficients, unsigned int order, float minRange,
                          float maxRange, unsigned int resolution, float *extremeVal);
        ~RSGISFindImageOffset();
    };


}}

#endif
