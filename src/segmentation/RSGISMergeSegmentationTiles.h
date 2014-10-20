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

namespace rsgis{namespace segment{

    class DllExport RSGISMergeSegmentationTiles
    {
    public:
        RSGISMergeSegmentationTiles();
        void createTileBorderClumpMask(GDALDataset *borderMaskDataset, std::vector<std::string> inputImagePaths, unsigned int tileBoundary, unsigned int tileOverlap, unsigned int tileBody, std::string colsName) throw(rsgis::img::RSGISImageCalcException);
        void mergeClumpBodies(GDALDataset *outputDataset, GDALDataset *borderMaskDataset, std::vector<std::string> inputImagePaths, unsigned int tileBoundary, unsigned int tileOverlap, unsigned int tileBody, std::string colsName) throw(rsgis::img::RSGISImageCalcException);
        void mergeClumpImages(GDALDataset *outputDataset, std::vector<std::string> inputImagePaths) throw(rsgis::img::RSGISImageCalcException);
        ~RSGISMergeSegmentationTiles();
    protected:
        unsigned int findColumnIndex(const GDALRasterAttributeTable *gdalATT, std::string colName) throw(RSGISException);
        unsigned int findColumnIndexOrCreate(GDALRasterAttributeTable *gdalATT, std::string colName, GDALRATFieldType dType) throw(RSGISException);
        size_t numberBodyClumps(GDALRasterAttributeTable *gdalATT, unsigned int outColIdx, unsigned int clumpPosColIdx, int tileBody, size_t clumpsOffset) throw(RSGISException);
        size_t numberClumps(GDALRasterAttributeTable *gdalATT, unsigned int outColIdx, size_t clumpsOffset) throw(RSGISException);
        void addTileBodyClumps(GDALDataset *outputDataset, GDALDataset *tileDataset, GDALDataset *borderMaskDataset, const GDALRasterAttributeTable *gdalATT, unsigned int outClumpIDColIdx, unsigned int clumpPosColIdx, unsigned int tileBody, unsigned int tileBoundary) throw(rsgis::img::RSGISImageCalcException);
        void addTileBorder2Mask(GDALDataset *tileDataset, GDALDataset *borderMaskDataset, GDALRasterAttributeTable *gdalATT, std::string clumpPosColName, unsigned int tileBoundary) throw(rsgis::img::RSGISImageCalcException);
        void addImageClumps(GDALDataset *outputDataset, GDALDataset *clumpsDataset, const GDALRasterAttributeTable *gdalATT, unsigned int outClumpIDColIdx) throw(rsgis::img::RSGISImageCalcException);
    };
    
    
}}
#endif

