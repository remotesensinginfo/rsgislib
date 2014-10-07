/*
 *  RSGISImageFilter.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/12/2008.
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

#ifndef RSGISImageFilter_H
#define RSGISImageFilter_H

#include <iostream>
#include <string>

#include "common/RSGISImageException.h"

#include "filtering/RSGISImageFilterException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

namespace rsgis{namespace filter{
	
	struct DllExport ImageFilter
	{
		int size;
		float **filter;
	};
	
	
	class DllExport RSGISImageFilter : public rsgis::img::RSGISCalcImageValue
		{
		public: 
			RSGISImageFilter(int numberOutBands, int size, std::string filenameEnding);
			void runFilter(GDALDataset **datasets, int numDS, std::string outputImage, std::string gdalFormat, GDALDataType outDataType) throw(rsgis::RSGISImageException);
			virtual rsgis::img::RSGISCalcImage* getCalcImage()throw(rsgis::RSGISImageException);
			virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
            virtual void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            virtual void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException) = 0;
            virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException) = 0;
			virtual void exportAsImage(std::string filename) throw(RSGISImageFilterException) = 0;
			virtual std::string getFileNameEnding();
			~RSGISImageFilter();
		protected:
			int size;
			std::string filenameEnding;
		};
}}

#endif

