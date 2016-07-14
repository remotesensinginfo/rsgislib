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
    
    RSGISLandsatFMaskPass1CloudMasking::RSGISLandsatFMaskPass1CloudMasking(unsigned int scaleFactor, unsigned int numLSBands) throw(rsgis::img::RSGISImageCalcException):rsgis::img::RSGISCalcImageValue(15)
    {
        this->scaleFactor = scaleFactor;
        if(numLSBands == 9)
        {
            this->coastalIdx = 0;
            this->coastal = true;
            this->blueIdx = 1;
            this->greenIdx = 2;
            this->redIdx = 3;
            this->nirIdx = 4;
            this->swir1Idx = 5;
            this->swir2Idx = 6;
            this->therm1Idx = 7;
            this->therm2Idx = 8;
            this->thermal2 = true;
            
            this->coastalSatIdx = 9;
            this->blueSatIdx = 10;
            this->greenSatIdx = 11;
            this->redSatIdx = 12;
            this->nirSatIdx = 13;
            this->swir1SatIdx = 14;
            this->swir2SatIdx = 15;
            this->therm1SatIdx = 16;
            this->therm2SatIdx = 17;
        }
        else if(numLSBands == 8)
        {
            this->coastalIdx = 0;
            this->coastal = false;
            this->blueIdx = 0;
            this->greenIdx = 1;
            this->redIdx = 2;
            this->nirIdx = 3;
            this->swir1Idx = 4;
            this->swir2Idx = 5;
            this->therm1Idx = 6;
            this->therm2Idx = 7;
            this->thermal2 = true;
            
            this->coastalSatIdx = 8;
            this->blueSatIdx = 8;
            this->greenSatIdx = 9;
            this->redSatIdx = 10;
            this->nirSatIdx = 11;
            this->swir1SatIdx = 12;
            this->swir2SatIdx = 13;
            this->therm1SatIdx = 14;
            this->therm2SatIdx = 15;
        }
        else if(numLSBands == 7)
        {
            this->coastalIdx = 0;
            this->coastal = false;
            this->blueIdx = 0;
            this->greenIdx = 1;
            this->redIdx = 2;
            this->nirIdx = 3;
            this->swir1Idx = 4;
            this->swir2Idx = 5;
            this->therm1Idx = 6;
            this->therm2Idx = 6;
            this->thermal2 = false;
            
            this->coastalSatIdx = 7;
            this->blueSatIdx = 7;
            this->greenSatIdx = 8;
            this->redSatIdx = 9;
            this->nirSatIdx = 10;
            this->swir1SatIdx = 11;
            this->swir2SatIdx = 12;
            this->therm1SatIdx = 13;
            this->therm2SatIdx = 13;
        }
        else
        {
            throw rsgis::img::RSGISImageCalcException("Number of landsat bands is not recognised.");
        }
    }
    
    void RSGISLandsatFMaskPass1CloudMasking::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
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
            if(coastal)
            {
                bandValues[coastalIdx] = bandValues[coastalIdx]/this->scaleFactor;
            }
            bandValues[blueIdx] = bandValues[blueIdx]/this->scaleFactor;
            bandValues[greenIdx] = bandValues[greenIdx]/this->scaleFactor;
            bandValues[redIdx] = bandValues[redIdx]/this->scaleFactor;
            bandValues[nirIdx] = bandValues[nirIdx]/this->scaleFactor;
            bandValues[swir1Idx] = bandValues[swir1Idx]/this->scaleFactor;
            bandValues[swir2Idx] = bandValues[swir2Idx]/this->scaleFactor;
            bandValues[therm1Idx] = bandValues[therm1Idx]/this->scaleFactor;
            if(thermal2)
            {
                bandValues[therm2Idx] = bandValues[therm2Idx]/this->scaleFactor;
            }
            
            // Equation 1:
            double ndsi = (bandValues[greenIdx] - bandValues[swir1Idx]) / (bandValues[greenIdx] + bandValues[swir1Idx]);
            double ndvi = (bandValues[nirIdx] - bandValues[redIdx]) / (bandValues[nirIdx] + bandValues[redIdx]);
            
            //std::cout << "NDVI = " << ndvi << std::endl;
            //std::cout << "NDSI = " << ndsi << std::endl;
            
            //std::cout << "Thermal = " << bandValues[therm1Idx]-(273.15 * this->scaleFactor) << std::endl;
            //std::cout << "Thermal Threshold = " << 27 * this->scaleFactor << std::endl;
            //std::cout << "SWIR2 = " << bandValues[swir2Idx] << std::endl;
            
            bool basicTest = (bandValues[swir2Idx] > 0.03) & (bandValues[therm1Idx] < 27) & (ndvi < 0.8) & (ndsi < 0.8);
            
            
            
            
            // Equation 2:
            double meanVis = (bandValues[blueIdx] + bandValues[greenIdx] + bandValues[redIdx])/3.0;
            //std::cout << "Mean Vis = " << meanVis << std::endl;
            
            double whiteness = fabs((bandValues[blueIdx]-meanVis)/meanVis) + fabs((bandValues[greenIdx]-meanVis)/meanVis) + fabs((bandValues[redIdx]-meanVis)/meanVis);
            
            bool whitenessTest = whiteness < 0.7;
            //std::cout << "Whiteness = " << ((bandValues[blueIdx]-meanVis)/meanVis + (bandValues[greenIdx]-meanVis)/meanVis + (bandValues[redIdx]-meanVis)/meanVis) << std::endl;
            
            // Equation 3 (HAZE):
            //std::cout << "Blue  = " << bandValues[blueIdx] << " Red = " << bandValues[redIdx] << std::endl;
            bool hotTest = (bandValues[blueIdx] - 0.5 * bandValues[redIdx] - 0.08) > 0;
            //std::cout << "Hot Test: " << (bandValues[blueIdx] - 0.5 * bandValues[redIdx] - 0.08) << std::endl;
            
            
            // Equation 4:
            bool nirswirTest = (bandValues[nirIdx] / bandValues[swir1Idx]) > 0.75;
            //std::cout << "nirswirTest: " << (bandValues[nirIdx] / bandValues[swir1Idx]) << std::endl;
            
            
            // Equation 5:
            bool waterTest = ((ndvi < 0.01) & (bandValues[nirIdx] < 0.11)) | ((ndvi < 0.1) & (bandValues[nirIdx] < 0.05));
            
            
            // Equation 6
            bool pcp = basicTest & whitenessTest & hotTest & hotTest;
            
            // This is an extra saturation test added by DERM, and is not part of the Fmask algorithm.
            // However, some cloud centres are saturated, and thus fail the whiteness and haze tests
            bool saturatedPxl = false;
            if(coastal)
            {
                if(bandValues[coastalSatIdx] == 1)
                {
                    saturatedPxl = true;
                }
            }
            if(bandValues[blueSatIdx] == 1)
            {
                saturatedPxl = true;
            }
            else if(bandValues[greenSatIdx] == 1)
            {
                saturatedPxl = true;
            }
            else if(bandValues[redSatIdx] == 1)
            {
                saturatedPxl = true;
            }
            else if(bandValues[nirSatIdx] == 1)
            {
                saturatedPxl = true;
            }
            else if(bandValues[swir1SatIdx] == 1)
            {
                saturatedPxl = true;
            }
            else if(bandValues[swir2SatIdx] == 1)
            {
                saturatedPxl = true;
            }
            else if(bandValues[therm1SatIdx] == 1)
            {
                saturatedPxl = true;
            }
            if(thermal2)
            {
                if(bandValues[therm2SatIdx] == 1)
                {
                    saturatedPxl = true;
                }
            }
            bool veryBrightPxl = meanVis > 0.45;
            if(veryBrightPxl & saturatedPxl)
            {
                pcp = true;
                whitenessTest = true;
                whiteness = 0.0;
            }
            
            // Equation 7:
            bool clearSkyLand = !pcp & !waterTest;
            
            
            // Equation 15:
            double modNDSI = ndsi;
            if(bandValues[greenSatIdx] == 1)
            {
                modNDSI = 0;
            }
            double modNDVI = ndvi;
            if(bandValues[redSatIdx] == 1)
            {
                modNDVI = 0;
            }
            double varProb = fabs(modNDVI);
            if(fabs(modNDSI) > varProb)
            {
                varProb = fabs(modNDSI);
            }
            if(whiteness > varProb)
            {
                varProb = whiteness;
            }
            varProb = 1 - varProb;
            
            // Equation 20:
            bool snowTest = (ndsi > 0.15) & (bandValues[therm1Idx] < 3.8) & (bandValues[nirIdx] > 0.11) & (bandValues[greenIdx] > 0.1);
            
            output[0] = ndsi;
            output[1] = ndvi;
            output[2] = basicTest;
            output[3] = meanVis;
            output[4] = whitenessTest;
            output[5] = hotTest;
            output[6] = nirswirTest;
            output[7] = waterTest;
            output[8] = pcp;
            output[9] = clearSkyLand;
            output[10] = snowTest;
            output[11] = varProb;
            output[12] = modNDVI;
            output[13] = modNDSI;
            output[14] = whiteness;
        }
        else
        {
            for(int i = 0; i < 15; ++i)
            {
                output[i] = 0; // Outside of image
            }
        }
    }
    
    RSGISLandsatFMaskPass1CloudMasking::~RSGISLandsatFMaskPass1CloudMasking()
    {
        
    }
    
    
    RSGISLandsatFMaskExportPass1LandWaterCloudMasking::RSGISLandsatFMaskExportPass1LandWaterCloudMasking()throw(rsgis::img::RSGISImageCalcException):rsgis::img::RSGISCalcImageValue(1)
    {
        
    }
    
    void RSGISLandsatFMaskExportPass1LandWaterCloudMasking::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        output[0] = 0;
        if(bandValues[9] == 1)
        {
            output[0] = 1; // land
        }
        if(bandValues[7] == 1)
        {
            output[0] = 2; // water
        }
        
    }
    
    RSGISLandsatFMaskExportPass1LandWaterCloudMasking::~RSGISLandsatFMaskExportPass1LandWaterCloudMasking()
    {
        
    }
    
    
    
    
    RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking::RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking(unsigned int scaleFactor, unsigned int numLSBands, double water82ndThres, double land82ndThres, double land17thThres) throw(rsgis::img::RSGISImageCalcException):rsgis::img::RSGISCalcImageValue(6)
    {
        this->scaleFactor = scaleFactor;
        this->numLSBands = numLSBands;
        this->water82ndThres = water82ndThres;
        this->land82ndThres = land82ndThres;
        this->land17thThres = land17thThres;
        
        if(numLSBands == 7)
        {
            this->landWaterIdx = 0;
            
            this->coastalIdx = 1;
            this->coastal = false;
            this->blueIdx = 1;
            this->greenIdx = 2;
            this->redIdx = 3;
            this->nirIdx = 4;
            this->swir1Idx = 5;
            this->swir2Idx = 6;
            this->therm1Idx = 7;
            this->thermal2 = false;
            this->therm2Idx = 7;
            
            this->varProbIdx = 19;
        }
        else if(numLSBands == 8)
        {
            this->landWaterIdx = 0;
            
            this->coastalIdx = 1;
            this->coastal = false;
            this->blueIdx = 1;
            this->greenIdx = 2;
            this->redIdx = 3;
            this->nirIdx = 4;
            this->swir1Idx = 5;
            this->swir2Idx = 6;
            this->therm1Idx = 7;
            this->thermal2 = true;
            this->therm2Idx = 8;
            
            this->varProbIdx = 20;
        }
        else if(numLSBands == 9)
        {
            this->landWaterIdx = 0;
            
            this->coastalIdx = 1;
            this->coastal = true;
            this->blueIdx = 2;
            this->greenIdx = 3;
            this->redIdx = 4;
            this->nirIdx = 5;
            this->swir1Idx = 6;
            this->swir2Idx = 7;
            this->therm1Idx = 8;
            this->thermal2 = true;
            this->therm2Idx = 9;
            
            this->varProbIdx = 21;
        }
        else
        {
            throw rsgis::img::RSGISImageCalcException("Number of landsat bands is not recognised.");
        }
    }
    
    void RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
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
            if(numLSBands == 7)
            {
                if(bandValues[therm1Idx] == 0)
                {
                    noData = true;
                }
            }
            else if((numBands == 8) | (numBands == 9))
            {
                if((bandValues[therm1Idx] == 0) && (bandValues[therm2Idx] == 0))
                {
                    noData = true;
                }
            }
        }
        
        if(!noData)
        {
            if(coastal)
            {
                bandValues[coastalIdx] = bandValues[coastalIdx]/this->scaleFactor;
            }
            bandValues[blueIdx] = bandValues[blueIdx]/this->scaleFactor;
            bandValues[greenIdx] = bandValues[greenIdx]/this->scaleFactor;
            bandValues[redIdx] = bandValues[redIdx]/this->scaleFactor;
            bandValues[nirIdx] = bandValues[nirIdx]/this->scaleFactor;
            bandValues[swir1Idx] = bandValues[swir1Idx]/this->scaleFactor;
            bandValues[swir2Idx] = bandValues[swir2Idx]/this->scaleFactor;
            bandValues[therm1Idx] = bandValues[therm1Idx]/this->scaleFactor;
            if(thermal2)
            {
                bandValues[therm2Idx] = bandValues[therm2Idx]/this->scaleFactor;
            }
            
            double wTempProb = (water82ndThres - bandValues[therm1Idx]) / 4;
            
            double brightnessProb = bandValues[swir1Idx];
            if(brightnessProb > 0.11)
            {
                brightnessProb = 0.11;
            }
            brightnessProb = brightnessProb / 0.11;
            double waterCloudProb = wTempProb * brightnessProb;
            
            
            
            double landTempProb = (land82ndThres + (4-bandValues[therm1Idx])) / (land82ndThres + (4 - (land17thThres - 4)));
            
            double landCloudProb = bandValues[varProbIdx] * landTempProb;
            
            
            output[0] = wTempProb;
            output[1] = brightnessProb;
            output[2] = waterCloudProb;
            output[3] = landTempProb;
            output[4] = bandValues[varProbIdx];
            output[5] = landCloudProb;
        }
        else
        {
            for(int i = 0; i < 6; ++i)
            {
                output[i] = 0; // Outside of image
            }
        }
        
    }
    
    RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking::~RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking()
    {
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    RSGISLandsatFMaskPass2CloudMasking::RSGISLandsatFMaskPass2CloudMasking(unsigned int scaleFactor, unsigned int numLSBands, double landCloudProbUpperThres, double waterCloudProbUpperThres) throw(rsgis::img::RSGISImageCalcException):rsgis::img::RSGISCalcImageValue(1)
    {
        this->scaleFactor = scaleFactor;
        this->numLSBands = numLSBands;
        this->landCloudProbUpperThres = landCloudProbUpperThres;
        this->waterCloudProbUpperThres = waterCloudProbUpperThres;
        
        
        if(numLSBands == 7)
        {
            this->landWaterIdx = 0;
            
            coastal = false;
            this->coastalIdx = 1;
            this->blueIdx = 1;
            this->greenIdx = 2;
            this->redIdx = 3;
            this->nirIdx = 4;
            this->swir1Idx = 5;
            this->swir2Idx = 6;
            this->therm1Idx = 7;
            this->therm2Idx = 7;
            thermal2 = false;
            
            this->pcpIdx = 16;
            this->waterTestIdx = 15;
            this->snowTestIdx = 18;
            this->landCloudProbIdx = 28;
            this->waterCloudProbIdx = 25;
        }
        else if(numLSBands == 8)
        {
            this->landWaterIdx = 0;
            
            coastal = false;
            this->coastalIdx = 1;
            this->blueIdx = 1;
            this->greenIdx = 2;
            this->redIdx = 3;
            this->nirIdx = 4;
            this->swir1Idx = 5;
            this->swir2Idx = 6;
            this->therm1Idx = 7;
            this->therm2Idx = 8;
            thermal2 = true;
            
            this->pcpIdx = 17;
            this->waterTestIdx = 16;
            this->snowTestIdx = 19;
            this->landCloudProbIdx = 29;
            this->waterCloudProbIdx = 26;
        }
        else if(numLSBands == 9)
        {
            this->landWaterIdx = 0;
            
            coastal = true;
            this->coastalIdx = 1;
            this->blueIdx = 2;
            this->greenIdx = 3;
            this->redIdx = 4;
            this->nirIdx = 5;
            this->swir1Idx = 6;
            this->swir2Idx = 7;
            this->therm1Idx = 8;
            this->therm2Idx = 9;
            thermal2 = true;
            
            this->pcpIdx = 18;
            this->waterTestIdx = 17;
            this->snowTestIdx = 19;
            this->landCloudProbIdx = 30;
            this->waterCloudProbIdx = 27;
        }
        else
        {
            throw rsgis::img::RSGISImageCalcException("Number of landsat bands is not recognised.");
        }
    }
    
    void RSGISLandsatFMaskPass2CloudMasking::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        bool noData = true;
        for(unsigned i = 1; i <= numLSBands; ++i)
        {
            if(bandValues[i] != 0.0)
            {
                noData = false;
            }
        }
        
        if(!noData)
        {
            if(numLSBands == 7)
            {
                if(bandValues[therm1Idx] == 0)
                {
                    noData = true;
                }
            }
            else if((numBands == 8) | (numBands == 9))
            {
                if((bandValues[therm1Idx] == 0) && (bandValues[therm2Idx] == 0))
                {
                    noData = true;
                }
            }
        }
        
        if(!noData)
        {
            if(coastal)
            {
                bandValues[coastalIdx] = bandValues[coastalIdx]/this->scaleFactor;
            }
            bandValues[blueIdx] = bandValues[blueIdx]/this->scaleFactor;
            bandValues[greenIdx] = bandValues[greenIdx]/this->scaleFactor;
            bandValues[redIdx] = bandValues[redIdx]/this->scaleFactor;
            bandValues[nirIdx] = bandValues[nirIdx]/this->scaleFactor;
            bandValues[swir1Idx] = bandValues[swir1Idx]/this->scaleFactor;
            bandValues[swir2Idx] = bandValues[swir2Idx]/this->scaleFactor;
            bandValues[therm1Idx] = bandValues[therm1Idx]/this->scaleFactor;
            if(thermal2)
            {
                bandValues[therm2Idx] = bandValues[therm2Idx]/this->scaleFactor;
            }
            
            if((bandValues[pcpIdx] == 1) & (bandValues[waterTestIdx] == 1) & (bandValues[waterCloudProbIdx] > waterCloudProbUpperThres))
            {
                output[0] = 1;
            }
            else if((bandValues[pcpIdx] == 1) & (bandValues[waterTestIdx] == 0) & (bandValues[landCloudProbIdx] > landCloudProbUpperThres))
            {
                output[0] = 1;
            }
            else if(bandValues[therm1Idx] < -35)
            {
                output[0] = 1;
            }
            else
            {
                output[0] = 0;
            }
            
            /*if(bandValues[snowTestIdx] == 1)
            {
                output[0] = 3;
            }*/
            
        }
        else
        {
            output[0] = 0; // No data
        }
        
    }
    
    RSGISLandsatFMaskPass2CloudMasking::~RSGISLandsatFMaskPass2CloudMasking()
    {
        
    }
    
    
    
    

    void RSGISCalcImageCloudMajorityFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numBands != 1)
        {
            rsgis::img::RSGISImageCalcException("Image must only have 1 band.");
        }
        
        int midPt = (winSize-1)/2;
        output[0] = dataBlock[0][midPt][midPt];
        
        if(dataBlock[0][midPt][midPt] != 1)
        {
            int numPxl = winSize * winSize;
            int numPxlThres = floor((numPxl*0.5)+0.5)+1;
            long cloudPxlCount = 0;
            
            for(unsigned int i = 0; i < winSize; ++i)
            {
                for(unsigned int j = 0; j < winSize; ++j)
                {
                    if(dataBlock[0][i][j] == 1)
                    {
                        ++cloudPxlCount;
                    }
                }
            }
            
            if(cloudPxlCount >= numPxlThres)
            {
                output[0] = 1;
            }
        }
        
    }
    
  
    void RSGISCalcImagePotentialCloudShadowsMask::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numBands != 3)
        {
            throw rsgis::img::RSGISImageCalcException("The number of image bands must be 3.");
        }
        
        if(bandValues[0] == 1)
        {
            bandValues[1] = bandValues[1] / this->scaleFactor;
            bandValues[2] = bandValues[2] / this->scaleFactor;
            float diffVal = bandValues[2] - bandValues[1];
            if(diffVal > 0.02)
            {
                output[0] = 1;
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
    
    
    
}}


