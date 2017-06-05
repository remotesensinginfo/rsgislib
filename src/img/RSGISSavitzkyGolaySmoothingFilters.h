/*
 *  RSGISSavitzkyGolaySmoothingFilters.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/06/2010.
 *  Copyright 2010 RSGISLib.
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

#ifndef RSGISSavitzkyGolaySmoothingFilters_H
#define RSGISSavitzkyGolaySmoothingFilters_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"

#include "math/RSGISPolyFit.h"
#include "math/RSGISVectors.h"

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
	
	/***
	 *
	 * This class provides an implementation of the Savitzky-Golay smoothing filters 
	 * these filters are commonly used to smooth spectral data.
	 *
	 * Smoothing is undertaken through a process of polynominal fitting.
	 *
	 */
	
	class DllExport RSGISSavitzkyGolaySmoothingFilters : public RSGISCalcImageValue
	{
	public: 
		RSGISSavitzkyGolaySmoothingFilters(int numberOutBands, int order, int window, rsgis::math::Vector *imagebandValues);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException);
		~RSGISSavitzkyGolaySmoothingFilters();
	private:
		int order;
		int window;
        rsgis::math::Vector *imagebandValues; 
	};
	
}}

#endif

