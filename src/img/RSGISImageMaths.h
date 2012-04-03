/*
 *  RSGISImageMaths.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/08/2010.
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

#ifndef RSGISImageMaths_H
#define RSGISImageMaths_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"

#include "muParser.h"

using namespace std;
using namespace mu;

namespace rsgis{namespace img{

	
	class RSGISImageMaths : public RSGISCalcImageValue
	{
	public: 
		RSGISImageMaths(int numberOutBands, Parser *muParser);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		~RSGISImageMaths();
	private:
		Parser *muParser;
		value_type inVal;
	};
	
}}

#endif

