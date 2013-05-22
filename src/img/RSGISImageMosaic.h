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

namespace rsgis 
{
	namespace img
	{
		class RSGISImageMosaic
        /**
         overlapBehaviour:
          0 - overwrite mosic with new pixel value
          1 - overwrite mosaic if new pixel value is smaller (min)
          2 - overwrite mosaic if new pixel value is larger (max)
         
         */
			{
			public: 
				RSGISImageMosaic();
				void mosaic(std::string *inputImages, int numDS, std::string outputImage, float background, bool projFromImage, std::string proj, std::string format="ENVI", GDALDataType imgDataType=GDT_Float32) throw(RSGISImageException);
				void mosaicSkipVals(std::string *inputImages, int numDS, std::string outputImage, float background, float skipVal, bool projFromImage, std::string proj, unsigned int skipBand = 0, unsigned int overlapBehaviour = 0, std::string format="ENVI", GDALDataType imgDataType=GDT_Float32) throw(RSGISImageException);
				void mosaicSkipThreash(std::string *inputImages, int numDS, std::string outputImage, float background, float skipLowerThreash, float skipUpperThreash, bool projFromImage, std::string proj, unsigned int threashBand = 0, unsigned int overlapBehaviour = 0, std::string format="ENVI", GDALDataType imgDataType=GDT_Float32) throw(RSGISImageException);
				void includeDatasets(GDALDataset *baseImage, std::string *inputImages, int numDS, std::vector<int> bands, bool bandsDefined) throw(RSGISImageException);
				~RSGISImageMosaic();
			};
	}
}

#endif

