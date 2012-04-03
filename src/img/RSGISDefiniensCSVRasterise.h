/*
 *  RSGISDefiniensCSVRasterise.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/02/2009.
 *  Copyright 2009 RSGISLib.
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

#ifndef RSGISDefiniensCSVRasterise_H
#define RSGISDefiniensCSVRasterise_H

#include <iostream>

#include "gdal_priv.h"

#include "common/RSGISImageException.h"
#include "common/RSGISFileException.h"

#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISImageStatistics.h"
#include "img/RSGISProjectionStrings.h"

#include "datastruct/SortedGenericList.cpp"

#include "utils/RSGISDefiniensWorkspaceFileName.h"

#include "math/RSGISMathsUtils.h"

using namespace std;
using namespace rsgis;
using namespace rsgis::datastruct;
using namespace rsgis::utils;
using namespace rsgis::math;

namespace rsgis{namespace img{
	
	class RSGISDefiniensCSVRasterise
		{
		public:
			RSGISDefiniensCSVRasterise(bool projFromImage, string proj);
			void rasteriseTiles(SortedGenericList<RSGISDefiniensWorkspaceFileName> *sortedTIFs, SortedGenericList<RSGISDefiniensWorkspaceFileName> *sortedCSVs, string outputDIR) throw(RSGISImageException, RSGISFileException);
			void rasteriseFile(string tifFile, string csvFile, string outputImage) throw(RSGISImageException, RSGISFileException);
			void calcNumFeaturesObjects(RSGISDefiniensWorkspaceFileName *csvFilepath, int *numFeatures, int *numObjects) throw(RSGISFileException);
            void calcNumFeaturesObjects(string csvFile, int *numFeatures, int *numObjects) throw(RSGISFileException);
			void readCSVToMemory(RSGISDefiniensWorkspaceFileName *csvFilepath, float **csvData, int numFeatures, int numObjects) throw(RSGISFileException);
            void readCSVToMemory(string csvFile, float **csvData, int numFeatures, int numObjects) throw(RSGISFileException);
			void checkTIFCreatePopulateImageTile(RSGISDefiniensWorkspaceFileName *tifFilepath, string outputDIR, int numFeatures, int numObjects, float **csvData) throw(RSGISImageException);
            void checkTIFCreatePopulateImageTile(string tifFilepath, string outputFile, int numFeatures, int numObjects, float **csvData) throw(RSGISImageException);
			~RSGISDefiniensCSVRasterise();
		private:
			void convertCSVLine(string strLine, float *data, int numFeatures, int objNumber) throw(RSGISFileException);
			bool imageProj;
			string proj;
		};
	
	class RSGISDefiniensCSVRasteriseValue : public RSGISCalcImageValue
		{
		public: 
			RSGISDefiniensCSVRasteriseValue(int numberOutBands, float **csvData, int numberObjects);
			void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException);
			void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
            void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
			bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			~RSGISDefiniensCSVRasteriseValue();
		protected:
			float **csvData;
			int numObjects;
		};
	
}}

#endif


