/*
 *  RSGISApply6SCoefficients.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/07/2011.
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

#include "RSGISApply6SCoefficients.h"

namespace rsgis{namespace calib{
    
    RSGISApply6SCoefficientsSingleParam::RSGISApply6SCoefficientsSingleParam(unsigned int *imageBands, float *aX, float *bX, float *cX, int numValues, float noDataVal, bool useNoDataVal, float scaleFactor):rsgis::img::RSGISCalcImageValue(numValues)
    {
		this->imageBands = imageBands;
        this->aX = aX;
        this->bX = bX;
        this->cX = cX;
        this->numValues = numValues;
        this->scaleFactor = scaleFactor;
        this->noDataVal = noDataVal;
        this->useNoDataVal = useNoDataVal;
    }
    
    void RSGISApply6SCoefficientsSingleParam::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numValues != this->numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input image bands needs to be equal to the number of output image bands.");
        }
        
        if(numBands != numValues)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input values needs to be equal to the number of input image bands.");
        }
        
        double tmpVal = 0;
		
        bool nodata = true;
        if(this->useNoDataVal)
        {
            for(int i = 0; i < numBands; ++i)
            {
                if(bandValues[i] != this->noDataVal)
                {
                    nodata = false;
                    break;
                }
            }
        }
        else
        {
            nodata = false;
        }
        
        
		
        if(nodata)
        {
            for(unsigned int i = 0; i < this->numValues; ++i)
            {
                output[i] = 0;
            }
        }
        else
        {            
            for(unsigned int i = 0; i < this->numValues; ++i)
            {
                if(imageBands[i] > numBands)
                {
                    std::cout << "Image band: " << imageBands[i] << std::endl;
                    throw rsgis::img::RSGISImageCalcException("Image band is not within image.");
                }
                
                tmpVal=aX[i]*bandValues[imageBands[i]]-bX[i];
                output[i] = (tmpVal/(1.0+cX[i]*tmpVal))*this->scaleFactor;
                
                if(this->useNoDataVal & (this->noDataVal == 0.0))
                {
                    if(output[i] < 1)
                    {
                        output[i] = 1.0;
                    }
                    else
                    {
                        output[i] = output[i] + 1.0;
                    }
                }
            }
        }
        
    }
    
    RSGISApply6SCoefficientsSingleParam::~RSGISApply6SCoefficientsSingleParam()
    {
        
    }
    
    RSGISApply6SCoefficientsElevLUTParam::RSGISApply6SCoefficientsElevLUTParam(unsigned int numOutBands, std::vector<LUT6SElevation> *lut, float noDataVal, bool useNoDataVal, float scaleFactor):rsgis::img::RSGISCalcImageValue(numOutBands)
    {
		this->lut = lut;
        this->scaleFactor = scaleFactor;
        this->noDataVal = noDataVal;
        this->useNoDataVal = useNoDataVal;
    }
    
    void RSGISApply6SCoefficientsElevLUTParam::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numBands-1 != this->numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input image bands needs to be equal to the number of output image bands.");
        }
        
        double tmpVal = 0;
        float elevVal = bandValues[0];
		
        bool nodata = true;
        if(this->useNoDataVal)
        {
            for(int i = 1; i < numBands; ++i)
            {
                if(bandValues[i] != this->noDataVal)
                {
                    nodata = false;
                    break;
                }
            }
        }
        else
        {
            nodata = false;
        }
        
        
		
        if(nodata)
        {
            for(unsigned int i = 0; i < this->numOutBands; ++i)
            {
                output[i] = 0;
            }
        }
        else
        {
            LUT6SElevation lutVal;
            float dist = 0.0;
            float minDist = 0.0;
            unsigned int lutIdx = 0;
            
            for(unsigned int i = 0; i < lut->size(); ++i)
            {
                dist = (lut->at(i).elev - elevVal) * (lut->at(i).elev - elevVal);
                if(i == 0)
                {
                    minDist = dist;
                    lutIdx = i;
                }
                else if(dist < minDist)
                {
                    minDist = dist;
                    lutIdx = i;
                }
            }
            
            lutVal = lut->at(lutIdx);
            
            for(unsigned int i = 0; i < lutVal.numValues; ++i)
            {
                if(lutVal.imageBands[i] > numBands)
                {
                    std::cout << "Image band: " << lutVal.imageBands[i] << std::endl;
                    throw rsgis::img::RSGISImageCalcException("Image band is not within image.");
                }
                
                tmpVal=lutVal.aX[i]*bandValues[lutVal.imageBands[i]]-lutVal.bX[i];
                output[i] = (tmpVal/(1.0+lutVal.cX[i]*tmpVal))*this->scaleFactor;
                
                if(this->useNoDataVal & (this->noDataVal == 0.0))
                {
                    if(output[i] < 1)
                    {
                        output[i] = 1.0;
                    }
                    else
                    {
                        output[i] = output[i] + 1.0;
                    }
                }
            }
        }
        
    }
    
    RSGISApply6SCoefficientsElevLUTParam::~RSGISApply6SCoefficientsElevLUTParam()
    {
        
    }
    
    
    
    
    RSGISApply6SCoefficientsElevAOTLUTParam::RSGISApply6SCoefficientsElevAOTLUTParam(unsigned int numOutBands, std::vector<LUT6SBaseElevAOT> *lut, float noDataVal, bool useNoDataVal, float scaleFactor):rsgis::img::RSGISCalcImageValue(numOutBands)
    {
		this->lut = lut;
        this->scaleFactor = scaleFactor;
        this->noDataVal = noDataVal;
        this->useNoDataVal = useNoDataVal;
    }
    
    void RSGISApply6SCoefficientsElevAOTLUTParam::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numBands-2 != this->numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input image bands needs to be equal to the number of output image bands.");
        }
        
        double tmpVal = 0;
        float elevVal = bandValues[0];
        float aotVal = bandValues[1];
		
        bool nodata = true;
        if(this->useNoDataVal)
        {
            for(int i = 2; i < numBands; ++i)
            {
                if(bandValues[i] != this->noDataVal)
                {
                    nodata = false;
                    break;
                }
            }
        }
        else
        {
            nodata = false;
        }
        
        
		
        if(nodata)
        {
            for(unsigned int i = 0; i < this->numOutBands; ++i)
            {
                output[i] = 0;
            }
        }
        else
        {
            LUT6SBaseElevAOT elevLUTVal;
            unsigned int elevLUTIdx = 0;
            float dist = 0.0;
            float minDist = 0.0;
            
            //std::cout << "LUT Size = " << lut->size() << std::endl;
            
            for(unsigned int i = 0; i < lut->size(); ++i)
            {
                dist = (lut->at(i).elev - elevVal) * (lut->at(i).elev - elevVal);
                if(i == 0)
                {
                    minDist = dist;
                    elevLUTIdx = i;
                }
                else if(dist < minDist)
                {
                    minDist = dist;
                    elevLUTIdx = i;
                }
            }
            
            if(elevLUTIdx >= lut->size())
            {
                std::cerr << "elevVal = " << elevVal << std::endl;
                std::cerr << "elevLUTIdx = " << elevLUTIdx << std::endl;
                std::cerr << "lut->size() = " << lut->size() << std::endl;
                
                throw rsgis::img::RSGISImageCalcException("Elevation value is not within the LUT.");
            }
            
            elevLUTVal = lut->at(elevLUTIdx);
            
            LUT6SAOT aotLUTVal;
            unsigned int aotLUTIdx = 0;
            
            for(unsigned int i = 0; i < elevLUTVal.aotLUT.size(); ++i)
            {
                dist = (elevLUTVal.aotLUT.at(i).aot - aotVal) * (elevLUTVal.aotLUT.at(i).aot - aotVal);
                if(i == 0)
                {
                    minDist = dist;
                    aotLUTIdx = i;
                }
                else if(dist < minDist)
                {
                    minDist = dist;
                    aotLUTIdx = i;
                }
            }
                        
            if(aotLUTIdx >= elevLUTVal.aotLUT.size())
            {
                std::cerr << "aotVal = " << aotVal << std::endl;
                std::cerr << "aotLUTIdx = " << aotLUTIdx << std::endl;
                std::cerr << "elevLUTVal.aotLUT.size() = " << elevLUTVal.aotLUT.size() << std::endl;
                
                throw rsgis::img::RSGISImageCalcException("AOT value is not within the LUT.");
            }
            
            aotLUTVal = elevLUTVal.aotLUT.at(aotLUTIdx);
            
            for(unsigned int i = 0; i < aotLUTVal.numValues; ++i)
            {
                if(aotLUTVal.imageBands[i] > numBands)
                {
                    std::cout << "Image band: " << aotLUTVal.imageBands[i] << std::endl;
                    throw rsgis::img::RSGISImageCalcException("Image band is not within image.");
                }
                
                tmpVal=aotLUTVal.aX[i]*bandValues[aotLUTVal.imageBands[i]]-aotLUTVal.bX[i];
                output[i] = (tmpVal/(1.0+aotLUTVal.cX[i]*tmpVal))*this->scaleFactor;
                
                if(this->useNoDataVal & (this->noDataVal == 0.0))
                {
                    if(output[i] < 1)
                    {
                        output[i] = 1.0;
                    }
                    else
                    {
                        output[i] = output[i] + 1.0;
                    }
                }
            }
        }
        
    }
    
    RSGISApply6SCoefficientsElevAOTLUTParam::~RSGISApply6SCoefficientsElevAOTLUTParam()
    {
        
    }
    
    
    
    
    RSGISApply6SCoefficients::RSGISApply6SCoefficients(int numberOutBands, unsigned int *imageBands, float **aX, float **bX, float **cX, int numValues, float *elevationThresh, int numElevation, float scaleFactor):rsgis::img::RSGISCalcImageValue(numberOutBands)
    {
        this->useTopo6S = true;
        this->bandOffset = 1;
		this->imageBands = imageBands;
        this->aX = aX;
        this->bX = bX;
        this->cX = cX;
		this->elevationThresh = elevationThresh;
        this->numValues = numValues;
        this->scaleFactor = scaleFactor;
		this->numElevation = numElevation;
		if (numElevation == 0)
		{
			this->useTopo6S = false;
            this->bandOffset = 0;
		}
    }
    
    void RSGISApply6SCoefficients::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(numValues != this->numOutBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input image bands needs to be equal to the number of output image bands.");
        }
        
        if(numBands <= numValues)
        {
            throw rsgis::img::RSGISImageCalcException("The number of input values needs to be equal to or less than the number of input image bands.");
        }
                
        double tmpVal = 0;
		
		unsigned int elv = 0;
		
        if (bandValues[this->bandOffset] == 0) // If first band == 0, assume image border
        {
            for(unsigned int i = 0; i < this->numValues; ++i)
            {
                output[i] = 0;
            }
        }
        else
        {
            if(this->useTopo6S)
            {
                // Round to nearest 50 m
                double elevationScale = bandValues[0] / 100.0;
                elevationScale = int(elevationScale + 0.5);
                int elevationInt = elevationScale * 100;
                
                if (elevationInt < this->elevationThresh[0]) 
                {
                    elv = 0;
                }
                else 
                {
                    for (unsigned int d = 1; d < numElevation; ++d) 
                    {
                        if((elevationInt >= this->elevationThresh[d - 1]) && (elevationInt < this->elevationThresh[d]))
                        {
                            elv = d;
                        }
                    }
                }
            }
            
            for(unsigned int i = 0; i < this->numValues; ++i)
            {
                if(imageBands[i]+bandOffset > numBands)
                {
                    std::cout << "Image band: " << imageBands[i] << std::endl;
                    throw rsgis::img::RSGISImageCalcException("Image band is not within image.");
                }
                
                tmpVal=aX[i][elv]*bandValues[imageBands[i]+bandOffset]-bX[i][elv];
                output[i] = (tmpVal/(1.0+cX[i][elv]*tmpVal))*this->scaleFactor;
            }
        }

    }
        
    RSGISApply6SCoefficients::~RSGISApply6SCoefficients()
    {
        
    }

    
    
    RSGISCalcSolarIrradianceElevLUTParam::RSGISCalcSolarIrradianceElevLUTParam(unsigned int numOutBands, std::vector<LUT6SElevation> *lut, double *meanSREFVec, unsigned int numSREFBands, float solarZenith):rsgis::img::RSGISCalcImageValue(numOutBands)
    {
        this->lut = lut;
        this->meanSREFVec = meanSREFVec;
        this->numSREFBands = numSREFBands;
        this->solarZenith = solarZenith;
    }
    
    void RSGISCalcSolarIrradianceElevLUTParam::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        /*
         * bandValues[0] - valid data mask
         * bandValues[1] - DEM
         * bandValues[2] - Incidence Angle
         * bandValues[3] - Slope
         * bandValues[4] - Shadow Mask
         */
        if(bandValues[0] == 1)
        {
            const double degreesToRadians = M_PI / 180.0;
            rsgis::calib::LUT6SElevation tmpElevPt;
            bool first = true;
            float nrElevDist = 0.0;
            float dist = 0.0;
            for(std::vector<LUT6SElevation>::iterator iterLUT = lut->begin(); iterLUT != lut->end(); ++iterLUT)
            {
                dist = ((*iterLUT).elev - bandValues[1])*((*iterLUT).elev - bandValues[1]);
                if(first)
                {
                    tmpElevPt = (*iterLUT);
                    nrElevDist = dist;
                    first = false;
                }
                else if(dist < nrElevDist)
                {
                    tmpElevPt = (*iterLUT);
                    nrElevDist = dist;
                }
            }
            
            float shadMask = 1;
            if(bandValues[4] == 1)
            {
                shadMask = 0;
            }
            
            double incAngRad = bandValues[2] * degreesToRadians;
            double solarZenRad = solarZenith * degreesToRadians;
            double incAngRatio = 0.0;
            double slopeRad = bandValues[3] * degreesToRadians;
            double vD = (1 + cos(slopeRad))/2;
            double vT = (1 - cos(slopeRad))/2;
            double totalIrr = 0.0;
            
            int dirIrrIdx = 0;
            int difIrrIdx = 0;
            int envIrrIdx = 0;
            int totIrrIdx = 0;
            for(int n = 0; n < numSREFBands; ++n)
            {
                dirIrrIdx = n * 4;
                difIrrIdx = (n * 4)+1;
                envIrrIdx = (n * 4)+2;
                totIrrIdx = (n * 4)+3;
                
                // Direct
                incAngRatio = cos(incAngRad)/cos(solarZenRad);
                output[dirIrrIdx] = shadMask * tmpElevPt.directIrr[n] * incAngRatio;
                
                // Diffuse
                output[difIrrIdx] = tmpElevPt.diffuseIrr[n] * vD;
                
                // Environment
                totalIrr = tmpElevPt.directIrr[n] + tmpElevPt.diffuseIrr[n] + tmpElevPt.envIrr[n];
                output[envIrrIdx] = totalIrr * vT * meanSREFVec[n];
                
                // Total
                output[totIrrIdx] = output[dirIrrIdx] + output[difIrrIdx] + output[envIrrIdx];
            }
        }
        else
        {
            for(int n = 0; n < numOutBands; ++n)
            {
                output[n] = 0.0;
            }
        }
    }
    
    RSGISCalcSolarIrradianceElevLUTParam::~RSGISCalcSolarIrradianceElevLUTParam()
    {
        
    }
    
    
    
    
    
    
    
    
    RSGISCalcStandardisedReflectanceSD2010::RSGISCalcStandardisedReflectanceSD2010(unsigned int numOutBands, unsigned int numSREFBands, float brdfBeta, float outIncidenceAngle, float outExitanceAngle, float reflScaleFactor):rsgis::img::RSGISCalcImageValue(numOutBands)
    {
        this->numSREFBands = numSREFBands;
        this->brdfBeta = brdfBeta;
        this->outIncidenceAngle = outIncidenceAngle;
        this->outExitanceAngle = outExitanceAngle;
        this->reflScaleFactor = reflScaleFactor;
    }
    
    void RSGISCalcStandardisedReflectanceSD2010::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
    {
        /*
         * bandValues[0] - valid data mask
         * bandValues[1] - Incidence Angle
         * bandValues[2] - Excidance Angle
         * bandValues[XXX] - SREF
         * bandValues[XXX] - Irradiance (4 x SREF bands)
         *                 * Direct
         *                 * Diffuse
         *                 * Environment
         *                 * Total
         */
        
        if(numOutBands != numSREFBands)
        {
            throw rsgis::img::RSGISImageCalcException("The number of output bands must be equal to the number of SREF bands.");
        }
        
        if(bandValues[0] == 1)
        {
            const double degreesToRadians = M_PI / 180.0;
            this->outIncidenceAngle = this->outIncidenceAngle * degreesToRadians;
            this->outExitanceAngle = this->outExitanceAngle * degreesToRadians;
            
            double inIncAngleRad = bandValues[1] * degreesToRadians;
            double inExitAngleRad = bandValues[2] * degreesToRadians;
            
            double gamma = (cos(inIncAngleRad) + cos(inExitAngleRad)) / (cos(this->outIncidenceAngle) + cos(this->outExitanceAngle));
            
            
            unsigned int srefBandIdx = 0;
            unsigned int irrDirBandIdx = 0;
            unsigned int irrDifandIdx = 0;
            unsigned int irrTotalBandIdx = 0;
            double LVal = 0.0;
            for(unsigned int i = 0; i < this->numSREFBands; ++i)
            {
                srefBandIdx = 3 + i;
                irrDirBandIdx = (3 + this->numSREFBands) + (i*4);
                irrDifandIdx = (3 + this->numSREFBands) + (i*4) + 1;
                irrTotalBandIdx = (3 + this->numSREFBands) + (i*4) + 3;
                
                LVal = (bandValues[srefBandIdx] / this->reflScaleFactor) * bandValues[irrTotalBandIdx];
                
                output[i] = (LVal * M_PI) / ((bandValues[irrDirBandIdx] * gamma) + (bandValues[irrDifandIdx] * this->brdfBeta));
                
                output[i] = output[i] * reflScaleFactor;
                if(reflScaleFactor > 99)
                {
                    if(output[i] < 1)
                    {
                        output[i] = 1;
                    }
                }
            }
            
        }
        else
        {
            for(int n = 0; n < numOutBands; ++n)
            {
                output[n] = 0.0;
            }
        }
    }
    
    RSGISCalcStandardisedReflectanceSD2010::~RSGISCalcStandardisedReflectanceSD2010()
    {
        
    }

    
    
}}


