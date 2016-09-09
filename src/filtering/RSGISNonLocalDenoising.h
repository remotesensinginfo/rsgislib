/*
 *  RSGISNonLocalDenoising.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 23/02/2013.
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

#ifndef RSGISNonLocalDenoising_H
#define RSGISNonLocalDenoising_H

#include <iostream>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "gdal_priv.h"

#include "common/RSGISImageException.h"

#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"

namespace rsgis{namespace filter{
	
	class DllExport RSGISApplyNonLocalDenoising
    {
        /**
            Implemention of the Non-local image denoising algorithm described in:
         
            Buades, A., Coll, B. & Morel, J.M., A non-local algorithm for image denoising. 2005.
            IEEE Computer Society Conference on Computer Vision and Pattern Recognition.

         */
        
    public: 
        RSGISApplyNonLocalDenoising();
        void ApplyFilter(GDALDataset **inputImageDS, int numDS, std::string outputImage, unsigned int filterWindowSize, unsigned int searchWindowSize, double aPar=2.0, double hPar=2.0, std::string gdalFormat="ENVI", GDALDataType gdalDataType=GDT_Float32) throw(rsgis::img::RSGISImageCalcException,rsgis::img::RSGISImageBandException);
        ~RSGISApplyNonLocalDenoising();
    protected:
        unsigned int searchWindowSize; // Window size of search space
        GDALDataset **inputImageDS; // GDAL dataset for input image
    };
}}

#endif



