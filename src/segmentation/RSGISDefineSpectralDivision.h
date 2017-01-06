/*
 *  RSGISDefineSpectralDivision.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/03/2012.
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

#ifndef RSGISDefineSpectralDivision_H
#define RSGISDefineSpectralDivision_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "common/RSGISAttributeTableException.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISImageStatistics.h"

#include "math/RSGISMathsUtils.h"

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
    
    class DllExport RSGISDefineSpectralDivision
    {
    public:
        RSGISDefineSpectralDivision();
        void findSpectralDivision(GDALDataset *inData, std::string outputImage, unsigned int subDivision, float noDataVal, bool noDataValProvided, bool projFromImage, std::string proj, std::string format)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISDefineSpectralDivision();
    private:
        void assignToCategory(GDALDataset *reflDataset, GDALDataset *catsDataset, std::vector<std::pair<float, float> > **catThresholds, unsigned int numBands, unsigned int numCats, float noDataVal, bool noDataValProvided)throw(rsgis::img::RSGISImageCalcException);
        void generateSpectralDivThresholds(std::pair<float, float> *pThreshold, unsigned int bandIdx, unsigned int numBands, unsigned int subDivision, std::vector<std::pair<float, float> > **bandThresholds, std::vector<std::pair<float, float> > **catThresholds);
    };
    
}}

#endif
