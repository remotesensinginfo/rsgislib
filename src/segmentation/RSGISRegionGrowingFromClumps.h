/*
 *  RSGISRegionGrowingFromClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/01/2012.
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

#ifndef RSGISRegionGrowingFromClumps_h
#define RSGISRegionGrowingFromClumps_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <math.h>

#include "gdal_priv.h"

#include "gsl/gsl_statistics_double.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"

#include "utils/RSGISTextUtils.h"

#include "math/RSGISMathsUtils.h"

using namespace rsgis::utils;
using namespace rsgis::math;
using namespace rsgis::img;

namespace rsgis{namespace segment{
    
    struct ImgSeeds
    {
        ImgSeeds()
        {
            
        };
        ImgSeeds(unsigned int xPxl, unsigned int yPxl)
        {
            this->xPxl = xPxl;
            this->yPxl = yPxl;
        };
        ImgSeeds(unsigned int xPxl, unsigned int yPxl, unsigned long seedID)
        {
            this->xPxl = xPxl;
            this->yPxl = yPxl;
            this->seedID = seedID;
        };
        unsigned int xPxl;
        unsigned int yPxl;
        unsigned long seedID;
    };
    
    struct ClumpSeed
    {
        ClumpSeed()
        {
            
        };
        ClumpSeed(unsigned long clumpID, unsigned long seedID)
        {
            this->clumpID = clumpID;
            this->seedID = seedID;
        };
        unsigned long clumpID;
        unsigned long seedID;
    };
    
    struct BandThreshold
    {
        BandThreshold()
        {
            
        };
        BandThreshold(unsigned int band, float threshold)
        {
            this->band = band;
            this->threshold = threshold;
        };
        unsigned int band;
        float threshold;
    };
    
    class RSGISFindClumpIDs
    {
    public:
        RSGISFindClumpIDs();
        void exportClumpIDsAsTxtFile(GDALDataset *clumps, string outputText, vector<ImgSeeds> *seedPxls) throw(RSGISImageCalcException);
        vector<ClumpSeed>* readClumpSeedIDs(string inputTextFile)throw(RSGISTextException);
        ~RSGISFindClumpIDs();
    };
    
    
    class RSGISRegionGrowingSegmentation
    {
    public:
        RSGISRegionGrowingSegmentation();
        void performRegionGrowUsingClumps(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *output, vector<ClumpSeed> *seeds, float initThreshold, float thresholdIncrements, float maxThreshold, unsigned int maxIterations )throw(RSGISImageCalcException);
        ~RSGISRegionGrowingSegmentation();
    protected:
        ImgClumpRG* growRegion(float threshold, unsigned int maxNumIterations, vector<ImgClumpRG*> *clumpTab, unsigned int numSpecBands, unsigned long seedClumpID, unsigned long seed, list<unsigned long> *regionClumps)throw(RSGISImageCalcException);
    };
    
    
    class RSGISGenerateSeeds
    {
    public:
        RSGISGenerateSeeds();
        void genSeedsHistogram(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *output, vector<BandThreshold> *thresholds) throw(RSGISImageCalcException);
        void genSeedsHistogram(GDALDataset *spectral, GDALDataset *clumps, string outputFile, vector<BandThreshold> *thresholds) throw(RSGISImageCalcException);
        ~RSGISGenerateSeeds();
    };
    
    class RSGISSelectClumps
    {
    public:
        enum ClumpSelection 
		{
			min,
            max,
            median,
            mean,
            percent75th,
            percent95th
		};
        RSGISSelectClumps();
        void selectClumps(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *largeClumps, GDALDataset *output, ClumpSelection selection) throw(RSGISImageCalcException);
        void selectClumps(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *largeClumps, string outputFile, ClumpSelection selection) throw(RSGISImageCalcException);
        ~RSGISSelectClumps();
    };
    
}}

#endif
