/*
 *  RSGISImageWindowStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/07/2008.
 *  Copyright 2008 RSGISLib.
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

#include "RSGISImageWindowStats.h"


namespace rsgis{namespace img{
    
    RSGISCalcImgPxlNeighboursDist::RSGISCalcImgPxlNeighboursDist() : RSGISCalcImageValue(4)
    {
        stats = new rsgis::math::RSGISStatsSummary();
        distVals = new std::vector<double>();
        gsl_set_error_handler_off();
    }
    
    void RSGISCalcImgPxlNeighboursDist::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
    {
        try
        {
            unsigned int cPt = (winSize-1)/2;
            unsigned int numPxls = winSize * winSize;
            double dist = 0;
            distVals->clear();
            distVals->reserve(numPxls);
            
            rsgis::math::RSGISMatrices matrixUtils;
            
            rsgis::math::Matrix *covarianceMatrix = matrixUtils.createMatrix(numBands, numBands);
            matrixUtils.setValues(covarianceMatrix, 0.0);
            
            double tmpVal = 0;
            for(int n = 0; n < numBands; ++n)
            {
                for(int m = 0; m < numBands; ++m)
                {
                    for(unsigned int i = 0; i < winSize; ++i)
                    {
                        for(unsigned int j = 0; j < winSize; ++j)
                        {
                            if(!((i == cPt) & (j == cPt)))
                            {
                                tmpVal = (dataBlock[n][i][j]-dataBlock[n][cPt][cPt]) * (dataBlock[m][i][j]-dataBlock[m][cPt][cPt]);
                                if(tmpVal == 0)
                                {
                                    tmpVal = 0.001;
                                }
                                covarianceMatrix->matrix[(n*numBands)+m] += tmpVal;
                            }
                        }
                    }
                }
            }
            
            size_t numVals = covarianceMatrix->m;
            
            gsl_matrix *coVarGSL = matrixUtils.convertRSGIS2GSLMatrix(covarianceMatrix);
            
            gsl_matrix *invCovarianceMatrix = gsl_matrix_alloc(covarianceMatrix->m, covarianceMatrix->n);
            gsl_permutation *p = gsl_permutation_alloc(covarianceMatrix->m);
            int signum = 0;
            gsl_linalg_LU_decomp(coVarGSL, p, &signum);

            int status = gsl_linalg_LU_invert (coVarGSL, p, invCovarianceMatrix);
            if(status)
            {
                // Error from GSL...
                output[0] = 0;
                output[1] = 0;
                output[2] = 0;
                output[3] = 0;
            }
            else
            {
                gsl_vector *dVals = gsl_vector_alloc(numVals);
                gsl_vector *outVec = gsl_vector_alloc(numVals);
                
                
                for(unsigned int i = 0; i < winSize; ++i)
                {
                    for(unsigned int j = 0; j < winSize; ++j)
                    {
                        if(!((i == cPt) & (j == cPt)))
                        {
                            for(int n = 0; n < numBands; ++n)
                            {
                                gsl_vector_set(dVals, n, (dataBlock[n][i][j] - dataBlock[n][cPt][cPt]));
                            }
                            
                            gsl_blas_dgemv(CblasNoTrans, 1.0, invCovarianceMatrix, dVals, 0.0, outVec );
                            
                            dist = 0;
                            for(int n = 0; n < numBands; ++n)
                            {
                                dist += gsl_vector_get(dVals, n) * gsl_vector_get(outVec, n);
                            }
                            dist = sqrt(dist);
                            
                            distVals->push_back(dist);
                        }
                    }
                }
                
                gsl_vector_free(dVals);
                gsl_vector_free(outVec);
                
                rsgis::math::RSGISMathsUtils mathUtils;
                mathUtils.initStatsSummary(stats);
                stats->calcMax = true;
                stats->calcMin = true;
                stats->calcMean = true;
                stats->calcMedian = true;
                
                mathUtils.generateStats(distVals, stats);
                
                output[0] = stats->mean;
                output[1] = stats->median;
                output[2] = stats->min;
                output[3] = stats->max;
            }
            
            gsl_permutation_free(p);
            gsl_matrix_free(coVarGSL);
            gsl_matrix_free(invCovarianceMatrix);
            matrixUtils.freeMatrix(covarianceMatrix);
        }
        catch(RSGISException &e)
        {
            throw RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISImageCalcException(e.what());
        }
    }
    
    RSGISCalcImgPxlNeighboursDist::~RSGISCalcImgPxlNeighboursDist()
    {
        delete stats;
        delete distVals;
    }
    
    
    
    
    
    
    RSGISCalcImgPxl2WindowDist::RSGISCalcImgPxl2WindowDist(gsl_matrix *invCovarianceMatrix, rsgis::math::Vector *varMeans, gsl_vector *dVals, gsl_vector *outVec) : RSGISCalcImageValue(4)
    {
        stats = new rsgis::math::RSGISStatsSummary();
        distVals = new std::vector<double>();
        this->invCovarianceMatrix = invCovarianceMatrix;
        this->varMeans = varMeans;
        this->dVals = dVals;
        this->outVec = outVec;

    }
    
    void RSGISCalcImgPxl2WindowDist::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
    {
        unsigned int numPxls = winSize * winSize;
        double dist = 0;
        distVals->clear();
        distVals->reserve(numPxls);
        
        for(unsigned int i = 0; i < winSize; ++i)
        {
            for(unsigned int j = 0; j < winSize; ++j)
            {
                for(int n = 0; n < numBands; ++n)
                {
                    gsl_vector_set(dVals, n, (dataBlock[n][i][j] - varMeans->vector[n]));
                }
                
                gsl_blas_dgemv(CblasNoTrans, 1.0, invCovarianceMatrix, dVals, 0.0, outVec );
                
                dist = 0;
                for(int n = 0; n < numBands; ++n)
                {
                    dist += gsl_vector_get(dVals, n) * gsl_vector_get(outVec, n);
                }
                dist = sqrt(dist);
                
                distVals->push_back(dist);
            }
        }
        
        rsgis::math::RSGISMathsUtils mathUtils;
        mathUtils.initStatsSummary(stats);
        stats->calcMax = true;
        stats->calcMin = true;
        stats->calcMean = true;
        stats->calcMedian = true;
        
        mathUtils.generateStats(distVals, stats);
        
        output[0] = stats->mean;
        output[1] = stats->median;
        output[2] = stats->min;
        output[3] = stats->max;
    }

    
    RSGISCalcImgPxl2WindowDist::~RSGISCalcImgPxl2WindowDist()
    {
        delete stats;
        delete distVals;
    }
    
    RSGISCalcImage2ImageCorrelation::RSGISCalcImage2ImageCorrelation(unsigned int bandA, unsigned int bandB) : RSGISCalcImageValue(1)
    {
        this->bandA = bandA;
        this->bandB = bandB;
    }
    
    void RSGISCalcImage2ImageCorrelation::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
    {
        
        if( (this->bandA > numBands ) | (this->bandB > numBands) )
        {
            throw rsgis::img::RSGISImageCalcException("Requested band not in image");
        }
        
        double sumRF = 0;
		double sumR = 0;
		double sumF = 0;
		double sumRSq = 0;
		double sumFSq = 0;
        unsigned int nPixels = 0; // Tally of number of pixels
        float blockCorrelation = 0;
        
        for(unsigned int i = 0; i < winSize; ++i)
        {
            for(unsigned int j = 0; j < winSize; ++j)
            {

				if( ( (boost::math::isfinite)(dataBlock[bandA][i][j]) ) & ( (boost::math::isfinite)(dataBlock[bandB][i][j]) ) )
				{
					sumRF += (dataBlock[bandA][i][j] * dataBlock[bandB][i][j]);
					sumR += dataBlock[bandA][i][j];
					sumF += dataBlock[bandB][i][j];
					sumRSq += (dataBlock[bandA][i][j] * dataBlock[bandA][i][j]);
					sumFSq += (dataBlock[bandB][i][j] * dataBlock[bandB][i][j]);
                    nPixels+=1;
				}
            }
        }

        if(nPixels > 1)
        {
            blockCorrelation = (((nPixels * sumRF) - (sumR * sumF))/sqrt(((nPixels*sumRSq)-(sumR*sumR))*((nPixels*sumFSq)-(sumF*sumF))));
        }
        if( !(boost::math::isfinite)(blockCorrelation)){blockCorrelation = 0;}
        output[0] = blockCorrelation;

    }
    
    
    RSGISCalcImage2ImageCorrelation::~RSGISCalcImage2ImageCorrelation(){}
	
}}

