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
    
    
    
    
    
    void RSGISCalcCloudParams::calcCloudHeights(GDALDataset *thermal, GDALDataset *cloudClumpsDS, GDALDataset *initCloudHeights, double lowerLandThres, double upperLandThres, float scaleFactor)throw(rsgis::img::RSGISImageCalcException)
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
                //std::cout << "r = " << r << std::endl;
                if(r > 8)
                {
                    percentile = ((r-8.0)*(r-8.0)) / (r*r);
                    //std::cout << "\tPercentile = " << percentile << std::endl;
                    env->init(minX[i], maxX[i], minY[i], maxY[i]);
                    
                    cloudBase[i] = calcImgPercentiles.getPercentile(thermal, 1, cloudClumpsDS, i, percentile, 0, true, env);
                }
                else
                {
                    cloudBase[i] = minBT[i];
                }
                //std::cout << "\t Value = " << cloudBase[i] << std::endl;
                
                
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
    
    
    void RSGISCalcCloudParams::projFitCloudShadow(GDALDataset *cloudClumpsDS, GDALDataset *initCloudHeights, GDALDataset *potentCloudShadowRegions, GDALDataset *cloudShadowTestRegionsDS, GDALDataset *cloudShadowRegionsDS, double sunAz, double sunZen, double senAz, double senZen)throw(rsgis::img::RSGISImageCalcException)
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
            //size_t i = 16;
            for(size_t i = 1; i < numClumps; ++i)
            {
                std::cout << "Processing clump " << i << " has " << cloudsRATHisto[i] << " pixels." << std::endl;
                //editShadImg.reset();
                
                std::vector<std::pair<std::pair<double,double>,double> > *pxPts = new std::vector<std::pair<std::pair<double,double>,double> >();
                pxPts->reserve(cloudsRATHisto[i]);
                env->init(minX[i], maxX[i], minY[i], maxY[i]);
                //std::cout << "Clump BBOX [" << minX[i] << ", " << maxX[i] << "][" << minY[i] << ", " << maxY[i] << "]\n";
                getPxPts.exportPixelsAsPointsWithVal(cloudClumpsDS, (float)i, initCloudHeights, 2, pxPts, env);
                
                baseHeight = hBaseMin[i];

                
                for(std::vector<std::pair<std::pair<double,double>,double> >::iterator iterPts = pxPts->begin(); iterPts != pxPts->end(); ++iterPts)
                {
                    pxlX = (*iterPts).first.first;
                    pxlY = (*iterPts).first.second;
                    cloudHgt = (baseHeight+(*iterPts).second) * 1000; // Conver to metres.
                    
                    //std::cout << "\t[" << pxlX << ", " << pxlY << "] = " << cloudHgt << "\n";
                    
                    // calculation taken from python-fmask
                    d = cloudHgt * tan(sunZen);
                    
                    // (x', y') are coordinates of each voxel projected onto the plane of the cloud base,
                    // for every voxel in the solid cloud
                    xDash = pxlX - d * sin(sunAz);
                    yDash = pxlY - d * cos(sunAz);
                    pxlOn = editShadImg.turnOnPxl(xDash, yDash);
                    /*
                    std::cout << "\t[" << xDash << ", " << yDash << "]";
                    if(pxlOn)
                    {
                        std::cout << "    ON\n";
                    }
                    else
                    {
                        std::cout << "    OFF\n";
                    }
                    */
                }
                
                
                delete pxPts;
            }
            
            
            
            
            
            
            
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
    }
    
    bool RSGISEditCloudShadowImg::turnOnPxl(double x, double y)throw(rsgis::img::RSGISCalcImageValue)
    {
        //std::cout << "BBOX [" << this->tlX << ", " << this->brX << "][" << this->brY << ", " << this->tlY << "]\n";
        //std::cout << "PT [" << x << ", " << y << "]\n";
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
            //std::cout << "[" << xPxlLoc << ", " << yPxlLoc << "]\n";

            this->testImgBand->RasterIO(GF_Write, xPxlLoc, yPxlLoc, 1, 1, &outValue, 1, 1, GDT_Float32, 0, 0);
            
            extent.expandToInclude(x, y);
        }
        
        return rtnStat;
    }
    
    void RSGISEditCloudShadowImg::reset()throw(rsgis::img::RSGISCalcImageValue)
    {
        this->testImgBand->Fill(0.0);
        extent.init(0,0,0,0);
    }
    
    void RSGISEditCloudShadowImg::calcCorrelation(GDALDataset *potentCloudShadowRegions, GDALDataset *cloudClumpsDS, double &corr, unsigned int &numPxlOverlap)throw(rsgis::img::RSGISCalcImageValue)
    {
        
    }
    
    RSGISEditCloudShadowImg::~RSGISEditCloudShadowImg()
    {
        
    }

    
    
    
    void RSGISCalcPxlCloudBaseAndTopHeight::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException)
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
    
}}


