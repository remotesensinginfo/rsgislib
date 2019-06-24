/*
 *  RSGISMergeSegmentationTiles.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/04/2013.
 *  Copyright 2013 RSGISLib.
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

#ifndef RSGISMergeSegmentationTiles_H
#define RSGISMergeSegmentationTiles_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "math/RSGISMathsUtils.h"

#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "common/RSGISAttributeTableException.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include "gdal_priv.h"
#include "gdal_rat.h"

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

    class DllExport RSGISMergeSegmentationTiles
    {
    public:
        RSGISMergeSegmentationTiles();
        void createTileBorderClumpMask(GDALDataset *borderMaskDataset, std::vector<std::string> inputImagePaths, unsigned int tileBoundary, unsigned int tileOverlap, unsigned int tileBody, std::string colsName);
        void mergeClumpBodies(GDALDataset *outputDataset, GDALDataset *borderMaskDataset, std::vector<std::string> inputImagePaths, unsigned int tileBoundary, unsigned int tileOverlap, unsigned int tileBody, std::string colsName);
        void mergeClumpImages(GDALDataset *outputDataset, std::vector<std::string> inputImagePaths, bool mergeRATs=false);
        ~RSGISMergeSegmentationTiles();
    protected:
        size_t numberBodyClumps(GDALRasterAttributeTable *gdalATT, std::string outColName, std::string clumpPosColName, int tileBody, size_t clumpsOffset);
        size_t numberClumps(GDALRasterAttributeTable *gdalATT, std::string outColName, size_t clumpsOffset);
        void addTileBodyClumps(GDALDataset *outputDataset, GDALDataset *tileDataset, GDALDataset *borderMaskDataset, GDALRasterAttributeTable *gdalATT, std::string outClumpIDColName, std::string clumpPosColName, unsigned int tileBody, unsigned int tileBoundary);
        void addTileBorder2Mask(GDALDataset *tileDataset, GDALDataset *borderMaskDataset, GDALRasterAttributeTable *gdalATT, std::string clumpPosColName, unsigned int tileBoundary);
        void addImageClumps(GDALDataset *outputDataset, GDALDataset *clumpsDataset, GDALRasterAttributeTable *gdalATT, std::string outClumpIDColName);
    };
    
    
}}
#endif

