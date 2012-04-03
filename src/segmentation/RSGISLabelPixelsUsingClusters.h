/*
 *  RSGISLabelPixelsUsingClusters.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/02/2012.
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

#ifndef RSGISLabelPixelsUsingClusters_H
#define RSGISLabelPixelsUsingClusters_H

#include <iostream>
#include <string>
#include <math.h>

#include "common/RSGISImageException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "math/RSGISMatrices.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

using namespace std;
using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::img;

namespace rsgis{namespace segment{
    
    class RSGISLabelPixelsUsingClusters
    {
    public:
        RSGISLabelPixelsUsingClusters();
        void labelPixelsUsingClusters(GDALDataset **datasets, int numDatasets, string output, string clusterCentresFile, bool ignoreZeros, string imageFormat, bool useImageProj, string outProjStr) throw(RSGISImageException);
        ~RSGISLabelPixelsUsingClusters();
    };
    
    class RSGISLabelPixelsUsingClustersCalcImg : public RSGISCalcImageValue
    {
    public: 
        RSGISLabelPixelsUsingClustersCalcImg(int numberOutBands, Matrix *clusterCentres, bool ignoreZeros);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISLabelPixelsUsingClustersCalcImg();
    private:
        Matrix *clusterCentres;
        bool ignoreZeros;
    };
    
}}

#endif




