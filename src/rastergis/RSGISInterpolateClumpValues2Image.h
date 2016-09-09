/*
 *  RSGISInterpolateClumpValues2Image.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/09/2013.
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

#ifndef RSGISInterpolateClumpValues2Image_H
#define RSGISInterpolateClumpValues2Image_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>

#include "common/RSGISAttributeTableException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISImageInterpolation.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalcValue.h"
#include "rastergis/RSGISRATCalc.h"

#include "math/RSGIS2DInterpolation.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

namespace rsgis{namespace rastergis{
    
    class DllExport RSGISInterpolateClumpValues2Image
    {
    public:
        RSGISInterpolateClumpValues2Image();
        void interpolateImageFromClumps(GDALDataset *clumpsDataset, std::string selectField, std::string eastingsField, std::string northingsField, std::string valueField, std::string outputFile, std::string imageFormat, GDALDataType dataType, rsgis::math::RSGIS2DInterpolator *interpolator, unsigned int ratband=1)throw(rsgis::RSGISAttributeTableException);
        ~RSGISInterpolateClumpValues2Image();
    };
    
    
    class DllExport RSGISExtractPtValues : public RSGISRATCalcValue
    {
    public:
        RSGISExtractPtValues(std::vector<rsgis::math::RSGISInterpolatorDataPoint> *pts, unsigned int *numSelectValues);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISExtractPtValues();
    private:
        std::vector<rsgis::math::RSGISInterpolatorDataPoint> *pts;
        unsigned int *numSelectValues;
    };
    
    
}}

#endif


