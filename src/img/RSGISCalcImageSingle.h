/*
 *  RSGISCalcImageSingle.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/07/2008.
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

#ifndef RSGISCalcImageSingle_H
#define RSGISCalcImageSingle_H

#include <iostream>
#include <string>

#include "gdal_priv.h"
#include <geos/geom/Envelope.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Coordinate.h>
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/LineString.h"
#include "geos/geom/PrecisionModel.h"
#include "geos/util/IllegalArgumentException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISPixelInPoly.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
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
	
	class DllExport RSGISCalcImageSingle
		{
		public:
			RSGISCalcImageSingle(RSGISCalcImageSingleValue *valueCalc);
			void calcImage(GDALDataset **datasetsA, GDALDataset **datasetsB, int numDS, double *outputValue, int bandA, int bandB) throw(RSGISImageCalcException,RSGISImageBandException);
			void calcImage(GDALDataset **datasetsA, int numDS, double *outputValue, int band) throw(RSGISImageCalcException,RSGISImageBandException);
			void calcImageWindow(GDALDataset **datasetsA, int numDS, double *outputValue) throw(RSGISImageCalcException,RSGISImageBandException);
			void calcImageWithinPolygon(GDALDataset **datasets, int numDS, double *outputValue, geos::geom::Envelope *env, geos::geom::Polygon *poly, bool output, pixelInPolyOption pixelPolyOption) throw(RSGISImageCalcException,RSGISImageBandException);
			void calcImageWithinRasterPolygon(GDALDataset **datasets, int numDS, double *outputValue, geos::geom::Envelope *env, long fid, bool output) throw(RSGISImageCalcException,RSGISImageBandException);
			RSGISCalcImageSingleValue* getRSGISCalcImageSingleValue();
			virtual ~RSGISCalcImageSingle();
		protected:
			RSGISCalcImageSingleValue *valueCalc;
		};
}}
#endif


