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

    RSGISCalcLinearSpectralUnmixing::RSGISCalcLinearSpectralUnmixing()
    {
        
    }
    
    void RSGISCalcLinearSpectralUnmixing::performUnconstainedLinearSpectralUnmixing(GDALDataset **datasets, int numDatasets, string outputImage, string endmembersFilePath)throw(RSGISImageCalcException)
    {
        try
        {
            unsigned int numOfImageBands = 0;
            for(int i = 0; i < numDatasets; ++i)
            {
                numOfImageBands += datasets[i]->GetRasterCount();
            }            
            
            RSGISMatrices matrixUtils;
            gsl_matrix *endmembersRaw = matrixUtils.readGSLMatrixFromTxt(endmembersFilePath);
            matrixUtils.printGSLMatrix(endmembersRaw);
            cout << endl;
            
            if(endmembersRaw->size1 != numOfImageBands)
            {
                throw RSGISImageCalcException("The number of image bands and wavelengths within the endmemebers should match.");
            }
            
            if(endmembersRaw->size2 >= endmembersRaw->size1)
            {
                gsl_matrix_free(endmembersRaw);
                throw RSGISImageCalcException("The number of endmember samples should be less than the number of input image bands.");
            }
            
            gsl_matrix *endmembersNorm = matrixUtils.normalisedColumnsMatrix(endmembersRaw);
            matrixUtils.printGSLMatrix(endmembersNorm);
            cout << endl;
            
            gsl_matrix *endmembersNormTrans = gsl_matrix_alloc (endmembersNorm->size2, endmembersNorm->size1);
            matrixUtils.transposeNonSquareGSL(endmembersNorm, endmembersNormTrans);
            matrixUtils.printGSLMatrix(endmembersNormTrans);
            cout << endl;
            
            gsl_matrix *q = gsl_matrix_alloc (endmembersNorm->size2, endmembersNorm->size2);
            gsl_matrix_set_zero(q);
            gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, endmembersNormTrans, endmembersNorm, 0.0, q);
            matrixUtils.printGSLMatrix(q);
            cout << endl;
            
            gsl_matrix *qInv = gsl_matrix_alloc (q->size1, q->size2);
            gsl_permutation *permutation = gsl_permutation_alloc(q->size1);
            int s;
            gsl_linalg_LU_decomp (q, permutation, &s);
            gsl_linalg_LU_invert(q, permutation, qInv);
            matrixUtils.printGSLMatrix(qInv);
            cout << endl;
            
            gsl_matrix *r = gsl_matrix_alloc (endmembersRaw->size2, endmembersRaw->size1);
            gsl_matrix_set_zero(r);
            gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, qInv, endmembersNormTrans, 0.0, r);
            matrixUtils.printGSLMatrix(r);
            cout << endl;
            
            RSGISUnconstrainedLinearSpectralUnmixing *calcUnconstrained = new RSGISUnconstrainedLinearSpectralUnmixing(endmembersRaw->size2, r);
            RSGISCalcImage calcImage(calcUnconstrained);
            calcImage.calcImage(datasets, numDatasets, outputImage);
            
            delete calcUnconstrained;
            gsl_matrix_free(endmembersRaw);
            gsl_matrix_free(endmembersNorm);
            gsl_matrix_free(endmembersNormTrans);
            gsl_matrix_free(q);
            gsl_matrix_free(qInv);
            gsl_permutation_free(permutation);
            gsl_matrix_free(r);
        }
        catch(RSGISException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
    }
    
    void RSGISCalcLinearSpectralUnmixing::performExhaustiveConstrainedSpectralUnmixing(GDALDataset **datasets, int numDatasets, string outputImage, string endmembersFilePath, float stepResolution)throw(RSGISImageCalcException)
    {
        try
        {
            unsigned int numOfImageBands = 0;
            for(int i = 0; i < numDatasets; ++i)
            {
                numOfImageBands += datasets[i]->GetRasterCount();
            }            
            
            RSGISMatrices matrixUtils;
            gsl_matrix *endmembersRaw = matrixUtils.readGSLMatrixFromTxt(endmembersFilePath);
            matrixUtils.printGSLMatrix(endmembersRaw);
            cout << endl;
            
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
            cout << endl;
            
            RSGISExhaustiveLinearSpectralUnmixing *calcExhaustive = new RSGISExhaustiveLinearSpectralUnmixing(endmembersNorm->size2+1, endmembersNorm, stepResolution);
            RSGISCalcImage calcImage(calcExhaustive);
            calcImage.calcImage(datasets, numDatasets, outputImage);
            
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
    
    
    RSGISUnconstrainedLinearSpectralUnmixing::RSGISUnconstrainedLinearSpectralUnmixing(int numberOutBands, gsl_matrix *r):RSGISCalcImageValue(numberOutBands)
    {
        this->r = r;
    }
        
    void RSGISUnconstrainedLinearSpectralUnmixing::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
    {
        RSGISMatrices matrixUtils;
        gsl_vector *s = gsl_vector_alloc(numBands);
        for(int i = 0; i < numBands; ++i)
        {
            gsl_vector_set(s, i, bandValues[i]);
        }
        
        gsl_vector *a = gsl_vector_alloc(r->size2);
        matrixUtils.productMatrixVectorGSL(r, s, a);
        
        double sqSum = 0;
        for(unsigned int i = 0; i < a->size; ++i)
        {
            sqSum += (gsl_vector_get(a, i)*gsl_vector_get(a, i));
        }
        
        double normVal = sqrt(sqSum);
        
        for(unsigned int i = 0; i < a->size; ++i)
        {
            output[i] = gsl_vector_get(a, i)/normVal;
        }
        
        gsl_vector_free(s);
        gsl_vector_free(a);
    }
    
    RSGISUnconstrainedLinearSpectralUnmixing::~RSGISUnconstrainedLinearSpectralUnmixing()
    {
        
    }
    
    
    RSGISExhaustiveLinearSpectralUnmixing::RSGISExhaustiveLinearSpectralUnmixing(int numberOutBands, gsl_matrix *endmembers, float stepRes):RSGISCalcImageValue(numberOutBands)
    {
        this->endmembers = endmembers;
        this->stepRes = stepRes;
        this->numOfEndMembers = endmembers->size2;
    }
    
    void RSGISExhaustiveLinearSpectralUnmixing::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
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
                            //cout << "[" << em1Val << "," << em2Val << "] = " << (em1Val+em2Val) << " Deviation = " << distVal << endl;
                            
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
                //cout << endl;
            }
            
            if(!first)
            {
                //cout << "Min Error = " << minError << endl;
                output[0] = minEM1Val;
                output[1] = minEM2Val;
                output[2] = minError;
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
                                //cout << "[" << em1Val << "," << em2Val << "," << em3Val << "] = " << (em1Val+em2Val+em3Val) << " Deviation = " << distVal << endl;
                                
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
                //cout << endl;
            }
            
            if(!first)
            {
                //cout << "Min Error = " << minError << endl;
                output[0] = minEM1Val;
                output[1] = minEM2Val;
                output[2] = minEM3Val;
                output[3] = minError;
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
    
    float RSGISExhaustiveLinearSpectralUnmixing::calcDistance2MeasuredSpectra(float em1Val, float em2Val, float em3Val, float *normSpectra, unsigned int numBands) throw(RSGISImageCalcException)
    {
        float *genSpectra = new float[numBands];
        for(unsigned int i = 0; i < numBands; ++i)
        {
            genSpectra[i] = (gsl_matrix_get(endmembers, i, 0) * em1Val) + (gsl_matrix_get(endmembers, i, 1) * em2Val) + (gsl_matrix_get(endmembers, i, 2) * em3Val);
            //cout << i << ": " <<  genSpectra[i] << endl;
        }
        
        float errorVal = 0;
        for(unsigned int i = 0; i < numBands; ++i)
        {
            errorVal += ((genSpectra[i] - normSpectra[i])*(genSpectra[i] - normSpectra[i]));
        }
        
        //cout << "errorVal = " << errorVal << endl;
        
        delete[] genSpectra;
        
        errorVal = sqrt(errorVal/numBands);
        
        return errorVal;
    }
    
    float RSGISExhaustiveLinearSpectralUnmixing::calcDistance2MeasuredSpectra(float em1Val, float em2Val, float *normSpectra, unsigned int numBands) throw(RSGISImageCalcException)
    {
        float *genSpectra = new float[numBands];
        for(unsigned int i = 0; i < numBands; ++i)
        {
            genSpectra[i] = (gsl_matrix_get(endmembers, i, 0) * em1Val) + (gsl_matrix_get(endmembers, i, 1) * em2Val);
            //cout << i << ": " <<  genSpectra[i] << endl;
        }
        
        float errorVal = 0;
        for(unsigned int i = 0; i < numBands; ++i)
        {
            errorVal += ((genSpectra[i] - normSpectra[i])*(genSpectra[i] - normSpectra[i]));
        }
        
        //cout << "errorVal = " << errorVal << endl;
        
        delete[] genSpectra;
        
        errorVal = sqrt(errorVal/numBands);
        
        return errorVal;
    }
    
    RSGISExhaustiveLinearSpectralUnmixing::~RSGISExhaustiveLinearSpectralUnmixing()
    {
        
    }
    
    
}}
