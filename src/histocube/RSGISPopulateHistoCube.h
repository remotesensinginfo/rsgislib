/*
 *  RSGISPopulateHistoCube.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/02/2017.
 *
 *  Copyright 2017 RSGISLib.
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

#ifndef RSGISPopulateHistoCube_H
#define RSGISPopulateHistoCube_H


// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
#ifdef rsgis_histocube_EXPORTS
#define DllExport   __declspec( dllexport )
#else
#define DllExport   __declspec( dllimport )
#endif
#else
#define DllExport
#endif


#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "common/RSGISHistoCubeException.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

#include "RSGISHistoCubeFileIO.h"
#include "RSGISHistoCubeUtils.h"

namespace rsgis {namespace histocube{
    
    
    class DllExport RSGISPopHistoCubeLayerFromImgBand : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISPopHistoCubeLayerFromImgBand(RSGISHistoCubeFile *hcFile, std::string layerName, unsigned int bandIdx, unsigned int maxRow, float scale, float offset, std::vector<int> bins);
        void calcImageValue(float *bandValues, int numBands, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        ~RSGISPopHistoCubeLayerFromImgBand();
    protected:
        RSGISHistoCubeFile *hcFile;
        std::string layerName;
        unsigned int bandIdx;
        unsigned int maxRow;
        float scale;
        float offset;
        std::vector<int> bins;
        RSGISHistoCubeUtils hcUtils;
        unsigned int *dataArr;
        unsigned int dataArrLen;
    };
    
    class DllExport RSGISPopHistoCubeLayerFromImgBandInMem : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISPopHistoCubeLayerFromImgBandInMem(unsigned int *dataArr, unsigned long dataArrLen, unsigned int bandIdx, unsigned int maxRow, float scale, float offset, std::vector<int> bins);
        void calcImageValue(float *bandValues, int numBands, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        ~RSGISPopHistoCubeLayerFromImgBandInMem();
    protected:
        std::string layerName;
        unsigned int bandIdx;
        unsigned int maxRow;
        float scale;
        float offset;
        std::vector<int> bins;
        RSGISHistoCubeUtils hcUtils;
        unsigned int *dataArr;
        unsigned long dataArrLen;
        unsigned int rowLen;
    };
    
}}

#endif

