/*
 *  RSGISLinearSpectralUnmixing.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/02/2012.
 *  Copyright 2012 RSGISLib.
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

#include "RSGISLinearSpectralUnmixing.h"


namespace rsgis{namespace img{

    RSGISCalcLinearSpectralUnmixing::RSGISCalcLinearSpectralUnmixing(std::string gdalFormat, GDALDataType gdalDataType, float gain, float offset)
    {
        this->gdalFormat = gdalFormat;
        this->gdalDataType = gdalDataType;
        this->gain = gain;
        this->offset = offset;
    }

    void RSGISCalcLinearSpectralUnmixing::performExhaustiveConstrainedSpectralUnmixing(GDALDataset **datasets, int numDatasets, std::string outputImage, std::string endmembersFilePath, float stepResolution)
    {
        try
        {
            unsigned int numOfImageBands = 0;
            for(int i = 0; i < numDatasets; ++i)
            {
                numOfImageBands += datasets[i]->GetRasterCount();
            }            
            
            rsgis::math::RSGISMatrices matrixUtils;
            gsl_matrix *endmembersRaw = matrixUtils.readGSLMatrixFromTxt(endmembersFilePath);
            matrixUtils.printGSLMatrix(endmembersRaw);
            std::cout << std::endl;
            
            if(endmembersRaw->size1 != numOfImageBands)
            {
                throw RSGISImageCalcException("The number of image bands and wavelengths within the endmemebers should match.");
            }
            
            if(endmembersRaw->size2 >= endmembersRaw->size1)
            {
                gsl_matrix_free(endmembersRaw);
                throw RSGISImageCalcException("The number of endmember samples should be less than the number of input image bands.");
            }
            
            if((endmembersRaw->size2 != 3) & (endmembersRaw->size2 != 2))
            {
                throw RSGISImageCalcException("Unmixing is only implemented for 2 or 3 endmembers.");
            }
            
            gsl_matrix *endmembersNorm = matrixUtils.normalisedColumnsMatrix(endmembersRaw);
            matrixUtils.printGSLMatrix(endmembersNorm);
            std::cout << std::endl;
            
            RSGISExhaustiveLinearSpectralUnmixing *calcExhaustive = new RSGISExhaustiveLinearSpectralUnmixing(endmembersNorm->size2+1, endmembersNorm, stepResolution, this->gain, this->offset);
            RSGISCalcImage calcImage(calcExhaustive);
            calcImage.calcImage(datasets, numDatasets, outputImage, false, NULL, gdalFormat, gdalDataType);
            
            delete calcExhaustive;
            gsl_matrix_free(endmembersRaw);
            gsl_matrix_free(endmembersNorm);
        }
        catch(RSGISException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
    }
    
    RSGISCalcLinearSpectralUnmixing::~RSGISCalcLinearSpectralUnmixing()
    {
        
    }
    
    RSGISExhaustiveLinearSpectralUnmixing::RSGISExhaustiveLinearSpectralUnmixing(int numberOutBands, gsl_matrix *endmembers, float stepRes, float gain, float offset):RSGISCalcImageValue(numberOutBands)
    {
        this->endmembers = endmembers;
        this->stepRes = stepRes;
        this->numOfEndMembers = endmembers->size2;
        this->gain = gain;
        this->offset = offset;
    }
    
    void RSGISExhaustiveLinearSpectralUnmixing::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        // All values have to be greater than zero.
        
        // Find the linear combination which is closest to 1 but less than 1. 
        
        // Also find the linear combination which is closest to 1 but greater than 1.
        
        // Provide option to except greater than 1 if closer result than less than 1 and under some threshold.
        
        unsigned int numOfSteps = (1/this->stepRes)+1;
        
        float threshold = 1 + this->stepRes;
        
        float *normBandVals = new float[numBands];
        
        double sqSum = 0;
        for(int i = 0; i < numBands; ++i)
        {
            sqSum += (bandValues[i]*bandValues[i]);
        }
        
        float normVal = sqrt(sqSum);
        
        if(this->numOfEndMembers == 2)
        {
            float em1Val = 0;
            float em2Val = 0;
            
            bool first = true;
            float distVal = 0;
            float minError = 0;
            float minEM1Val = 0;
            float minEM2Val = 0;
            
            if( normVal > 0)
            {
                for(int i = 0; i < numBands; ++i)
                {
                    normBandVals[i] = bandValues[i]/normVal;
                }
                
                for(unsigned int i = 0; i < numOfSteps; ++i)
                {
                    em2Val = 0;
                    for(unsigned int j = 0; j < numOfSteps; ++j)
                    {
                        if((em1Val+em2Val) < threshold)
                        {
                            distVal = this->calcDistance2MeasuredSpectra(em1Val, em2Val, normBandVals, numBands);
                            
                            if(first)
                            {
                                minError = distVal;
                                minEM1Val = em1Val;
                                minEM2Val = em2Val;
                                first = false;
                            }
                            else if(distVal < minError)
                            {
                                minError = distVal;
                                minEM1Val = em1Val;
                                minEM2Val = em2Val;
                            }
                        }
                        em2Val += this->stepRes;
                    }
                    em1Val += this->stepRes;
                }
            }
            
            if(!first)
            {
                output[0] = offset + (minEM1Val * gain);
                output[1] = offset + (minEM2Val * gain);
                output[2] = offset + (minError * gain);
            }
            else
            {
                output[0] = 0;
                output[1] = 0;
                output[2] = 0;
            }
        }
        else if(this->numOfEndMembers == 3)
        {
            float em1Val = 0;
            float em2Val = 0;
            float em3Val = 0;
            
            bool first = true;
            float distVal = 0;
            float minError = 0;
            float minEM1Val = 0;
            float minEM2Val = 0;
            float minEM3Val = 0;
                        
            if( normVal > 0)
            {
                for(int i = 0; i < numBands; ++i)
                {
                    normBandVals[i] = bandValues[i]/normVal;
                }
                
                for(unsigned int i = 0; i < numOfSteps; ++i)
                {
                    em2Val = 0;
                    for(unsigned int j = 0; j < numOfSteps; ++j)
                    {
                        em3Val = 0;
                        for(unsigned int k = 0; k < numOfSteps; ++k)
                        {
                            if((em1Val+em2Val+em3Val) < threshold)
                            {
                                distVal = this->calcDistance2MeasuredSpectra(em1Val, em2Val, em3Val, normBandVals, numBands);
                                
                                if(first)
                                {
                                    minError = distVal;
                                    minEM1Val = em1Val;
                                    minEM2Val = em2Val;
                                    minEM3Val = em3Val;
                                    first = false;
                                }
                                else if(distVal < minError)
                                {
                                    minError = distVal;
                                    minEM1Val = em1Val;
                                    minEM2Val = em2Val;
                                    minEM3Val = em3Val; 
                                }
                            }
                            em3Val += this->stepRes;
                        }
                        em2Val += this->stepRes;
                    }
                    em1Val += this->stepRes;
                }
            }
            
            if(!first)
            {
                output[0] = offset + (minEM1Val * gain);
                output[1] = offset + (minEM2Val * gain);
                output[2] = offset + (minEM3Val * gain);
                output[3] = offset + (minError * gain);
            }
            else
            {
                output[0] = 0;
                output[1] = 0;
                output[2] = 0;
                output[3] = 0;
            }
        }
        else
        {
            throw RSGISImageCalcException("Unmixing is only implemented for 3 endmembers.");
        }
                
        delete[] normBandVals;
    }
    
    float RSGISExhaustiveLinearSpectralUnmixing::calcDistance2MeasuredSpectra(float em1Val, float em2Val, float em3Val, float *normSpectra, unsigned int numBands) 
    {
        float *genSpectra = new float[numBands];
        for(unsigned int i = 0; i < numBands; ++i)
        {
            genSpectra[i] = (gsl_matrix_get(endmembers, i, 0) * em1Val) + (gsl_matrix_get(endmembers, i, 1) * em2Val) + (gsl_matrix_get(endmembers, i, 2) * em3Val);
        }
        
        float errorVal = 0;
        for(unsigned int i = 0; i < numBands; ++i)
        {
            errorVal += ((genSpectra[i] - normSpectra[i])*(genSpectra[i] - normSpectra[i]));
        }
        
        delete[] genSpectra;
        
        errorVal = sqrt(errorVal/numBands);
        
        return errorVal;
    }
    
    float RSGISExhaustiveLinearSpectralUnmixing::calcDistance2MeasuredSpectra(float em1Val, float em2Val, float *normSpectra, unsigned int numBands) 
    {
        float *genSpectra = new float[numBands];
        for(unsigned int i = 0; i < numBands; ++i)
        {
            genSpectra[i] = (gsl_matrix_get(endmembers, i, 0) * em1Val) + (gsl_matrix_get(endmembers, i, 1) * em2Val);
        }
        
        float errorVal = 0;
        for(unsigned int i = 0; i < numBands; ++i)
        {
            errorVal += ((genSpectra[i] - normSpectra[i])*(genSpectra[i] - normSpectra[i]));
        }
                
        delete[] genSpectra;
        
        errorVal = sqrt(errorVal/numBands);
        
        return errorVal;
    }
    
    RSGISExhaustiveLinearSpectralUnmixing::~RSGISExhaustiveLinearSpectralUnmixing()
    {
        
    }
    
    
}}
