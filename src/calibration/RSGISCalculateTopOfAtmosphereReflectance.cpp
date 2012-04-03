/*
 *  RSGISCalculateTopOfAtmosphereReflectance.cpp
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

#include "RSGISCalculateTopOfAtmosphereReflectance.h"

namespace rsgis{namespace calib{
    
    RSGISCalculateTopOfAtmosphereReflectance::RSGISCalculateTopOfAtmosphereReflectance(int numberOutBands, float *solarIrradiance, double distance, float solarZenith, float scaleFactor):RSGISCalcImageValue(numberOutBands)
    {
        this->solarIrradiance = solarIrradiance;
        this->distance = distance;
        this->solarZenith = solarZenith;
        this->scaleFactor = scaleFactor;
    }
    
    void RSGISCalculateTopOfAtmosphereReflectance::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
    {
        if(numBands != this->numOutBands)
        {
            throw RSGISImageCalcException("The number of input and output image bands needs to be the same.");
        }
        
        double distSq = distance * distance;
        
        for(int i = 0; i < this->numOutBands; ++i)
        {
            output[i] = ((M_PI * bandValues[i] * distSq)/(solarIrradiance[i] * cos(solarZenith))) * this->scaleFactor;
        }
    }
    
    RSGISCalculateTopOfAtmosphereReflectance::~RSGISCalculateTopOfAtmosphereReflectance()
    {
        
    }
}}

