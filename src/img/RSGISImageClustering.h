/*
 *  RSGISImageClustering.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/02/2012.
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

#ifndef RSGISImageClustering_H
#define RSGISImageClustering_H

#include <iostream>
#include <string>
#include <math.h>

#include "common/RSGISImageException.h"

#include "math/RSGISClustering.h"
#include "math/RSGISClustererException.h"
#include "math/RSGISMatrices.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_img_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace img{
    
    class DllExport RSGISImageClustering
    {
    public:
        RSGISImageClustering();
        void findKMeansCentres(GDALDataset *dataset, std::string outputMatrix, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, rsgis::math::InitClustererMethods initMethod)throw(rsgis::RSGISImageException, rsgis::math::RSGISClustererException);
        void findISODataCentres(GDALDataset *dataset, std::string outputMatrix, unsigned int numClusters, unsigned int maxNumIterations, unsigned int subSample, bool ignoreZeros, float degreeOfChange, rsgis::math::InitClustererMethods initMethod, float minDistBetweenClusters, unsigned int minNumFeatures, float maxStdDev, unsigned int minNumClusters, unsigned int startIteration, unsigned int endIteration)throw(rsgis::RSGISImageException, rsgis::math::RSGISClustererException);
        std::vector< std::vector<float> >* sampleImage(GDALDataset *dataset, unsigned int subSample, bool ignoreZeros) throw(rsgis::RSGISImageException);
        ~RSGISImageClustering();
    };
    
}}

#endif




