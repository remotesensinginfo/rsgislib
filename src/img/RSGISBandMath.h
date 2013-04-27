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
#include "img/RSGISCalcImage.h"

#include "muParser.h"

namespace rsgis{namespace img{
	
	struct VariableBands
	{
        std::string name;
		int band;
	};
    
    struct VariableStruct
    {
        std::string image;
        std::string name;
        int bandNum;
    };
	
	class RSGISBandMath : public RSGISCalcImageValue
		{
		public: 
			RSGISBandMath(int numberOutBands, VariableBands **variables, int numVariables, mu::Parser *muParser);
			void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException);
			void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
            void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
			bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			~RSGISBandMath();
            static void executeBandMaths(VariableStruct *variables, unsigned int numVars, std::string outputImage, std::string mathsExpression, std::string gdalFormat, GDALDataType outDataType)throw(rsgis::RSGISImageException);
		private:
			VariableBands **variables;
			int numVariables;
            mu::Parser *muParser;
            mu::value_type *inVals;
		};
	
}}

#endif

