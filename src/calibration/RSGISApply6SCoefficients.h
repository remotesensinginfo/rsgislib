/*
 *  RSGISApply6SCoefficients.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/07/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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
 *
 *  Modified by Dan Clewley on 26/10/2011 to used different coefficients based on elevation.
 *
 */

#ifndef RSGISApply6SCoefficients_h
#define RSGISApply6SCoefficients_h

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "common/RSGISException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_calib_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace calib{
    
    
    struct DllExport LUT6SElevation
    {
        float elev;
        unsigned int *imageBands;
        float *aX;
        float *bX;
        float *cX;
        float *directIrr;
        float *diffuseIrr;
        float *envIrr;
        unsigned int numValues;
    };
    
    struct DllExport LUT6SAOT
    {
        float aot;
        unsigned int *imageBands;
        float *aX;
        float *bX;
        float *cX;
        float *directIrr;
        float *diffuseIrr;
        float *envIrr;
        unsigned int numValues;
    };
    
    struct DllExport LUT6SBaseElevAOT
    {
        float elev;
        std::vector<LUT6SAOT> aotLUT;
    };
	    
	class DllExport RSGISApply6SCoefficientsSingleParam : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISApply6SCoefficientsSingleParam(unsigned int *imageBands, float *aX, float *bX, float *cX, int numValues, float noDataVal = 0.0, bool useNoDataVal=false, float scaleFactor = 1.0);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISApply6SCoefficientsSingleParam();
    protected:
        unsigned int *imageBands;
        float *aX;
        float *bX;
        float *cX;
        unsigned int numValues;
        float scaleFactor;
        float noDataVal;
        bool useNoDataVal;
    };
    
    
    
    class DllExport RSGISApply6SCoefficientsElevLUTParam : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISApply6SCoefficientsElevLUTParam(unsigned int numOutBands, std::vector<LUT6SElevation> *lut, float demNoDataVal, float noDataVal = 0.0, bool useNoDataVal=false, float scaleFactor = 1.0);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISApply6SCoefficientsElevLUTParam();
    protected:
        std::vector<LUT6SElevation> *lut;
        LUT6SElevation minElevCoeffs;
        float scaleFactor;
        float demNoDataVal;
        float noDataVal;
        bool useNoDataVal;
    };
    
    
    class DllExport RSGISApply6SCoefficientsElevAOTLUTParam : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISApply6SCoefficientsElevAOTLUTParam(unsigned int numOutBands, std::vector<LUT6SBaseElevAOT> *lut, float noDataVal = 0.0, bool useNoDataVal=false, float scaleFactor = 1.0);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISApply6SCoefficientsElevAOTLUTParam();
    protected:
        std::vector<LUT6SBaseElevAOT> *lut;
        float scaleFactor;
        float noDataVal;
        bool useNoDataVal;
    };
    
    
    
    class DllExport RSGISApply6SCoefficients : public rsgis::img::RSGISCalcImageValue
    {
		/** Class to apply 6S coeffiecients to an image
         Coefficients are supplied for each band.
         Optionally a DEM may be used, passed in as the first image band, with a set of threasholds (elevationThresh) coresponding to specific coefficients for each elevation.
		 
         When no DEM is used the coeffiecents are structured as follows:
         
         ax[0][0] = ax for band 1
         ax[1][0] = ax for band 2
		 
         When a DEM is used, coefficients are structured as follows:
         
         ax[0][0] = ax for band 1, when elevation is less than elevationThresh[0]
         ax[0][1] = ax for band 1, when elevation is between elevationThresh[0] and elevationThresh[1]
         ax[0][2] = ax for band 1, when elevation is between elevationThresh[1] and elevationThresh[2]
         ax[1][0] = ax for band 2, when elevation is less than elevationThresh[0]
         ax[1][1] = ax for band 2, when elevation is between elevationThresh[0] and elevationThresh[1]
         ax[1][2] = ax for band 2, when elevation is between elevationThresh[1] and elevationThresh[2]
		 
         The DEM is provided as the first band of the input array (bandValues)
		 
         The same elevation threasholds are used for all bands.
		 
		 */
    public:
        RSGISApply6SCoefficients(int numberOutBands, unsigned int *imageBands, float **aX, float **bX, float **cX, int numValues, float *elevationThresh = NULL, int numElevation = 0, float scaleFactor = 1.0);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISApply6SCoefficients();
    protected:
        unsigned int *imageBands;
        float **aX;
        float **bX;
        float **cX;
		float *elevationThresh;
        unsigned int numValues;
		unsigned int numElevation;
		bool useTopo6S;
        unsigned int bandOffset;
        float scaleFactor;
    };
    
    
    
    
    
    class DllExport RSGISCalcSolarIrradianceElevLUTParam : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcSolarIrradianceElevLUTParam(unsigned int numOutBands, std::vector<LUT6SElevation> *lut, double *meanSREFVec, unsigned int numSREFBands, float solarZenith);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISCalcSolarIrradianceElevLUTParam();
    protected:
        std::vector<LUT6SElevation> *lut;
        double *meanSREFVec;
        unsigned int numSREFBands;
        float solarZenith;
    };
    
    
    
    
    class DllExport RSGISCalcStandardisedReflectanceSD2010 : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcStandardisedReflectanceSD2010(unsigned int numOutBands, unsigned int numSREFBands, float brdfBeta, float outIncidenceAngle, float outExitanceAngle, float reflScaleFactor);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISCalcStandardisedReflectanceSD2010();
    protected:
        unsigned int numSREFBands;
        float brdfBeta;
        float outIncidenceAngle;
        float outExitanceAngle;
        float reflScaleFactor;
    };
    
    
    
    
    
}}

#endif
