/*
 *  RSGISMergeSegmentations.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 08/12/2014.
 *  Copyright 2014 RSGISLib.
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

#ifndef RSGISMergeSegmentations_H
#define RSGISMergeSegmentations_H

#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>

#include "common/RSGISAttributeTableException.h"
#include "common/rsgis-tqdm.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISImageStatistics.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_segmentation_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace segment{
    
    inline int RSGISSegStatsTextProgress( double dfComplete, const char *pszMessage, void *pData)
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
    
    class DllExport RSGISMergeSegmentations
    {
    public:
        RSGISMergeSegmentations();
        void includeRegions(GDALDataset *clumpsImage, GDALDataset *regionsImage, std::string outputImage, std::string format);
        ~RSGISMergeSegmentations();
    };
    
    class DllExport RSGISAddWithSegOffset : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISAddWithSegOffset(unsigned long offset);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        ~RSGISAddWithSegOffset();
    private:
        unsigned long offset;
    };
    
}}

#endif
