/*
 *  RSGISMorphologyGradient.h
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

#ifndef RSGISMorphologyGradient_H
#define RSGISMorphologyGradient_H

#include <iostream>
#include <string>
#include "gdal_priv.h"

#include "img/RSGISImageBandException.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

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
    
    class DllExport RSGISImageMorphologyGradient
    {
    public:
        RSGISImageMorphologyGradient();
        void calcGradientImage(GDALDataset **dataset, std::string outputImage, rsgis::math::Matrix *matrixOperator, std::string format, GDALDataType outDataType);
        void calcGradientImageAll(GDALDataset **dataset, std::string outputImage, rsgis::math::Matrix *matrixOperator, std::string format, GDALDataType outDataType);
        ~RSGISImageMorphologyGradient(){};
    };
    
    class DllExport RSGISMorphologyGradient : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISMorphologyGradient(int numberOutBands, rsgis::math::Matrix *matrixOperator); 			
        virtual rsgis::img::RSGISCalcImage* getCalcImage(){return new rsgis::img::RSGISCalcImage(this, "", true);};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        ~RSGISMorphologyGradient();
    private:
        rsgis::math::Matrix *matrixOperator;
    };
    
    class DllExport RSGISMorphologyGradientAll : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISMorphologyGradientAll(int numberOutBands, rsgis::math::Matrix *matrixOperator); 			
        virtual rsgis::img::RSGISCalcImage* getCalcImage(){return new rsgis::img::RSGISCalcImage(this, "", true);};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
        ~RSGISMorphologyGradientAll();
    private:
        rsgis::math::Matrix *matrixOperator;
    };
    
}}

#endif
