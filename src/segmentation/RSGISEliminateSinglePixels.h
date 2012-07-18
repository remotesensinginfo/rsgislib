/*
 *  RSGISEliminateSinglePixels.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/05/2012.
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

#ifndef RSGISEliminateSinglePixels_H
#define RSGISEliminateSinglePixels_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "math/RSGISMathsUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

namespace rsgis{namespace segment{
    
    class RSGISEliminateSinglePixels
    {
    public:
        RSGISEliminateSinglePixels();
        void eliminate(GDALDataset *inSpecData, GDALDataset *inClumpsData, GDALDataset *tmpData, std::string outputImage, float noDataVal, bool noDataValProvided, bool projFromImage, std::string proj, std::string format)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISEliminateSinglePixels();
    private:
        unsigned long findSinglePixels(GDALDataset *inClumpsData, GDALDataset *tmpData, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException);
        bool eliminateSinglePixels(GDALDataset *inSpecData, GDALDataset *inClumpsData, GDALDataset *tmpData, GDALDataset *outDataset, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException);
        inline float eucDistance(float **vals1, float **vals2, unsigned int numBands, unsigned int col1, unsigned int col2);
    };
    
}}

#endif
