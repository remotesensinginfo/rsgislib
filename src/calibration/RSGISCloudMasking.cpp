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
    
    RSGISLandsatFMaskPass1CloudMasking::RSGISLandsatFMaskPass1CloudMasking(unsigned int scaleFactor, unsigned int numLSBands, double whitenessThreshold) :rsgis::img::RSGISCalcImageValue(16)
    {
        this->scaleFactor = scaleFactor;
        this->whitenessThreshold = whitenessThreshold;
        
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
    
    void RSGISLandsatFMaskPass1CloudMasking::calcImageValue(float *bandValues, int numBands, double *output) 
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
            
            // Equation 1 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            double ndsi = (bandValues[greenIdx] - bandValues[swir1Idx]) / (bandValues[greenIdx] + bandValues[swir1Idx]);
            double ndvi = (bandValues[nirIdx] - bandValues[redIdx]) / (bandValues[nirIdx] + bandValues[redIdx]);
            bool basicTest = (bandValues[swir2Idx] > 0.03) & (bandValues[therm1Idx] < 27) & (ndvi < 0.8) & (ndsi < 0.8); // True are potential clouds.
            
            
            // Equation 2 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            double meanVis = (bandValues[blueIdx] + bandValues[greenIdx] + bandValues[redIdx])/3.0;
            double whiteness = fabs((bandValues[blueIdx]-meanVis)/meanVis) + fabs((bandValues[greenIdx]-meanVis)/meanVis) + fabs((bandValues[redIdx]-meanVis)/meanVis);
            
            bool whitenessTest = whiteness < whitenessThreshold; // Whiteness threshold defined as 0.7 in the Zhu and Woodcock 2012, RSE 118, pp83-94 paper but is useful to be able to edit.
            
            // Equation 3 (HAZE) (Zhu and Woodcock 2012, RSE 118, pp83-94):
            bool hotTest = (bandValues[blueIdx] - 0.5 * bandValues[redIdx] - 0.08) > 0;
            
            // Equation 4 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            bool nirswirTest = (bandValues[nirIdx] / bandValues[swir1Idx]) > 0.75;
            
            // Equation 5 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            bool waterTest = ((ndvi < 0.01) & (bandValues[nirIdx] < 0.11)) | ((ndvi < 0.1) & (bandValues[nirIdx] < 0.05));
            
            // Equation 6 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            bool pcp = basicTest & whitenessTest & hotTest & nirswirTest;
            
            ///////////////////////////////////////////////////////////////
            // This is an extra saturation test added by DERM and copied
            // from the python-fmask implementation, and is not part of the
            // Fmask algorithm. However, some cloud centres are saturated,
            // and thus fail the whiteness and haze tests
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
            ///////////////////////////////////////////////////////////////
            
            // Equation 7 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            bool clearSkyWater = waterTest & (bandValues[swir2Idx] < 0.03);
            
            // Equation 12 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            bool clearSkyLand = !pcp & !waterTest;
            
            // Equations 8 onwards require percentiles and are therefore in the next pass (see class RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking)
            
            
            // Equation 15 (Zhu and Woodcock 2012, RSE 118, pp83-94) - saving values to file rather than recalculating components later on:
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
            
            // Equation 20 (Zhu and Woodcock 2012, RSE 118, pp83-94):
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
            output[15] = clearSkyWater;
        }
        else
        {
            for(int i = 0; i < 16; ++i)
            {
                output[i] = 0; // Outside of image
            }
        }
    }
    
    RSGISLandsatFMaskPass1CloudMasking::~RSGISLandsatFMaskPass1CloudMasking()
    {
        
    }
    
    
    RSGISLandsatFMaskExportPass1LandWaterCloudMasking::RSGISLandsatFMaskExportPass1LandWaterCloudMasking():rsgis::img::RSGISCalcImageValue(1)
    {
        this->numValidPxls = 0.0;
        this->numPCPPxls = 0.0;
    }
    
    void RSGISLandsatFMaskExportPass1LandWaterCloudMasking::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        output[0] = 0;
        if(bandValues[10] == 1)
        {
            output[0] = 1; // land
        }
        if(bandValues[16] == 1)
        {
            output[0] = 2; // water
        }
        
        if(bandValues[0] == 1)
        {
            this->numValidPxls = this->numValidPxls + 1.0;
        }
        if(bandValues[9] == 1)
        {
            this->numPCPPxls = this->numPCPPxls + 1.0;
        }
        
    }
    
    double RSGISLandsatFMaskExportPass1LandWaterCloudMasking::propOfPCPPixels()
    {
        double outPCPProp = 0.0;
        if(this->numValidPxls > 0)
        {
            outPCPProp = this->numPCPPxls / this->numValidPxls;
        }
        return outPCPProp;
    };
    
    RSGISLandsatFMaskExportPass1LandWaterCloudMasking::~RSGISLandsatFMaskExportPass1LandWaterCloudMasking()
    {
        
    }
    
    
    
    
    RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking::RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking(unsigned int scaleFactor, unsigned int numLSBands, double water82ndThres, double land82ndThres, double land17thThres) :rsgis::img::RSGISCalcImageValue(6)
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
    
    void RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking::calcImageValue(float *bandValues, int numBands, double *output) 
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
            
            // Equation 9 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            double wTempProb = (water82ndThres - bandValues[therm1Idx]) / 4;
            
            // Equation 10 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            double brightnessProb = bandValues[swir1Idx];
            if(brightnessProb > 0.11)
            {
                brightnessProb = 0.11;
            }
            brightnessProb = brightnessProb / 0.11;
            
            // Equation 11 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            double waterCloudProb = wTempProb * brightnessProb;
            
            // Equation 14 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            double landTempProb = (land82ndThres + (4-bandValues[therm1Idx])) / (land82ndThres + (4 - (land17thThres - 4)));
            
            // Equation 16 (Zhu and Woodcock 2012, RSE 118, pp83-94):
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
    
    
    RSGISLandsatFMaskPass2CloudMasking::RSGISLandsatFMaskPass2CloudMasking(unsigned int scaleFactor, unsigned int numLSBands, double landCloudProbUpperThres, double waterCloudProbUpperThres, double lowerLandTempThres) :rsgis::img::RSGISCalcImageValue(1)
    {
        this->scaleFactor = scaleFactor;
        this->numLSBands = numLSBands;
        this->landCloudProbUpperThres = landCloudProbUpperThres;
        this->waterCloudProbUpperThres = waterCloudProbUpperThres;
        this->lowerLandTempThres = lowerLandTempThres;
        
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
            this->landCloudProbIdx = 29;
            this->waterCloudProbIdx = 26;
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
            this->landCloudProbIdx = 30;
            this->waterCloudProbIdx = 27;
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
            this->landCloudProbIdx = 31;
            this->waterCloudProbIdx = 28;
        }
        else
        {
            throw rsgis::img::RSGISImageCalcException("Number of landsat bands is not recognised.");
        }
    }
    
    void RSGISLandsatFMaskPass2CloudMasking::calcImageValue(float *bandValues, int numBands, double *output) 
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
            
            
            // Equation 18 (Zhu and Woodcock 2012, RSE 118, pp83-94):
            
            if((bandValues[pcpIdx] == 1) & (bandValues[waterTestIdx] == 1) & (bandValues[waterCloudProbIdx] > waterCloudProbUpperThres))
            {
                output[0] = 1;
            }
            else if((bandValues[pcpIdx] == 1) & (bandValues[waterTestIdx] == 0) & (bandValues[landCloudProbIdx] > landCloudProbUpperThres))
            {
                output[0] = 1;
            }
            else if((bandValues[waterTestIdx] == 0) & (bandValues[landCloudProbIdx] > 0.99))
            {
                output[0] = 1;
            }
            else if(bandValues[therm1Idx] < (this->lowerLandTempThres-35))
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
            output[0] = 0; // No data
        }
        
    }
    
    RSGISLandsatFMaskPass2CloudMasking::~RSGISLandsatFMaskPass2CloudMasking()
    {
        
    }
    
    
    
    

    void RSGISCalcImageCloudMajorityFilter::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
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
    
  
    void RSGISCalcImagePotentialCloudShadowsMask::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        if(numBands != 5)
        {
            throw rsgis::img::RSGISImageCalcException("The number of image bands must be 5.");
        }
        
        if(bandValues[0] == 1)
        {
            bandValues[1] = bandValues[1] / this->scaleFactor;
            bandValues[2] = bandValues[2] / this->scaleFactor;
            float diffValNIR = bandValues[2] - bandValues[1];
            
            bandValues[3] = bandValues[3] / this->scaleFactor;
            bandValues[4] = bandValues[4] / this->scaleFactor;
            float diffValSWIR = bandValues[4] - bandValues[3];
            
            if((diffValNIR > 0.02) & (diffValSWIR > 0.02))
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
    
    
    
    void RSGISCalcImagePotentialCloudShadowsMaskSingleInput::calcImageValue(float *bandValues, int numBands, double *output) 
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
    
    
    
    
    
    void RSGISCalcCloudParams::calcCloudHeights(GDALDataset *thermal, GDALDataset *cloudClumpsDS, GDALDataset *initCloudHeights, double lowerLandThres, double upperLandThres, float scaleFactor)
    {
        try
        {
            rsgis::rastergis::RSGISPopRATWithStats popRatStats;
            
            std::vector<rsgis::rastergis::RSGISBandAttStats *> bandStats = std::vector<rsgis::rastergis::RSGISBandAttStats *>();
            rsgis::rastergis::RSGISBandAttStats *thermB1Stats = new rsgis::rastergis::RSGISBandAttStats();
            thermB1Stats->init();
            thermB1Stats->band = 1;
            thermB1Stats->calcMin = true;
            thermB1Stats->minField = "ThermalMin";
            thermB1Stats->calcMax = true;
            thermB1Stats->maxField = "ThermalMax";
            bandStats.push_back(thermB1Stats);
            popRatStats.populateRATWithBasicStats(cloudClumpsDS, thermal, &bandStats, 1);
            bandStats.clear();
            delete thermB1Stats;
            
            rsgis::rastergis::RSGISCalcClusterLocation calcLoc;
            calcLoc.populateAttWithClumpLocationExtent(cloudClumpsDS, 1, "MinXX", "MinXY", "MaxXX", "MaxXY", "MinYX", "MinYY", "MaxXY", "MaxYY");
            
            GDALRasterAttributeTable *cloudsRAT = cloudClumpsDS->GetRasterBand(1)->GetDefaultRAT();
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            size_t numcloudsRATHistoRows = 0;
            size_t tmpSize = 0;
            int *cloudsRATHisto = attUtils.readIntColumn(cloudsRAT, "Histogram", &numcloudsRATHistoRows);
            double *minBT = attUtils.readDoubleColumn(cloudsRAT, "ThermalMin", &tmpSize);
            if(tmpSize != numcloudsRATHistoRows)
            {
                rsgis::img::RSGISImageCalcException("Strange error, columns in the RAT are different lengths!");
            }
            double *maxBT = attUtils.readDoubleColumn(cloudsRAT, "ThermalMax", &tmpSize);
            if(tmpSize != numcloudsRATHistoRows)
            {
                rsgis::img::RSGISImageCalcException("Strange error, columns in the RAT are different lengths!");
            }
            
            double *minX = attUtils.readDoubleColumn(cloudsRAT, "MinXX", &tmpSize);
            double *maxX = attUtils.readDoubleColumn(cloudsRAT, "MaxXX", &tmpSize);
            double *minY = attUtils.readDoubleColumn(cloudsRAT, "MinYY", &tmpSize);
            double *maxY = attUtils.readDoubleColumn(cloudsRAT, "MaxYY", &tmpSize);
            
            double *cloudBase = new double[numcloudsRATHistoRows];
            double *hBaseMin = new double[numcloudsRATHistoRows];
            double *hBaseMax = new double[numcloudsRATHistoRows];
            
            geos::geom::Envelope *env = new geos::geom::Envelope();
            
            rsgis::img::RSGISImagePercentiles calcImgPercentiles;
            
            double pi = 3.141592653589793;
            double r = 0.0;
            double percentile = 0.0;
            cloudBase[0] = 0.0;
            for(size_t i = 1; i < numcloudsRATHistoRows; ++i)
            {
                r = sqrt(cloudsRATHisto[i] / (2 * pi));
                if(r > 8)
                {
                    percentile = ((r-8.0)*(r-8.0)) / (r*r);
                    env->init(minX[i], maxX[i], minY[i], maxY[i]);
                    
                    cloudBase[i] = calcImgPercentiles.getPercentile(thermal, 1, cloudClumpsDS, i, percentile, 0, true, env, true);
                }
                else
                {
                    cloudBase[i] = minBT[i];
                }
                
                cloudBase[i] = cloudBase[i]/scaleFactor;
                minBT[i] = minBT[i]/scaleFactor;
                maxBT[i] = maxBT[i]/scaleFactor;
                
                hBaseMin[i] = (lowerLandThres - 4 - cloudBase[i])/9.8;
                if(0.2 > hBaseMin[i])
                {
                    hBaseMin[i] = 0.2;
                }
                hBaseMax[i] = (maxBT[i] + 4 - cloudBase[i]);
                if(12 < hBaseMax[i])
                {
                    hBaseMax[i] = 12;
                }
                
            }
            
            cloudBase[0] = 0;
            hBaseMin[0] = 0;
            hBaseMax[0] = 0;
            attUtils.writeRealColumn(cloudsRAT, "CloudBase", cloudBase, numcloudsRATHistoRows);
            attUtils.writeRealColumn(cloudsRAT, "hBaseMin", hBaseMin, numcloudsRATHistoRows);
            attUtils.writeRealColumn(cloudsRAT, "hBaseMax", hBaseMax, numcloudsRATHistoRows);
            
            rsgis::calib::RSGISCalcPxlCloudBaseAndTopHeight calcImgInitHeights = rsgis::calib::RSGISCalcPxlCloudBaseAndTopHeight(cloudBase, hBaseMin, numcloudsRATHistoRows, scaleFactor);
            rsgis::img::RSGISCalcImage calcInitHeights = rsgis::img::RSGISCalcImage(&calcImgInitHeights);
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = cloudClumpsDS;
            datasets[1] = thermal;
            calcInitHeights.calcImage(datasets, 1, 1, initCloudHeights);
            delete[] datasets;
            
            delete env;
            delete[] cloudBase;
            delete[] hBaseMin;
            delete[] hBaseMax;
            delete[] cloudsRATHisto;
            delete[] minBT;
            delete[] maxBT;
            delete[] minX;
            delete[] maxX;
            delete[] minY;
            delete[] maxY;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        
    }
    
    void RSGISCalcCloudParams::calcCloudHeightsNoThermal(GDALDataset *cloudClumpsDS, GDALDataset *initCloudHeightsDS)
    {
        try
        {
            rsgis::rastergis::RSGISCalcClusterLocation calcLoc;
            calcLoc.populateAttWithClumpLocationExtent(cloudClumpsDS, 1, "MinXX", "MinXY", "MaxXX", "MaxXY", "MinYX", "MinYY", "MaxXY", "MaxYY");
            
            GDALRasterAttributeTable *cloudsRAT = cloudClumpsDS->GetRasterBand(1)->GetDefaultRAT();
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            size_t numcloudsRATHistoRows = 0;
            int *cloudsRATHisto = attUtils.readIntColumn(cloudsRAT, "Histogram", &numcloudsRATHistoRows);
            
            double *cloudBase = new double[numcloudsRATHistoRows];
            double *hBaseMin = new double[numcloudsRATHistoRows];
            double *hBaseMax = new double[numcloudsRATHistoRows];
            
            for(size_t i = 1; i < numcloudsRATHistoRows; ++i)
            {
                cloudBase[i] = 8.0;
                hBaseMin[i] = 0.2;
                hBaseMax[i] = 12.0;
            }
            cloudBase[0] = 0.0;
            hBaseMin[0] = 0.0;
            hBaseMax[0] = 0.0;
            attUtils.writeRealColumn(cloudsRAT, "CloudBase", cloudBase, numcloudsRATHistoRows);
            attUtils.writeRealColumn(cloudsRAT, "hBaseMin", hBaseMin, numcloudsRATHistoRows);
            attUtils.writeRealColumn(cloudsRAT, "hBaseMax", hBaseMax, numcloudsRATHistoRows);
            
            rsgis::calib::RSGISCalcPxlCloudBaseAndTopHeightNoThermal calcImgInitHeights = rsgis::calib::RSGISCalcPxlCloudBaseAndTopHeightNoThermal(cloudBase, hBaseMin, numcloudsRATHistoRows);
            rsgis::img::RSGISCalcImage calcInitHeights = rsgis::img::RSGISCalcImage(&calcImgInitHeights);
            calcInitHeights.calcImage(&cloudClumpsDS, 1, 0, initCloudHeightsDS);
            
            delete[] cloudBase;
            delete[] hBaseMin;
            delete[] hBaseMax;
            delete[] cloudsRATHisto;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    
    void RSGISCalcCloudParams::projFitCloudShadow(GDALDataset *cloudClumpsDS, GDALDataset *initCloudHeights, GDALDataset *potentCloudShadowRegions, GDALDataset *cloudShadowTestRegionsDS, GDALDataset *cloudShadowRegionsDS, double sunAz, double sunZen, double senAz, double senZen)
    {
        try
        {
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            GDALRasterAttributeTable *cloudsRAT = cloudClumpsDS->GetRasterBand(1)->GetDefaultRAT();
            size_t numClumps = 0;
            int *cloudsRATHisto = attUtils.readIntColumn(cloudsRAT, "Histogram", &numClumps);
            double *minX = attUtils.readDoubleColumn(cloudsRAT, "MinXX", &numClumps);
            double *maxX = attUtils.readDoubleColumn(cloudsRAT, "MaxXX", &numClumps);
            double *minY = attUtils.readDoubleColumn(cloudsRAT, "MinYY", &numClumps);
            double *maxY = attUtils.readDoubleColumn(cloudsRAT, "MaxYY", &numClumps);
            double *cloudBase = attUtils.readDoubleColumn(cloudsRAT, "CloudBase", &numClumps);
            double *hBaseMin = attUtils.readDoubleColumn(cloudsRAT, "hBaseMin", &numClumps);
            double *hBaseMax = attUtils.readDoubleColumn(cloudsRAT, "hBaseMax", &numClumps);
            
            RSGISEditCloudShadowImg editShadImg = RSGISEditCloudShadowImg(cloudShadowTestRegionsDS, 1);
            RSGISEditCloudShadowImg editFinalShadImg = RSGISEditCloudShadowImg(cloudShadowRegionsDS, 1);
            editFinalShadImg.reset();
            rsgis::img::RSGISExtractPxlsAsPts getPxPts;
            geos::geom::Envelope *env = new geos::geom::Envelope();
            double baseHeight = 0.0;
            double pxlX = 0.0;
            double pxlY = 0.0;
            double cloudHgt = 0.0;
            double d = 0.0;
            double xDash = 0.0;
            double yDash = 0.0;
            bool pxlOn = false;
            bool first = true;
            double maxH = 0.0;
            double maxProp = 0.0;
            unsigned long maxOverlap = 0;
            double cloudPropOverlap = 0.0;
            unsigned long numPxlOverlap = 0;
            bool calcdShadProp = true;
            double *bestFitBaseLine = new double[numClumps];
            bestFitBaseLine[0] = 0.0;
            
            int feedback = numClumps/10.0;
            int feedbackCounter = 0;
            std::cout << "Iteratively finding optimal cloud height. This step may take a while; there are " << numClumps << " clumps\n";
            std::cout << "Started ." << std::flush;
            for(size_t i = 1; i < numClumps; ++i)
            {
                if((feedback != 0) && ((i % feedback) == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                std::vector<std::pair<std::pair<double,double>,double> > *pxPts = new std::vector<std::pair<std::pair<double,double>,double> >();
                pxPts->reserve(cloudsRATHisto[i]);
                env->init(minX[i], maxX[i], minY[i], maxY[i]);
                getPxPts.exportPixelsAsPointsWithVal(cloudClumpsDS, (float)i, initCloudHeights, 2, pxPts, true, env);
                
                
                first = true;
                for(baseHeight=hBaseMin[i]; baseHeight < hBaseMax[i]; baseHeight+=0.25)
                {
                    editShadImg.reset();
                    for(std::vector<std::pair<std::pair<double,double>,double> >::iterator iterPts = pxPts->begin(); iterPts != pxPts->end(); ++iterPts)
                    {
                        pxlX = (*iterPts).first.first;
                        pxlY = (*iterPts).first.second;
                        cloudHgt = (baseHeight+(*iterPts).second) * 1000; // Convert to metres.
                        
                        // calculation taken from python-fmask
                        d = cloudHgt * tan(sunZen);
                        
                        // (x', y') are coordinates of each voxel projected onto the plane of the cloud base,
                        // for every voxel in the solid cloud
                        xDash = pxlX - d * sin(sunAz);
                        yDash = pxlY - d * cos(sunAz);
                        pxlOn = editShadImg.turnOnPxl(xDash, yDash);
                    }
                    // Calculate shadow fit.
                    calcdShadProp = editShadImg.calcCorrelation(cloudClumpsDS, potentCloudShadowRegions, cloudShadowTestRegionsDS, &cloudPropOverlap, &numPxlOverlap);
                    if(calcdShadProp)
                    {
                        if(first)
                        {
                            maxH = baseHeight;
                            maxProp = cloudPropOverlap;
                            maxOverlap = numPxlOverlap;
                            first = false;
                        }
                        else if(cloudPropOverlap > maxProp)
                        {
                            maxH = baseHeight;
                            maxProp = cloudPropOverlap;
                            maxOverlap = numPxlOverlap;
                        }
                    }
                    else
                    {
                        if(first)
                        {
                            maxH = baseHeight;
                            maxProp = 0.0;
                            maxOverlap = 0.0;
                        }
                        break;
                    }
                }
                
                // Shadow best fit base height is 'maxH'
                bestFitBaseLine[i] = maxH;
                
                delete pxPts;
            }
            std::cout << ". Complete.\n";
            attUtils.writeRealColumn(cloudsRAT, "FitBaseLine", bestFitBaseLine, numClumps);
            rsgis::math::RSGISMathsUtils mathUtils;
            
            double histMinVal = 0.0;
            double histMaxVal = 0.0;
            unsigned int histNumBins = 0;
            double histBinWidth = 0.1;
            unsigned int *hist = NULL;
            bool gotHist = true;
            try
            {
                hist = mathUtils.calcHistogram(bestFitBaseLine, numClumps, histBinWidth, &histMinVal, &histMaxVal, &histNumBins, true);
            }
            catch(rsgis::math::RSGISMathException &e)
            {
                gotHist = false;
            }
            if(gotHist)
            {
                double bestFitBaseLineLowQuat = mathUtils.calcPercentile(25, histMinVal, histBinWidth, histNumBins, hist);
                double bestFitBaseLineMedian = mathUtils.calcPercentile(50, histMinVal, histBinWidth, histNumBins, hist);
                double bestFitBaseLineUpQuat = mathUtils.calcPercentile(75, histMinVal, histBinWidth, histNumBins, hist);
                delete [] hist;
                
                for(size_t i = 1; i < numClumps; ++i)
                {
                    if(bestFitBaseLine[i] < bestFitBaseLineLowQuat)
                    {
                        bestFitBaseLine[i] = bestFitBaseLineMedian;
                    }
                    else if(bestFitBaseLine[i] > bestFitBaseLineUpQuat)
                    {
                        bestFitBaseLine[i] = bestFitBaseLineMedian;
                    }
                }
            }
            attUtils.writeRealColumn(cloudsRAT, "FitBaseLineEdit", bestFitBaseLine, numClumps);
            
            feedbackCounter = 0;
            std::cout << "Producing cloud shadow mask using optimal heights:\n";
            std::cout << "Started ." << std::flush;
            // Add Shadow with most correspondance to Final Shadow Image.
            for(size_t i = 1; i < numClumps; ++i)
            {
                if((feedback != 0) && ((i % feedback) == 0))
                {
                    std::cout << "." << feedbackCounter << "." << std::flush;
                    feedbackCounter = feedbackCounter + 10;
                }
                
                std::vector<std::pair<std::pair<double,double>,double> > *pxPts = new std::vector<std::pair<std::pair<double,double>,double> >();
                pxPts->reserve(cloudsRATHisto[i]);
                env->init(minX[i], maxX[i], minY[i], maxY[i]);
                getPxPts.exportPixelsAsPointsWithVal(cloudClumpsDS, (float)i, initCloudHeights, 2, pxPts, true, env);
                
                for(std::vector<std::pair<std::pair<double,double>,double> >::iterator iterPts = pxPts->begin(); iterPts != pxPts->end(); ++iterPts)
                {
                    pxlX = (*iterPts).first.first;
                    pxlY = (*iterPts).first.second;
                    cloudHgt = (bestFitBaseLine[i]+(*iterPts).second) * 1000; // Conver to metres.
                    
                    // calculation taken from python-fmask
                    d = cloudHgt * tan(sunZen);
                    
                    // (x', y') are coordinates of each voxel projected onto the plane of the cloud base,
                    // for every voxel in the solid cloud
                    xDash = pxlX - d * sin(sunAz);
                    yDash = pxlY - d * cos(sunAz);
                    pxlOn = editFinalShadImg.turnOnPxl(xDash, yDash);
                }
                
                delete pxPts;
            }
            std::cout << ". Complete.\n";
            
            delete env;
            delete[] cloudBase;
            delete[] hBaseMin;
            delete[] hBaseMax;
            delete[] minX;
            delete[] maxX;
            delete[] minY;
            delete[] maxY;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    
    RSGISEditCloudShadowImg::RSGISEditCloudShadowImg(GDALDataset *testImg, int band)
    {
        this->testImg = testImg;
        this->testImgBand = this->testImg->GetRasterBand(band);
        this->band = band;
        double *trans = new double[6];
        this->testImg->GetGeoTransform(trans);
        
        this->tlX = trans[0];
        this->tlY = trans[3];
        this->xRes = trans[1];
        this->yRes = trans[5];
        if(this->yRes < 0)
        {
            this->yRes = this->yRes * (-1);
        }
        delete[] trans;
        
        this->nXPxl = testImg->GetRasterXSize();
        this->nYPxl = testImg->GetRasterYSize();
        
        this->brX = this->tlX + (this->nXPxl * this->xRes);
        this->brY = this->tlY - (this->nYPxl * this->yRes);
        
        extent.init(0,0,0,0);
        this->firstPts = true;
        
        this->calcCloudShadCorr = new RSGISCalcCloudShadowCorrespondance();
        this->imgCalc = new rsgis::img::RSGISCalcImage(this->calcCloudShadCorr);
    }
    
    bool RSGISEditCloudShadowImg::turnOnPxl(double x, double y)
    {
        bool rtnStat = true;
        if((x < this->tlX) | (x > this->brX))
        {
            rtnStat = false;
        }
        if((y > this->tlY) | (y < this->brY))
        {
            rtnStat = false;
        }
        
        if(rtnStat)
        {
            float outValue = 1.0;
            double xPxlLocF = (x - this->tlX) / this->xRes;
            double yPxlLocF = (this->tlY - y) / this->yRes;
            
            long xPxlLoc = floor(xPxlLocF + 0.5);
            long yPxlLoc = floor(yPxlLocF + 0.5);
            
            if( (xPxlLoc >= 0) & (xPxlLoc < this->nXPxl) & (yPxlLoc >= 0) & (yPxlLoc < this->nYPxl) )
            {
                this->testImgBand->RasterIO(GF_Write, xPxlLoc, yPxlLoc, 1, 1, &outValue, 1, 1, GDT_Float32, 0, 0);
                if(this->firstPts)
                {
                    extent.init(x,x,y,y);
                    this->firstPts=false;
                }
                else
                {
                    extent.expandToInclude(x, y);
                }
            }
            else
            {
                rtnStat = false;
            }
            
        }
        
        return rtnStat;
    }
    
    void RSGISEditCloudShadowImg::reset()
    {
        this->testImgBand->Fill(0.0);
        extent.init(0,0,0,0);
        this->firstPts = true;
    }
    
    bool RSGISEditCloudShadowImg::calcCorrelation(GDALDataset *cloudClumpsDS, GDALDataset *potentCloudShadowRegions, GDALDataset *cloudShadowTestRegionsDS, double *cloudPropOverlap, unsigned long *numPxlOverlap)
    {
        bool insideimg = true;
        try
        {
            // Check envelope is within image (i.e., shadow hasn't been projected outside of the image extent)
            if((extent.getMinX() < this->tlX) | (extent.getMinX() > this->brX))
            {
                insideimg = false;
            }
            else if((extent.getMaxX() < this->tlX) | (extent.getMaxX() > this->brX))
            {
                insideimg = false;
            }
            else if((extent.getMinY() < this->brY) | (extent.getMinY() > this->tlY))
            {
                insideimg = false;
            }
            else if((extent.getMaxY() < this->brY) | (extent.getMaxY() > this->tlY))
            {
                insideimg = false;
            }
            else if(extent.getWidth() < (this->xRes*2))
            {
                insideimg = false;
            }
            else if(extent.getHeight() < (this->yRes*2))
            {
                insideimg = false;
            }
            
            if(insideimg)
            {
                GDALDataset **datasets = new GDALDataset*[3];
                datasets[0] = cloudClumpsDS;
                datasets[1] = potentCloudShadowRegions;
                datasets[2] = cloudShadowTestRegionsDS;
                
                this->calcCloudShadCorr->reset();
                this->imgCalc->calcImage(datasets, 3, 0, &extent, true);
                delete[] datasets;
                
                unsigned long nShadPxlsVal = 0;
                this->calcCloudShadCorr->getNPxls(&nShadPxlsVal, numPxlOverlap);
                
                if(nShadPxlsVal == 0)
                {
                    (*cloudPropOverlap) = 0.0;
                    *numPxlOverlap = 0;
                    insideimg = false;
                }
                else
                {
                    (*cloudPropOverlap) = ((double)(*numPxlOverlap))/((double)nShadPxlsVal);
                }
            }
            else
            {
                (*cloudPropOverlap) = 0.0;
                *numPxlOverlap = 0;
            }
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        
        return insideimg;
    }
    
    RSGISEditCloudShadowImg::~RSGISEditCloudShadowImg()
    {
        delete this->imgCalc;
        delete this->calcCloudShadCorr;
    }

    
    
    
    void RSGISCalcPxlCloudBaseAndTopHeight::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
    {
        if(numIntVals != 1)
        {
            rsgis::img::RSGISImageCalcException("The cloud clumps band must only have 1 band.");
        }
        float lapseRateWet = 6.5; // degrees/km
        unsigned long fid = intBandValues[0];
        if(fid > 0)
        {
            floatBandValues[0] = floatBandValues[0]/scaleFactor;
            
            if(fid < numClumps)
            {
                if(floatBandValues[0] > cloudBase[fid])
                {
                    output[0] = cloudBase[fid];
                }
                else
                {
                    output[0] = floatBandValues[0];
                }
                
                if((output[0] - hBaseMin[fid]) == 0)
                {
                    output[1] = 0.0;
                }
                else
                {
                    output[1] = (cloudBase[fid] - output[0]) / lapseRateWet;
                }
                if(output[1] < 0)
                {
                    output[1] = output[1] * (-1);
                }
            }
            else
            {
                rsgis::img::RSGISImageCalcException("FID is larger than the number of known clouds...");
            }
        }
        else
        {
            output[0] = 0.0;
            output[1] = 0.0;
        }
    }
    
    
    void RSGISCalcPxlCloudBaseAndTopHeightNoThermal::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
    {
        if(numIntVals != 1)
        {
            rsgis::img::RSGISImageCalcException("The cloud clumps band must only have 1 band.");
        }
        unsigned long fid = intBandValues[0];
        if((fid > 0) & (fid <= numClumps))
        {
            output[0] = hBaseMin[fid];
            output[1] = 0.0;
        }
        else
        {
            output[0] = 0.0;
            output[1] = 0.0;
        }
    }
    
    
    RSGISCalcCloudShadowCorrespondance::RSGISCalcCloudShadowCorrespondance():rsgis::img::RSGISCalcImageValue(0)
    {
        nShadPxls = 0;
        nShadPxlsInPotent = 0;
    }
    
    
    void RSGISCalcCloudShadowCorrespondance::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) 
    {
        if(numIntVals != 3)
        {
            throw rsgis::img::RSGISImageCalcException("Incorrection number of bands, expecting 3 integer bands.");
        }
        
        if(intBandValues[0] == 0)
        {
            if(intBandValues[2] == 1)
            {
                ++nShadPxls;
            
                if(intBandValues[1] == 1)
                {
                    ++nShadPxlsInPotent;
                }
            }
        }
    }
    
    void RSGISCalcCloudShadowCorrespondance::reset()
    {
        nShadPxls = 0;
        nShadPxlsInPotent = 0;
    }
    
    void RSGISCalcCloudShadowCorrespondance::getNPxls(unsigned long *nShadPxlsVal, unsigned long *nShadPxlsInPotentVal)
    {
        (*nShadPxlsVal) = this->nShadPxls;
        (*nShadPxlsInPotentVal) = this->nShadPxlsInPotent;
    }
    
    RSGISCalcCloudShadowCorrespondance::~RSGISCalcCloudShadowCorrespondance()
    {
        
    }
    
    
    void RSGISCalcCombineMasks::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
    {
        if(numIntVals != 2)
        {
            throw rsgis::img::RSGISImageCalcException("Expecting 2 integer values.");
        }
        
        if(intBandValues[0] == 1)
        {
            output[0] = 1;
        }
        else if(intBandValues[1] == 1)
        {
            output[0] = 2;
        }
        else
        {
            output[0] = 0;
        }
        
    }
    
    
    void RSGISExportMaskForOverPCPThreshold::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) 
    {
        if(numIntVals != 1)
        {
            throw rsgis::img::RSGISImageCalcException("Expecting 1 integer values.");
        }
        
        
        if(intBandValues[0] == 1)
        {
            output[0] = 1;
        }
        else
        {
            output[0] = 0;
        }
    }
    
}}


