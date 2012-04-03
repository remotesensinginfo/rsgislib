/*
 *  RSGISBandMath.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/11/2008.
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

#ifndef RSGISBandMath_H
#define RSGISBandMath_H

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
	
	struct VariableBands
	{
		string name;
		int band;
	};
	
	
	class RSGISBandMath : public RSGISCalcImageValue
		{
		public: 
			RSGISBandMath(int numberOutBands, VariableBands **variables, int numVariables, Parser *muParser);
			void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException);
			void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
            void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
			bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			~RSGISBandMath();
		private:
			VariableBands **variables;
			int numVariables;
			Parser *muParser;
			value_type *inVals;
		};
	
}}

#endif

