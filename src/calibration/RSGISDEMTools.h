/*
 *  RSGISDEMTools.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2011.
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
 */

#ifndef RSGISDEMTools_h
#define RSGISDEMTools_h

#include <iostream>
#include <string>
#include <cmath>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISExtractImagePixelsInPolygon.h"

#include "math/RSGISMathsUtils.h"

#include <boost/math/special_functions/fpclassify.hpp>

#ifndef M_PI
# define M_PI  3.1415926535897932384626433832795
#endif

#ifndef ASTROUNIT
# define ASTROUNIT  149597870.7
#endif

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

    
    /***
     * If output type is set to 1 then output is radians if 0 then degrees.
     *
     */
	class DllExport RSGISCalcSlope: public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISCalcSlope(unsigned int band, float ewRes, float nsRes, int outType, double noDataVal);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
		~RSGISCalcSlope(){};
    private:
        unsigned int band;
        float ewRes;
        float nsRes;
        int outType;
        double noDataVal;
	};

    /***
     * If output type is set to 1 then output is radians if 0 then degrees.
     *
     */
    class DllExport RSGISCalcSlopePerPxlRes: public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcSlopePerPxlRes(unsigned int band, int outType, double noDataVal, unsigned int ew_res_band, unsigned int ns_res_band);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        ~RSGISCalcSlopePerPxlRes(){};
    private:
        unsigned int band;
        int outType;
        double noDataVal;
        unsigned int ew_res_band;
        unsigned int ns_res_band;
    };



    class DllExport RSGISCalcAspect : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISCalcAspect(unsigned int band, float ewRes, float nsRes, double noDataVal);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
		~RSGISCalcAspect(){};
    private:
        unsigned int band;
        float ewRes;
        float nsRes;
        double noDataVal;
	};

    class DllExport RSGISCalcAspectPerPxlRes : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcAspectPerPxlRes(unsigned int band, double noDataVal, unsigned int ew_res_band, unsigned int ns_res_band);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        ~RSGISCalcAspectPerPxlRes(){};
    private:
        unsigned int band;
        unsigned int ew_res_band;
        unsigned int ns_res_band;
        double noDataVal;
    };



    class DllExport RSGISRecodeAspect : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISRecodeAspect();
		void calcImageValue(float *bandValues, int numBands, double *output);
		~RSGISRecodeAspect(){};
	};

    
    
	class DllExport RSGISCalcHillShade : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISCalcHillShade(unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth, double noDataVal);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
		~RSGISCalcHillShade(){};
    private:
        unsigned int band;
        float ewRes;
        float nsRes;
        float sunZenith;
        float sunAzimuth;
        double noDataVal;
	};

    class DllExport RSGISCalcHillShadePerPxlRes : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcHillShadePerPxlRes(unsigned int band, float sunZenith, float sunAzimuth, double noDataVal, unsigned int ew_res_band, unsigned int ns_res_band);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        ~RSGISCalcHillShadePerPxlRes(){};
    private:
        unsigned int band;
        float sunZenith;
        float sunAzimuth;
        double noDataVal;
        unsigned int ew_res_band;
        unsigned int ns_res_band;
    };



    class DllExport RSGISCalcShadowBinaryMask : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISCalcShadowBinaryMask(GDALDataset *inputImage, unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth, float maxElevHeight, double noDataVal);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, OGREnvelope extent);
		~RSGISCalcShadowBinaryMask();
    private:
        unsigned int band;
        float ewRes;
        float nsRes;
        float sunZenith;
        float sunAzimuth;
        double sunRange;
        double demWidth;
        double demHeight;
        float maxElevHeight;
        GDALDataset *inputImage;
        double noDataVal;
        double degreesToRadians;
        double radiansToDegrees;
        rsgis::img::RSGISExtractImagePixelsOnLine *extractPixels;
	};



    class DllExport RSGISCalcRayIncidentAngle : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISCalcRayIncidentAngle(unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth, double noDataVal);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
		~RSGISCalcRayIncidentAngle(){};
    private:
        unsigned int band;
        float ewRes;
        float nsRes;
        float sunZenith;
        float sunAzimuth;
        double noDataVal;
	};


    class DllExport RSGISCalcRayExitanceAngle : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISCalcRayExitanceAngle(unsigned int band, float ewRes, float nsRes, float viewZenith, float viewAzimuth, double noDataVal);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
		~RSGISCalcRayExitanceAngle(){};
    private:
        unsigned int band;
        float ewRes;
        float nsRes;
        float viewZenith;
        float viewAzimuth;
        double noDataVal;
	};


    
    class DllExport RSGISFilterDTMWithAspectMedianFilter : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISFilterDTMWithAspectMedianFilter(float aspectRange, double noDataVal);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        ~RSGISFilterDTMWithAspectMedianFilter(){};
    private:
        float aspectRange;
        double noDataVal;
	};

    

    class DllExport RSGISDetreadDEMUsingPlaneFit : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISDetreadDEMUsingPlaneFit(double noDataVal, int winSize);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        ~RSGISDetreadDEMUsingPlaneFit();
    private:
        rsgis::math::RSGISMathsUtils *mathUtils;
        double noDataVal;
        double *xVals;
        double *yVals;
        double *zVals;
        int nVals;
    };
    
    
}}

#endif


