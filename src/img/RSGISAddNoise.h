/*
 *  RSGISAddNoise.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/08/2008.
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

#ifndef RSGISAddNoise_H
#define RSGISAddNoise_H

#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"

#include "math/RSGISRandomDistro.h"

#include <geos/geom/Envelope.h>

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

namespace rsgis 
{
	namespace img
	{
		
		enum noiseType 
		{
			randomNoise,
			percentGaussianNoise
		};
		
		class DllExport RSGISAddRandomNoise : public RSGISCalcImageValue
			{
			public: 
				RSGISAddRandomNoise(int numberOutBands, float scale);
				void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
				void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
				void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
				~RSGISAddRandomNoise();
			protected:
				float scale;
			};
		
		class DllExport RSGISAddRandomGaussianNoisePercent : public RSGISCalcImageValue
		{
		public: 
			RSGISAddRandomGaussianNoisePercent(int numberOutBands, float scale);
			void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
			void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
			void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
			bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
			~RSGISAddRandomGaussianNoisePercent();
		protected:
			float scale;
            rsgis::math::RSGISRandDistroGaussian *gRand;
		};
	}
}

#endif

