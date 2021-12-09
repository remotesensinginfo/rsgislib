/*
 *  RSGISMaskImage.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/05/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISMaskImage_H
#define RSGISMaskImage_H

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISImageUtils.h"

#include "boost/math/special_functions/fpclassify.hpp"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
	
	class DllExport RSGISMaskImage
		{
		public: 
			RSGISMaskImage();
			void maskImage(GDALDataset *dataset, GDALDataset *mask, std::string outputImage, std::string imageFormat, GDALDataType outDataType, double outputValue, std::vector<float> maskValues);
            void genFiniteImgMask(GDALDataset *dataset, std::string outputImage, std::string imageFormat);
            void genValidImgMask(GDALDataset **dataset, unsigned int numImages, std::string outputImage, std::string imageFormat, float noDataVal);
            void genImgEdgeMask(GDALDataset *dataset, std::string outputImage, std::string imageFormat, unsigned int nEdgePxls);
        };
	
	class DllExport RSGISApplyImageMask : public RSGISCalcImageValue
		{
		public: 
			RSGISApplyImageMask(int numberOutBands, double outputValue, std::vector<float> maskValues);
			void calcImageValue(float *bandValues, int numBands, double *output);
			~RSGISApplyImageMask();
		protected:
			double outputValue;
            std::vector<float> maskValues;
		};
    
    class DllExport RSGISCreateFiniteImageMask : public RSGISCalcImageValue
    {
    public:
        RSGISCreateFiniteImageMask();
        void calcImageValue(float *bandValues, int numBands, double *output);
        ~RSGISCreateFiniteImageMask();
    };
    
    class DllExport RSGISGenValidImageMask : public RSGISCalcImageValue
    {
    public:
        RSGISGenValidImageMask(float noDataVal);
        void calcImageValue(float *bandValues, int numBands, double *output);
        ~RSGISGenValidImageMask();
    protected:
        float noDataVal;
    };
	
}}

#endif

