/*
 *  RSGISCalcEditImage.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/08/2011.
 *  Copyright 2011 RSGISLib.
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

#ifndef RSGISCalcEditImage_H
#define RSGISCalcEditImage_H

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/Coordinate.h"

#include "img/RSGISPixelInPoly.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"

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

namespace rsgis 
{
	namespace img
	{
		class DllExport RSGISCalcEditImage
        {
        public:
            RSGISCalcEditImage(RSGISCalcImageValue *valueCalc);
            void calcImage(GDALDataset *dataset);
            void calcImageUseOut(GDALDataset *dataset);
            void calcImageWindowData(GDALDataset *dataset, int windowSize, float fillval=0);
            ~RSGISCalcEditImage();
        private:
            RSGISCalcImageValue *calc;
        };
	}
}

#endif



