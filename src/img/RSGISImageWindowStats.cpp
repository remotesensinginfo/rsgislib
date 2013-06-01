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
    
    RSGISCalcImgPxlNeighboursDist::RSGISCalcImgPxlNeighboursDist(gsl_matrix *invCovarianceMatrix, rsgis::math::Vector *varMeans, gsl_vector *dVals, gsl_vector *outVec) : RSGISCalcImageValue(4)
    {
        stats = new rsgis::math::RSGISStatsSummary();
        distVals = new std::vector<double>();
        this->invCovarianceMatrix = invCovarianceMatrix;
        this->varMeans = varMeans;
        this->dVals = dVals;
        this->outVec = outVec;
    }
    
    void RSGISCalcImgPxlNeighboursDist::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
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
                //std::cout << "\nVector D:\n";
                //vecUtils.printGSLVector(dVals);
                
                gsl_blas_dgemv(CblasNoTrans, 1.0, invCovarianceMatrix, dVals, 0.0, outVec );
                
                //std::cout << "Mah Out Vec:\n";
                //vecUtils.printGSLVector(outVec);
                
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
    
    RSGISCalcImgPxlNeighboursDist::~RSGISCalcImgPxlNeighboursDist()
    {
        delete stats;
        delete distVals;
    }
    
	
}}

