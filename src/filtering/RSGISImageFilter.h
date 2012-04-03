/*
 *  RSGISImageFilter.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/12/2008.
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

#ifndef RSGISImageFilter_H
#define RSGISImageFilter_H

#include <iostream>

#include "common/RSGISImageException.h"

#include "filtering/RSGISImageFilterException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

using namespace rsgis;
using namespace rsgis::img;

namespace rsgis{namespace filter{
	
	struct ImageFilter
	{
		int size;
		float **filter;
	};
	
	
	class RSGISImageFilter : public RSGISCalcImageValue
		{
		public: 
			RSGISImageFilter(int numberOutBands, int size, string filenameEnding);
			void runFilter(GDALDataset **datasets, int numDS, string outputImage) throw(RSGISImageException);
			virtual RSGISCalcImage* getCalcImage()throw(RSGISImageException);
			virtual void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException) = 0;
            void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException) = 0;
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException) = 0;
			virtual string getFileNameEnding();
			~RSGISImageFilter();
		protected:
			int size;
			string filenameEnding;
		};
}}

#endif

