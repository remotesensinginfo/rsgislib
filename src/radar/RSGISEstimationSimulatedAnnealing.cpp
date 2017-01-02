/*
 *  RSGISEstimationSimulatedAnnealing.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 31/03/2010.
 *  Copyright 2010 RSGISLib.
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

#include "RSGISEstimationSimulatedAnnealing.h"


namespace rsgis {namespace radar{

    RSGISEstimationSimulatedAnnealingWithAP::RSGISEstimationSimulatedAnnealingWithAP(std::vector<rsgis::math::RSGISMathNVariableFunction*> *allFunctions,
                                                                           double **minMaxIntervalAll,
                                                                           double minEnergy,
                                                                           double startTemp,
                                                                           unsigned int runsStep,
                                                                           unsigned int runsTemp,
                                                                           double cooling,
                                                                           unsigned int maxItt,
                                                                           gsl_matrix *covMatrixP,
                                                                           gsl_matrix *invCovMatrixD,
                                                                           gsl_vector *aPrioriPar)
    {
        this->startTemp = startTemp;
        this->runsStep = runsStep; // Number of runs at each step size
        this->runsTemp = runsTemp; // Number of times step is changed at each temperature
        this->cooling = cooling; // Cooling factor
        this->minEnergy = minEnergy; // Set the target energy
        this->maxItt = maxItt; // Maximum number of itterations
        this->useAP = true;

        this->allFunctions = allFunctions;

        this->minMaxIntervalAll = minMaxIntervalAll; // minA, maxA, minStepSizeA

        this->nPar = allFunctions->at(0)->numVariables();
        this->nData = invCovMatrixD->size1;
        // Set up vector for input data.
        this->inputData = gsl_vector_alloc(this->nData);

        for(unsigned int i = 0; i < allFunctions->size(); ++i)
        {
            if(allFunctions->at(i)->numVariables() != this->nPar)
            {
                throw RSGISException("All functions must have the same number of variables");
            }
        }

        // Set up random number generator
        this->randgsl = gsl_rng_alloc (gsl_rng_taus2);
        double seed = time(0) + std::rand();
        gsl_rng_set (randgsl, seed);


        this->initialStepSize = new double[this->nPar];

        for(unsigned int j = 0; j < this->nPar; ++j)
        {
            this->initialStepSize[j] = (minMaxIntervalAll[j][1] - minMaxIntervalAll[j][0]) / 5.;
        }

        this->aPrioriPar = aPrioriPar;
        this->covMatrixP = covMatrixP;
        this->invCovMatrixD = invCovMatrixD;
        // Invert covariance matrix
        this->invCovMatrixP = gsl_matrix_alloc(nPar,nPar);
        gsl_matrix_set_zero(invCovMatrixP);

        for (unsigned int i = 0; i < this->nPar; i++)
        {
            // If all diagonal values of covarience matrix greater than 1e8, don't use covarience matrix
            if(gsl_matrix_get(this->covMatrixP, i, i) > 1e8){this->useAP = false;}
            else
            {
                gsl_matrix_set(this->invCovMatrixP, i, i, 1. / gsl_matrix_get(this->covMatrixP, i, i));
            }
        }
        if(this->useAP){std::cout << "\tUsing a priori values" << std::endl;}

        // Set up vectors for least squares calculation
        this->deltaD = gsl_vector_alloc(this->nData);
        this->deltaX = gsl_vector_alloc(this->nPar);
        this->tempD = gsl_vector_alloc(this->nData);
        this->tempX = gsl_vector_alloc(this->nPar);

    }
    int RSGISEstimationSimulatedAnnealingWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
    {
        if(inData->size != this->nData)
        {
            throw RSGISException("Data provided does not match number of data values initialised with!");
        }

        // Check for no-data
        for(unsigned int i = 0;i < this->nData; i++)
        {
            if (boost::math::isnan(gsl_vector_get(inData, i)))
            {
            	for(unsigned int j = 0; j < this->nPar; j++)
                {
            		gsl_vector_set(outParError, j, gsl_vector_get(initialPar, j));
                }
                gsl_vector_set(outParError, nPar, +std::numeric_limits<double>::infinity());
                return 0;
            }
            else
            {
                // Copy to class variable.
                gsl_vector_set(this->inputData, i, gsl_vector_get(inData,i));
            }
        }

        rsgis::math::RSGISVectors vectorUtils;
        rsgis::math::RSGISMatrices matrixUtils;

        unsigned int numItt = 0;
        double temp = startTemp;
        double stepRand = 0.0;
        double newEnergy = 0.0;
        double boltzmanProb = 0;
        double dataPow = 0;
        for(unsigned int d = 0; d < this->nData; ++d)
        {
            dataPow = dataPow + pow(gsl_vector_get(this->inputData, d),2);
        }

        std::vector<double> *currentParError = new std::vector<double>();
        std::vector<double> *bestParError = new std::vector<double>();
        std::vector<double> *testPar = new std::vector<double>();
        double *accepted = new double[this->nPar];
        double *stepSize = new double[this->nPar];

        // Set current and best parameters to inital values
        for (unsigned int j = 0; j < this->nPar; j++)
        {
            currentParError->push_back(gsl_vector_get(initialPar, j));
            testPar->push_back(gsl_vector_get(initialPar, j));
            bestParError->push_back(gsl_vector_get(initialPar, j));
            stepSize[j] = this->initialStepSize[j];
        }

        // Set initial energy;
        currentParError->push_back(99999);
        bestParError->push_back(99999);

        unsigned int tRuns = this->maxItt;// / (runsStep * runsTemp * 5);
        for(unsigned int t = 0; t < tRuns; ++t)
        {
        	// Decrease tempreature
            if(t == (tRuns - 1)) // Last Run
            {
                // Set temperature to zero
                temp = 0;
                for (unsigned int l = 0; l < this->nPar; l++)
                {
                    currentParError->at(l) = bestParError->at(l);
                }
            }
            else
            {
                // Reduce temperature by cooling factor
                temp = pow(this->cooling,double(t)) * this->startTemp;
                // Reset step size
                for (unsigned int j = 0; j < this->nPar; j++)
                {
                    stepSize[j] = initialStepSize[j];
                }
            }

            for(unsigned int n = 0; n < this->runsTemp; n++)
            {
                for (unsigned int j = 0; j < this->nPar; j++)
                {
                    // Reset number of accepted steps;
                    accepted[j] = 0;
                }
                for(unsigned int m = 0; m < this->runsStep; m++)
                {
                    bool withinLimits = true;
                    // Loop through parameters
                    for(unsigned int j = 0; j < this->nPar; j++)
                    {
                        withinLimits = true;
                        // Select new vector
                        stepRand = (gsl_rng_uniform(this->randgsl)*2.0) - 1.0;
                        testPar->at(j) = currentParError->at(j) + (stepRand * stepSize[j]);

                        if (testPar->at(j) < minMaxIntervalAll[j][0])
                        {
                            testPar->at(j) = currentParError->at(j);
                            withinLimits = false;
                        }
                        else if(testPar->at(j) > minMaxIntervalAll[j][1])
                        {
                            testPar->at(j) = currentParError->at(j);
                            withinLimits = false;
                        }

                    }
                    if (withinLimits)
                    {
                        double prevEnergy = currentParError->at(this->nPar);
                        // Calculate energy
                        newEnergy = this->calcLeastSquares(testPar);
                        double error = newEnergy;
                        if(newEnergy < currentParError->at(this->nPar)) // If new energy is lower accept
                        {

                            for(unsigned int j = 0; j < this->nPar; j++)
                            {
                                currentParError->at(j) = testPar->at(j);
                                accepted[j]++;
                            }

                            currentParError->at(this->nPar) = newEnergy;

                            if(currentParError->at(this->nPar) < bestParError->at(this->nPar)) // If new energy is less than best, update best.
                            {
                                for (unsigned int k = 0; k < this->nPar + 1; k++)
                                {
                                    bestParError->at(k) = currentParError->at(k);
                                }
                                bestParError->at(this->nPar) = currentParError->at(this->nPar);

                                if (bestParError->at(this->nPar) < this->minEnergy) // If new energy is less than threashold return.
                                {
                                    bestParError->at(this->nPar) = error;

                                    for (unsigned int k = 0; k < this->nPar + 1; k++)
                                    {
                                        gsl_vector_set(outParError, k, bestParError->at(k));
                                    }

                                    // Tidy
                                    delete currentParError;
                                    delete testPar;
                                    delete bestParError;
                                    delete[] accepted;
                                    delete[] stepSize;

                                    // Exit
                                    return 1;
                                }
                            }
                        }
                        else // If new energy is lower, accept based on Boltzman probability
                        {
                            boltzmanProb = exp((-1*(newEnergy - currentParError->at(this->nPar)))/temp);
                            if (boltzmanProb > gsl_rng_uniform(this->randgsl))
                            {
                                for(unsigned int j = 0; j < this->nPar; j++)
                                {
                                    currentParError->at(j) = testPar->at(j);
                                    accepted[j]++;
                                    currentParError->at(this->nPar) = newEnergy;
                                }
                            }
                        }
                        if(fabs(newEnergy - prevEnergy)  < 10e-10)
                        {
                            for(unsigned int j = 0; j < this->nPar; j++)
                            {
                                currentParError->at(j) = gsl_vector_get(initialPar, j);
                            }
                            temp = startTemp;
                        }
                    }

                    }
                    ++numItt;

                }

                // Change step size, uses formula of Corana et al.
                for(unsigned int j = 0; j < this->nPar; j++)
                {
                    double njDivNs = accepted[j] / runsStep;
                    double currentStepSize = stepSize[j];
                    if(njDivNs > 0.6)
                    {
                        stepSize[j] = currentStepSize*(1 + (2*((njDivNs - 0.6)/0.4)));
                    }
                    else if (njDivNs < 0.4)
                    {
                        stepSize[j] = currentStepSize*(1/ (1 + (2*((0.4- njDivNs)/0.4))));
                    }
                    if (stepSize[j] < minMaxIntervalAll[j][2])
                    {
                        stepSize[j] = minMaxIntervalAll[j][2];
                    }
                    if (stepSize[j] > initialStepSize[j] * 2.5)
                    {
                        stepSize[j] = initialStepSize[j];
                    }
                }
            }

        // Set output to best value
        for (unsigned int j = 0; j < this->nPar + 1; j++)
        {
			gsl_vector_set(outParError, j, bestParError->at(j));
        }

        // Tidy
        delete currentParError;
        delete testPar;
        delete bestParError;
        delete[] accepted;
        delete[] stepSize;

        // Exit
        return 0;

    }
    double RSGISEstimationSimulatedAnnealingWithAP::calcLeastSquares(std::vector<double> *values)
    {
        /** L(X) = 1/2 { || f(X) - d0 || ^2 + || X - Xap || ^2 } */

        rsgis::math::RSGISMatrices matrixUtils;
        rsgis::math::RSGISVectors vectorUtils;

        double dataDiff = 0;
        double valueDiff = 0;
        double diffX = 0;

        // || f(X) - d0 || ^2
        for(unsigned int i = 0; i < this->allFunctions->size(); ++i)
        {
            dataDiff = gsl_vector_get(this->inputData, i) - this->allFunctions->at(i)->calcFunction(values);
            gsl_vector_set(this->deltaD, i, dataDiff);
        }

        matrixUtils.productMatrixVectorGSL(this->invCovMatrixD, this->deltaD, this->tempD);

        double diffD = vectorUtils.dotProductVectorVectorGSL(this->tempD, this->deltaD);

        // || X - Xap || ^2
        if (this->useAP)
        {
            for(unsigned int i = 0; i < values->size(); ++i)
            {
                valueDiff = values->at(i) - gsl_vector_get(this->aPrioriPar, i);
                gsl_vector_set(this->deltaX, i, valueDiff);
            }
            matrixUtils.productMatrixVectorGSL(this->invCovMatrixP, this->deltaX, this->tempX);

            diffX = vectorUtils.dotProductVectorVectorGSL(this->tempX, this->deltaX);
        }
        return (diffD + diffX) / 2;

    }
    RSGISEstimationSimulatedAnnealingWithAP::~RSGISEstimationSimulatedAnnealingWithAP()
    {
        delete[] initialStepSize;
        gsl_rng_free(randgsl);
        gsl_matrix_free(invCovMatrixP);
        gsl_vector_free(this->deltaD);
        gsl_vector_free(this->deltaX);
        gsl_vector_free(this->tempD);
        gsl_vector_free(this->tempX);
        gsl_vector_free(this->inputData);
    }

}}



