/*
 *  RSGISCalcImageValue.h
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

#ifndef RSGISCalcImageValue_H
#define RSGISCalcImageValue_H

#include <iostream>
#include <string>
#include "img/RSGISImageCalcException.h"

#include "gdal_priv.h"

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

    class DllExport RSGISCalcImageValue
    {
        public:
            RSGISCalcImageValue(int numberOutBands);
            virtual void calcImageValue(float *bandValues, int numBands, double *output) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(float *bandValues, int numBands, double *output, double *outRefVal, unsigned int nOutRefVals) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(float *bandValues, int numBands) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output, double *outRefVal, unsigned int nOutRefVals) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, OGREnvelope extent) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(float *bandValues, int numBands, OGREnvelope extent) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(float *bandValues, int numBands, double *output, OGREnvelope extent) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, double *outRefVal, unsigned int nOutRefVals) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            /**
             * Extent only refers to the central window.
             */
            virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, OGREnvelope extent) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw RSGISImageCalcException("Not Implemented - RSGISCalcImageValue Base Class");};
            virtual int getNumOutBands();
            virtual void setNumOutBands(int bands);
            virtual ~RSGISCalcImageValue(){};
        protected:
            int numOutBands;
    };
    
    
    
    class DllExport RSGISCalcValuesFromMultiResInputs
    {
    public:
        RSGISCalcValuesFromMultiResInputs(int numberOutBands);
        virtual void calcImageValue(float *bandValues, int numInVals, bool useNoData, float noDataVal, double *output)  = 0;
        virtual int getNumOutBands();
        virtual void setNumOutBands(int bands);
        virtual ~RSGISCalcValuesFromMultiResInputs();
    protected:
        int numOutBands;
    };
    
    
    
}}

#endif


