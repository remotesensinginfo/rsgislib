/*
 *  RSGISRegionGrowSegmentsPixels.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/03/2012.
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

#ifndef RSGISRegionGrowSegmentsPixels_H
#define RSGISRegionGrowSegmentsPixels_H

#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <list>
#include <deque>

#include "common/RSGISImageException.h"

#include "utils/RSGISTextException.h"
#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageUtils.h"
//#include "rastergis/RSGISHierarchicalClumps.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

namespace rsgis{namespace segment{
/*
    struct DllExport RSGISRegionGrowPxlSeeds
    {
        RSGISRegionGrowPxlSeeds()
        {
            
        }
        RSGISRegionGrowPxlSeeds(unsigned long fid, unsigned int xPxl, unsigned int yPxl)
        {
            this->fid = fid;
            this->xPxl = xPxl;
            this->yPxl = yPxl;
        }
        unsigned long fid;
        unsigned int xPxl;
        unsigned int yPxl;
    };
    
    
    class DllExport RSGISRegionGrowSegmentsPixels
    {
    public:
        RSGISRegionGrowSegmentsPixels(GDALDataset *inRefl, GDALDataset *outDataset)throw(rsgis::RSGISImageException);
        void performRegionGrowing(std::vector<RSGISRegionGrowPxlSeeds> *seeds, float threshold)throw(rsgis::img::RSGISImageCalcException);
        void performRegionGrowing(std::vector<RSGISRegionGrowPxlSeeds> *seeds, float initThreshold, float thresholdIncrements, float maxThreshold, unsigned int maxIterations)throw(rsgis::img::RSGISImageCalcException);
        void growSeed(RSGISRegionGrowPxlSeeds *seed, float threshold)throw(rsgis::img::RSGISImageCalcException);
        void growSeed(RSGISRegionGrowPxlSeeds *seed, float initThreshold, float thresholdIncrements, float maxThreshold, unsigned int maxNumPixels)throw(rsgis::img::RSGISImageCalcException);
        static std::vector<RSGISRegionGrowPxlSeeds>* parseSeedsText(std::string inFile) throw(rsgis::utils::RSGISTextException);
        ~RSGISRegionGrowSegmentsPixels();
    private:
        double eucDistance(float *vals1, float *vals2, unsigned int numVals)
        {
            double sqSum = 0;
            for(unsigned int i = 0; i < numVals; ++i)
            {
                sqSum += (vals1[i] - vals2[i])*(vals1[i] - vals2[i]);
            }
            
            return sqrt(sqSum)/numVals;
        }
        GDALDataset *inRefl;
        GDALDataset *outDataset;
        unsigned int width;
        unsigned int height;
        unsigned int numSpecBands;
        
        GDALRasterBand **spectralBands;
        GDALRasterBand *outBand;
        float *specPxlSumVals;
        float *specPxlMeanVals;
        unsigned int numPxls;
        float *specCPxlVals;
        
        bool **mask;
    };
    
    
    class DllExport RSGISFindRegionGrowingSeeds
    {
    public:
        RSGISFindRegionGrowingSeeds();
        std::vector<RSGISRegionGrowPxlSeeds>* findSeeds(GDALDataset *inRefl, GDALDataset *clumps, std::vector<rsgis::rastergis::RSGISSubClumps*> *regions)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISFindRegionGrowingSeeds();
    };
    */
}}

#endif

