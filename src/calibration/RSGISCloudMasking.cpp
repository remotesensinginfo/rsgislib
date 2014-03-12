/*
 *  RSGISCloudMasking.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/02/2014.
 *  Copyright 2014 RSGISLib. All rights reserved.
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

#include "RSGISCloudMasking.h"

namespace rsgis{namespace calib{
    
    RSGISLandsatFMaskPass1CloudMasking::RSGISLandsatFMaskPass1CloudMasking(unsigned int scaleFactor):rsgis::img::RSGISCalcImageValue(1)
    {
        this->scaleFactor = scaleFactor;
        this->blueIdx = 0;
        this->greenIdx = 1;
        this->redIdx = 2;
        this->nirIdx = 3;
        this->swir1Idx = 4;
        this->swir2Idx = 5;
        this->therm1Idx = 6;
        this->therm2Idx = 7;
    }
    
    void RSGISLandsatFMaskPass1CloudMasking::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        bool noData = true;
        for(unsigned i = 0; i < numBands; ++i)
        {
            if(bandValues[i] != 0.0)
            {
                noData = false;
            }
        }
        
        if(!noData)
        {
            if(numBands == 8)
            {
                if((bandValues[therm1Idx] == 0) && (bandValues[therm2Idx] == 0))
                {
                    noData = true;
                }
            }
            else if(numBands == 7)
            {
                if(bandValues[therm1Idx] == 0)
                {
                    noData = true;
                }
            }
        }
        
        if(!noData)
        {
            double ndsi = (bandValues[greenIdx] - bandValues[swir1Idx]) / (bandValues[greenIdx] + bandValues[swir1Idx]);
            double ndvi = (bandValues[nirIdx] - bandValues[redIdx]) / (bandValues[nirIdx] + bandValues[redIdx]);
            
            //std::cout << "NDVI = " << ndvi << std::endl;
            //std::cout << "NDSI = " << ndsi << std::endl;
            
            //std::cout << "Thermal = " << bandValues[therm1Idx]-(273.15 * this->scaleFactor) << std::endl;
            //std::cout << "Thermal Threshold = " << 27 * this->scaleFactor << std::endl;
            //std::cout << "SWIR2 = " << bandValues[swir2Idx] << std::endl;
            
            bool basicTest = (bandValues[swir2Idx] > (0.03 * this->scaleFactor)) & (bandValues[therm1Idx] < (27 * this->scaleFactor)) & (ndvi < 0.8) & (ndsi < 0.8);
            
            double meanVis = (bandValues[blueIdx] + bandValues[greenIdx] + bandValues[redIdx])/3;
            
            //std::cout << "Mean Vis = " << meanVis << std::endl;
            
            bool whitenessTest = ((fabs(bandValues[blueIdx]-meanVis)/meanVis) + (fabs(bandValues[greenIdx]-meanVis)/meanVis) + (fabs(bandValues[redIdx]-meanVis)/meanVis)) < 0.7;
            
            //std::cout << "Whiteness = " << ((bandValues[blueIdx]-meanVis)/meanVis + (bandValues[greenIdx]-meanVis)/meanVis + (bandValues[redIdx]-meanVis)/meanVis) << std::endl;
            
            bool hotTest = (((bandValues[blueIdx]/this->scaleFactor)-0.5) * ((bandValues[redIdx]/this->scaleFactor)-0.08)) < 0;
            
            //std::cout << "Hot Test: " << ((bandValues[blueIdx]-(0.5 * this->scaleFactor)) * (bandValues[redIdx]-(0.08 * this->scaleFactor))) << std::endl;
            
            bool nirswirTest = (bandValues[nirIdx] / bandValues[swir1Idx]) > 0.75;
            
            //std::cout << "nirswirTest: " << (bandValues[nirIdx] / bandValues[swir1Idx]) << std::endl;
            
            bool waterTest = ((ndvi < 0.01) & (bandValues[nirIdx] < (0.11 * this->scaleFactor))) | ((ndvi < 0.1) & (bandValues[nirIdx] < (0.05 * this->scaleFactor)));
            
            if(waterTest)
            {
                if(basicTest & whitenessTest & nirswirTest & hotTest)
                {
                    output[0] = 5;
                }
                else if((bandValues[swir2Idx] < (0.03 * this->scaleFactor)))
                {
                    output[0] = 3;
                }
                else
                {
                    output[0] = 2;
                }
            }
            else
            {
                if(basicTest & whitenessTest & nirswirTest & hotTest)
                {
                    output[0] = 4;
                }
                else
                {
                    output[0] = 1;
                }
            }
        }
        else
        {
            output[0] = 0;
        }
    }
    
    RSGISLandsatFMaskPass1CloudMasking::~RSGISLandsatFMaskPass1CloudMasking()
    {
        
    }
    
    
    
    
    
    
    
    
    
    RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking::RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking(unsigned int scaleFactor, unsigned int numThermalBands, double water82ndThres, double land82ndThres, double land17thThres):rsgis::img::RSGISCalcImageValue(1)
    {
        this->scaleFactor = scaleFactor;
        this->numThermalBands = numThermalBands;
        this->water82ndThres = water82ndThres;
        this->land82ndThres = land82ndThres;
        this->land17thThres = land17thThres;
        this->pass1Idx = 0;
        this->blueIdx = 1;
        this->greenIdx = 2;
        this->redIdx = 3;
        this->nirIdx = 4;
        this->swir1Idx = 5;
        this->swir2Idx = 6;
        this->therm1Idx = 7;
        if(numThermalBands == 1)
        {
            this->therm2Idx = 7;
            
            this->blueSatIdx = 8;
            this->greenSatIdx = 9;
            this->redSatIdx = 10;
            this->nirSatIdx = 11;
            this->swir1SatIdx = 12;
            this->swir2SatIdx = 13;
            this->therm1SatIdx = 14;
            this->therm2SatIdx = 14;
        }
        else
        {
            this->therm2Idx = 8;
            
            this->blueSatIdx = 9;
            this->greenSatIdx = 10;
            this->redSatIdx = 11;
            this->nirSatIdx = 12;
            this->swir1SatIdx = 13;
            this->swir2SatIdx = 14;
            this->therm1SatIdx = 15;
            this->therm2SatIdx = 16;
        }
    }
    
    void RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        bool noData = true;
        for(unsigned i = 1; i < numBands; ++i)
        {
            if(bandValues[i] != 0.0)
            {
                noData = false;
            }
        }
        
        if(!noData)
        {
            if(numBands == 9)
            {
                if((bandValues[therm1Idx] == 0) && (bandValues[therm2Idx] == 0))
                {
                    noData = true;
                }
            }
            else if(numBands == 8)
            {
                if(bandValues[therm1Idx] == 0)
                {
                    noData = true;
                }
            }
        }
        
        if(!noData)
        {
            if((bandValues[pass1Idx] == 1) | (bandValues[pass1Idx] == 4))
            {
                double landTempProb = (land82ndThres + ((4*this->scaleFactor)-bandValues[therm1Idx])) / (land82ndThres + ((4*this->scaleFactor) - (land17thThres - (4*this->scaleFactor))));
                
                double ndsi = (bandValues[greenIdx] - bandValues[swir1Idx]) / (bandValues[greenIdx] + bandValues[swir1Idx]);
                if((bandValues[greenSatIdx] == 1.0) & (bandValues[swir1Idx] >= bandValues[greenIdx]))
                {
                    ndsi = 0;
                }
                ndsi = fabs(ndsi);
                
                double ndvi = (bandValues[nirIdx] - bandValues[redIdx]) / (bandValues[nirIdx] + bandValues[redIdx]);
                if((bandValues[redSatIdx] == 1.0) & (bandValues[nirIdx] >= bandValues[redIdx]))
                {
                    ndvi = 0;
                }
                ndvi = fabs(ndvi);
                
                double meanVis = (bandValues[blueIdx] + bandValues[greenIdx] + bandValues[redIdx])/3;
                double whiteness = ((fabs(bandValues[blueIdx]-meanVis)/meanVis) + (fabs(bandValues[greenIdx]-meanVis)/meanVis) + (fabs(bandValues[redIdx]-meanVis)/meanVis));
                
                double variabMaxVal = ndsi;
                if(ndvi > variabMaxVal)
                {
                    variabMaxVal = ndvi;
                }
                else if(whiteness > variabMaxVal)
                {
                    variabMaxVal = whiteness;
                }
                
                double variabilityProb = 1-variabMaxVal;
                
                output[0] = variabilityProb * landTempProb;
            }
            else
            {
                output[0] = 0;
            }
        }
        else
        {
            output[0] = 0;
        }
        
    }
    
    RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking::~RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking()
    {
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    RSGISLandsatFMaskPass2CloudMasking::RSGISLandsatFMaskPass2CloudMasking(unsigned int scaleFactor, unsigned int numThermalBands, double water82ndThres, double land82ndThres, double land17thThres, double landCloudProb82ndThres):rsgis::img::RSGISCalcImageValue(1)
    {
        this->scaleFactor = scaleFactor;
        this->numThermalBands = numThermalBands;
        this->water82ndThres = water82ndThres;
        this->land82ndThres = land82ndThres;
        this->land17thThres = land17thThres;
        this->landCloudProb82ndThres = landCloudProb82ndThres;
        this->pass1Idx = 0;
        this->blueIdx = 1;
        this->greenIdx = 2;
        this->redIdx = 3;
        this->nirIdx = 4;
        this->swir1Idx = 5;
        this->swir2Idx = 6;
        this->therm1Idx = 7;
        if(numThermalBands == 1)
        {
            this->therm2Idx = 7;

            this->blueSatIdx = 8;
            this->greenSatIdx = 9;
            this->redSatIdx = 10;
            this->nirSatIdx = 11;
            this->swir1SatIdx = 12;
            this->swir2SatIdx = 13;
            this->therm1SatIdx = 14;
            this->therm2SatIdx = 14;
        }
        else
        {
            this->therm2Idx = 8;
            
            this->blueSatIdx = 9;
            this->greenSatIdx = 10;
            this->redSatIdx = 11;
            this->nirSatIdx = 12;
            this->swir1SatIdx = 13;
            this->swir2SatIdx = 14;
            this->therm1SatIdx = 15;
            this->therm2SatIdx = 16;
        }
    }
    
    void RSGISLandsatFMaskPass2CloudMasking::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        bool noData = true;
        for(unsigned i = 1; i < numBands; ++i)
        {
            if(bandValues[i] != 0.0)
            {
                noData = false;
            }
        }
        
        if(!noData)
        {
            if(numBands == 9)
            {
                if((bandValues[therm1Idx] == 0) && (bandValues[therm2Idx] == 0))
                {
                    noData = true;
                }
            }
            else if(numBands == 8)
            {
                if(bandValues[therm1Idx] == 0)
                {
                    noData = true;
                }
            }
        }
        
        if(!noData)
        {
            if(bandValues[pass1Idx] == 5) // POSSIBLE WATER CLOUDS!!
            {
                double waterTempProb = (water82ndThres-bandValues[therm1Idx])/4;
                
                double brightMin = 0.11 * ((double)this->scaleFactor);
                if(bandValues[swir1Idx] < brightMin)
                {
                    brightMin = bandValues[swir1Idx];
                }
                double waterBrightProb = brightMin/(0.11 * ((double)this->scaleFactor));
                
                double wCloudProb = waterTempProb - waterBrightProb;
                
                if(wCloudProb > 0.5)
                {
                    output[0] = 2;
                }
                else
                {
                    output[0] = 1;
                }
                
            }
            else if((bandValues[pass1Idx] == 4) | (bandValues[pass1Idx] == 1)) // POSSIBLE LAND CLOUDS!!
            {
                double landTempProb = (land82ndThres + ((4*this->scaleFactor)-bandValues[therm1Idx])) / (land82ndThres + ((4*this->scaleFactor) - (land17thThres - (4*this->scaleFactor))));
                
                double ndsi = (bandValues[greenIdx] - bandValues[swir1Idx]) / (bandValues[greenIdx] + bandValues[swir1Idx]);
                if((bandValues[greenSatIdx] == 1.0) & (bandValues[swir1Idx] >= bandValues[greenIdx]))
                {
                    ndsi = 0;
                }
                ndsi = fabs(ndsi);
                
                double ndvi = (bandValues[nirIdx] - bandValues[redIdx]) / (bandValues[nirIdx] + bandValues[redIdx]);
                if((bandValues[redSatIdx] == 1.0) & (bandValues[nirIdx] >= bandValues[redIdx]))
                {
                    ndvi = 0;
                }
                ndvi = fabs(ndvi);
                
                double meanVis = (bandValues[blueIdx] + bandValues[greenIdx] + bandValues[redIdx])/3;
                double whiteness = ((fabs(bandValues[blueIdx]-meanVis)/meanVis) + (fabs(bandValues[greenIdx]-meanVis)/meanVis) + (fabs(bandValues[redIdx]-meanVis)/meanVis));
                
                double variabMaxVal = ndsi;
                if(ndvi > variabMaxVal)
                {
                    variabMaxVal = ndvi;
                }
                else if(whiteness > variabMaxVal)
                {
                    variabMaxVal = whiteness;
                }
                
                double variabilityProb = 1-variabMaxVal;
                
                double lCloudPProb = variabilityProb * landTempProb;
                
                if((bandValues[pass1Idx] == 4) & (lCloudPProb > this->landCloudProb82ndThres))
                {
                    output[0] = 2;
                }
                else if(lCloudPProb > 0.99)
                {
                    output[0] = 2;
                }
                else if(bandValues[therm1Idx] < (this->land17thThres - (35 * this->scaleFactor)))
                {
                    output[0] = 2;
                }
                else
                {
                    output[0] = 1;
                }
            }
            else
            {
                output[0] = 1;
            }
        }
        else
        {
            output[0] = 0;
        }
        
    }
    
    RSGISLandsatFMaskPass2CloudMasking::~RSGISLandsatFMaskPass2CloudMasking()
    {
        
    }
    
}}


