/*
 *  RSGISCalcImageMatrix.h
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

#ifndef RSGISCalcImageMatrix_H
#define RSGISCalcImageMatrix_H

#include <iostream>
#include <fstream>
#include <string>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"

#include "math/RSGISMatrices.h"

namespace rsgis{namespace img{
	
	class DllExport RSGISCalcImageMatrix
		{
		public:
			RSGISCalcImageMatrix(RSGISCalcImageSingle *calcImage);
            rsgis::math::Matrix* calcImageMatrix(GDALDataset **datasetsA, GDALDataset **datasetsB, int numDS) throw(RSGISImageCalcException,RSGISImageBandException);
			rsgis::math::Matrix* calcImageVector(GDALDataset **datasetsA, int numDS) throw(RSGISImageCalcException,RSGISImageBandException);
			~RSGISCalcImageMatrix();
		protected:
			RSGISCalcImageSingle *calcImage;
		};
}}

#endif

