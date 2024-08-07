/*
 *  RSGISGenMeanSegImage.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/01/2012.
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

#ifndef RSGISGenMeanSegImage_h
#define RSGISGenMeanSegImage_h

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cmath>

#include "gdal_priv.h"

#include "common/rsgis-tqdm.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

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
    
    class DllExport RSGISGenMeanSegImage
    {
    public:
        RSGISGenMeanSegImage();
        void generateMeanImage(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *meanImg);
        void generateMeanImageUsingClumpTable(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *meanImg);
        void generateMeanImageUsingCalcImage(GDALDataset *spectral, GDALDataset *clumps, GDALDataset *meanImg);
        ~RSGISGenMeanSegImage();
    };
    
    
    
    class DllExport RSGISPopulateMeans : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISPopulateMeans(std::vector<rsgis::img::ImgClumpMean*> *clumpTable, unsigned int numSpecBands, unsigned int numClumps);
        void calcImageValue(float *bandValues, int numBands);
        ~RSGISPopulateMeans();
    protected:
        std::vector<rsgis::img::ImgClumpMean*> *clumpTable;
        unsigned int numSpecBands;
        unsigned int numClumps;
    };
    
    
    class DllExport RSGISApplyMeans2Output : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISApplyMeans2Output(std::vector<rsgis::img::ImgClumpMean*> *clumpTable, unsigned int numSpecBands, unsigned int numClumps);
        void calcImageValue(float *bandValues, int numBands, double *output);
        ~RSGISApplyMeans2Output();
    protected:
        std::vector<rsgis::img::ImgClumpMean*> *clumpTable;
        unsigned int numSpecBands;
        unsigned int numClumps;
    };
    
}}

#endif
