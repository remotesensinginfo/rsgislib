/*
 *  RSGISCloudMasking.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/02/2014.
 *  Copyright 2014 RSGISLib. All rights reserved.
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

#ifndef RSGISCloudMasking_h
#define RSGISCloudMasking_h

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "geos/geom/Envelope.h"

#include "common/RSGISException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISImageStatistics.h"
#include "img/RSGISExtractImageValues.h"

#include "rastergis/RSGISPopRATWithStats.h"
#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISCalcClusterLocation.h"

#include "math/RSGISMathsUtils.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
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
    
	class DllExport RSGISLandsatFMaskPass1CloudMasking : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISLandsatFMaskPass1CloudMasking(unsigned int scaleFactor, unsigned int numLSBands, double whitenessThreshold=0.7)throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISLandsatFMaskPass1CloudMasking();
    protected:
        unsigned int scaleFactor;
        unsigned int coastalIdx;
        unsigned int blueIdx;
        unsigned int greenIdx;
        unsigned int redIdx;
        unsigned int nirIdx;
        unsigned int swir1Idx;
        unsigned int swir2Idx;
        unsigned int therm1Idx;
        unsigned int therm2Idx;
        unsigned int coastalSatIdx;
        unsigned int blueSatIdx;
        unsigned int greenSatIdx;
        unsigned int redSatIdx;
        unsigned int nirSatIdx;
        unsigned int swir1SatIdx;
        unsigned int swir2SatIdx;
        unsigned int therm1SatIdx;
        unsigned int therm2SatIdx;
        bool coastal;
        bool thermal2;
        unsigned long *waterTempHist;
        unsigned long numWaterBins;
        double minTempWater;
        double tempBinWidthWater;
        unsigned long *landTempHist;
        unsigned long numLandBins;
        double minTempLand;
        double tempBinWidthLand;
        double whitenessThreshold;
    };
    
    class DllExport RSGISLandsatFMaskExportPass1LandWaterCloudMasking : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISLandsatFMaskExportPass1LandWaterCloudMasking()throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        double propOfPCPPixels();
        ~RSGISLandsatFMaskExportPass1LandWaterCloudMasking();
    protected:
        double numPCPPxls;
        double numValidPxls;
    };
    
    class DllExport RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking(unsigned int scaleFactor, unsigned int numLSBands, double water82ndThres, double land82ndThres, double land17thThres)throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking();
    protected:
        unsigned int scaleFactor;
        unsigned int numLSBands;
        unsigned int landWaterIdx;
        unsigned int coastalIdx;
        unsigned int blueIdx;
        unsigned int greenIdx;
        unsigned int redIdx;
        unsigned int nirIdx;
        unsigned int swir1Idx;
        unsigned int swir2Idx;
        unsigned int therm1Idx;
        unsigned int therm2Idx;
        unsigned int coastalSatIdx;
        unsigned int blueSatIdx;
        unsigned int greenSatIdx;
        unsigned int redSatIdx;
        unsigned int nirSatIdx;
        unsigned int swir1SatIdx;
        unsigned int swir2SatIdx;
        unsigned int therm1SatIdx;
        unsigned int therm2SatIdx;
        double water82ndThres;
        double land82ndThres;
        double land17thThres;
        bool coastal;
        bool thermal2;
        unsigned int varProbIdx;
    };
    
    class DllExport RSGISLandsatFMaskPass2CloudMasking : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISLandsatFMaskPass2CloudMasking(unsigned int scaleFactor, unsigned int numLSBands, double landCloudProbUpperThres, double waterCloudProbUpperThres, double lowerLandTempThres)throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISLandsatFMaskPass2CloudMasking();
    protected:
        unsigned int scaleFactor;
        unsigned int numLSBands;
        unsigned int landWaterIdx;
        unsigned int coastalIdx;
        unsigned int blueIdx;
        unsigned int greenIdx;
        unsigned int redIdx;
        unsigned int nirIdx;
        unsigned int swir1Idx;
        unsigned int swir2Idx;
        unsigned int therm1Idx;
        unsigned int therm2Idx;
        unsigned int pcpIdx;
        unsigned int landCloudProbIdx;
        unsigned int waterCloudProbIdx;
        unsigned int waterTestIdx;
        unsigned int snowTestIdx;
        bool coastal;
        bool thermal2;
        double landCloudProbUpperThres;
        double waterCloudProbUpperThres;
        double lowerLandTempThres;
    };
    
    
    class DllExport RSGISCalcImageCloudMajorityFilter : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcImageCloudMajorityFilter():rsgis::img::RSGISCalcImageValue(1){};
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISCalcImageCloudMajorityFilter(){};
    };
    
    
    class DllExport RSGISCalcImagePotentialCloudShadowsMask : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcImagePotentialCloudShadowsMask(unsigned int scaleFactor):rsgis::img::RSGISCalcImageValue(1){this->scaleFactor = scaleFactor;};
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISCalcImagePotentialCloudShadowsMask(){};
    protected:
        unsigned int scaleFactor;
    };
    
    class DllExport RSGISCalcCloudParams
    {
    public:
        RSGISCalcCloudParams(){};
        void calcCloudHeights(GDALDataset *thermal, GDALDataset *cloudClumpsDS, GDALDataset *initCloudHeights, double lowerLandThres, double upperLandThres, float scaleFactor)throw(rsgis::img::RSGISImageCalcException);
        void projFitCloudShadow(GDALDataset *cloudClumpsDS, GDALDataset *initCloudHeights, GDALDataset *potentCloudShadowRegions, GDALDataset *cloudShadowTestRegionsDS, GDALDataset *cloudShadowRegionsDS, double sunAz, double sunZen, double senAz, double senZen)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISCalcCloudParams(){};
    };
    
    class DllExport RSGISCalcCloudShadowCorrespondance : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcCloudShadowCorrespondance();
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void reset();
        void getNPxls(unsigned long *nShadPxlsVal, unsigned long *nShadPxlsInPotentVal);
        ~RSGISCalcCloudShadowCorrespondance();
    protected:
        unsigned long nShadPxls;
        unsigned long nShadPxlsInPotent;
    };
    
    
    class DllExport RSGISEditCloudShadowImg
    {
    public:
        RSGISEditCloudShadowImg(GDALDataset *testImg, int band);
        bool turnOnPxl(double x, double y)throw(rsgis::img::RSGISImageCalcException);
        void reset()throw(rsgis::img::RSGISImageCalcException);
        bool calcCorrelation(GDALDataset *cloudClumpsDS, GDALDataset *potentCloudShadowRegions, GDALDataset *cloudShadowTestRegionsDS, double *cloudPropOverlap, unsigned long *numPxlOverlap)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISEditCloudShadowImg();
    protected:
        GDALDataset *testImg;
        GDALRasterBand *testImgBand;
        int band;
        double tlX;
        double tlY;
        double brX;
        double brY;
        double xRes;
        double yRes;
        unsigned long nXPxl;
        unsigned long nYPxl;
        geos::geom::Envelope extent;
        bool firstPts;
        RSGISCalcCloudShadowCorrespondance *calcCloudShadCorr;
        rsgis::img::RSGISCalcImage *imgCalc;
    };
    
    
    
    class DllExport RSGISCalcPxlCloudBaseAndTopHeight : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcPxlCloudBaseAndTopHeight(double *cloudBase, double *hBaseMin, long numClumps, float scaleFactor):rsgis::img::RSGISCalcImageValue(2)
        {
            this->cloudBase = cloudBase;
            this->hBaseMin = hBaseMin;
            this->numClumps = numClumps;
            this->scaleFactor = scaleFactor;
            
        };
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISCalcPxlCloudBaseAndTopHeight()
        {
        };
    protected:
        double *cloudBase;
        double *hBaseMin;
        long numClumps;
        float scaleFactor;
        
    };
    
    
    class DllExport RSGISCalcCombineMasks : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcCombineMasks():rsgis::img::RSGISCalcImageValue(1){};
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISCalcCombineMasks(){};
    };
    
    
    class DllExport RSGISExportMaskForOverPCPThreshold : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISExportMaskForOverPCPThreshold():rsgis::img::RSGISCalcImageValue(1){};
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISExportMaskForOverPCPThreshold(){};
    };
    
    
    
}}

#endif
