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

namespace rsgis 
{
	namespace radar
	{
        using namespace std;
        using namespace rsgis::img;
        
		class RSGISConvert2dB : public RSGISCalcImageValue
			/**
			 * Class to convert from intensity to dB using the following formula:<br>
			 * dB = (10 * log10(intensity)) + calFactor <br>
			 * Where 'calFactor' is an optional callibration factor used for ALOS data<br>
			 */
			{
			public: 
				RSGISConvert2dB(int numOutputBands, double calFactor);
				void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands)  throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
				void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException) {throw RSGISImageCalcException("Not implemented");};
				void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output)throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};								
				~RSGISConvert2dB();
			protected:
				double calFactor;
			};
	}
}

#endif

