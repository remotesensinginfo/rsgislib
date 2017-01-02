/*
 *  RSGISNonLinearFit.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 14/06/2010.
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

#include "RSGISNonLinearFit.h"

namespace rsgis {namespace math{
	double RSGISNonLinearFit::nonLinearFit1Var(gsl_vector *indVar, gsl_vector *depVar, gsl_vector *variance, gsl_vector *coeff, bool *fixCoeff, RSGISMathFunction *function)
	{
		unsigned int nCoeff = function->numCoefficients();
		gsl_vector *outCoeffError;
		outCoeffError = gsl_vector_alloc(nCoeff + 1);
		
		// Set output coefficeints
		for (unsigned int i = 0; i < nCoeff; i++) 
		{
			gsl_vector_set(outCoeffError, i, gsl_vector_get(coeff, i));
		}
		
		this->minimiseChiSqSA1Var(indVar, depVar, variance, outCoeffError, fixCoeff, function);
		
		// Set output coefficeints
		for (unsigned int i = 0; i < nCoeff; i++) 
		{
			gsl_vector_set(coeff, i, gsl_vector_get(outCoeffError, i));
		}
		
		// Set chiSq
		double chiSq = gsl_vector_get(outCoeffError, nCoeff);
		
		// Tidy up
		gsl_vector_free(outCoeffError);
		
		return chiSq;
	}
	
	double RSGISNonLinearFit::nonLinearFit2Var(gsl_matrix *indVar, gsl_vector *depVar, gsl_vector *variance, gsl_vector *coeff, bool *fixCoeff, RSGISMathTwoVariableFunction *function)
	{
		unsigned int nCoeff = function->numCoefficients();
		gsl_vector *outCoeffError;
		outCoeffError = gsl_vector_alloc(nCoeff + 1);
		
		// Set output coefficeints
		for (unsigned int i = 0; i < nCoeff; i++) 
		{
			gsl_vector_set(outCoeffError, i, gsl_vector_get(coeff, i));
		}
		
		this->minimiseChiSqSA2Var(indVar, depVar, variance, outCoeffError, fixCoeff, function);
		
		// Set output coefficeints
		for (unsigned int i = 0; i < nCoeff; i++) 
		{
			gsl_vector_set(coeff, i, gsl_vector_get(outCoeffError, i));
		}
		
		// Set chiSq
		double chiSq = gsl_vector_get(outCoeffError, nCoeff);
		
		// Tidy up
		gsl_vector_free(outCoeffError);
		
		return chiSq;
	}

	gsl_matrix* RSGISNonLinearFit::nonLinearTest1Var(gsl_vector *indVar, gsl_vector *depVar, RSGISMathFunction *function)
	{
		gsl_matrix *measuredVpredictted;
		measuredVpredictted = gsl_matrix_alloc(indVar->size, 2); // Set up matrix to hold measured and predicted y values.
		
		for (unsigned int i = 0; i < depVar->size; i++) 
		{
			double x = gsl_vector_get(indVar, i);
			double y = function->calcFunction(x);
			gsl_matrix_set(measuredVpredictted, i, 0, gsl_vector_get(depVar, i));
			gsl_matrix_set(measuredVpredictted, i, 1, y);
		}
		
		return measuredVpredictted;
	}
	
	gsl_matrix* RSGISNonLinearFit::nonLinearTest2Var(gsl_matrix *indVar, gsl_vector *depVar, RSGISMathTwoVariableFunction *function)
	{
		gsl_matrix *measuredVpredictted;
		measuredVpredictted = gsl_matrix_alloc(indVar->size1, 2); // Set up matrix to hold measured and predicted y values.
		
		for (unsigned int i = 0; i < depVar->size; i++) 
		{
			double x0 = gsl_matrix_get(indVar, i, 0);
			double x1 = gsl_matrix_get(indVar, i, 1);
			double y = function->calcFunction(x0, x1);
			gsl_matrix_set(measuredVpredictted, i, 0, gsl_vector_get(depVar, i));
			gsl_matrix_set(measuredVpredictted, i, 1, y);
		}
		
		return measuredVpredictted;
	}
	
	int RSGISNonLinearFit::minimiseChiSqSA1Var(gsl_vector *indVar, gsl_vector *depVar, gsl_vector *variance, gsl_vector *outCoeffError, bool *fixCoeff, RSGISMathFunction *function)
	{
		RSGISVectors vectorUtils;
		RSGISMatrices matrixUtils;
		
		double startTemp = 1000000;
		unsigned int runsStep = 10; // Number of runs at each step size
		unsigned int runsTemp = 10; // Number of times step is changed at each temperature
		double cooling = 0.85; // Cooling factor
		unsigned int maxItt = 100000; // Maximum number of itterations*/
		
		unsigned int nCoeff = function->numCoefficients();
		unsigned int numItt = 0;
		double temp = startTemp;
		double stepRand = 0.0;
		double newEnergy = 0.0;
		double boltzmanProb = 0;
		
		double *currentCoeffError = new double[nCoeff + 1];
		double *testCoeff = new double[nCoeff];
		double *bestCoeffError = new double[nCoeff + 1];
		double *accepted = new double[nCoeff];
		double *stepSize = new double[nCoeff];
		
		gsl_rng *randgsl = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (randgsl, 0);
		
		// Set current and best parameters to 1
		for (unsigned int j = 0; j < nCoeff; j++) 
		{
			currentCoeffError[j] = gsl_vector_get(outCoeffError,j);
			testCoeff[j] = gsl_vector_get(outCoeffError,j);
			bestCoeffError[j] = gsl_vector_get(outCoeffError,j);
			stepSize[j] = sqrt(pow(gsl_vector_get(outCoeffError,j)/2,2));
		}
		// Set energy to Lage number;
		currentCoeffError[nCoeff] = 1e10;
		bestCoeffError[nCoeff] = 1e10;
		
		/* Set maximum number of temperature runs
		 Divide by runsStep * runsTemp * 2 so temperature will be reduced in maximum number of itterations
		 */
		unsigned int tRuns = maxItt / (runsStep * runsTemp * 2); 
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease tempreature
			temp = pow(cooling,((double)t)) * startTemp;
			
			// Reset step size
			for (unsigned int j = 0; j < nCoeff; j++) 
			{
				/* Set step size to half best coeffiencts.
				 As initial parameters may be different scales to true
				 values it makes more sense to chose a step size based on the best available information
				 which is the current best estimate */
				stepSize[j] = bestCoeffError[j] / 2;
			}
			
			for(unsigned int n = 0; n < runsTemp; n++)
			{
				for (unsigned int j = 0; j < nCoeff; j++) 
				{
					accepted[j] = 0; // Reset number of accepted
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					// Loop through parameters
					for(unsigned int j = 0; j < nCoeff; j++)
					{
						if(fixCoeff[j] == false) // If coefficiens are not fixed move though.
						{
							// Select new vector
							stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
							testCoeff[j] = currentCoeffError[j] + (stepRand * stepSize[j]);
							
							// Update coefficeints in function
							function->updateCoefficents(testCoeff);
							
							// Calculate Chi Squared
							newEnergy = this->chiSq1Var(indVar, depVar, variance, function);
							
							if(newEnergy < currentCoeffError[nCoeff]) // If new energy is lower accept
							{
								currentCoeffError[j] = testCoeff[j];
								
								currentCoeffError[nCoeff] = newEnergy;
								accepted[j]++;
								
								if(currentCoeffError[nCoeff] < bestCoeffError[nCoeff]) // If new energy is less than best, update best.
								{
									bestCoeffError[j] = testCoeff[j];
									bestCoeffError[nCoeff] = currentCoeffError[nCoeff];
								}
								
							}
							else // If new energy is lower, accept based on Boltzman probability
							{
								boltzmanProb = exp((-1*(newEnergy - currentCoeffError[nCoeff]))/temp);
								if (boltzmanProb > gsl_rng_uniform(randgsl))
								{
									currentCoeffError[j] = testCoeff[j];
									currentCoeffError[nCoeff] = newEnergy;
									accepted[j]++;
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
				}
				
			}
		}
		
		// Set output to best value
		for (unsigned int j = 0; j < nCoeff + 1; j++) 
		{
			gsl_vector_set(outCoeffError, j, bestCoeffError[j]);
		}
		
		// Tidy
		delete[] currentCoeffError;
		delete[] testCoeff;
		delete[] bestCoeffError;
		delete[] accepted;
		delete[] stepSize;
		gsl_rng_free(randgsl);
		
		// Exit
		return 0;
		
	}
	
	int RSGISNonLinearFit::minimiseChiSqSA2Var(gsl_matrix *indVar, gsl_vector *depVar, gsl_vector *variance, gsl_vector *outCoeffError, bool *fixCoeff, RSGISMathTwoVariableFunction *function)
	{
		RSGISVectors vectorUtils;
		RSGISMatrices matrixUtils;
		
		double startTemp = 1000000;
		unsigned int runsStep = 10; // Number of runs at each step size
		unsigned int runsTemp = 10; // Number of times step is changed at each temperature
		double cooling = 0.85; // Cooling factor
		unsigned int maxItt = 100000; // Maximum number of itterations*/
				
		unsigned int nCoeff = function->numCoefficients();
		unsigned int numItt = 0;
		double temp = startTemp;
		double stepRand = 0.0;
		double newEnergy = 0.0;
		double boltzmanProb = 0;
		
		double *currentCoeffError = new double[nCoeff + 1];
		double *testCoeff = new double[nCoeff];
		double *bestCoeffError = new double[nCoeff + 1];
		double *accepted = new double[nCoeff];
		double *stepSize = new double[nCoeff];
		
		gsl_rng *randgsl = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (randgsl, 0);
		
		// Set current and best parameters to 1
		for (unsigned int j = 0; j < nCoeff; j++) 
		{
			currentCoeffError[j] = gsl_vector_get(outCoeffError,j);
			testCoeff[j] = gsl_vector_get(outCoeffError,j);
			bestCoeffError[j] = gsl_vector_get(outCoeffError,j);
			stepSize[j] = sqrt(pow(gsl_vector_get(outCoeffError,j)/2,2));
		}
		// Set energy to Lage number;
		currentCoeffError[nCoeff] = 1e10;
		bestCoeffError[nCoeff] = 1e10;
		
		/* Set maximum number of temperature runs
		 Divide by runsStep * runsTemp * 2 so temperature will be reduced in maximum number of itterations
		 */
		unsigned int tRuns = maxItt / (runsStep * runsTemp * 2); 
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease tempreature
			temp = pow(cooling,((double)t)) * startTemp;
			
			// Reset step size
			for (unsigned int j = 0; j < nCoeff; j++) 
			{
				/* Set step size to half best coeffiencts.
				 As initial parameters may be different scales to true
				 values it makes more sense to chose a step size based on the best available information
				 which is the current best estimate */
				{
					stepSize[j] = bestCoeffError[j] / 2;
				}
			}
			
			for(unsigned int n = 0; n < runsTemp; n++)
			{
				for (unsigned int j = 0; j < nCoeff; j++) 
				{
					accepted[j] = 0; // Reset number of accepted
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					// Loop through parameters
					for(unsigned int j = 0; j < nCoeff; j++)
					{
						if(fixCoeff[j] == false)
						{
							// Select new vector
							stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
							testCoeff[j] = currentCoeffError[j] + (stepRand * stepSize[j]);
							
							// Update coefficeints in function
							function->updateCoefficents(testCoeff);
							
							// Calculate Chi Squared
							newEnergy = this->chiSq2Var(indVar, depVar, variance, function);
							
							if(newEnergy < currentCoeffError[nCoeff]) // If new energy is lower accept
							{
								currentCoeffError[j] = testCoeff[j];
								
								currentCoeffError[nCoeff] = newEnergy;
								accepted[j]++;
								
								if(currentCoeffError[nCoeff] < bestCoeffError[nCoeff]) // If new energy is less than best, update best.
								{
									bestCoeffError[j] = testCoeff[j];
									bestCoeffError[nCoeff] = currentCoeffError[nCoeff];
								}
								
							}
							else // If new energy is lower, accept based on Boltzman probability
							{
								boltzmanProb = exp((-1*(newEnergy - currentCoeffError[nCoeff]))/temp);
								if (boltzmanProb > gsl_rng_uniform(randgsl)) 
								{
									currentCoeffError[j] = testCoeff[j];
									currentCoeffError[nCoeff] = newEnergy;
									accepted[j]++;
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
				}

			}
		}
		
		// Set output to best value
		for (unsigned int j = 0; j < nCoeff + 1; j++) 
		{
			gsl_vector_set(outCoeffError, j, bestCoeffError[j]);
		}
		
		// Tidy
		delete[] currentCoeffError;
		delete[] testCoeff;
		delete[] bestCoeffError;
		delete[] accepted;
		delete[] stepSize;
		gsl_rng_free(randgsl);
		
		// Exit
		return 0;
		
	}

	double RSGISNonLinearFit::chiSq1Var(gsl_vector *indVar, gsl_vector *depVar, gsl_vector *variance, RSGISMathFunction *function)
	{
		double chiSq = 0;
		
		for (unsigned int i = 0; i < depVar->size ; i++) 
		{
			// [(y - f(X)) / sigma]^2
			
			double y = gsl_vector_get(depVar, i);
			double x0 = gsl_vector_get(indVar, i);
			double sigma = gsl_vector_get(variance, i);
			double predictY = function->calcFunction(x0);
			double chiElement = y - predictY;
			double chiElementVar = chiElement / sigma;
			double chiElementVarSq = chiElementVar * chiElementVar;
			chiSq = chiSq + chiElementVarSq;
		}
		
		return chiSq;
	}
	
	double RSGISNonLinearFit::chiSq2Var(gsl_matrix *indVar, gsl_vector *depVar, gsl_vector *variance, RSGISMathTwoVariableFunction *function)
	{
		double chiSq = 0;
		
		for (unsigned int i = 0; i < depVar->size ; i++) 
		{
			// [(y - f(X)) / sigma]^2
			
			double y = gsl_vector_get(depVar, i);
			double x0 = gsl_matrix_get(indVar, i, 0);
			double x1 = gsl_matrix_get(indVar, i, 1);
			double sigma = gsl_vector_get(variance, i);
			double predictY = function->calcFunction(x0, x1);
			double chiElement = y - predictY;
			double chiElementVar = chiElement / sigma;
			double chiElementVarSq = chiElementVar * chiElementVar;
			chiSq = chiSq + chiElementVarSq;
		}
		
		return chiSq;
	}
}}
