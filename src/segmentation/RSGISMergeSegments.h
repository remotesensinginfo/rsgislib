/*
 *  RSGISMergeSegments.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/04/2015.
 *  Copyright 2015 RSGISLib.
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

#ifndef RSGISMergeSegments_H
#define RSGISMergeSegments_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "common/RSGISAttributeTableException.h"

#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISImageStatistics.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "segmentation/RSGISMergeSegmentations.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISFindClumpNeighbours.h"
#include "rastergis/RSGISPopRATWithStats.h"

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
    
    struct rsgisClumpInfo
    {
        unsigned int clumpID;
        std::vector<unsigned int> origClumpIDs;
        double *meanVals;
        double *sumVals;
        double numPxls;
        unsigned int numVals;
        unsigned int selected;
        unsigned int noDataRegion;
        bool merge;
        rsgisClumpInfo *mergeTo;
        std::list<rsgisClumpInfo*> neighbours;
        bool removed;
    };
    
    struct rsgisClumpMergeInfo
    {
        unsigned int clumpID;
        std::vector<unsigned int> origClumpIDs;
        std::vector<long> clumpVal;
        bool merge;
        rsgisClumpMergeInfo *mergeTo;
        std::list<rsgisClumpMergeInfo*> neighbours;
    };
    
    class DllExport RSGISMergeSegments
    {
    public:
        RSGISMergeSegments();
        void mergeSelectedClumps(GDALDataset *clumpsImage, GDALDataset *valsImageDS, std::string clumps2MergeCol, std::string noDataClumpsCol)throw(rsgis::img::RSGISImageCalcException);
        void mergeEquivlentClumpsInRAT(GDALDataset *clumpsImage, std::vector<std::string> clumpsCols2Merge)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISMergeSegments();
    protected:
        double calcDist(double *valsRef, double *valsTest, int numVals)
        {
            double outVal = 0.0;
            for(int i = 0; i < numVals; ++i)
            {
                outVal = (valsRef[i] - valsTest[i])*(valsRef[i] - valsTest[i]);
            }
            outVal = sqrt(outVal/numVals);
            return outVal;
        };
        void mergeClump2Neighbours(rsgisClumpMergeInfo *baseClump, rsgisClumpMergeInfo *testClump, unsigned int outIdx);
    };
    

    
}}

#endif
