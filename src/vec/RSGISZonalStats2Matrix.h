/*
 *  RSGISZonalStats2Matrix.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/11/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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


#ifndef RSGISZonalStats2Matrix_H
#define RSGISZonalStats2Matrix_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorZonalException.h"
#include "vec/RSGISPolygonData.h"
#include "vec/RSGISVectorIO.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISMathsUtils.h"
#include "vec/RSGISClassPolygon.h"
#include "img/RSGISPolygonPixelCount.h"
#include "img/RSGISCalcImageSingle.h"
#include "img/RSGISPopulateMatrix.h"
#include "img/RSGISImageBand2Matrix.h"
#include "vec/RSGISEmptyPolygon.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISPixelInPoly.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Coordinate.h"

using namespace std;
using namespace rsgis::img;
using namespace geos::geom;
using namespace rsgis::math;

namespace rsgis{namespace vec{
		
	struct ClassVariables
	{
		string name;
		Matrix *matrix;
		int numPxls;
	};
	
	class RSGISZonalStats2Matrix
	{
	public:
		RSGISZonalStats2Matrix();
		ClassVariables** findPixelStats(GDALDataset **image, int numImgs, OGRLayer *shpfile, string classAttribute, int *numMatrices, pixelInPolyOption method);
		Matrix** findPixelsForImageBand(GDALDataset **image, int numImgs, OGRLayer *shpfile, int *numMatrices, int band);
		~RSGISZonalStats2Matrix();
	};
}}
#endif


