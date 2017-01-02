/*
 *  RSGISEstimationThresholdAccepting.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 02/06/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISEstimationThresholdAccepting.h"

namespace rsgis {namespace radar{
	
	RSGISEstimationThresholdAccepting2Var2Data::RSGISEstimationThresholdAccepting2Var2Data(
																							 rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
																							 rsgis::math::RSGISMathTwoVariableFunction *functionHV,
																							 double *minMaxIntervalA,
																							 double *minMaxIntervalB,
																							 double minEnergy,
																							 double startThreshold,
																							 unsigned int runsStep,
																							 unsigned int runsThreshold,
																							 double cooling,
																							 unsigned int maxItt)
	{		
		this->startThreshold = startThreshold;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsThreshold = runsThreshold; // Number of times step is changed at each threasherature
		this->cooling = cooling; // Cooling factor
		this->minEnergy = minEnergy; // Set the target energy
		this->maxItt = maxItt; // Maximum number of itterations
		
		this->functionHH = functionHH;
		this->functionHV = functionHV;
		
		this->minMaxIntervalA = minMaxIntervalA; // minA, maxA, minStepSizeA
		this->minMaxIntervalB = minMaxIntervalB; // minB, maxB, minStepSizeB
		
		this->nPar = 2;
		
		this->randgsl = gsl_rng_alloc (gsl_rng_taus2);
		double seed = time(0) + rand();
		gsl_rng_set (randgsl, seed);
		
		this->initialStepSize = new double[2];
		this->initialStepSize[0] = (minMaxIntervalA[1] - minMaxIntervalA[0]) / 5;
		this->initialStepSize[1] = (minMaxIntervalB[1] - minMaxIntervalB[0]) / 5;
	}
	int RSGISEstimationThresholdAccepting2Var2Data::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double threash = startThreshold;
		double stepRand = 0.0;
		double newEnergy = 0.0;

		
		double *currentParError = new double[nPar + 1];
		double *testPar = new double[nPar];
		double *bestParError = new double[nPar + 1];
		double *accepted = new double[nPar];
		double *stepSize = new double[nPar];
		double *minStepSize = new double[nPar];
		double *lowerLimit = new double[nPar];
		double *upperLimit = new double[nPar];
		
		// Set upper and lower limits
		lowerLimit[0] = minMaxIntervalA[0];
		upperLimit[0] = minMaxIntervalA[1];
		
		lowerLimit[1] = minMaxIntervalB[0];
		upperLimit[1] = minMaxIntervalB[1];
		
		minStepSize[0] = minMaxIntervalA[2];
		minStepSize[1] = minMaxIntervalB[2];
		
		// Set current and best parameters to inital values
		for (unsigned int j = 0; j < nPar; j++) 
		{
			currentParError[j] = gsl_vector_get(initialPar, j);
			testPar[j] = gsl_vector_get(initialPar, j);
			bestParError[j] = gsl_vector_get(initialPar, j);
			stepSize[j] = initialStepSize[j];
		}
		// Set energy to 1;
		currentParError[nPar] = 99999;
		bestParError[nPar] = 99999;
		
		rsgis::math::RSGISFunction2Var2DataLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunction2Var2DataLeastSquares(functionHH, functionHV, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1)); 
		
		/* Set maximum number of threasherature runs
		 Divide by runsStep * runsThreshold * 2 so threasherature will be reduced in maximum number of itterations
		 */
		
		unsigned int tRuns = maxItt / (runsStep * runsThreshold * 2); 
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease threashreature
			if(t == (tRuns - 1)) // Last Run
			{
				// Set threasherature to zero (quench)
				threash = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				threash = pow(this->cooling,double(t)) * startThreshold;
			}
			
			// Reset step size
			for (unsigned int j = 0; j < nPar; j++) 
			{
				stepSize[j] = initialStepSize[j] / 2;
			}
			
			for(unsigned int n = 0; n < runsThreshold; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if ((testPar[j] > lowerLimit[j]) && (testPar[j] < upperLimit[j]))
						{
							// Calculate energy
							newEnergy = leastSquares->calcFunction(testPar[0], testPar[1]);
							
							if(newEnergy < currentParError[nPar]) // If new energy is lower accept
							{
								currentParError[j] = testPar[j];
								
								currentParError[nPar] = newEnergy;
								accepted[j]++;
								
								if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
								{
									for(unsigned int par = 0; par < nPar; par++)
									{
										bestParError[par] = currentParError[par];
									}
									bestParError[nPar] = currentParError[nPar];
									
									if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
									{
										// Calculate normalised error (differnt to least squares difference)
										double error = 0.0;
										error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1])),2); // Predicted - Measured data (HH)
										error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (HV)										error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2));
										bestParError[nPar] = error;
										
										for (unsigned int k = 0; k < nPar + 1; k++) 
										{
											gsl_vector_set(outParError, k, bestParError[k]);
										}
										
										// Tidy
										delete[] currentParError;
										delete[] testPar;
										delete[] bestParError;
										delete[] accepted;
										delete[] stepSize;
										delete[] lowerLimit;
										delete[] upperLimit;
										delete[] minStepSize;
										delete leastSquares;
										
										// Exit
										return 1;
									}
								}
								
							}
							else // If new energy is lower, accept based on Boltzman probability
							{
								if(currentParError[nPar] < threash)
								{
									currentParError[j] = testPar[j];
									currentParError[nPar] = newEnergy;
									accepted[j]++;
								}
								else // Reset value
								{
									testPar[j] = currentParError[j];
								}
							}
							numItt++;
						}
					}
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < 2; j++)
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
					// Else, don't change step size.
					accepted[j] = 0;
					if (stepSize[j] < minStepSize[j]) 
					{
						stepSize[j] = minStepSize[j];
					}
					
				}
			}
		}
		
		// Calculate normalised error (differnt to least squares difference)
		double error = 0.0;
		error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1])),2); // Predicted - Measured data (HH)
		error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (HV)										error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2));
		bestParError[nPar] = error;
		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
		
		// Tidy
		delete[] currentParError;
		delete[] testPar;
		delete[] bestParError;
		delete[] accepted;
		delete[] stepSize;
		delete[] lowerLimit;
		delete[] upperLimit;
		delete[] minStepSize;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationThresholdAccepting2Var2Data::~RSGISEstimationThresholdAccepting2Var2Data()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
	}
	
	RSGISEstimationThresholdAccepting2Var2DataWithAP::RSGISEstimationThresholdAccepting2Var2DataWithAP(
																										 rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
																										 rsgis::math::RSGISMathTwoVariableFunction *functionHV,
																										 double *minMaxIntervalA,
																										 double *minMaxIntervalB,
																										 double minEnergy,
																										 double startThreshold,
																										 unsigned int runsStep,
																										 unsigned int runsThreshold,
																										 double cooling,
																										 unsigned int maxItt,
																										 gsl_matrix *covMatrixP, 
																										 gsl_matrix *invCovMatrixD,
																										 gsl_vector *aPrioriPar)
	{
		rsgis::math::RSGISMatrices matrixUtils;
		
		this->startThreshold = startThreshold;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsThreshold = runsThreshold; // Number of times step is changed at each threasherature
		this->cooling = cooling; // Cooling factor
		this->minEnergy = minEnergy; // Set the target energy
		this->maxItt = maxItt; // Maximum number of itterations
		
		this->functionHH = functionHH;
		this->functionHV = functionHV;
		
		this->minMaxIntervalA = minMaxIntervalA; // minA, maxA, minStepSizeA
		this->minMaxIntervalB = minMaxIntervalB; // minB, maxB, minStepSizeB
		
		this->nPar = 2;
		
		this->randgsl = gsl_rng_alloc (gsl_rng_taus2);
		double seed = time(0) + rand();
		//double seed = 0; // Always seed at the same point (for comparison)
		gsl_rng_set (randgsl, seed);
		
		this->initialStepSize = new double[2];
		this->initialStepSize[0] = (minMaxIntervalA[1] - minMaxIntervalA[0]) / 5;
		this->initialStepSize[1] = (minMaxIntervalB[1] - minMaxIntervalB[0]) / 5;
		
		this->aPrioriPar = aPrioriPar;
		this->covMatrixP = covMatrixP;
		this->invCovMatrixD = invCovMatrixD;
		// Invert covariance matrix
		this->invCovMatrixP = gsl_matrix_alloc(2,2);
		matrixUtils.inv2x2GSLMatrix(covMatrixP, this->invCovMatrixP);
	}
	int RSGISEstimationThresholdAccepting2Var2DataWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		
		// Open text file to write errors to
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double threash = startThreshold;
		double stepRand = 0.0;
		double newEnergy = 0.0;

		
		double *currentParError = new double[nPar + 1];
		double *testPar = new double[nPar];
		double *bestParError = new double[nPar + 1];
		double *accepted = new double[nPar];
		double *stepSize = new double[nPar];
		double *minStepSize = new double[nPar];
		double *lowerLimit = new double[nPar];
		double *upperLimit = new double[nPar];
		
		// Set upper and lower limits
		lowerLimit[0] = minMaxIntervalA[0];
		upperLimit[0] = minMaxIntervalA[1];
		
		lowerLimit[1] = minMaxIntervalB[0];
		upperLimit[1] = minMaxIntervalB[1];
		
		minStepSize[0] = minMaxIntervalA[2];
		minStepSize[1] = minMaxIntervalB[2];
		
		// Set current and best parameters to inital values
		for (unsigned int j = 0; j < nPar; j++) 
		{
			currentParError[j] = gsl_vector_get(initialPar, j);
			testPar[j] = gsl_vector_get(initialPar, j);
			bestParError[j] = gsl_vector_get(initialPar, j);
			stepSize[j] = initialStepSize[j];
		}
		// Set energy to 1;
		currentParError[nPar] = 99999;
		bestParError[nPar] = 99999;
		
		rsgis::math::RSGISFunction2Var2DataPreconditionedLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunction2Var2DataPreconditionedLeastSquares(functionHH, functionHV, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1), gsl_vector_get(this->aPrioriPar, 0), gsl_vector_get(this->aPrioriPar, 1), this->invCovMatrixP, this->invCovMatrixD);
		
		/* Set maximum number of threasherature runs
		 Divide by runsStep * runsThreshold * 2 so threasherature will be reduced in maximum number of itterations
		 */
		
		unsigned int tRuns = maxItt / (runsStep * runsThreshold * nPar);
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease threashreature
			if(t == (tRuns - 1)) // Last Run
			{
				// Set threasherature to zero (quench)
				threash = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				threash = pow(this->cooling,double(t)) * startThreshold;
				// Reset step size
				for (unsigned int j = 0; j < nPar; j++) 
				{
					stepSize[j] = initialStepSize[j];
				}
			}
			
			for(unsigned int n = 0; n < runsThreshold; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{
						
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if ((testPar[j] > lowerLimit[j]) && (testPar[j] < upperLimit[j]))
						{
							// Calculate energy
							newEnergy = leastSquares->calcFunction(testPar[0], testPar[1]);

                            double prevEnergy = currentParError[nPar];
							if(newEnergy < currentParError[nPar]) // If new energy is lower accept
							{
								currentParError[j] = testPar[j];
								
								currentParError[nPar] = newEnergy;
								accepted[j]++;
								
								if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
								{
									for(unsigned int par = 0; par < nPar; par++)
									{
										bestParError[par] = currentParError[par];
									}
									bestParError[nPar] = currentParError[nPar];
									
									if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
									{
										// Calculate normalised error (differnt to least squares difference)
										double error = 0.0;
										error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1])),2); // Predicted - Measured data (HH)
										error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (HV)										error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2));
										bestParError[nPar] = error;
										
										for (unsigned int k = 0; k < nPar + 1; k++) 
										{
											gsl_vector_set(outParError, k, bestParError[k]);
										}
										
										// Tidy
										delete[] currentParError;
										delete[] testPar;
										delete[] bestParError;
										delete[] accepted;
										delete[] stepSize;
										delete[] lowerLimit;
										delete[] upperLimit;
										delete[] minStepSize;
										delete leastSquares;
										
										// Exit
										return 1;
									}
								}
								
							}
							else // If new energy is lower, accept based on Boltzman probability
							{
								if(currentParError[nPar] < threash)
								{
									currentParError[j] = testPar[j];
									currentParError[nPar] = newEnergy;
									accepted[j]++;
								}
								else // Reset value
								{
									testPar[j] = currentParError[j];
								}
							}
							if(fabs(newEnergy - prevEnergy) < 1e-10)
							{
								threash = startThreshold;
								t = t / 2;
							}
							numItt++;
						}
					}
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < 2; j++)
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
					// Else, don't change step size.
					accepted[j] = 0;
					if (stepSize[j] < minStepSize[j]) 
					{
						stepSize[j] = minStepSize[j];
					}
					
				}
			}
		}
		
		// Calculate normalised error (differnt to least squares difference)
		double error = 0.0;
		error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1])),2); // Predicted - Measured data (HH)
	    error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (HV)
		error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2));
		bestParError[nPar] = error;
		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
		
		// Tidy
		delete[] currentParError;
		delete[] testPar;
		delete[] bestParError;
		delete[] accepted;
		delete[] stepSize;
		delete[] lowerLimit;
		delete[] upperLimit;
		delete[] minStepSize;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationThresholdAccepting2Var2DataWithAP::~RSGISEstimationThresholdAccepting2Var2DataWithAP()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
		gsl_matrix_free(invCovMatrixP);
	}
	
	RSGISEstimationThresholdAccepting2Var3Data::RSGISEstimationThresholdAccepting2Var3Data(
																							 rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
																							 rsgis::math::RSGISMathTwoVariableFunction *functionHV,
																							 rsgis::math::RSGISMathTwoVariableFunction *functionVV,
																							 double *minMaxIntervalA,
																							 double *minMaxIntervalB,
																							 double minEnergy,
																							 double startThreshold,
																							 unsigned int runsStep,
																							 unsigned int runsThreshold,
																							 double cooling,
																							 unsigned int maxItt)
	{
		this->startThreshold = startThreshold;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsThreshold = runsThreshold; // Number of times step is changed at each threasherature
		this->cooling = cooling; // Cooling factor
		this->minEnergy = minEnergy; // Set the target energy
		this->maxItt = maxItt; // Maximum number of itterations
		
		this->functionHH = functionHH;
		this->functionHV = functionHV;
		this->functionVV = functionVV;
		
		this->minMaxIntervalA = minMaxIntervalA; // minA, maxA, minStepSizeA
		this->minMaxIntervalB = minMaxIntervalB; // minB, maxB, minStepSizeB
		
		this->nPar = 2;
		
		this->randgsl = gsl_rng_alloc (gsl_rng_taus2);
		double seed = time(0) + rand();
		gsl_rng_set (randgsl, seed);
		
		this->initialStepSize = new double[2];
		this->initialStepSize[0] = (minMaxIntervalA[1] - minMaxIntervalA[0]) / 5;
		this->initialStepSize[1] = (minMaxIntervalB[1] - minMaxIntervalB[0]) / 5;
	}
	int RSGISEstimationThresholdAccepting2Var3Data::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double threash = startThreshold;
		double stepRand = 0.0;
		double newEnergy = 0.0;

		
		double *currentParError = new double[nPar + 1];
		double *testPar = new double[nPar];
		double *bestParError = new double[nPar + 1];
		double *accepted = new double[nPar];
		double *stepSize = new double[nPar];
		double *minStepSize = new double[nPar];
		double *lowerLimit = new double[nPar];
		double *upperLimit = new double[nPar];
		
		// Set upper and lower limits
		lowerLimit[0] = minMaxIntervalA[0];
		upperLimit[0] = minMaxIntervalA[1];
		
		lowerLimit[1] = minMaxIntervalB[0];
		upperLimit[1] = minMaxIntervalB[1];
		
		minStepSize[0] = minMaxIntervalA[2];
		minStepSize[1] = minMaxIntervalB[2];
		
		// Set current and best parameters to inital values
		for (unsigned int j = 0; j < nPar; j++) 
		{
			currentParError[j] = gsl_vector_get(initialPar, j);
			testPar[j] = gsl_vector_get(initialPar, j);
			bestParError[j] = gsl_vector_get(initialPar, j);
			stepSize[j] = initialStepSize[j];
		}
		// Set energy to 1;
		currentParError[nPar] = 99999;
		bestParError[nPar] = 99999;
		
		rsgis::math::RSGISFunction2Var3DataLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunction2Var3DataLeastSquares(this->functionHH, this->functionHV, this->functionVV, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1), gsl_vector_get(inData, 2)); 
		
		/* Set maximum number of threasherature runs
		 Divide by runsStep * runsThreshold * 2 so threasherature will be reduced in maximum number of itterations
		 */
		
		unsigned int tRuns = maxItt / (runsStep * runsThreshold * 2); 
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease threashreature
			if(t == (tRuns - 1)) // Last Run
			{
				// Set threasherature to zero (quench)
				threash = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				threash = pow(this->cooling,double(t)) * startThreshold;
				// Reset step size
				for (unsigned int j = 0; j < nPar; j++) 
				{
					stepSize[j] = initialStepSize[j];
				}
			}
			
			// Reset step size
			for (unsigned int j = 0; j < nPar; j++) 
			{
				stepSize[j] = initialStepSize[j] / 2;
			}
			
			for(unsigned int n = 0; n < runsThreshold; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if ((testPar[j] > lowerLimit[j]) && (testPar[j] < upperLimit[j]))
						{
							// Calculate energy
							newEnergy = leastSquares->calcFunction(testPar[0], testPar[1]);

                            double prevEnergy = currentParError[nPar];
							if(newEnergy < currentParError[nPar]) // If new energy is lower accept
							{
								currentParError[j] = testPar[j];
								
								currentParError[nPar] = newEnergy;
								accepted[j]++;
								
								if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
								{
									for(unsigned int par = 0; par < nPar; par++)
									{
										bestParError[par] = currentParError[par];
									}
									bestParError[nPar] = currentParError[nPar];
									
									if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
									{
										// Calculate normalised error (differnt to least squares difference)
										double error = 0.0;
										error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1])),2); // Predicted - Measured data (HH)
										error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (HV)
										error = error + pow((gsl_vector_get(inData, 2) - functionVV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (VV)
										
										error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2)+pow(gsl_vector_get(inData, 2),2));
										bestParError[nPar] = error;
										
										for (unsigned int k = 0; k < nPar + 1; k++) 
										{
											gsl_vector_set(outParError, k, bestParError[k]);
										}
										
										// Tidy
										delete[] currentParError;
										delete[] testPar;
										delete[] bestParError;
										delete[] accepted;
										delete[] stepSize;
										delete[] lowerLimit;
										delete[] upperLimit;
										delete[] minStepSize;
										delete leastSquares;
										
										// Exit
										return 1;
									}
								}
								
							}
							else // If new energy is lower, accept based on Boltzman probability
							{
								if(currentParError[nPar] < threash)
								{
									currentParError[j] = testPar[j];
									currentParError[nPar] = newEnergy;
									accepted[j]++;
								}
								else // Reset value
								{
									testPar[j] = currentParError[j];
								}
							}
							if(fabs(newEnergy - prevEnergy) < 1e-10)
							{
								threash = startThreshold;
								t = 0;
							}
							numItt++;
						}
					}
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < 2; j++)
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
					// Else, don't change step size.
					accepted[j] = 0;
					if (stepSize[j] < minStepSize[j]) 
					{
						stepSize[j] = minStepSize[j];
					}
					
				}
			}
		}
		
		// Calculate normalised error (differnt to least squares difference)
		double error = 0.0;
		error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1])),2); // Predicted - Measured data (HH)
		error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (HV)
		error = error + pow((gsl_vector_get(inData, 2) - functionVV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (VV)
		
		error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2)+pow(gsl_vector_get(inData, 2),2));
		bestParError[nPar] = error;
		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
		
		// Tidy
		delete[] currentParError;
		delete[] testPar;
		delete[] bestParError;
		delete[] accepted;
		delete[] stepSize;
		delete[] lowerLimit;
		delete[] upperLimit;
		delete[] minStepSize;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationThresholdAccepting2Var3Data::~RSGISEstimationThresholdAccepting2Var3Data()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
	}
	
	RSGISEstimationThresholdAccepting2Var3DataWithAP::RSGISEstimationThresholdAccepting2Var3DataWithAP(
																										 rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
																										 rsgis::math::RSGISMathTwoVariableFunction *functionHV,
																										 rsgis::math::RSGISMathTwoVariableFunction *functionVV,
																										 double *minMaxIntervalA,
																										 double *minMaxIntervalB,
																										 double minEnergy,
																										 double startThreshold,
																										 unsigned int runsStep,
																										 unsigned int runsThreshold,
																										 double cooling,
																										 unsigned int maxItt,
																										 gsl_matrix *covMatrixP, 
																										 gsl_matrix *invCovMatrixD,
																										 gsl_vector *aPrioriPar)
	{
		rsgis::math::RSGISMatrices matrixUtils;
		
		this->startThreshold = startThreshold;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsThreshold = runsThreshold; // Number of times step is changed at each threasherature
		this->cooling = cooling; // Cooling factor
		this->minEnergy = minEnergy; // Set the target energy
		this->maxItt = maxItt; // Maximum number of itterations
		
		this->functionHH = functionHH;
		this->functionHV = functionHV;
		this->functionVV = functionVV;
		
		this->minMaxIntervalA = minMaxIntervalA; // minA, maxA, minStepSizeA
		this->minMaxIntervalB = minMaxIntervalB; // minB, maxB, minStepSizeB
		
		this->nPar = 2;
		
		this->randgsl = gsl_rng_alloc (gsl_rng_taus2);
		double seed = time(0) + rand();
		gsl_rng_set (randgsl, seed);
		
		this->initialStepSize = new double[2];
		this->initialStepSize[0] = (minMaxIntervalA[1] - minMaxIntervalA[0]) / 5;
		this->initialStepSize[1] = (minMaxIntervalB[1] - minMaxIntervalB[0]) / 5;
		
		this->aPrioriPar = aPrioriPar;
		this->covMatrixP = covMatrixP;
		this->invCovMatrixD = invCovMatrixD;
		// Invert covariance matrix
		this->invCovMatrixP = gsl_matrix_alloc(2,2);
		matrixUtils.inv2x2GSLMatrix(covMatrixP, this->invCovMatrixP);
	}
	int RSGISEstimationThresholdAccepting2Var3DataWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double threash = startThreshold;
		double stepRand = 0.0;
		double newEnergy = 0.0;

		
		double *currentParError = new double[nPar + 1];
		double *testPar = new double[nPar];
		double *bestParError = new double[nPar + 1];
		double *accepted = new double[nPar];
		double *stepSize = new double[nPar];
		double *minStepSize = new double[nPar];
		double *lowerLimit = new double[nPar];
		double *upperLimit = new double[nPar];
		
		// Set upper and lower limits
		lowerLimit[0] = minMaxIntervalA[0];
		upperLimit[0] = minMaxIntervalA[1];
		
		lowerLimit[1] = minMaxIntervalB[0];
		upperLimit[1] = minMaxIntervalB[1];
		
		minStepSize[0] = minMaxIntervalA[2];
		minStepSize[1] = minMaxIntervalB[2];
		
		// Set current and best parameters to inital values
		for (unsigned int j = 0; j < nPar; j++) 
		{
			currentParError[j] = gsl_vector_get(initialPar, j);
			testPar[j] = gsl_vector_get(initialPar, j);
			bestParError[j] = gsl_vector_get(initialPar, j);
			stepSize[j] = initialStepSize[j];
		}
		// Set energy to 1;
		currentParError[nPar] = 99999;
		bestParError[nPar] = 99999;
		
		rsgis::math::RSGISFunction2Var3DataPreconditionedLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunction2Var3DataPreconditionedLeastSquares(this->functionHH, this->functionHV, this->functionVV, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1), gsl_vector_get(inData, 2), gsl_vector_get(this->aPrioriPar, 0), gsl_vector_get(this->aPrioriPar, 1), this->invCovMatrixP, this->invCovMatrixD);
		
		/* Set maximum number of threasherature runs
		 Divide by runsStep * runsThreshold * 2 so threasherature will be reduced in maximum number of itterations
		 */
		
		unsigned int tRuns = maxItt / (runsStep * runsThreshold * 2); 
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease threashreature
			if(t == (tRuns - 1)) // Last Run
			{
				// Set threasherature to zero (quench)
				threash = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				threash = pow(this->cooling,double(t)) * startThreshold;
			}
			
			// Reset step size
			for (unsigned int j = 0; j < nPar; j++) 
			{
				stepSize[j] = initialStepSize[j] / 2;
			}
			
			for(unsigned int n = 0; n < runsThreshold; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{
						
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if ((testPar[j] > lowerLimit[j]) && (testPar[j] < upperLimit[j]))
						{
							// Calculate energy
							newEnergy = leastSquares->calcFunction(testPar[0], testPar[1]);
							
							double prevEnergy = currentParError[nPar];
							if(newEnergy < currentParError[nPar]) // If new energy is lower accept
							{
								currentParError[j] = testPar[j];
								
								currentParError[nPar] = newEnergy;
								accepted[j]++;
								
								if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
								{
									for(unsigned int par = 0; par < nPar; par++)
									{
										bestParError[par] = currentParError[par];
									}
									bestParError[nPar] = currentParError[nPar];
									
									if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
									{
										// Calculate normalised error (differnt to least squares difference)
										double error = 0.0;
										error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1])),2); // Predicted - Measured data (HH)
										error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (HV)
										error = error + pow((gsl_vector_get(inData, 2) - functionVV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (VV)
										
										error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2)+pow(gsl_vector_get(inData, 2),2));
										bestParError[nPar] = error;
										
										
										for (unsigned int k = 0; k < nPar + 1; k++) 
										{
											gsl_vector_set(outParError, k, bestParError[k]);
										}
										
										// Tidy
										delete[] currentParError;
										delete[] testPar;
										delete[] bestParError;
										delete[] accepted;
										delete[] stepSize;
										delete[] lowerLimit;
										delete[] upperLimit;
										delete[] minStepSize;
										delete leastSquares;
										
										// Exit
										return 1;
									}
								}
								
							}
							else // If new energy is lower, accept based on Boltzman probability
							{
								if(currentParError[nPar] < threash)
								{
									currentParError[j] = testPar[j];
									currentParError[nPar] = newEnergy;
									accepted[j]++;
								}
								else // Reset value
								{
									testPar[j] = currentParError[j];
								}
							}
							if(fabs(newEnergy - prevEnergy) < 1e-10)
							{
								threash = startThreshold;
								t = t / 2;
							}
							numItt++;
						}
					}
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < 2; j++)
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
					// Else, don't change step size.
					accepted[j] = 0;
					if (stepSize[j] < minStepSize[j]) 
					{
						stepSize[j] = minStepSize[j];
					}
					
				}
			}
		}
		
		// Calculate normalised error (differnt to least squares difference)
		double error = 0.0;
		error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1])),2); // Predicted - Measured data (HH)
		error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (HV)
		error = error + pow((gsl_vector_get(inData, 2) - functionVV->calcFunction(bestParError[0], bestParError[1])),2); // + Predicted - Measured data (VV)
		
		error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2)+pow(gsl_vector_get(inData, 2),2));
		bestParError[nPar] = error;
		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
		
		// Tidy
		delete[] currentParError;
		delete[] testPar;
		delete[] bestParError;
		delete[] accepted;
		delete[] stepSize;
		delete[] lowerLimit;
		delete[] upperLimit;
		delete[] minStepSize;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationThresholdAccepting2Var3DataWithAP::~RSGISEstimationThresholdAccepting2Var3DataWithAP()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
		gsl_matrix_free(invCovMatrixP);
	}
	
	RSGISEstimationThresholdAccepting3Var3DataWithAP::RSGISEstimationThresholdAccepting3Var3DataWithAP(
																										 rsgis::math::RSGISMathThreeVariableFunction *functionHH, 
																										 rsgis::math::RSGISMathThreeVariableFunction *functionHV,
																										 rsgis::math::RSGISMathThreeVariableFunction *functionVV,
																										 double *minMaxIntervalA,
																										 double *minMaxIntervalB,
																										 double *minMaxIntervalC,
																										 double minEnergy,
																										 double startThreshold,
																										 unsigned int runsStep,
																										 unsigned int runsThreshold,
																										 double cooling,
																										 unsigned int maxItt,
																										 gsl_matrix *covMatrixP, 
																										 gsl_matrix *invCovMatrixD,
																										 gsl_vector *aPrioriPar)
	{
		this->startThreshold = startThreshold;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsThreshold = runsThreshold; // Number of times step is changed at each threasherature
		this->cooling = cooling; // Cooling factor
		this->minEnergy = minEnergy; // Set the target energy
		this->maxItt = maxItt; // Maximum number of itterations
		
		this->functionHH = functionHH;
		this->functionHV = functionHV;
		this->functionVV = functionVV;
		
		this->minMaxIntervalA = minMaxIntervalA; // minA, maxA, minStepSizeA
		this->minMaxIntervalB = minMaxIntervalB; // minB, maxB, minStepSizeB
		this->minMaxIntervalC = minMaxIntervalC; // minB, maxB, minStepSizeB
		
		this->nPar = 3;
		
		this->randgsl = gsl_rng_alloc (gsl_rng_taus2);
		double seed = 0; // time(0) + rand();
		gsl_rng_set (randgsl, seed);
		
		this->initialStepSize = new double[nPar];
		this->initialStepSize[0] = (minMaxIntervalA[1] - minMaxIntervalA[0]) / 5;
		this->initialStepSize[1] = (minMaxIntervalB[1] - minMaxIntervalB[0]) / 5;
		this->initialStepSize[2] = (minMaxIntervalC[1] - minMaxIntervalC[0]) / 5;
		
		this->aPrioriPar = aPrioriPar;
		this->covMatrixP = covMatrixP;
		this->invCovMatrixD = invCovMatrixD;
		// Invert covariance matrix
		this->invCovMatrixP = gsl_matrix_alloc(nPar,nPar);
		gsl_matrix_set_zero(invCovMatrixP);
		for (unsigned int i = 0; i < nPar; i++) 
		{
			gsl_matrix_set(this->invCovMatrixP, i, i, 1 / gsl_matrix_get(covMatrixP, i, i));
		}
	}
	int RSGISEstimationThresholdAccepting3Var3DataWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double threash = startThreshold;
		double stepRand = 0.0;
		double newEnergy = 0.0;

		
		double *currentParError = new double[nPar + 1];
		double *testPar = new double[nPar];
		double *bestParError = new double[nPar + 1];
		double *accepted = new double[nPar];
		double *stepSize = new double[nPar];
		double *minStepSize = new double[nPar];
		double *lowerLimit = new double[nPar];
		double *upperLimit = new double[nPar];
		
		// Set upper and lower limits
		lowerLimit[0] = minMaxIntervalA[0];
		upperLimit[0] = minMaxIntervalA[1];
		
		lowerLimit[1] = minMaxIntervalB[0];
		upperLimit[1] = minMaxIntervalB[1];
		
		lowerLimit[2] = minMaxIntervalC[0];
		upperLimit[2] = minMaxIntervalC[1];
		
		minStepSize[0] = minMaxIntervalA[2];
		minStepSize[1] = minMaxIntervalB[2];
		minStepSize[2] = minMaxIntervalC[2];
		
		// Set current and best parameters to inital values
		for (unsigned int j = 0; j < nPar; j++) 
		{
			currentParError[j] = gsl_vector_get(initialPar, j);
			testPar[j] = gsl_vector_get(initialPar, j);
			bestParError[j] = gsl_vector_get(initialPar, j);
			stepSize[j] = initialStepSize[j];
		}
		// Set energy to 1;
		currentParError[nPar] = 99999;
		bestParError[nPar] = 99999;
		
		rsgis::math::RSGISFunction3Var3DataPreconditionedLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunction3Var3DataPreconditionedLeastSquares(this->functionHH, this->functionHV, this->functionVV, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1), gsl_vector_get(inData, 2), gsl_vector_get(this->aPrioriPar, 0), gsl_vector_get(this->aPrioriPar, 1), gsl_vector_get(this->aPrioriPar, 2), this->invCovMatrixP, this->invCovMatrixD);
		
		/* Set maximum number of threasherature runs
		 Divide by runsStep * runsThreshold * nPar so threasherature will be reduced in maximum number of itterations
		 */
		
		unsigned int tRuns = maxItt / (runsStep * runsThreshold * nPar); 
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease threashreature
			if(t == (tRuns - 1)) // Last Run
			{
				// Set threasherature to zero (quench)
				threash = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				threash = pow(this->cooling,double(t)) * startThreshold;
				// Reset step size
				for (unsigned int j = 0; j < nPar; j++) 
				{
					stepSize[j] = initialStepSize[j] / 2;
				}
			}
			
			
			for(unsigned int n = 0; n < runsThreshold; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{						
						
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if ((testPar[j] > lowerLimit[j]) && (testPar[j] < upperLimit[j]))
						{
							// Calculate energy
							newEnergy = leastSquares->calcFunction(testPar[0], testPar[1], testPar[2]);
							
							double prevEnergy = currentParError[nPar];
							if(newEnergy < currentParError[nPar]) // If new energy is lower accept
							{
								currentParError[j] = testPar[j];
								
								currentParError[nPar] = newEnergy;
								accepted[j]++;
								
								if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
								{
									for(unsigned int par = 0; par < nPar; par++)
									{
										bestParError[par] = currentParError[par];
									}
									bestParError[nPar] = currentParError[nPar];
									
									if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
									{
										// Calculate normalised error (differnt to least squares difference)
										double error = 0.0;
										error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // Predicted - Measured data (HH)
										error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (HV)
										error = error + pow((gsl_vector_get(inData, 2) - functionVV->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (VV)
										
										error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2)+pow(gsl_vector_get(inData, 2),2));
										bestParError[nPar] = error;
										
										
										for (unsigned int k = 0; k < nPar + 1; k++) 
										{
											gsl_vector_set(outParError, k, bestParError[k]);
										}
										
										// Tidy
										delete[] currentParError;
										delete[] testPar;
										delete[] bestParError;
										delete[] accepted;
										delete[] stepSize;
										delete[] lowerLimit;
										delete[] upperLimit;
										delete[] minStepSize;
										delete leastSquares;
										
										// Exit
										return 1;
									}
								}
								
							}
							else // If new energy is lower, accept based on Boltzman probability
							{
								if(currentParError[nPar] < threash)
								{
									currentParError[j] = testPar[j];
									currentParError[nPar] = newEnergy;
									accepted[j]++;
								}
								else // Reset value
								{
									testPar[j] = currentParError[j];
								}
								
							}
							if(fabs(newEnergy - prevEnergy) < 1e-8)
							{
								threash = startThreshold;
								t = t / 2;
							}
							numItt++;
						}
					}
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < 2; j++)
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
					// Else, don't change step size.
					accepted[j] = 0;
					if (stepSize[j] < minStepSize[j]) 
					{
						stepSize[j] = minStepSize[j];
					}
					
				}
			}
		}
		
		// Calculate normalised error (differnt to least squares difference)
		double error = 0.0;
		error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // Predicted - Measured data (HH)
		error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (HV)
		error = error + pow((gsl_vector_get(inData, 2) - functionVV->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (VV)
		
		error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2)+pow(gsl_vector_get(inData, 2),2));
		bestParError[nPar] = error;
		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
		
		// Tidy
		delete[] currentParError;
		delete[] testPar;
		delete[] bestParError;
		delete[] accepted;
		delete[] stepSize;
		delete[] lowerLimit;
		delete[] upperLimit;
		delete[] minStepSize;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationThresholdAccepting3Var3DataWithAP::~RSGISEstimationThresholdAccepting3Var3DataWithAP()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
		gsl_matrix_free(invCovMatrixP);
	}
	
	RSGISEstimationThresholdAccepting3Var4DataWithAP::RSGISEstimationThresholdAccepting3Var4DataWithAP(
																										 rsgis::math::RSGISMathThreeVariableFunction *function1, 
																										 rsgis::math::RSGISMathThreeVariableFunction *function2, 
																										 rsgis::math::RSGISMathThreeVariableFunction *function3, 
																										 rsgis::math::RSGISMathThreeVariableFunction *function4, 
																										 double *minMaxIntervalA,
																										 double *minMaxIntervalB,
																										 double *minMaxIntervalC,
																										 double minEnergy,
																										 double startThreshold,
																										 unsigned int runsStep,
																										 unsigned int runsThreshold,
																										 double cooling,
																										 unsigned int maxItt,
																										 gsl_matrix *covMatrixP, 
																										 gsl_matrix *invCovMatrixD,
																										 gsl_vector *aPrioriPar)
	{
		this->startThreshold = startThreshold;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsThreshold = runsThreshold; // Number of times step is changed at each threasherature
		this->cooling = cooling; // Cooling factor
		this->minEnergy = minEnergy; // Set the target energy
		this->maxItt = maxItt; // Maximum number of itterations
		
		this->function1 = function1;
		this->function2 = function2;
		this->function3 = function3;
		this->function4 = function4;
		
		this->minMaxIntervalA = minMaxIntervalA; // minA, maxA, minStepSizeA
		this->minMaxIntervalB = minMaxIntervalB; // minB, maxB, minStepSizeB
		this->minMaxIntervalC = minMaxIntervalC; // minB, maxB, minStepSizeB
		
		this->nPar = 3;
		
		this->randgsl = gsl_rng_alloc (gsl_rng_taus2);
		double seed = 0; // time(0) + rand();
		gsl_rng_set (randgsl, seed);
		
		this->initialStepSize = new double[nPar];
		this->initialStepSize[0] = (minMaxIntervalA[1] - minMaxIntervalA[0]) / 5;
		this->initialStepSize[1] = (minMaxIntervalB[1] - minMaxIntervalB[0]) / 5;
		this->initialStepSize[2] = (minMaxIntervalC[1] - minMaxIntervalC[0]) / 5;
		
		this->aPrioriPar = aPrioriPar;
		this->covMatrixP = covMatrixP;
		this->invCovMatrixD = invCovMatrixD;
		// Invert covariance matrix
		this->invCovMatrixP = gsl_matrix_alloc(nPar,nPar);
		gsl_matrix_set_zero(invCovMatrixP);
		for (unsigned int i = 0; i < nPar; i++) 
		{
			gsl_matrix_set(this->invCovMatrixP, i, i, 1 / gsl_matrix_get(covMatrixP, i, i));
		}
	}
	int RSGISEstimationThresholdAccepting3Var4DataWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double threash = startThreshold;
		double stepRand = 0.0;
		double newEnergy = 0.0;

		
		double *currentParError = new double[nPar + 1];
		double *testPar = new double[nPar];
		double *bestParError = new double[nPar + 1];
		double *accepted = new double[nPar];
		double *stepSize = new double[nPar];
		double *minStepSize = new double[nPar];
		double *lowerLimit = new double[nPar];
		double *upperLimit = new double[nPar];
		
		// Set upper and lower limits
		lowerLimit[0] = minMaxIntervalA[0];
		upperLimit[0] = minMaxIntervalA[1];
		
		lowerLimit[1] = minMaxIntervalB[0];
		upperLimit[1] = minMaxIntervalB[1];
		
		lowerLimit[2] = minMaxIntervalC[0];
		upperLimit[2] = minMaxIntervalC[1];
		
		minStepSize[0] = minMaxIntervalA[2];
		minStepSize[1] = minMaxIntervalB[2];
		minStepSize[2] = minMaxIntervalC[2];
		
		// Set current and best parameters to inital values
		for (unsigned int j = 0; j < nPar; j++) 
		{
			currentParError[j] = gsl_vector_get(initialPar, j);
			testPar[j] = gsl_vector_get(initialPar, j);
			bestParError[j] = gsl_vector_get(initialPar, j);
			stepSize[j] = initialStepSize[j];
		}
		// Set energy to 1;
		currentParError[nPar] = 99999;
		bestParError[nPar] = 99999;
		
		rsgis::math::RSGISFunction3Var4DataPreconditionedLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunction3Var4DataPreconditionedLeastSquares(this->function1, this->function2, this->function3, this->function4, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1), gsl_vector_get(inData, 2), gsl_vector_get(inData, 3), gsl_vector_get(this->aPrioriPar, 0), gsl_vector_get(this->aPrioriPar, 1), gsl_vector_get(this->aPrioriPar, 2), this->invCovMatrixP, this->invCovMatrixD);
		
		/* Set maximum number of threasherature runs
		 Divide by runsStep * runsThreshold * nPar so threasherature will be reduced in maximum number of itterations
		 */
		unsigned int tRuns = maxItt / (runsStep * runsThreshold * nPar); 
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease threashreature
			if(t == (tRuns - 1)) // Last Run
			{
				// Set threasherature to zero (quench)
				threash = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else
			{
				// Reset step size
				for (unsigned int j = 0; j < nPar; j++) 
				{
					stepSize[j] = initialStepSize[j] / 2;
				}
				
				threash = pow(this->cooling,double(t)) * startThreshold;
				
			}
			
			for(unsigned int n = 0; n < runsThreshold; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{						
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if ((testPar[j] > lowerLimit[j]) && (testPar[j] < upperLimit[j]))
						{
							// Calculate energy
							newEnergy = leastSquares->calcFunction(testPar[0], testPar[1], testPar[2]);

                            double prevEnergy = currentParError[nPar];
							if(newEnergy < currentParError[nPar]) // If new energy is lower accept
							{
								currentParError[j] = testPar[j];
								
								currentParError[nPar] = newEnergy;
								accepted[j]++;
								
								if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
								{
									for(unsigned int par = 0; par < nPar; par++)
									{
										bestParError[par] = currentParError[par];
									}
									bestParError[nPar] = currentParError[nPar];
									
									if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
									{
										// Calculate normalised error (differnt to least squares difference)
										double error = 0.0;
										error = pow((gsl_vector_get(inData, 0) - function1->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // Predicted - Measured data (1)
										error = error + pow((gsl_vector_get(inData, 1) - function2->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (2)
										error = error + pow((gsl_vector_get(inData, 2) - function3->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (3)
										error = error + pow((gsl_vector_get(inData, 2) - function4->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (4)
										
										error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2)+pow(gsl_vector_get(inData, 2),2)+pow(gsl_vector_get(inData, 3),2));
										bestParError[nPar] = error;
										
										
										for (unsigned int k = 0; k < nPar + 1; k++) 
										{
											gsl_vector_set(outParError, k, bestParError[k]);
										}
										
										// Tidy
										delete[] currentParError;
										delete[] testPar;
										delete[] bestParError;
										delete[] accepted;
										delete[] stepSize;
										delete[] lowerLimit;
										delete[] upperLimit;
										delete[] minStepSize;
										delete leastSquares;
										
										// Exit
										return 1;
									}
								}
							}
							else // If new energy is lower, accept based on Boltzman probability
							{
								if(currentParError[nPar] < threash)
								{
									currentParError[j] = testPar[j];
									currentParError[nPar] = newEnergy;
									accepted[j]++;
								}
								else // Reset value
								{
									testPar[j] = currentParError[j];
								}
								
							}
							if(fabs(newEnergy - prevEnergy) < 1e-10)
							{
								threash = startThreshold;
								t = t / 2;
								currentParError[0] = (minMaxIntervalA[1] - minMaxIntervalA[0]) / 2;
								currentParError[1] = (minMaxIntervalB[1] - minMaxIntervalB[0]) / 2;
								currentParError[2] = (minMaxIntervalC[1] - minMaxIntervalC[0]) / 2;
							}
							numItt++;
						}
					}
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < 2; j++)
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
					// Else, don't change step size.
					accepted[j] = 0;
					if (stepSize[j] < minStepSize[j])
					{
						stepSize[j] = minStepSize[j];
					}
					
				}
			}
		}
		
		// Calculate normalised error (differnt to least squares difference)
		double error = 0.0;
		error = pow((gsl_vector_get(inData, 0) - function1->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // Predicted - Measured data (1)
		error = error + pow((gsl_vector_get(inData, 1) - function2->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (2)
		error = error + pow((gsl_vector_get(inData, 2) - function3->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (3)
		error = error + pow((gsl_vector_get(inData, 3) - function4->calcFunction(bestParError[0], bestParError[1], bestParError[2])),2); // + Predicted - Measured data (4)
		
		error = error / (pow(gsl_vector_get(inData, 0),2)+pow(gsl_vector_get(inData, 1),2)+pow(gsl_vector_get(inData, 2),2)+pow(gsl_vector_get(inData, 3),2));
		bestParError[nPar] = error;
		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
		
		// Tidy
		delete[] currentParError;
		delete[] testPar;
		delete[] bestParError;
		delete[] accepted;
		delete[] stepSize;
		delete[] lowerLimit;
		delete[] upperLimit;
		delete[] minStepSize;
		delete leastSquares;
				
		// Exit
		return 0;
		
	}
	RSGISEstimationThresholdAccepting3Var4DataWithAP::~RSGISEstimationThresholdAccepting3Var4DataWithAP()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
		gsl_matrix_free(invCovMatrixP);
	}
	
}}
