/*
 *  RSGISRadarUtils.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 22/07/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#ifndef RSGISRadarUtils_H
#define RSGISRadarUtils_H

#include <iostream>
#include <math.h>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_radar_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis 
{
	namespace radar
	{
        
		class DllExport RSGISConvert2dB : public rsgis::img::RSGISCalcImageValue
			/**
			 * Class to convert from intensity to dB using the following formula:<br>
			 * dB = (10 * log10(intensity)) + calFactor <br>
			 * Where 'calFactor' is an optional callibration factor used for ALOS data<br>
			 */
			{
			public: 
				RSGISConvert2dB(int numOutputBands, double calFactor);
				void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands)  throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};								
				~RSGISConvert2dB();
			protected:
				double calFactor;
			};
	}
}

#endif

