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
#include "math/RSGISnnls.h"

#include <geos/geom/Envelope.h>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

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
    
    class DllExport RSGISCalcLinearSpectralUnmixing
    {
    public:
        RSGISCalcLinearSpectralUnmixing(std::string gdalFormat="ENVI", GDALDataType gdalDataType=GDT_Float32, float gain=1, float offset=0);
        void performUnconstainedLinearSpectralUnmixing(GDALDataset **datasets, int numDatasets, std::string outputImage, std::string endmembersFilePath);
        void performPartConstainedLinearSpectralUnmixing(GDALDataset **datasets, int numDatasets, std::string outputImage, std::string endmembersFilePath, float weight);
        void performConstainedNNLinearSpectralUnmixing(GDALDataset **datasets, int numDatasets, std::string outputImage, std::string endmembersFilePath, float weight);
        void performExhaustiveConstrainedSpectralUnmixing(GDALDataset **datasets, int numDatasets, std::string outputImage, std::string endmembersFilePath, float stepResolution);
        ~RSGISCalcLinearSpectralUnmixing();
    protected:
        std::string gdalFormat;
        GDALDataType gdalDataType;
        float gain;
        float offset;
    };
    
    
    class DllExport RSGISUnconstrainedLinearSpectralUnmixing : public RSGISCalcImageValue
    {
    public: 
        RSGISUnconstrainedLinearSpectralUnmixing(int numberOutBands, gsl_matrix *endmembers, gsl_matrix *V, gsl_vector *S, gsl_vector *work, gsl_vector *b, gsl_vector *x, float gain, float offset);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        ~RSGISUnconstrainedLinearSpectralUnmixing();
    protected:
        gsl_matrix *endmembers;
        gsl_matrix *V;
        gsl_vector *S;
        gsl_vector *work;
        gsl_vector *b;
        gsl_vector *x;
        float gain;
        float offset;
    };
    
    class DllExport RSGISPartConstrainedLinearSpectralUnmixing : public RSGISCalcImageValue
    {
    public: 
        RSGISPartConstrainedLinearSpectralUnmixing(int numberOutBands, float weight, gsl_matrix *endmembers, gsl_matrix *V, gsl_vector *S, gsl_vector *work, gsl_vector *b, gsl_vector *x, float gain, float offset);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
       void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        ~RSGISPartConstrainedLinearSpectralUnmixing();
    protected:
        float weight;
        gsl_matrix *endmembers;
        gsl_matrix *V;
        gsl_vector *S;
        gsl_vector *work;
        gsl_vector *b;
        gsl_vector *x;
        float gain;
        float offset;
    };
    
    class DllExport RSGISExhaustiveLinearSpectralUnmixing : public RSGISCalcImageValue
    {
    public: 
        RSGISExhaustiveLinearSpectralUnmixing(int numberOutBands, gsl_matrix *endmembers, float stepRes, float gain, float offset);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        ~RSGISExhaustiveLinearSpectralUnmixing();
    protected:
        float calcDistance2MeasuredSpectra(float em1Val, float em2Val, float em3Val, float *normSpectra, unsigned int numBands);
        float calcDistance2MeasuredSpectra(float em1Val, float em2Val, float *normSpectra, unsigned int numBands);
        gsl_matrix *endmembers;
        float stepRes;
        unsigned int numOfEndMembers;
        float gain;
        float offset;
    };
    
    
}}

#endif
