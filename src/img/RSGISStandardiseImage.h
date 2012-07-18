/*
 *  RSGISStandardiseImage.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/08/2008.
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

#ifndef RSGISStandardiseImage_H
#define RSGISStandardiseImage_H

#include <iostream>
#include <string>
#include "img/RSGISImageCalcException.h"
#include <geos/geom/Envelope.h>
#include "math/RSGISMatrices.h"
#include "img/RSGISCalcImageValue.h"

namespace rsgis 
{
	namespace img
	{
		class RSGISStandardiseImage : public RSGISCalcImageValue
			{
			public: 
				RSGISStandardiseImage(int numberOutBands, rsgis::math::Matrix *meanVector);
				void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException);
				void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
                void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
				~RSGISStandardiseImage();
			protected:
                rsgis::math::Matrix *meanVector;
			};
	}
}

#endif


