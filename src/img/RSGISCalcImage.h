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

namespace rsgis 
{
	namespace img
	{
		class DllExport RSGISCalcImage
			{
			public:
				RSGISCalcImage(RSGISCalcImageValue *valueCalc, std::string proj="", bool useImageProj=true);
				void calcImage(GDALDataset **datasets, int numDS, std::string outputImage, bool setOutNames = false, std::string *bandNames = NULL, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32);
                void calcImage(GDALDataset **datasets, int numDS, std::string outputImage, std::string outputRefIntImage, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32);
				void calcImage(GDALDataset **datasets, int numDS, GDALDataset *outputImageDS);
                void calcImagePartialOutput(GDALDataset **datasets, int numDS, GDALDataset *outputImageDS);
				void calcImage(GDALDataset **datasets, int numDS);
                void calcImage(GDALDataset **datasets, int numIntDS, int numFloatDS, std::string outputImage, bool setOutNames = false, std::string *bandNames = NULL, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32);
                void calcImage(GDALDataset **datasets, int numIntDS, int numFloatDS, std::string outputImage, std::string outputRefIntImage, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32);
                void calcImage(GDALDataset **datasets, int numIntDS, int numFloatDS, geos::geom::Envelope *env=NULL, bool quiet=false);
                void calcImage(GDALDataset **datasets, int numIntDS, int numFloatDS, GDALDataset *outputImageDS);
                void calcImageBand(GDALDataset **datasets, int numDS, std::string outputImageBase, std::string gdalFormat="KEA");
                void calcImageInEnv(GDALDataset **datasets, int numDS, std::string outputImage, geos::geom::Envelope *env, bool setOutNames = false, std::string *bandNames = NULL, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32);
                void calcImageInEnv(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, bool quiet=false);
                void calcImageInEnv(GDALDataset **datasets, int numIntDS, int numFloatDS, geos::geom::Envelope *env, bool quiet=false);
                void calcImagePosPxl(GDALDataset **datasets, int numDS);
                void calcImagePosPxl(GDALDataset **datasets, int numIntDS, int numFloatDS);
                void calcImageExtent(GDALDataset **datasets, int numDS, geos::geom::Envelope *env=NULL, bool quiet=false);
                void calcImageExtent(GDALDataset **datasets, int numIntDS, int numFloatDS);
                void calcImageExtent(GDALDataset **datasets, int numDS, std::string outputImage, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32);
				void calcImageWindowData(GDALDataset **datasets, int numDS, int windowSize);
                void calcImageWindowData(GDALDataset **datasets, int numDS, std::string outputImage, int windowSize, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32);
                void calcImageWindowData(GDALDataset **datasets, int numDS, std::string outputImage, std::string outputRefIntImage, int windowSize, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32);                
                void calcImageWindowData(GDALDataset **datasets, int numDS, GDALDataset *outputImageDS, int windowSize, bool passPxlXY=false);
                void calcImageWindowDataExtent(GDALDataset **datasets, int numDS, std::string outputImage, int windowSize, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32);
				void calcImageWithinPolygon(GDALDataset **datasets, int numDS, std::string outputImage, geos::geom::Envelope *env, geos::geom::Polygon *poly, float nodata, pixelInPolyOption pixelPolyOption, std::string gdalFormat="KEA",  GDALDataType gdalDataType=GDT_Float32);
				void calcImageWithinPolygon(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, geos::geom::Polygon *poly, pixelInPolyOption pixelPolyOption);
                void calcImageWithinPolygonExtent(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, geos::geom::Polygon *poly, pixelInPolyOption pixelPolyOption);
                void calcImageWithinPolygonExtentInMem(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, geos::geom::Polygon *poly, pixelInPolyOption pixelPolyOption);
				void calcImageWithinRasterPolygon(GDALDataset **datasets, int numDS, geos::geom::Envelope *env, long fid);
                void calcImageBorderPixels(GDALDataset *dataset, bool returnInt);
                virtual ~RSGISCalcImage();
			private:
				RSGISCalcImageValue *calc;
				int numOutBands;
				std::string proj;
				bool useImageProj;
			};
        
        
        class DllExport RSGISCalcImageMultiImgRes
        {
        public:
            RSGISCalcImageMultiImgRes(RSGISCalcValuesFromMultiResInputs *valueCalcSum);
            void calcImageHighResForLowRegions(GDALDataset *refDataset, GDALDataset *statsDataset, unsigned int statsImgBand, std::string outputImage, std::string gdalFormat="KEA", GDALDataType gdalDataType=GDT_Float32, bool useNoDataVal=true, unsigned int xIOGrid=16, unsigned int yIOGrid=16, bool setOutNames = false, std::string *bandNames = NULL);
            virtual ~RSGISCalcImageMultiImgRes();
        protected:
            RSGISCalcValuesFromMultiResInputs *valueCalcSum;
        };
        
        
	}
}

#endif
