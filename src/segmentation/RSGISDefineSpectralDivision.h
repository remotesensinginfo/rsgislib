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

//#include "utils/RSGISAttributeTable.h"

#include "math/RSGISMathsUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"


using namespace std;
using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::img;

namespace rsgis{namespace segment{
    
    class RSGISDefineSpectralDivision
    {
    public:
        RSGISDefineSpectralDivision();
        void findSpectralDivision(GDALDataset *inData, string outputImage, unsigned int subDivision, float noDataVal, bool noDataValProvided, bool projFromImage, string proj, string format)throw(RSGISImageCalcException);
        ~RSGISDefineSpectralDivision();
    private:
        void assignToCategory(GDALDataset *reflDataset, GDALDataset *catsDataset, vector<pair<float, float> > **catThresholds, unsigned int numBands, unsigned int numCats, float noDataVal, bool noDataValProvided)throw(RSGISImageCalcException);
        void generateSpectralDivThresholds(pair<float, float> *pThreshold, unsigned int bandIdx, unsigned int numBands, unsigned int subDivision, vector<pair<float, float> > **bandThresholds, vector<pair<float, float> > **catThresholds);
    };
    
}}

#endif
