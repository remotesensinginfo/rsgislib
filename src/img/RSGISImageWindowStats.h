/*
 *  RSGISImageWindowStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/07/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISImageWindowStats_H
#define RSGISImageWindowStats_H

#include <iostream>
#include <string>
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMathsUtils.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcCovariance.h"
#include "img/RSGISCalcImage.h"

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

namespace rsgis{namespace img{

    class RSGISCalcImgPxlNeighboursDist: public RSGISCalcImageValue
    {
    public:
        RSGISCalcImgPxlNeighboursDist();
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        ~RSGISCalcImgPxlNeighboursDist();
    private:
        rsgis::math::RSGISStatsSummary *stats;
        std::vector<double> *distVals;
    };
    
    
    class RSGISCalcImgPxl2WindowDist: public RSGISCalcImageValue
    {
    public:
        RSGISCalcImgPxl2WindowDist(gsl_matrix *invCovarianceMatrix, rsgis::math::Vector *varMeans, gsl_vector *dVals, gsl_vector *outVec);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        ~RSGISCalcImgPxl2WindowDist();
    private:
        rsgis::math::RSGISStatsSummary *stats;
        std::vector<double> *distVals;
        gsl_matrix *invCovarianceMatrix;
        rsgis::math::Vector *varMeans;
        gsl_vector *dVals;
        gsl_vector *outVec;

    };

    
    
}}

#endif
