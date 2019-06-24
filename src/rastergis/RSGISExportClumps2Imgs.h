/*
 *  RSGISExportClumps2Imgs.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/03/2016.
 *  Copyright 2016 RSGISLib.
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

#ifndef RSGISExportClumps2Imgs_H
#define RSGISExportClumps2Imgs_H

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"
#include "common/RSGISImageException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISImageUtils.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISCalcImageStatsAndPyramids.h"

#include "utils/RSGISTextUtils.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_rastergis_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace rastergis{
    
    class DllExport RSGISExportClumps2Images
    {
    public:
        RSGISExportClumps2Images();
        void exportClumps2Images(GDALDataset *clumpsDataset, std::string outImgBase, std::string imgFileExt, std::string imageFormat, bool binaryOut, std::string minXPxl, std::string maxXPxl, std::string minYPxl, std::string maxYPxl, std::string tlX, std::string tlY, unsigned int ratBand=1);
        ~RSGISExportClumps2Images();
    };
    
    
    class DllExport RSGISCopyImageBand4ClumpExport : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISCopyImageBand4ClumpExport(unsigned int band, size_t fid, bool binOut);
        void calcImageValue(float *bandValues, int numBands, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void setFid(size_t fid){this->fid = fid;};
        ~RSGISCopyImageBand4ClumpExport();
    protected:
        unsigned int band;
        size_t fid;
        bool binOut;
    };
    
}}

#endif
