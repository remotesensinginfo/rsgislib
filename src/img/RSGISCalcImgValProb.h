/*
 *  RSGISCalcImgValProb.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/06/2016.
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

#ifndef RSGISCalcImgValProb_H
#define RSGISCalcImgValProb_H

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageUtils.h"

namespace rsgis{namespace img{
    
    class DllExport RSGISCalcImgValProb
    {
    public:
        RSGISCalcImgValProb(){};
        void calcMaskImgPxlValProb(GDALDataset *inImgDS, std::vector<unsigned int> inImgBandIdxs, GDALDataset *inMaskDS, int maskVal, std::string outputImage, std::string gdalFormat, std::vector<float> histBinWidths, bool calcHistBinWidth, bool useImgNoData)throw(RSGISImageCalcException);
        ~RSGISCalcImgValProb(){};
    };
  
}}

#endif
