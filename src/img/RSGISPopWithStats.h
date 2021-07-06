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

#include "common/rsgis-tqdm.h"

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
    
    inline int RSGISTQDMProgress( double dfComplete, const char *pszMessage, void *pData)
    {
        rsgis_tqdm *pbar = reinterpret_cast<rsgis_tqdm *>( pData );
        int nPercent = int(dfComplete*100);
        if(nPercent >= 100)
        {
            pbar->finish();
        }
        else
        {
            pbar->progress(nPercent, 100);
        }
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
        void calcImageValue(float *bandValues, int numBands);
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
        void calcImageValue(float *bandValues, int numBands);
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

