/*
 *  RSGISImageKernelFilter.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/12/2008.
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

#ifndef RSGISImageKernelFilter_H
#define RSGISImageKernelFilter_H

#include <iostream>

#include "common/RSGISImageException.h"

#include "filtering/RSGISImageFilterException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "filtering/RSGISImageFilter.h"

using namespace rsgis;
using namespace rsgis::img;

namespace rsgis{namespace filter{
	
	
	class RSGISImageKernelFilter : public RSGISImageFilter
		{
		public: 
			RSGISImageKernelFilter(int numberOutBands, int size, string filenameEnding, ImageFilter *filter);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			virtual void exportAsImage(string filename) throw(RSGISImageFilterException);
			~RSGISImageKernelFilter();
		protected:
			ImageFilter *filter;
		};
}}

#endif


