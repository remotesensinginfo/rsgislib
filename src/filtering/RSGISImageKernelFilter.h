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
#include <string>

#include "common/RSGISImageException.h"

#include "filtering/RSGISImageFilterException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "filtering/RSGISImageFilter.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_filter_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace filter{
	
	
	class DllExport RSGISImageKernelFilter : public RSGISImageFilter
		{
		public: 
			RSGISImageKernelFilter(int numberOutBands, int size, std::string filenameEnding, ImageFilter *filter);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void exportAsImage(std::string filename) throw(RSGISImageFilterException);
			~RSGISImageKernelFilter();
		protected:
			ImageFilter *filter;
		};
}}

#endif


