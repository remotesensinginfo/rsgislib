/*
 *  RSGISCalcImgValProb.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/06/2016.
 *  Copyright 2016 RSGISLib.
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

#ifndef RSGISCalcImgValProb_H
#define RSGISCalcImgValProb_H

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISImageStatistics.h"

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
    
    class DllExport RSGISCalcImgValProb
    {
    public:
        RSGISCalcImgValProb(){};
        void calcMaskImgPxlValProb(GDALDataset *inImgDS, std::vector<unsigned int> inImgBandIdxs, GDALDataset *inMaskDS, int maskVal, std::string outputImage, std::string gdalFormat, std::vector<float> histBinWidths, bool calcHistBinWidth, bool useImgNoData, bool rescaleProbs);
        ~RSGISCalcImgValProb(){};
    };
  
    class DllExport RSGISCalcImagePopNDHist : public RSGISCalcImageValue
    {
    public:
        RSGISCalcImagePopNDHist(std::vector<unsigned int> inImgBandIdxs, long maskVal, double *noDataVals, bool useNoData, double *bandMin, double *bandMax, std::vector<float> histBinWidths, unsigned long *numBins, double *hist, unsigned long totalNumBins);
        void calcImageValue(float *bandValues, int numBands, double *output);
        void calcImageValue(float *bandValues, int numBands){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcImagePopNDHist();
    protected:
        std::vector<unsigned int> inImgBandIdxs;
        long maskVal;
        double *noDataVals;
        bool useNoData;
        double *bandMin;
        double *bandMax;
        std::vector<float> histBinWidths;
        unsigned long *numBins;
        double *hist;
        unsigned long totalNumBins;
    };
    
}}

#endif
