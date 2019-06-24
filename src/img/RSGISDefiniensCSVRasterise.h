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

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
	
	class DllExport RSGISDefiniensCSVRasterise
		{
		public:
			RSGISDefiniensCSVRasterise(bool projFromImage, std::string proj);
			void rasteriseTiles(rsgis::datastruct::SortedGenericList<rsgis::utils::RSGISDefiniensWorkspaceFileName> *sortedTIFs, rsgis::datastruct::SortedGenericList<rsgis::utils::RSGISDefiniensWorkspaceFileName> *sortedCSVs, std::string outputDIR);
			void rasteriseFile(std::string tifFile, std::string csvFile, std::string outputImage);
			void calcNumFeaturesObjects(rsgis::utils::RSGISDefiniensWorkspaceFileName *csvFilepath, int *numFeatures, int *numObjects);
            void calcNumFeaturesObjects(std::string csvFile, int *numFeatures, int *numObjects);
			void readCSVToMemory(rsgis::utils::RSGISDefiniensWorkspaceFileName *csvFilepath, float **csvData, int numFeatures, int numObjects);
            void readCSVToMemory(std::string csvFile, float **csvData, int numFeatures, int numObjects);
			void checkTIFCreatePopulateImageTile(rsgis::utils::RSGISDefiniensWorkspaceFileName *tifFilepath, std::string outputDIR, int numFeatures, int numObjects, float **csvData);
            void checkTIFCreatePopulateImageTile(std::string tifFilepath, std::string outputFile, int numFeatures, int numObjects, float **csvData);
			~RSGISDefiniensCSVRasterise();
		private:
			void convertCSVLine(std::string strLine, float *data, int numFeatures, int objNumber);
			bool imageProj;
            std::string proj;
		};
	
	class DllExport RSGISDefiniensCSVRasteriseValue : public RSGISCalcImageValue
		{
		public: 
			RSGISDefiniensCSVRasteriseValue(int numberOutBands, float **csvData, int numberObjects);
			void calcImageValue(float *bandValues, int numBands, double *output);
			void calcImageValue(float *bandValues, int numBands);
            void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent);
			void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent);
			void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
			bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output);
			~RSGISDefiniensCSVRasteriseValue();
		protected:
			float **csvData;
			int numObjects;
		};
	
}}

#endif


