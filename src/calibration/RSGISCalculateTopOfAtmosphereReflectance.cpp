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
    
    RSGISCalculateTopOfAtmosphereReflectance::RSGISCalculateTopOfAtmosphereReflectance(int numberOutBands, float *solarIrradiance, double distance, float solarZenith, float scaleFactor):rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->solarIrradiance = solarIrradiance;
        this->distance = distance;
        this->solarZenith = solarZenith;
        this->scaleFactor = scaleFactor;
        this->distSq = distance * distance;
    }
    
    void RSGISCalculateTopOfAtmosphereReflectance::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numBands != this->numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input and output image bands needs to be the same.");
        }
        
        for(int i = 0; i < this->numOutBands; ++i)
        {
            output[i] = ((M_PI * bandValues[i] * distSq)/(solarIrradiance[i] * cos(solarZenith))) * this->scaleFactor;
        }
    }
    
    RSGISCalculateTopOfAtmosphereReflectance::~RSGISCalculateTopOfAtmosphereReflectance()
    {
        
    }
    
    
    
    RSGISCalculateTOAThermalBrightness::RSGISCalculateTOAThermalBrightness(int numberOutBands, float *k1, float *k2, float scaleFactor):rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->k1 = k1;
        this->k2 = k2;
        this->scaleFactor = scaleFactor;
    }
    
    void RSGISCalculateTOAThermalBrightness::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numBands != this->numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input and output image bands needs to be the same.");
        }
        
        for(unsigned int i = 0; i < numBands; ++i)
        {
            if(bandValues[i] != 0.0)
            {
                double temp =  k2[i] / log((k1[i] / bandValues[i]) + 1.0);
                
                output[i] = (temp - 273.15);
                output[i] = output[i] * this->scaleFactor;
            }
            else
            {
                output[i] = 0.0;
            }
        }
        
        
    }
    
    RSGISCalculateTOAThermalBrightness::~RSGISCalculateTOAThermalBrightness()
    {
        
    }
}}

