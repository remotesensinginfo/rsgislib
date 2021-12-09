/*
 *  RSGISCopyImage.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/11/2008.
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

#ifndef RSGISCopyImage_H
#define RSGISCopyImage_H

#include <iostream>
#include <bitset>
#include <string>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"

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
	
	
	class DllExport RSGISCopyImage : public RSGISCalcImageValue
		{
		public: 
			RSGISCopyImage(int numberOutBands);
			void calcImageValue(float *bandValues, int numBands, double *output);
			~RSGISCopyImage();
		};
    
    class DllExport RSGISCopyImageBandSelect : public RSGISCalcImageValue
    {
    public:
        RSGISCopyImageBandSelect(std::vector<unsigned int> bands);
        void calcImageValue(float *bandValues, int numBands, double *output);
        ~RSGISCopyImageBandSelect();
    protected:
        std::vector<unsigned int> bands;
    };


    class DllExport RSGISCopyImageBandToBits : public RSGISCalcImageValue
    {
    public:
        RSGISCopyImageBandToBits(int numberOutBands, unsigned int img_band, GDALDataType gdalDataType);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        ~RSGISCopyImageBandToBits();
    protected:
        unsigned int imgBandIdx;
        GDALDataType gdalDataType;
    };
	
}}

#endif


