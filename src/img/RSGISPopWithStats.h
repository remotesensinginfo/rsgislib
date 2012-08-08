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

#include "gdal.h"
#include "cpl_string.h"
#include "gdal_priv.h"

namespace rsgis { namespace img {
    
    inline int StatsTextProgress( double dfComplete, const char *pszMessage, void *pData)
    {
        if( pszMessage != NULL )
            printf( "%d%% complete: %s\r", (int) (dfComplete*100), pszMessage );
        else
            printf( "%d%% complete.\r", (int) (dfComplete*100) );
        
        return TRUE;
    };
    
    
    class RSGISPopWithStats
    {
    public:
        void addpyramid( GDALDataset *handle );
        void getRangeMean(float *pData,int size,float &min,float &max,float &mean, bool ignore, float ignoreVal);
        float getStdDev(float *pData, int size, float fmean, bool ignore, float ignoreVal);
        float* getSubSampledImage( GDALRasterBand *hBand, int nLevel, int *pnSize );
        void getHistogramIgnore( GDALRasterBand *pBand, double dfMin, double dfMax, int nBuckets, int *panHistogram, int bIncludeOutOfRange, bool bIgnore, float fIgnore );
        void calcPopStats( GDALDataset *hHandle, bool bIgnore, float fIgnoreVal, bool bPyramid );
    private:
        static const int HISTO_NBINS = 256;
        //static const int CONTIN_STATS_MIN_LIMIT = 1000000;
        static const int MINOVERVIEWDIM = 33;
    };
}}
 
#endif

