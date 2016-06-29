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
 *  Added 'mosaicSkipVals' and 'mosaicSkipThresh'
 *  to skip values in input image
 * 
 */

#ifndef RSGISImageMosaic_H
#define RSGISImageMosaic_H

#include <iostream>
#include <string>

#include "libkea/KEAImageIO.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImage.h"

namespace rsgis{namespace img{
    
    struct DllExport RSGISImageValidDataMetric
    {
        std::string imageFile;
        unsigned int validPxlCount;
        unsigned int noDataPxlCount;
        unsigned int totalNumPxls;
        double validPxlFunc;
    };
    
    inline bool compare_ImageValidPxlCounts (const RSGISImageValidDataMetric& first, const RSGISImageValidDataMetric& second)
    {
        return ( first.validPxlFunc < second.validPxlFunc );
    }
    
    class DllExport RSGISImageMosaic
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
        void mosaicSkipThresh(std::string *inputImages, int numDS, std::string outputImage, float background, float skipLowerThresh, float skipUpperThresh, bool projFromImage, std::string proj, unsigned int threshBand = 0, unsigned int overlapBehaviour = 0, std::string format="ENVI", GDALDataType imgDataType=GDT_Float32) throw(RSGISImageException);
        void includeDatasets(GDALDataset *baseImage, std::string *inputImages, int numDS, std::vector<int> bands, bool bandsDefined) throw(RSGISImageException);
        void includeDatasetsSkipVals(GDALDataset *baseImage, std::string *inputImages, int numDS, std::vector<int> bands, bool bandsDefined, float skipVal) throw(RSGISImageException);
        void includeDatasetsIgnoreOverlap(GDALDataset *baseImage, std::string *inputImages, int numDS, int numOverlapPxls) throw(RSGISImageException);
        void orderInImagesValidData(std::vector<std::string> images, std::vector<std::string> *orderedImages, float noDataValue) throw(RSGISImageException);
        ~RSGISImageMosaic();
    };
    
    class DllExport RSGISCountValidPixels : public RSGISCalcImageValue
    {
    public:
        RSGISCountValidPixels(RSGISImageValidDataMetric *validPxlsObj, float noDataVal);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void resetValidPxlsObj();
        ~RSGISCountValidPixels();
    protected:
        RSGISImageValidDataMetric *validPxlsObj;
        float noDataVal;
    };
    
    class DllExport RSGISCombineImgTileOverview
    {
    public:
        RSGISCombineImgTileOverview();
        void combineKEAImgTileOverviews(GDALDataset *baseImg, std::vector<std::string> inputImages, std::vector<int> pyraScaleVals) throw(RSGISImageException);
        ~RSGISCombineImgTileOverview();
    };
}}

#endif

