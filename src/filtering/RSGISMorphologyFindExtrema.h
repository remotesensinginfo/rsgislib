/*
 *  RSGISMorphologyFindExtrema.h
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 01/03/2012
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

#ifndef RSGISMorphologyFindExtrema_H
#define RSGISMorphologyFindExtrema_H

#include <iostream>
#include <string>
#include "gdal_priv.h"

#include "img/RSGISImageBandException.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

#include "math/RSGISMatrices.h"

using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::img;

namespace rsgis{namespace filter{
    
    class RSGISImageMorphologyFindExtrema
    {
    public:
        enum RSGISMinimaOutputs
        {
            binary,
            sequential
        };
        RSGISImageMorphologyFindExtrema();
        void findMinima(GDALDataset **datasets, string outputImage, Matrix *matrixOperator, RSGISMinimaOutputs outputType, bool allowEquals) throw(RSGISImageCalcException, RSGISImageBandException);
        void findMinimaAll(GDALDataset **datasets, string outputImage, Matrix *matrixOperator, RSGISMinimaOutputs outputType, bool allowEquals) throw(RSGISImageCalcException, RSGISImageBandException);
        ~RSGISImageMorphologyFindExtrema(){};
    };
    
    class RSGISMorphologyFindLocalMinima : public RSGISCalcImageValue
    {
    public: 
        RSGISMorphologyFindLocalMinima(int numberOutBands, Matrix *matrixOperator, RSGISImageMorphologyFindExtrema::RSGISMinimaOutputs outputType, bool allowEquals); 			
        virtual RSGISCalcImage* getCalcImage()throw(RSGISImageException){return new RSGISCalcImage(this, "", true);};
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        ~RSGISMorphologyFindLocalMinima();
    private:
        Matrix *matrixOperator;
        RSGISImageMorphologyFindExtrema::RSGISMinimaOutputs outputType;
        bool allowEquals;
        unsigned long *outVal;
    };
    
    class RSGISMorphologyFindLocalMinimaAll : public RSGISCalcImageValue
    {
    public: 
        RSGISMorphologyFindLocalMinimaAll(int numberOutBands, Matrix *matrixOperator, RSGISImageMorphologyFindExtrema::RSGISMinimaOutputs outputType, bool allowEquals); 			
        virtual RSGISCalcImage* getCalcImage()throw(RSGISImageException){return new RSGISCalcImage(this, "", true);};
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented.");};
        ~RSGISMorphologyFindLocalMinimaAll();
    private:
        Matrix *matrixOperator;
        RSGISImageMorphologyFindExtrema::RSGISMinimaOutputs outputType;
        bool allowEquals;
        unsigned long outVal;
    };
    
}}

#endif
