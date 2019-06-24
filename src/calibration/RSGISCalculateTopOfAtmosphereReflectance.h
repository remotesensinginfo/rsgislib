/*
 *  RSGISCalculateTopOfAtmosphereReflectance.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/05/2011.
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

#ifndef RSGISCalculateTopOfAtmosphereReflectance_H
#define RSGISCalculateTopOfAtmosphereReflectance_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "common/RSGISException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "boost/date_time/gregorian/gregorian.hpp"

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
    
    inline double rsgisCalcSolarDistance(int julianDay)
    {
        /*
         From: http://aa.usno.navy.mil/faq/docs/SunApprox.php
         
         Given below is a simple algorithm for computing the Sun's angular coordinates to an accuracy of about 
         1 arcminute within two centuries of 2000. The algorithm's accuracy degrades gradually beyond its four-century 
         window of applicability. This accuracy is quite adequate for computing, for example, the times of 
         sunrise and sunset, or solar transit. For navigational purposes it would provide about 1 nautical mile accuracy. 
         The algorithm requires only the Julian date of the time for which the Sun's coordinates are needed 
         (Julian dates are a form of Universal Time.)
         
         First, compute D, the number of days and fraction (+ or –) from the epoch referred to as "J2000.0", 
         which is 2000 January 1.5, Julian date 2451545.0:
         
         D = JD – 2451545.0
         where JD is the Julian date of interest. Then compute
         
         Mean anomaly of the Sun:	g = 357.529 + 0.98560028 D
         Mean longitude of the Sun:	q = 280.459 + 0.98564736 D
         Geocentric apparent ecliptic longitude
         of the Sun (adjusted for aberration):	L = q + 1.915 sin g + 0.020 sin 2g
         where all the constants (therefore g, q, and L) are in degrees. It may be necessary or 
         desirable to reduce g, q, and L to the range 0° to 360°.
         
         The Sun's ecliptic latitude, b, can be approximated by b=0. The distance of the Sun from the Earth, R, 
         in astronomical units (AU), can be approximated by
         
         R = 1.00014 – 0.01671 cos g – 0.00014 cos 2g
         */
        
        double D = julianDay - 2451545.0;
        double g = (357.529 + 0.98560028 * D)*(M_PI/180);
        double solarDist = 1.00014 - 0.01671 * cos(g) - 0.00014 * cos(2 * g);
        return solarDist;
    }
    
    inline unsigned int rsgisGetJulianDay(unsigned int day, unsigned int month, unsigned int year)
    {
        boost::gregorian::date d(year,month,day);
        int julianDay = d.julian_day();
        return julianDay;
    }
    
	class DllExport RSGISCalculateTopOfAtmosphereReflectance : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISCalculateTopOfAtmosphereReflectance(int numberOutBands, float *solarIrradiance, double distance, float solarZenith, float scaleFactor = 1);
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
        ~RSGISCalculateTopOfAtmosphereReflectance();
    protected:
        float *solarIrradiance;
        double distance;
        float solarZenith;
        float scaleFactor;
        double distSq;
    };
    
    class DllExport RSGISCalculateTOAThermalBrightness : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalculateTOAThermalBrightness(int numberOutBands, float *k1, float *k2, float scaleFactor = 1);
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
        ~RSGISCalculateTOAThermalBrightness();
    protected:
        float *k1;
        float *k2;
        float scaleFactor;
    };
	
    
    class DllExport RSGISCalculateRadianceFromTOAReflectance : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalculateRadianceFromTOAReflectance(int numberOutBands, float *solarIrradiance, double distance, float solarZenith, float scaleFactor = 1);
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
        ~RSGISCalculateRadianceFromTOAReflectance();
    protected:
        float *solarIrradiance;
        double distance;
        float solarZenith;
        float scaleFactor;
        double distSq;
    };
    
    
}}

#endif

