/*
 *  RSGISLinearSpectralUnmixing.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/02/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISLinearSpectralUnmixing_H
#define RSGISLinearSpectralUnmixing_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "math/RSGISMatrices.h"

#include <geos/geom/Envelope.h>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis::math;

namespace rsgis{namespace img{
    
    class RSGISCalcLinearSpectralUnmixing
    {
    public:
        RSGISCalcLinearSpectralUnmixing();
        void performUnconstainedLinearSpectralUnmixing(GDALDataset **datasets, int numDatasets, string outputImage, string endmembersFilePath)throw(RSGISImageCalcException);
        void performExhaustiveConstrainedSpectralUnmixing(GDALDataset **datasets, int numDatasets, string outputImage, string endmembersFilePath, float stepResolution)throw(RSGISImageCalcException);
        ~RSGISCalcLinearSpectralUnmixing();
    };
    
    class RSGISUnconstrainedLinearSpectralUnmixing : public RSGISCalcImageValue
    {
    public: 
        RSGISUnconstrainedLinearSpectralUnmixing(int numberOutBands, gsl_matrix *r);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISUnconstrainedLinearSpectralUnmixing();
    protected:
        gsl_matrix *r;
    };
    
    class RSGISExhaustiveLinearSpectralUnmixing : public RSGISCalcImageValue
    {
    public: 
        RSGISExhaustiveLinearSpectralUnmixing(int numberOutBands, gsl_matrix *endmembers, float stepRes);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISExhaustiveLinearSpectralUnmixing();
    protected:
        float calcDistance2MeasuredSpectra(float em1Val, float em2Val, float em3Val, float *normSpectra, unsigned int numBands) throw(RSGISImageCalcException);
        float calcDistance2MeasuredSpectra(float em1Val, float em2Val, float *normSpectra, unsigned int numBands) throw(RSGISImageCalcException);
        gsl_matrix *endmembers;
        float stepRes;
        unsigned int numOfEndMembers;
    };
    
    
}}

#endif
