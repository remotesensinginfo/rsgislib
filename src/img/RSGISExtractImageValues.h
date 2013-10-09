/*
 *  RSGISExtractImageValues.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 9/10/2013.
 *  Copyright 2013 RSGISLib.
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


#ifndef RSGISExtractImageValues_H
#define RSGISExtractImageValues_H

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "utils/RSGISExportData2HDF.h"

#include "utils/RSGISExportForPlottingIncremental.h"

namespace rsgis{namespace img{
	
    
    class RSGISExtractImageValues
    {
    public:
        RSGISExtractImageValues();
        void extractDataWithinMask2HDF(GDALDataset *mask, GDALDataset *image, std::string outHDFFile, float maskValue) throw(RSGISImageException);
        ~RSGISExtractImageValues();
    };
    
	
	class RSGISExtractImageValuesWithMask : public RSGISCalcImageValue
	{
	public:
		RSGISExtractImageValuesWithMask(std::vector<float*> *pxlVals, float maskValue);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException) {throw RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException) {throw RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException) {throw RSGISImageCalcException("No implemented");};
		~RSGISExtractImageValuesWithMask();
    private:
        std::vector<float*> *pxlVals;
        float maskValue;
	};
	
}}

#endif



