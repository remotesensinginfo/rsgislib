/*
 *  RSGISRelabelPixelValuesFromLUT.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/02/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#ifndef RSGISRelabelPixelValuesFromLUT_H
#define RSGISRelabelPixelValuesFromLUT_H

#include <cmath>
#include <limits>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

#include "math/RSGISMatrices.h"

#include <gsl/gsl_matrix.h>

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

namespace rsgis { namespace img {
	
    class DllExport RSGISRelabelPixelValuesFromLUT
    {
    public:
        RSGISRelabelPixelValuesFromLUT();
        void relabelPixelValues(GDALDataset *inData, std::string outputFile, std::string matrixLUTFile, std::string imageFormat);
        ~RSGISRelabelPixelValuesFromLUT();
    };
    
	
	class DllExport RSGISRelabelPixelValuesFromLUTCalcVal : public RSGISCalcImageValue
	{
	public:
		RSGISRelabelPixelValuesFromLUTCalcVal(int numOutBands, gsl_matrix *lut);
		void calcImageValue(float *bandValues, int numBands, double *output);
		~RSGISRelabelPixelValuesFromLUTCalcVal();
	private:
		gsl_matrix *lut;
	};
	
}}

#endif
