/*
 *  RSGISImageMosaic.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/11/2008.
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
 *
 *  Modified by Dan Clewley on 21/11/2010
 *  Added 'mosaicSkipVals' and 'mosaicSkipThreash'
 *  to skip values in input image
 * 
 */

#ifndef RSGISImageMosaic_H
#define RSGISImageMosaic_H

#include <iostream>
#include <string>
#include "img/RSGISImageUtils.h"

using namespace std;

namespace rsgis 
{
	namespace img
	{
		class RSGISImageMosaic
			{
			public: 
				RSGISImageMosaic();
				void mosaic(string *inputImages, int numDS, string outputImage, float background, bool projFromImage, string proj) throw(RSGISImageException);
				void mosaicSkipVals(string *inputImages, int numDS, string outputImage, float background, float skipVal, bool projFromImage, string proj, unsigned int skipBand = 0) throw(RSGISImageException);
				void mosaicSkipThreash(string *inputImages, int numDS, string outputImage, float background, float skipLowerThreash, float skipUpperThreash, bool projFromImage, string proj, unsigned int threashBand = 0) throw(RSGISImageException);
				void includeDatasets(GDALDataset *baseImage, string *inputImages, int numDS, vector<int> bands, bool bandsDefined) throw(RSGISImageException);
				~RSGISImageMosaic();
			};
	}
}

#endif

