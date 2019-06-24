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

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_filter_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace filter{
    
    class DllExport RSGISImageMorphologyTopHat
    {
    public:
        RSGISImageMorphologyTopHat();
        void performBlackTopHat(GDALDataset *dataset, std::string outputImage, std::string tempImage, bool useMemory, rsgis::math::Matrix *matrixOperator, std::string format, GDALDataType outDataType);
        void performWhiteTopHat(GDALDataset *dataset, std::string outputImage, std::string tempImage, bool useMemory, rsgis::math::Matrix *matrixOperator, std::string format, GDALDataType outDataType);
        ~RSGISImageMorphologyTopHat();
    };
    
    
    class DllExport RSGISMorphologyImageDiff : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISMorphologyImageDiff(int numberOutBands); 			
        virtual rsgis::img::RSGISCalcImage* getCalcImage(){return new rsgis::img::RSGISCalcImage(this, "", true);};
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not Implemented.");};
        ~RSGISMorphologyImageDiff();
    };
    
}}

#endif
