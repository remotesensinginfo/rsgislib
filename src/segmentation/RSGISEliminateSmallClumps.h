/*
 *  RSGISEliminateSmallClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/01/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISEliminateSmallClumps_h
#define RSGISEliminateSmallClumps_h

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <list>
#include <cmath>

#include "gdal_priv.h"

#include "common/RSGISAttributeTableException.h"
#include "common/RSGISFileException.h"

#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISStretchImage.h"

#include "rastergis/RSGISRasterAttUtils.h"

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
    
    class DllExport RSGISEliminateSmallClumps
    {
    public:
        RSGISEliminateSmallClumps();
        void eliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold);
        void stepwiseEliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail);
        void stepwiseIterativeEliminateSmallClumps(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail);
        void stepwiseEliminateSmallClumpsNoMean(GDALDataset *spectral, GDALDataset *clumps, unsigned int minClumpSize, float specThreshold, std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats, bool bandStatsAvail);
        ~RSGISEliminateSmallClumps();
    };
    
    class DllExport RSGISPopulateMeansPxlLocs : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISPopulateMeansPxlLocs(std::vector<rsgis::img::ImgClump*> *clumpTable, unsigned int numSpecBands);
        void calcImageValue(float *bandValues, int numBands, OGREnvelope extent);
        ~RSGISPopulateMeansPxlLocs();
    protected:
        std::vector<rsgis::img::ImgClump*> *clumpTable;
        unsigned int numSpecBands;
    };
    
    class DllExport RSGISRemoveClumpsBelowThreshold : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISRemoveClumpsBelowThreshold(float threshold, int *clumpHisto, size_t numHistVals);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        ~RSGISRemoveClumpsBelowThreshold();
    protected:
        float threshold;
        int *clumpHisto;
        size_t numHistVals;
    };
    
}}

#endif
