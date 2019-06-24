/*
 *  RSGISDistMetrics.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/03/2015.
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

#ifndef RSGISDistMetrics_H
#define RSGISDistMetrics_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISMathException.h"

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

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
    
    
    class DllExport RSGISCalcDistMetric
    {
    public:
        RSGISCalcDistMetric(){this->initalised = false;};
        virtual void init() = 0;
        virtual double calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2) = 0;
        virtual ~RSGISCalcDistMetric(){};
    protected:
        bool initalised;
    };
    
    
    class DllExport RSGISCalcEuclideanDistMetric: public RSGISCalcDistMetric
    {
    public:
        RSGISCalcEuclideanDistMetric();
        virtual void init();
        virtual double calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2);
        virtual ~RSGISCalcEuclideanDistMetric();
    };
    
    class DllExport RSGISCalcManhattenDistMetric: public RSGISCalcDistMetric
    {
    public:
        RSGISCalcManhattenDistMetric();
        virtual void init();
        virtual double calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2);
        virtual ~RSGISCalcManhattenDistMetric();
    };
    
    class DllExport RSGISCalcMahalanobisDistMetric: public RSGISCalcDistMetric
    {
    public:
        RSGISCalcMahalanobisDistMetric(double **covarMatrixm, size_t n);
        virtual void init();
        virtual double calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2);
        virtual ~RSGISCalcMahalanobisDistMetric();
    protected:
        double **covarMatrix;
        gsl_matrix *invCovarianceMatrix;
        gsl_vector *diffVals;
        gsl_vector *tmpVec;
        size_t n;
    };
    
    class DllExport RSGISCalcMinkowskiDistMetric: public RSGISCalcDistMetric
    {
    public:
        RSGISCalcMinkowskiDistMetric();
        virtual void init();
        virtual double calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2);
        virtual ~RSGISCalcMinkowskiDistMetric();
    };
    
    class DllExport RSGISCalcChebyshevDistMetric: public RSGISCalcDistMetric
    {
    public:
        RSGISCalcChebyshevDistMetric();
        virtual void init();
        virtual double calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2);
        virtual ~RSGISCalcChebyshevDistMetric();
    };
    
    
}}

#endif
