/*
 *  RSGISImageNormalisation.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/05/2008.
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

#ifndef RSGISImageNormalisation_H
#define RSGISImageNormalisation_H

#include <iostream>
#include <string>
#include <cmath>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISImageStatistics.h"

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

namespace rsgis{namespace img{
	
	class DllExport RSGISImageNormalisation
		{
		public: 
			RSGISImageNormalisation();
			void normaliseImage(GDALDataset *dataset, double *imageMax, double *imageMin, double *outMax, double *outMin, bool calcStats, std::string outputImage);
		};
	
	class DllExport RSGISNormaliseImage : public RSGISCalcImageValue
		{
		public: 
			RSGISNormaliseImage(int numberOutBands, double *imageMaxIn, double *imageMinIn, double *outMaxIn, double *outMinIn);
			void calcImageValue(float *bandValues, int numBands, double *output);
			~RSGISNormaliseImage();
		protected:
			double *imageMax;
			double *imageMin;
			double *outMax;
			double *outMin;
		};

}}

#endif
