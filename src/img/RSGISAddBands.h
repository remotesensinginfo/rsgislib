/*
 *  RSGISAddBands.h
 *  
 *	RSGIS IMG
 *
 *	A class to stack image bands.
 *
 *  Created by Pete Bunting on 04/02/2008.
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

#ifndef RSGISAddBands_H
#define RSGISAddBands_H

#include <iostream>
#include <string>
#include "gdal_priv.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"

#include "math/RSGISMathsUtils.h"

namespace rsgis 
{
	namespace img
	{
		class DllExport RSGISAddBands
		{
			public:
				RSGISAddBands();
				void addMultipleBands(GDALDataset *input, GDALDataset **toAdd, std::string *outputFile, int *band, int numAddBands) throw(RSGISImageBandException);
				void addBandToFile(GDALDataset *input, GDALDataset *toAdd, std::string *outputFile, int band) throw(RSGISImageBandException);
				void stackImages(GDALDataset **datasets, int numDS, std::string outputImage, std::string *imageBandNames, bool skipPixels, float skipValue = 0, float noDataValue = 0, std::string gdalFormat="ENVI", GDALDataType gdalDataType=GDT_Float32, bool replaceBandNames=false) throw(RSGISImageBandException);
				~RSGISAddBands();
		};
        
        
        
        class DllExport RSGISCombineImagesIgnoreNoData : public RSGISCalcImageValue
        {
        public:
            RSGISCombineImagesIgnoreNoData(float noDataVal);
            void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException);
            void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
            void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
            bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
            ~RSGISCombineImagesIgnoreNoData();
        protected:
            float noDataVal;
        };
	}
}

#endif

