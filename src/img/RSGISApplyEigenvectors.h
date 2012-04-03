/*
 *  RSGISApplyEigenvectors.h
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

#ifndef RSGISApplyEigenvectors_H
#define RSGISApplyEigenvectors_H

#include <iostream>
#include <string>
#include "img/RSGISImageCalcException.h"
#include <geos/geom/Envelope.h>
#include "math/RSGISMatrices.h"
#include "img/RSGISCalcImageValue.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis::math;

namespace rsgis 
{
	namespace img
	{
		class RSGISApplyEigenvectors : public RSGISCalcImageValue
			{
			public: 
				RSGISApplyEigenvectors(int numberOutBands, Matrix *eigenvectors);
				void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException);
				void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
                void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
				~RSGISApplyEigenvectors();
			protected:
				Matrix *eigenvectors;
			};
	}
}

#endif


