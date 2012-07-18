/*
 *  RSGISCalculateTopOfAtmosphereReflectance.h
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

#ifndef RSGISCalculateTopOfAtmosphereReflectance_H
#define RSGISCalculateTopOfAtmosphereReflectance_H

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "common/RSGISException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

namespace rsgis{namespace calib{
    
    inline double rsgisCalcSolarDistance(int julianDay)throw(rsgis::RSGISException)
    {
        if((julianDay < 1) | (julianDay > 366))
        {
            rsgis::RSGISException("Julian Day must be between 1 and 366.");
        }
        double radiansJulianDay = ((julianDay - 4)*0.9856)*(M_PI/180);
        //cout << "radiansJulianDay = " << radiansJulianDay << endl;
        //cout << "cos(radiansJulianDay) = " << cos(radiansJulianDay) << endl;
        double solarDist = 1 - 0.01672 * cos(radiansJulianDay);
        return solarDist;
    }
    
	class RSGISCalculateTopOfAtmosphereReflectance : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISCalculateTopOfAtmosphereReflectance(int numberOutBands, float *solarIrradiance, double distance, float solarZenith, float scaleFactor = 1);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISCalculateTopOfAtmosphereReflectance();
    protected:
        float *solarIrradiance;
        double distance;
        float solarZenith;
        float scaleFactor;
    };
	
}}

#endif

