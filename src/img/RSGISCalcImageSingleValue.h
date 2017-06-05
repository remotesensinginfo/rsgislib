/*
 *  RSGISCalcImageSingleValue.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/07/2008.
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

#ifndef RSGISCalcImageSingleValue_H
#define RSGISCalcImageSingleValue_H

#include <math.h>
#include <iostream>
#include <string>
#include "img/RSGISImageCalcException.h"
#include <geos/geom/Envelope.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/Point.h>

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
	
	class DllExport RSGISCalcImageSingleValue
		{
		public:
			RSGISCalcImageSingleValue(int numOutputValues);
			virtual void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException) = 0;
			virtual void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException) = 0;
			virtual void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) throw(RSGISImageCalcException) = 0;
			virtual void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(RSGISImageCalcException) = 0;
			int getNumberOfOutValues();
			virtual double* getOutputValues() throw(RSGISImageCalcException) = 0;
			virtual void reset() = 0;
			virtual ~RSGISCalcImageSingleValue();
		protected:
			int numOutputValues;
			double *outputValues;
		};
}}

#endif


