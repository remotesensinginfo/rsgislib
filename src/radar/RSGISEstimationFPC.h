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

namespace rsgis { namespace radar{
    
    using namespace rsgis::img;
    using namespace rsgis::math;
    
    class RSGISEstimationFPCCanopyScattering : public RSGISCalcImageValue
    {
        /// Calculate canopy scattering from FPC
        /** Takes FPC image and returns canopy scattering in HH, HV and VV.
         */
    public: 
        RSGISEstimationFPCCanopyScattering(int numberOutBands, RSGISMathFunction *calcScatteringHH, RSGISMathFunction *calcScatteringHV, RSGISMathFunction *calcScatteringVV);
        virtual void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        virtual void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};											
        void calcValue(double fpc, double *canopyScattering);
        ~RSGISEstimationFPCCanopyScattering();
    private:
        RSGISMathFunction *calcScatteringHH;
        RSGISMathFunction *calcScatteringHV;
        RSGISMathFunction *calcScatteringVV;
    };
    
    class RSGISEstimationFPCCanopyAttenuation : public RSGISCalcImageValue
    {
    public:
        /// Calculate canopy attenuation from FPC
        /** Takes FPC image and returns canopy attenuation at H and V polarisation.
         */
        RSGISEstimationFPCCanopyAttenuation(int numberOutBands, RSGISMathFunction *calcAttenuationH, RSGISMathFunction *calcAttenuationV);
        virtual void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        virtual void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};											
        void calcValue(double fpc, double *canopyAttenuation);
        ~RSGISEstimationFPCCanopyAttenuation();
    private:
        RSGISMathFunction *calcAttenuationH;
        RSGISMathFunction *calcAttenuationV;
    };
    
    class RSGISEstimationFPCCanopyScatteringAttenuation : public RSGISCalcImageValue
    {
        /// Calculate canopy scattering and attenuation from FPC
        /** Takes FPC image and returns 5 band image with canopy scattering in HH, HV and VV and
         *  canopy attenuation at H and V polarisation.
         */
    public: 
        RSGISEstimationFPCCanopyScatteringAttenuation(int numberOutBands, RSGISMathFunction *calcScatteringHH, RSGISMathFunction *calcScatteringHV, RSGISMathFunction *calcScatteringVV,RSGISMathFunction *calcAttenuationH, RSGISMathFunction *calcAttenuationV);
        virtual void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        virtual void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISEstimationFPCCanopyScatteringAttenuation();
    private:
        RSGISMathFunction *calcScatteringHH;
        RSGISMathFunction *calcScatteringHV;
        RSGISMathFunction *calcScatteringVV;
        RSGISMathFunction *calcAttenuationH;
        RSGISMathFunction *calcAttenuationV;
    };
    
    class RSGISEstimationFPCDualPolTrunkGround : public RSGISCalcImageValue
    {
        /// Calculates calcuates trunk-ground + ground return from total backscatter using FPC
        /** Takes 2 band image FPC, HH, HV
         * Functions are provided to calculate canopy scattering and attenuation from FPC.
         * Returns 2 band image trunk-ground + ground scattering in HH and HV.
         */
    public: 
        RSGISEstimationFPCDualPolTrunkGround(int numberOutBands, RSGISMathFunction *calcScatteringHH, RSGISMathFunction *calcScatteringHV, RSGISMathFunction *calcAttenuationH, RSGISMathFunction *calcAttenuationV);
        virtual void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        virtual void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};											
        void calcValue(double fpc, double totalHH, double totalHV, double *trunkGround);
        ~RSGISEstimationFPCDualPolTrunkGround();
    private:
        RSGISMathFunction *calcScatteringHH;
        RSGISMathFunction *calcScatteringHV;
        RSGISMathFunction *calcAttenuationH;
        RSGISMathFunction *calcAttenuationV;
    };
    
    class RSGISEstimationFPCFullPolTrunkGroundDualPol : public RSGISCalcImageValue
    {
        /// Calculates calcuates trunk-ground + ground return from total backscatter using FPC
        /** Takes 4 band image FPC, HH, HV, VV
         * Functions are provided to calculate canopy scattering and attenuation from FPC.
         * Returns 3 band image trunk-ground + ground scattering in HH, HV and VV
         */
    public: 
        RSGISEstimationFPCFullPolTrunkGroundDualPol(int numberOutBands, RSGISMathFunction *calcScatteringHH, RSGISMathFunction *calcScatteringHV, RSGISMathFunction *calcScatteringVV, RSGISMathFunction *calcAttenuationH, RSGISMathFunction *calcAttenuationV);
        virtual void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        virtual void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};											
        void calcValue(double fpc, double totalHH, double totalHV, double totalVV, double *trunkGround);
        ~RSGISEstimationFPCFullPolTrunkGroundDualPol();
    private:
        RSGISMathFunction *calcScatteringHH;
        RSGISMathFunction *calcScatteringHV;
        RSGISMathFunction *calcScatteringVV;
        RSGISMathFunction *calcAttenuationH;
        RSGISMathFunction *calcAttenuationV;
    };
}}

#endif
