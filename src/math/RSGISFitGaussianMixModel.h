/*
 *  RSGISFitGaussianMixModel.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/03/2015.
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

#ifndef RSGISFitGaussianMixModel_H
#define RSGISFitGaussianMixModel_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

#include "math/RSGISMathException.h"
#include "math/cmpfit/rsgis_mpfit.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace math{
    
    struct DllExport GaussianModelData
    {
        double *xVal;
        double *amplitude;
        double *error;
    };
    
    struct DllExport GaussianModelParams
    {
        double noise;
        double offset;
        double amplitude;
        double fwhm;
    };
    
    /*
     * int m     - number of data points
     * int n     - number of parameters
     * double *p - array of n parameters
     * double *deviates - array of m deviates to be returned by myfunct()
     * double **derivs - used for user-computed derivatives (see below)
     * (= 0  when automatic finite differences are computed)
     */
    inline int gaussianSum(int m, int n, double *p, double *deviates, double **derivs, void *data)
    {
        GaussianModelData *modelData = (GaussianModelData*) data;
        int numPeaks = (n-1)/3;
        float predVal = 0;
        int idx = 0;
        for(int i = 0; i < m; ++i)
        {
            predVal = 0;
            /*
             * p[0] = noise
             * p[1] = amplitude
             * p[2] = xVal (offset)
             * p[3] = width
             */
            for(int j = 0; j < numPeaks; ++j)
            {
                idx = (j * 3) + 1;
                predVal += (p[idx] * exp((-1.0)*
                                         (
                                          pow(modelData->xVal[i] - p[idx+1], 2)
                                          /
                                          (2.0 * pow(p[idx+2], 2))
                                          )));
            }
            predVal += p[0];
            deviates[i] = (modelData->amplitude[i] - predVal) / modelData->error[i];
        }
        
        return 0;
    }
    
    
    class DllExport RSGISFitGaussianMixModel
    {
    public:
        RSGISFitGaussianMixModel();
        std::vector<GaussianModelParams> performFit(std::vector<std::pair<double, double> > *hist, float binWidth, double peakThres=0.005, double ampVar=0.01, unsigned int peakLocVar=2, unsigned int initWidth=2, double minWidth=0.01, double maxWidth=10, bool debug_info=false)throw(RSGISMathException);
        ~RSGISFitGaussianMixModel();
        
    };
    
    
    
}}

#endif
