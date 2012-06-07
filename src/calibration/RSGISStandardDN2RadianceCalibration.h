/*
 *  RSGISStandardDN2RadianceCalibration.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/05/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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


#ifndef RSGISStandardDN2RadianceCalibration_H
#define RSGISStandardDN2RadianceCalibration_H

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

namespace rsgis{namespace calib{
	
    using namespace std;
    using namespace rsgis::img;
    
	struct LandsatRadianceGainsOffsets
    {
        string bandName;
        unsigned int band;
        float lMax;
        float lMin;
        float qCalMax;
        float qCalMin;
    };
    
    struct SPOTRadianceGainsOffsets
    {
        string bandName;
        unsigned int band;
        float gain;
    };
    
    struct IkonosRadianceGainsOffsets
    {
        string bandName;
        unsigned int band;
        float calCoef;
        float bandwidth;
    };
    
    struct ASTERRadianceGainsOffsets
    {
        string bandName;
        unsigned int band;
        float unitConCoef;
    };
    
    struct IRSRadianceGainsOffsets
    {
        string bandName;
        unsigned int band;
        float lMax;
        float lMin;
        float qCalMax;
        float qCalMin;
    };
    
    struct Quickbird16bitRadianceGainsOffsets
    {
        string bandName;
        unsigned int band;
        float calFactor;
        float bandIntegrate;
    };
    
    struct Quickbird8bitRadianceGainsOffsets
    {
        string bandName;
        unsigned int band;
        float calFactor;
        float bandIntegrate;
        float k;
    };
    
    struct WorldView2RadianceGainsOffsets
    {
        string bandName;
        unsigned int band;
        float calFactor;
        float bandIntegrate;
    };
	
	class RSGISLandsatRadianceCalibration : public RSGISCalcImageValue
    {
    public: 
        RSGISLandsatRadianceCalibration(unsigned int numberOutBands, LandsatRadianceGainsOffsets *radGainOff):RSGISCalcImageValue(numberOutBands)
        {
            this->radGainOff = radGainOff;
        };
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        ~RSGISLandsatRadianceCalibration(){};
    protected:
        LandsatRadianceGainsOffsets *radGainOff;
    };
    
    class RSGISSPOTRadianceCalibration : public RSGISCalcImageValue
    {
    public: 
        RSGISSPOTRadianceCalibration(unsigned int numberOutBands, SPOTRadianceGainsOffsets *radGainOff):RSGISCalcImageValue(numberOutBands)
        {
            this->radGainOff = radGainOff;
        };
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        ~RSGISSPOTRadianceCalibration(){};
    protected:
        SPOTRadianceGainsOffsets *radGainOff;
    };
    
    class RSGISIkonosRadianceCalibration : public RSGISCalcImageValue
    {
    public: 
        RSGISIkonosRadianceCalibration(unsigned int numberOutBands, IkonosRadianceGainsOffsets *radGainOff):RSGISCalcImageValue(numberOutBands)
        {
            this->radGainOff = radGainOff;
        };
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        ~RSGISIkonosRadianceCalibration(){};
    protected:
        IkonosRadianceGainsOffsets *radGainOff;
    };
    
    class RSGISASTERRadianceCalibration : public RSGISCalcImageValue
    {
    public: 
        RSGISASTERRadianceCalibration(unsigned int numberOutBands, ASTERRadianceGainsOffsets *radGainOff):RSGISCalcImageValue(numberOutBands)
        {
            this->radGainOff = radGainOff;
        };
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        ~RSGISASTERRadianceCalibration(){};
    protected:
        ASTERRadianceGainsOffsets *radGainOff;
    };
    
    class RSGISIRSRadianceCalibration : public RSGISCalcImageValue
    {
    public: 
        RSGISIRSRadianceCalibration(unsigned int numberOutBands, IRSRadianceGainsOffsets *radGainOff):RSGISCalcImageValue(numberOutBands)
        {
            this->radGainOff = radGainOff;
        };
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        ~RSGISIRSRadianceCalibration(){};
    protected:
        IRSRadianceGainsOffsets *radGainOff;
    };

	class RSGISQuickbird16bitRadianceCalibration : public RSGISCalcImageValue
    {
    public: 
        RSGISQuickbird16bitRadianceCalibration(unsigned int numberOutBands, Quickbird16bitRadianceGainsOffsets *radGainOff):RSGISCalcImageValue(numberOutBands)
        {
            this->radGainOff = radGainOff;
        };
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        ~RSGISQuickbird16bitRadianceCalibration(){};
    protected:
        Quickbird16bitRadianceGainsOffsets *radGainOff;
    };
    
    class RSGISQuickbird8bitRadianceCalibration : public RSGISCalcImageValue
    {
    public: 
        RSGISQuickbird8bitRadianceCalibration(unsigned int numberOutBands, Quickbird8bitRadianceGainsOffsets *radGainOff):RSGISCalcImageValue(numberOutBands)
        {
            this->radGainOff = radGainOff;
        };
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        ~RSGISQuickbird8bitRadianceCalibration(){};
    protected:
        Quickbird8bitRadianceGainsOffsets *radGainOff;
    };

    class RSGISWorldView2RadianceCalibration : public RSGISCalcImageValue
    {
    public: 
        RSGISWorldView2RadianceCalibration(unsigned int numberOutBands, WorldView2RadianceGainsOffsets *radGainOff):RSGISCalcImageValue(numberOutBands)
        {
            this->radGainOff = radGainOff;
        };
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implmented.");};
        ~RSGISWorldView2RadianceCalibration(){};
    protected:
        WorldView2RadianceGainsOffsets *radGainOff;
    };

    
    
}}

#endif

