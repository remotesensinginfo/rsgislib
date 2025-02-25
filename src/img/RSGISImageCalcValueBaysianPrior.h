/*
 *  RSGISImageCalcValueBaysianPrior.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 12/12/2008.
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

#ifndef RSGISImageCalcValueBaysianPrior_H
#define RSGISImageCalcValueBaysianPrior_H

#include <iostream>
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"
#include "math/RSGISBaysianStatsPrior.h"
#include "math/RSGISBaysianStatsException.h"
#include "math/RSGISBaysianDeltaType.h"
#include "math/RSGISProbDistro.h"
#include "common/RSGISImageException.h"

#include "gdal_priv.h"

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
	class DllExport RSGISImageCalcValueBaysianPrior	: public RSGISCalcImageValue
		{
		public:
			RSGISImageCalcValueBaysianPrior(int numberOutBands, rsgis::math::RSGISMathFunction *function, rsgis::math::RSGISProbDistro *probDistro, double variance, double interval, double minVal, double maxVal, double lowerLimit, double upperLimit, rsgis::math::deltatypedef deltatype);
			void calcImageValue(float *bandValues, int numBands, double *output);
			~RSGISImageCalcValueBaysianPrior();
		protected:
			rsgis::math::RSGISMathFunction *function;
			rsgis::math::RSGISProbDistro *probDistro;
			double variance;
			double interval;
			double minVal;
			double maxVal;
			double upperLimit;
			double lowerLimit;
			double* outputVals;
			rsgis::math::deltatypedef deltatype;
			rsgis::math::RSGISBaysianStatsPrior *baysianStats;
		};	
}}
#endif
