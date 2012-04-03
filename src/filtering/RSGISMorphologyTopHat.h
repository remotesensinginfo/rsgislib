/*
 *  RSGISMorphologyTopHat.h
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 03/03/2012
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

#ifndef RSGISMorphologyTopHat_H
#define RSGISMorphologyTopHat_H

#include <iostream>
#include <string>
#include "gdal_priv.h"

#include "img/RSGISImageBandException.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"

#include "filtering/RSGISMorphologyErode.h"
#include "filtering/RSGISMorphologyDilate.h"

#include "math/RSGISMatrices.h"

using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::img;

namespace rsgis{namespace filter{
    
    class RSGISImageMorphologyTopHat
    {
    public:
        RSGISImageMorphologyTopHat();
        void performBlackTopHat(GDALDataset *dataset, string outputImage, string tempImage, bool useMemory, Matrix *matrixOperator) throw(RSGISImageCalcException, RSGISImageBandException);
        void performWhiteTopHat(GDALDataset *dataset, string outputImage, string tempImage, bool useMemory, Matrix *matrixOperator) throw(RSGISImageCalcException, RSGISImageBandException);
        ~RSGISImageMorphologyTopHat();
    };
    
    
    class RSGISMorphologyImageDiff : public RSGISCalcImageValue
    {
    public: 
        RSGISMorphologyImageDiff(int numberOutBands); 			
        virtual RSGISCalcImage* getCalcImage()throw(RSGISImageException){return new RSGISCalcImage(this, "", true);};
        void calcImageValue(float *bandValues, int numBands, float *output)throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        ~RSGISMorphologyImageDiff();
    };
    
}}

#endif
