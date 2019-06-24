/*
 *  RSGISImageBand2Matrix.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/11/2008.
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

#ifndef RSGISImageBand2Matrix_H
#define RSGISImageBand2Matrix_H

#include <iostream>
#include <string>
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"
#include "math/RSGISMatrices.h"

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
	
	class DllExport RSGISImageBand2Matrix : public RSGISCalcImageSingleValue
		{
		public:
			RSGISImageBand2Matrix(int numOutputValues, int band, int x, int y);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB);
			void calcImageValue(float *bandValuesImage, int numBands, int band);
			void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent);
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt);
			rsgis::math::Matrix* getMatrix();
			virtual double* getOutputValues();
			void reset(int band, int x, int y);
			virtual ~RSGISImageBand2Matrix();
		protected:
			virtual void reset();
			rsgis::math::Matrix *matrix;
			int matrixCounter;
			int band;
		};
}}

#endif

