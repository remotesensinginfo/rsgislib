/*
 *  RSGISHydroDEMFillSoilleGratin94.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/07/2016.
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
 *
 *  Implementation of the Soille and Gratin 1994 algorithm for filling local
 *  minima in a DEM (or any other image dataset).
 *
 *  Soille, P., and Gratin, C. (1994). An efficient algorithm for drainage
 *  network extraction on DEMs. J. Visual Communication and Image Representation.
 *  5(2). 181-189.
 *
 */

#ifndef RSGISHydroDEMFillSoilleGratin94_h
#define RSGISHydroDEMFillSoilleGratin94_h

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISExtractImagePixelsInPolygon.h"
#include "img/RSGISImageStatistics.h"


#include "math/RSGISMathsUtils.h"

#include <boost/math/special_functions/fpclassify.hpp>

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
    
    
    struct Q2DPxl
    {
        Q2DPxl()
        {
            this->x = 0;
            this->y = 0;
        };
        Q2DPxl(long x, long y)
        {
            this->x = x;
            this->y = y;
        };
        long x;
        long y;
    };
    
    
    class DllExport RSGISHydroDEMFillSoilleGratin94
    {
    public:
        RSGISHydroDEMFillSoilleGratin94();
        void performSoilleGratin94Fill(GDALDataset *inDEMImgDS, GDALDataset *inValidImgDS, GDALDataset *outImgDS, bool calcBorderVal, long borderVal=0);
        ~RSGISHydroDEMFillSoilleGratin94();
    protected:
        bool qEmpty(long hcrt);
        Q2DPxl qPopFront(long hcrt);
        void qPushBack(long hcrt, Q2DPxl pxl);
        std::list<Q2DPxl>* getNeighbours(Q2DPxl pxl, GDALRasterBand *inValidImg);
        long getPxlVal(Q2DPxl pxl, GDALRasterBand *imgData);
        void setPxlVal(Q2DPxl pxl, long val, GDALRasterBand *imgData);
        long rtnMax(long val1, long val2);
        void getImagesEdgesToInitFill(GDALRasterBand *imgData, double borderVal, std::list<Q2DPxl> *pxQ);
        std::list<Q2DPxl> **pxQ;
        long minVal;
        long maxVal;
        long numLevels;
    };
  
    
    class DllExport RSGISInitOutputImageSoilleGratin94 : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISInitOutputImageSoilleGratin94(double noDataVal, double dataVal, double borderVal, std::list<Q2DPxl> *pxQ);
        void calcImageValue(float *bandValues, int numBands, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent);
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISInitOutputImageSoilleGratin94();
    protected:
        double noDataVal;
        double dataVal;
        double borderVal;
        std::list<Q2DPxl> *pxQ;
    };
    
}}

#endif


