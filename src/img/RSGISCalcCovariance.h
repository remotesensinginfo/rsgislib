/*
 *  RSGISCalcCovariance.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/07/2008.
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

#ifndef RSGISCalcCovariance_H
#define RSGISCalcCovariance_H

#include <iostream>
#include <string>
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
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
	
	class DllExport RSGISCalcCovariance : public RSGISCalcImageSingleValue
		{
		public:
			RSGISCalcCovariance(int numOutputValues, rsgis::math::Matrix *aMeans, rsgis::math::Matrix *bMeans);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB);
			double* getOutputValues();
			void reset();
		private:
			int n;
			double sum;
			rsgis::math::Matrix *aMeans;
			rsgis::math::Matrix *bMeans;
		};
    
    
    
    class DllExport RSGISCreateCovarianceMatrix: public RSGISCalcImageValue
    {
    public:
        RSGISCreateCovarianceMatrix(rsgis::math::Vector *meanVector, rsgis::math::Matrix *covarianceMatrix);
        void calcImageValue(float *bandValues, int numBands);
        ~RSGISCreateCovarianceMatrix();
    private:
        rsgis::math::Vector *meanVector;
        rsgis::math::Matrix *covarianceMatrix;
    };
    
    
}}

#endif
