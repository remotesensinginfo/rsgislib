 /*
 *  RSGISEstimationFPC.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 05/01/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#ifndef RSGISEstimationFPC
#define RSGISEstimationFPC

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

#include "math/RSGISMathFunction.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_radar_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis { namespace radar{
    
    class DllExport RSGISEstimationFPCCanopyScattering : public rsgis::img::RSGISCalcImageValue
    {
        /// Calculate canopy scattering from FPC
        /** Takes FPC image and returns canopy scattering in HH, HV and VV.
         */
    public: 
        RSGISEstimationFPCCanopyScattering(int numberOutBands, rsgis::math::RSGISMathFunction *calcScatteringHH, rsgis::math::RSGISMathFunction *calcScatteringHV, rsgis::math::RSGISMathFunction *calcScatteringVV);
        virtual void calcImageValue(float *bandValues, int numBands, double *output);
        virtual void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};											
        void calcValue(double fpc, double *canopyScattering);
        ~RSGISEstimationFPCCanopyScattering();
    private:
        rsgis::math::RSGISMathFunction *calcScatteringHH;
        rsgis::math::RSGISMathFunction *calcScatteringHV;
        rsgis::math::RSGISMathFunction *calcScatteringVV;
    };
    
    class DllExport RSGISEstimationFPCCanopyAttenuation : public rsgis::img::RSGISCalcImageValue
    {
    public:
        /// Calculate canopy attenuation from FPC
        /** Takes FPC image and returns canopy attenuation at H and V polarisation.
         */
        RSGISEstimationFPCCanopyAttenuation(int numberOutBands, rsgis::math::RSGISMathFunction *calcAttenuationH, rsgis::math::RSGISMathFunction *calcAttenuationV);
        virtual void calcImageValue(float *bandValues, int numBands, double *output);
        virtual void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};											
        void calcValue(double fpc, double *canopyAttenuation);
        ~RSGISEstimationFPCCanopyAttenuation();
    private:
        rsgis::math::RSGISMathFunction *calcAttenuationH;
        rsgis::math::RSGISMathFunction *calcAttenuationV;
    };
    
    class DllExport RSGISEstimationFPCCanopyScatteringAttenuation : public rsgis::img::RSGISCalcImageValue
    {
        /// Calculate canopy scattering and attenuation from FPC
        /** Takes FPC image and returns 5 band image with canopy scattering in HH, HV and VV and
         *  canopy attenuation at H and V polarisation.
         */
    public: 
        RSGISEstimationFPCCanopyScatteringAttenuation(int numberOutBands, rsgis::math::RSGISMathFunction *calcScatteringHH, rsgis::math::RSGISMathFunction *calcScatteringHV, rsgis::math::RSGISMathFunction *calcScatteringVV,rsgis::math::RSGISMathFunction *calcAttenuationH, rsgis::math::RSGISMathFunction *calcAttenuationV);
        virtual void calcImageValue(float *bandValues, int numBands, double *output);
        virtual void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISEstimationFPCCanopyScatteringAttenuation();
    private:
        rsgis::math::RSGISMathFunction *calcScatteringHH;
        rsgis::math::RSGISMathFunction *calcScatteringHV;
        rsgis::math::RSGISMathFunction *calcScatteringVV;
        rsgis::math::RSGISMathFunction *calcAttenuationH;
        rsgis::math::RSGISMathFunction *calcAttenuationV;
    };
    
    class DllExport RSGISEstimationFPCDualPolTrunkGround : public rsgis::img::RSGISCalcImageValue
    {
        /// Calculates calcuates trunk-ground + ground return from total backscatter using FPC
        /** Takes 2 band image FPC, HH, HV
         * Functions are provided to calculate canopy scattering and attenuation from FPC.
         * Returns 2 band image trunk-ground + ground scattering in HH and HV.
         */
    public: 
        RSGISEstimationFPCDualPolTrunkGround(int numberOutBands, rsgis::math::RSGISMathFunction *calcScatteringHH, rsgis::math::RSGISMathFunction *calcScatteringHV, rsgis::math::RSGISMathFunction *calcAttenuationH, rsgis::math::RSGISMathFunction *calcAttenuationV);
        virtual void calcImageValue(float *bandValues, int numBands, double *output);
        virtual void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};											
        void calcValue(double fpc, double totalHH, double totalHV, double *trunkGround);
        ~RSGISEstimationFPCDualPolTrunkGround();
    private:
        rsgis::math::RSGISMathFunction *calcScatteringHH;
        rsgis::math::RSGISMathFunction *calcScatteringHV;
        rsgis::math::RSGISMathFunction *calcAttenuationH;
        rsgis::math::RSGISMathFunction *calcAttenuationV;
    };
    
    class DllExport RSGISEstimationFPCFullPolTrunkGroundDualPol : public rsgis::img::RSGISCalcImageValue
    {
        /// Calculates calcuates trunk-ground + ground return from total backscatter using FPC
        /** Takes 4 band image FPC, HH, HV, VV
         * Functions are provided to calculate canopy scattering and attenuation from FPC.
         * Returns 3 band image trunk-ground + ground scattering in HH, HV and VV
         */
    public: 
        RSGISEstimationFPCFullPolTrunkGroundDualPol(int numberOutBands, rsgis::math::RSGISMathFunction *calcScatteringHH, rsgis::math::RSGISMathFunction *calcScatteringHV, rsgis::math::RSGISMathFunction *calcScatteringVV, rsgis::math::RSGISMathFunction *calcAttenuationH, rsgis::math::RSGISMathFunction *calcAttenuationV);
        virtual void calcImageValue(float *bandValues, int numBands, double *output);
        virtual void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};											
        void calcValue(double fpc, double totalHH, double totalHV, double totalVV, double *trunkGround);
        ~RSGISEstimationFPCFullPolTrunkGroundDualPol();
    private:
        rsgis::math::RSGISMathFunction *calcScatteringHH;
        rsgis::math::RSGISMathFunction *calcScatteringHV;
        rsgis::math::RSGISMathFunction *calcScatteringVV;
        rsgis::math::RSGISMathFunction *calcAttenuationH;
        rsgis::math::RSGISMathFunction *calcAttenuationV;
    };
}}

#endif
