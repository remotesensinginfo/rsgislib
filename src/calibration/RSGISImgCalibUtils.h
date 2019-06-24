/*
 *  RSGISImgCalibUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/11/2016.
 *  Copyright 2016 RSGISLib. All rights reserved.
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

#ifndef RSGISImgCalibUtils_H
#define RSGISImgCalibUtils_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImage.h"

#include "rastergis/RSGISRasterAttUtils.h"

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
    
    
    class DllExport RSGISImgCalibUtils
    {
    public:
        RSGISImgCalibUtils(){};
        void calcNadirImgViewAngle(GDALDataset *imgFPDataset, std::string outViewAngleImg, std::string gdalFormat, double sateAltitude, std::string minXXCol, std::string minXYCol, std::string maxXXCol, std::string maxXYCol, std::string minYXCol, std::string minYYCol, std::string maxYXCol, std::string maxYYCol);
        ~RSGISImgCalibUtils(){};
    };
    
    
    class DllExport RSGISCalcNadirViewAngle: public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCalcNadirViewAngle(double clSlope, double clOff, double sateAltitude);
        void calcImageValue(float *bandValues, int numBands, double *output)  {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands)  {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent)  {throw rsgis::img::RSGISImageCalcException("No implemented");};

        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent);
        
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        ~RSGISCalcNadirViewAngle();
    private:
        double clSlope;
        double clOff;
        double sateAltitude;
    };
    
    
    
}}

#endif


