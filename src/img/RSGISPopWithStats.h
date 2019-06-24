/*
 *  RSGISPopWithStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/04/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
 *
 *  This code is edited from code provided by 
 *  Sam Gillingham
 *
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

#ifndef RSGISPopWithStats_H
#define RSGISPopWithStats_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <math.h>

#include "gdal_priv.h"
#include "gdal_rat.h"
#include "cpl_string.h"

#include "common/RSGISImageException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImage.h"

#include "utils/RSGISTextUtils.h"

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

namespace rsgis { namespace img {
    
    inline int StatsTextProgress( double dfComplete, const char *pszMessage, void *pData)
    {
        int nPercent = int(dfComplete*100);
        int *pnLastComplete = (int*)pData;
        
        if(nPercent < 10)
        {
            nPercent = 0;
        }
        else if(nPercent < 20)
        {
            nPercent = 10;
        }
        else if(nPercent < 30)
        {
            nPercent = 20;
        }
        else if(nPercent < 40)
        {
            nPercent = 30;
        }
        else if(nPercent < 50)
        {
            nPercent = 40;
        }
        else if(nPercent < 60)
        {
            nPercent = 50;
        }
        else if(nPercent < 70)
        {
            nPercent = 60;
        }
        else if(nPercent < 80)
        {
            nPercent = 70;
        }
        else if(nPercent < 90)
        {
            nPercent = 80;
        }
        else if(nPercent < 95)
        {
            nPercent = 90;
        }
        else
        {
            nPercent = 100;
        }
        
        if( (pnLastComplete != NULL) && (nPercent != *pnLastComplete ))
        {
            if(nPercent == 0)
            {
                std::cout << "Started ." << nPercent << "." << std::flush;
            }
            else if(nPercent == 100)
            {
                std::cout << "." << nPercent << ". Complete." << std::endl;
            }
            else
            {
                std::cout << "." << nPercent << "." << std::flush;
            }
        }
        
        *pnLastComplete = nPercent;
        
        return true;
    };
    
    class DllExport RSGISPopWithStats
    {
    public:
        RSGISPopWithStats(){};
        void calcPopStats( GDALDataset *imgDS, bool useNoDataVal, float noDataVal, bool calcPyramid, std::vector<int> decimatFactors=std::vector<int>());
        ~RSGISPopWithStats(){};
    private:
        void addPyramids(GDALDataset *imgDS, std::vector<int> decimatFactors);
        unsigned int findColumnIndexOrCreate(GDALRasterAttributeTable *gdalATT, std::string colName, GDALRATFieldType dType, GDALRATFieldUsage dUsage=GFU_Generic);
    };
    
    
    
    
    class DllExport RSGISCalcImageMinMaxMean : public RSGISCalcImageValue
    {
    public:
        RSGISCalcImageMinMaxMean(int numVals, bool useNoData, double noDataVal, double *minVal, double *maxVal, double *sumVal, unsigned long *nVals);
        void calcImageValue(float *bandValues, int numBands, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcImageMinMaxMean();
    protected:
        bool *first;
        int numVals;
        bool useNoData;
        double noDataVal;
        double *minVal;
        double *maxVal;
        double *sumVal;
        unsigned long *nVals;
    };
    
    
    class DllExport RSGISCalcImageStdDevPopHist : public RSGISCalcImageValue
    {
    public:
        RSGISCalcImageStdDevPopHist(int numVals, bool useNoData, double noDataVal, double *minVal, double *maxVal, double *meanVal, double *sumVal, unsigned long *nVals, double *histMin, double *histMax, double *histWidth, unsigned int **bandHist, unsigned int numBins);
        void calcImageValue(float *bandValues, int numBands, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcImageStdDevPopHist();
    protected:
        bool *first;
        int numVals;
        bool useNoData;
        double noDataVal;
        double *minVal;
        double *maxVal;
        double *meanVal;
        double *sumVal;
        unsigned long *nVals;
        double *histMin;
        double *histMax;
        double *histWidth;
        unsigned int **bandHist;
        unsigned int numBins;
    };

}}
 
#endif

