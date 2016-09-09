/*
 *  RSGISDistMetrics.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/03/2015.
 *  Copyright 2015 RSGISLib.
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


#include "RSGISDistMetrics.h"


namespace rsgis{namespace math{
    
    

    RSGISCalcEuclideanDistMetric::RSGISCalcEuclideanDistMetric(): RSGISCalcDistMetric()
    {
        
    }
    
    void RSGISCalcEuclideanDistMetric::init() throw(RSGISMathException)
    {
        this->initalised = true;
    }
    
    double RSGISCalcEuclideanDistMetric::calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2) throw(RSGISMathException)
    {
        double dist = std::numeric_limits<double>::signaling_NaN();
        try
        {
            if(!this->initalised)
            {
                throw RSGISMathException("The metric calulator was not initialised.");
            }
            if((eIdx1-sIdx1) != (eIdx2-sIdx2))
            {
                throw RSGISMathException("The length of the two arrays must be the same for the distance to be calculated.");
            }
            size_t numVals = eIdx1-sIdx1;

            double diff = 0;
            dist = 0;
            for(size_t i = 0; i < numVals; ++i)
            {
                diff = vals1[i+sIdx1] - vals2[i+sIdx2];
                diff = diff * diff;
                dist += diff;
            }

            dist = sqrt(dist/numVals);
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        return dist;
    }
    
    RSGISCalcEuclideanDistMetric::~RSGISCalcEuclideanDistMetric()
    {
        
    }

    
    
    RSGISCalcManhattenDistMetric::RSGISCalcManhattenDistMetric(): RSGISCalcDistMetric()
    {
        
    }
    
    void RSGISCalcManhattenDistMetric::init() throw(RSGISMathException)
    {
        this->initalised = true;
    }
    
    double RSGISCalcManhattenDistMetric::calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2) throw(RSGISMathException)
    {
        double dist = std::numeric_limits<double>::signaling_NaN();
        try
        {
            if(!this->initalised)
            {
                throw RSGISMathException("The metric calulator was not initialised.");
            }
            if((eIdx1-sIdx1) != (eIdx2-sIdx2))
            {
                throw RSGISMathException("The length of the two arrays must be the same for the distance to be calculated.");
            }
            size_t numVals = eIdx1-sIdx1;
            double diff = 0;
            dist = 0;
            for(size_t i = 0; i < numVals; ++i)
            {
                diff = vals1[i+sIdx1] - vals2[i+sIdx2];
                diff = fabs(diff);
                dist += diff;
            }
            
            dist = sqrt(dist/numVals);
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        return dist;
    }
    
    RSGISCalcManhattenDistMetric::~RSGISCalcManhattenDistMetric()
    {
        
    }
    
    
    RSGISCalcMahalanobisDistMetric::RSGISCalcMahalanobisDistMetric(double **covarMatrix, size_t n): RSGISCalcDistMetric()
    {
        this->covarMatrix = covarMatrix;
        this->n = n;
    }
    
    void RSGISCalcMahalanobisDistMetric::init() throw(RSGISMathException)
    {
        this->invCovarianceMatrix = gsl_matrix_alloc(n, n);
        gsl_matrix *coVarGSL = gsl_matrix_alloc(n, n);
        // Copy covariance matrix into GSL Matrix
        size_t idx = 0;
        for(size_t i = 0; i < n; ++i)
        {
            for(size_t j = 0; j < n; ++j)
            {
                coVarGSL->data[idx++] = this->covarMatrix[i][j];
            }
        }
        
        gsl_permutation *p = gsl_permutation_alloc(n);
        int signum = 0;
        gsl_linalg_LU_decomp(coVarGSL, p, &signum);
        gsl_linalg_LU_invert (coVarGSL, p, this->invCovarianceMatrix);
        gsl_permutation_free(p);
        gsl_matrix_free(coVarGSL);
        
        /*
        rsgis::math::RSGISMatrices matrixUtils;
        std::cout << "Inverse Covariance Matrix:\n";
        matrixUtils.printGSLMatrix(invCovarianceMatrix);
        */
        this->diffVals = gsl_vector_alloc(this->n);
        this->tmpVec = gsl_vector_alloc(this->n);
        
        this->initalised = true;
    }
    
    double RSGISCalcMahalanobisDistMetric::calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2) throw(RSGISMathException)
    {
        double dist = std::numeric_limits<double>::signaling_NaN();
        try
        {
            if(!this->initalised)
            {
                throw RSGISMathException("The metric calulator was not initialised.");
            }
            if(((eIdx1-sIdx1) != (eIdx2-sIdx2)) | ((eIdx1-sIdx1) != n))
            {
                throw RSGISMathException("The length of the two arrays and covariance matrix dimensions must be the same for the distance to be calculated.");
            }
            
            for(size_t i = sIdx1, j = sIdx2, k = 0; k < n; ++i, ++j, ++k)
            {
                gsl_vector_set(this->diffVals, k, (vals1[i] - vals2[j]));
            }
            
            gsl_blas_dgemv(CblasNoTrans, 1.0, this->invCovarianceMatrix, this->diffVals, 0.0, this->tmpVec);
            
            dist = 0;
            for(int i = 0; i < n; ++i)
            {
                dist += gsl_vector_get(this->diffVals, i) * gsl_vector_get(this->tmpVec, i);
            }
            dist = sqrt(dist);
            //std::cout << "MAH DIST = " << dist << std::endl;
            
            //throw RSGISMathException("RSGISCalcMahalanobisDistMetric is not implemented!");
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        return dist;
    }
    
    RSGISCalcMahalanobisDistMetric::~RSGISCalcMahalanobisDistMetric()
    {
        for(size_t i = 0; i < n; ++i)
        {
            delete[] this->covarMatrix[i];
        }
        delete[] this->covarMatrix;
        gsl_matrix_free(this->invCovarianceMatrix);
        gsl_vector_free(this->diffVals);
        gsl_vector_free(this->tmpVec);
    }
    
    
    
    
    RSGISCalcMinkowskiDistMetric::RSGISCalcMinkowskiDistMetric(): RSGISCalcDistMetric()
    {
        
    }
    
    void RSGISCalcMinkowskiDistMetric::init() throw(RSGISMathException)
    {
        this->initalised = true;
    }
    
    double RSGISCalcMinkowskiDistMetric::calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2) throw(RSGISMathException)
    {
        double dist = std::numeric_limits<double>::signaling_NaN();
        try
        {
            if(!this->initalised)
            {
                throw RSGISMathException("The metric calulator was not initialised.");
            }
            if((eIdx1-sIdx1) != (eIdx2-sIdx2))
            {
                throw RSGISMathException("The length of the two arrays must be the same for the distance to be calculated.");
            }
            
            throw RSGISMathException("RSGISCalcMinkowskiDistMetric is not implemented!");
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        return dist;
    }
    
    RSGISCalcMinkowskiDistMetric::~RSGISCalcMinkowskiDistMetric()
    {
        
    }
    
    
    
    
    
    RSGISCalcChebyshevDistMetric::RSGISCalcChebyshevDistMetric(): RSGISCalcDistMetric()
    {
        
    }
    
    void RSGISCalcChebyshevDistMetric::init() throw(RSGISMathException)
    {
        this->initalised = true;
    }
    
    double RSGISCalcChebyshevDistMetric::calcDist(double *vals1, size_t sIdx1, size_t eIdx1, double *vals2, size_t sIdx2, size_t eIdx2) throw(RSGISMathException)
    {
        double dist = std::numeric_limits<double>::signaling_NaN();
        try
        {
            if(!this->initalised)
            {
                throw RSGISMathException("The metric calulator was not initialised.");
            }
            if((eIdx1-sIdx1) != (eIdx2-sIdx2))
            {
                throw RSGISMathException("The length of the two arrays must be the same for the distance to be calculated.");
            }
            
            throw RSGISMathException("RSGISCalcChebyshevDistMetric is not implemented!");
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        return dist;
    }
    
    RSGISCalcChebyshevDistMetric::~RSGISCalcChebyshevDistMetric()
    {
        
    }
    
    
}}

