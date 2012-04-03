/*
 *  RSGISCalcImage.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/04/2008.
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

#ifndef RSGISCalcImage_H
#define RSGISCalcImage_H

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Point.h"
#include "geos/geom/LineString.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/PrecisionModel.h"

#include "img/RSGISPixelInPoly.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"

#include "math/RSGISMathsUtils.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis::math;

namespace rsgis 
{
	namespace img
	{
		class RSGISCalcImage
			{
			public:
				RSGISCalcImage(RSGISCalcImageValue *valueCalc, string proj="", bool useImageProj=true);
				void calcImage(GDALDataset **datasets, int numDS, string outputImage, bool setOutNames = false, string *bandNames = NULL, string gdalFormat="ENVI") throw(RSGISImageCalcException,RSGISImageBandException);
				void calcImage(GDALDataset **datasets, int numDS, GDALDataset *outputImageDS) throw(RSGISImageCalcException,RSGISImageBandException);
				void calcImage(GDALDataset **datasets, int numDS) throw(RSGISImageCalcException,RSGISImageBandException);
                void calcImageBand(GDALDataset **datasets, int numDS, string outputImageBase, string gdalFormat="ENVI") throw(RSGISImageCalcException,RSGISImageBandException);
                void calcImageInEnv(GDALDataset **datasets, int numDS, string outputImage, Envelope *env, bool setOutNames = false, string *bandNames = NULL, string gdalFormat="ENVI") throw(RSGISImageCalcException,RSGISImageBandException);
				void calcImageExtent(GDALDataset **datasets, int numDS) throw(RSGISImageCalcException,RSGISImageBandException);
				void calcImageExtent(GDALDataset **datasets, int numDS, string outputImage, string gdalFormat="ENVI") throw(RSGISImageCalcException,RSGISImageBandException);
				void calcImageWindowData(GDALDataset **datasets, int numDS, string outputImage, int windowSize, string gdalFormat="ENVI") throw(RSGISImageCalcException,RSGISImageBandException);
                void calcImageWindowData(GDALDataset **datasets, int numDS, GDALDataset *outputImageDS, int windowSize) throw(RSGISImageCalcException,RSGISImageBandException);
                void calcImageWindowDataExtent(GDALDataset **datasets, int numDS, string outputImage, int windowSize, string gdalFormat="ENVI") throw(RSGISImageCalcException,RSGISImageBandException);
				void calcImageWindowDataLoop(GDALDataset **datasets, int numDS, string outputImage, int windowSize, string gdalFormat="ENVI") throw(RSGISImageCalcException,RSGISImageBandException);	
				void calcImageWithinPolygon(GDALDataset **datasets, int numDS, string outputImage, Envelope *env, Polygon *poly, float nodata, pixelInPolyOption pixelPolyOption, string gdalFormat="ENVI") throw(RSGISImageCalcException,RSGISImageBandException);
				void calcImageWithinPolygon(GDALDataset **datasets, int numDS, Envelope *env, Polygon *poly, pixelInPolyOption pixelPolyOption) throw(RSGISImageCalcException,RSGISImageBandException);
                void calcImageWithinPolygonExtent(GDALDataset **datasets, int numDS, Envelope *env, Polygon *poly, pixelInPolyOption pixelPolyOption) throw(RSGISImageCalcException,RSGISImageBandException);
				void calcImageWithinRasterPolygon(GDALDataset **datasets, int numDS, Envelope *env, long fid) throw(RSGISImageCalcException,RSGISImageBandException);
				virtual ~RSGISCalcImage();
			private:
				RSGISCalcImageValue *calc;
				int numOutBands;
				string proj;
				bool useImageProj;
			};
	}
}

#endif
