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
	
	RSGISEstimationSimulatedAnnealing2Var2Data::RSGISEstimationSimulatedAnnealing2Var2Data(
																						   rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
																						   rsgis::math::RSGISMathTwoVariableFunction *functionHV,
																						   double *minMaxIntervalA,
																						   double *minMaxIntervalB,
																						   double minEnergy,
																						   double startTemp,
																						   unsigned int runsStep,
																						   unsigned int runsTemp,
																						   double cooling,
																						   unsigned int maxItt)
	{
		/*
		 this->startTemp = 1000000;
		 this->runsStep = 10; // Number of runs at each step size
		 this->runsTemp = 20; // Number of times step is changed at each temperature
		 this->cooling = 0.85; // Cooling factor
		 this->minEnergy = minEnergy; // Set the target energy
		 this->maxItt = 100000; // Maximum number of itterations*/
		
		this->startTemp = startTemp;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsTemp = runsTemp; // Number of times step is changed at each temperature
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
	int RSGISEstimationSimulatedAnnealing2Var2Data::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		// Check for no-data
		unsigned int nData = 2;
		for(unsigned int i = 0;i < nData; i++)
		{
			if (boost::math::isnan(gsl_vector_get(inData, i))) 
			{
				for(unsigned int j = 0; j < nPar; j++) 
				{
					gsl_vector_set(outParError, j, gsl_vector_get(initialPar, j));
				}
				gsl_vector_set(outParError, nPar, 999);
				return 0;
			}
		}
		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double temp = startTemp;
		double stepRand = 0.0;
		double newEnergy = 0.0;
		double boltzmanProb = 0;
		
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
		
		/* Set maximum number of temperature runs
		 Divide by runsStep * runsTemp * 2 so temperature will be reduced in maximum number of itterations
		 */
		
		unsigned int tRuns = maxItt / (runsStep * runsTemp * 2); 
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease tempreature
			if((t == (tRuns - 1)) | (numItt < (runsStep * runsTemp) )) // Last Run
			{
				// Set temperature to zero
				temp = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				temp = cooling * temp; //pow(this->cooling,double(t)) * startTemp;
			}
			
			// Reset step size
			for (unsigned int j = 0; j < nPar; j++) 
			{
				stepSize[j] = initialStepSize[j] / 2;
			}
			
			for(unsigned int n = 0; n < runsTemp; n++)
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
							
							//cout << "Predict HH " << functionHH->calcFunction(testPar[0], testPar[1]) << endl;
							
							//cout << "newEnergy = " << newEnergy << " old energy = " << currentParError[nPar] << endl;
							
							//cout << "testPar = " << testPar[j] << " currentParError = " << currentParError[j] << endl;
							
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
										//cout << "itterations = " << numItt << endl;
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
								boltzmanProb = exp((-1*(newEnergy - currentParError[nPar]))/temp);
								//cout << "boltzmanProb = " << boltzmanProb << endl;
								if (boltzmanProb > gsl_rng_uniform(randgsl)) 
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
							
						}
						numItt++;
					}
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < nPar; j++)
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
				
				//cout << "step size = " << stepSize[0] << " " << stepSize[1] << endl;
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
	RSGISEstimationSimulatedAnnealing2Var2Data::~RSGISEstimationSimulatedAnnealing2Var2Data()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
	}
	
	RSGISEstimationSimulatedAnnealing2Var2DataWithAP::RSGISEstimationSimulatedAnnealing2Var2DataWithAP(
																									   rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
																									   rsgis::math::RSGISMathTwoVariableFunction *functionHV,
																									   double *minMaxIntervalA,
																									   double *minMaxIntervalB,
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
		/*
		 this->startTemp = 1000000;
		 this->runsStep = 10; // Number of runs at each step size
		 this->runsTemp = 20; // Number of times step is changed at each temperature
		 this->cooling = 0.85; // Cooling factor
		 this->minEnergy = minEnergy; // Set the target energy
		 this->maxItt = 100000; // Maximum number of itterations*/
		rsgis::math::RSGISMatrices matrixUtils;
		
		this->startTemp = startTemp;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsTemp = runsTemp; // Number of times step is changed at each temperature
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
	int RSGISEstimationSimulatedAnnealing2Var2DataWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		
		// Check for no-data
		unsigned int nData = 2;
		for(unsigned int i = 0;i < nData; i++)
		{
			if (boost::math::isnan(gsl_vector_get(inData, i))) 
			{
				for(unsigned int j = 0; j < nPar; j++) 
				{
					gsl_vector_set(outParError, j, gsl_vector_get(initialPar, j));
				}
				gsl_vector_set(outParError, nPar, 999);
				return 0;
			}
		}
		
		// Open text file to write errors to
		 /*ofstream outTxtFile;
		 outTxtFile.open("/Users/danclewley/Documents/Research/PhD/Inversion/ALOS/PLR/SAIttTest/SAIttTest.csv");
		 outTxtFile << "itt,bestHErr,bestDErr,CurrHErr,CurrDErr,lsDiff" << endl;*/
		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double temp = startTemp;
		double stepRand = 0.0;
		double newEnergy = 0.0;
		double boltzmanProb = 0;
		
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
		
		/* Set maximum number of temperature runs
		 Divide by runsStep * runsTemp * 2 so temperature will be reduced in maximum number of itterations
		 */
		
		unsigned int tRuns = maxItt / (runsStep * runsTemp * 5);
		

		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease tempreature
			if((t == (tRuns - 1)) | (numItt < (runsStep * runsTemp) )) // Last Run
			{
				// Set temperature to zero
				temp = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				temp = cooling * temp; //pow(this->cooling,double(t)) * startTemp;
				// Reset step size
				for (unsigned int j = 0; j < nPar; j++) 
				{
					stepSize[j] = initialStepSize[j];
				}
			}
			for(unsigned int n = 0; n < runsTemp; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					if (numItt > maxItt) 
					{
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
						return 0;
					}
					
					bool withinLimits = true;
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{
						withinLimits = true;
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if (testPar[j] < lowerLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
						else if(testPar[j] > upperLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
					}
					if (withinLimits) 
					{
						// Calculate energy
						newEnergy = leastSquares->calcFunction(testPar[0], testPar[1]);

						double prevEnergy = currentParError[nPar];
						if(newEnergy < currentParError[nPar]) // If new energy is lower accept
						{
							
							for (unsigned int j = 0; j < nPar; j++)
							{
								currentParError[j] = testPar[j];
								accepted[j]++;
							}
							
							// Calculate rel error (Test)
							/*double realCDepth = 2.270901;
							double realDensity = 0.4444736;
							
							double bestcDepthError = sqrt(pow((realCDepth - bestParError[0]),2)) / realCDepth; 
							double bestdensError = sqrt(pow((realDensity - bestParError[1]),2)) / realDensity;
							
							double currentcDepthError = sqrt(pow((realCDepth - currentParError[0]),2)) / realCDepth; 
							double currentdensError = sqrt(pow((realDensity - currentParError[1]),2)) / realDensity;
							
							outTxtFile << numItt << "," << bestcDepthError << "," << bestdensError << "," << currentcDepthError << "," << currentdensError << "," << newEnergy << endl;*/
							
							
							currentParError[nPar] = newEnergy;
							
							if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
							{
								
								//cout << numItt << "," << newEnergy << endl;
								
								//cout << "testPar = " << testPar[j] << " currentParError = " << currentParError[j] << endl;
								
								for(unsigned int par = 0; par < nPar; par++)
								{
									bestParError[par] = currentParError[par];
								}
								bestParError[nPar] = currentParError[nPar];
								
								if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
								{
									//cout << "itterations = " << numItt << endl;
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
							boltzmanProb = exp((-1*(newEnergy - currentParError[nPar]))/temp);
							//cout << "boltzmanProb = " << boltzmanProb << endl;
							//if ((boltzmanProb > gsl_rng_uniform(randgsl)) && (newEnergy < 1)) 
							if ((boltzmanProb > gsl_rng_uniform(randgsl))) 
							{
								// Calculate rel error (Test)
								/*double realCDepth = 2.270901;
								double realDensity = 0.4444736;
								
								double bestcDepthError = sqrt(pow((realCDepth - bestParError[0]),2)) / realCDepth; 
								double bestdensError = sqrt(pow((realDensity - bestParError[1]),2)) / realDensity;
								
								double currentcDepthError = sqrt(pow((realCDepth - currentParError[0]),2)) / realCDepth; 
								double currentdensError = sqrt(pow((realDensity - currentParError[1]),2)) / realDensity;
								
								outTxtFile << numItt << "," << bestcDepthError << "," << bestdensError << "," << currentcDepthError << "," << currentdensError << "," << newEnergy << endl;*/
								
								//cout << numItt << "," << newEnergy << endl;
								
								//cout << "testPar = " << testPar[j] << " currentParError = " << currentParError[j] << endl;
								
								for (unsigned int j = 0; j < nPar; j++)
								{
									currentParError[j] = testPar[j];
									accepted[j]++;
								}
								currentParError[nPar] = newEnergy;
							}
						}
						if(abs(newEnergy - prevEnergy)  < 10e-7)
						{
							temp = startTemp;
							t = 0;
							for (unsigned int j = 0; j < nPar; j++) 
							{
								currentParError[j] = gsl_vector_get(initialPar, j);
							}
						}
						
					}
					numItt++;
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < nPar; j++)
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
				
				//cout << "step size = " << stepSize[0] << " " << stepSize[1] << endl;
			}
			
			if (t == (tRuns - 1)) 
			{
				temp = startTemp;
				t = 0;
				for (unsigned int j = 0; j < nPar; j++) 
				{
					currentParError[j] = gsl_vector_get(initialPar, j);
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
	RSGISEstimationSimulatedAnnealing2Var2DataWithAP::~RSGISEstimationSimulatedAnnealing2Var2DataWithAP()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
		gsl_matrix_free(invCovMatrixP);
	}
	
	RSGISEstimationSimulatedAnnealing2Var3Data::RSGISEstimationSimulatedAnnealing2Var3Data(
																						   rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
																						   rsgis::math::RSGISMathTwoVariableFunction *functionHV,
																						   rsgis::math::RSGISMathTwoVariableFunction *functionVV,
																						   double *minMaxIntervalA,
																						   double *minMaxIntervalB,
																						   double minEnergy,
																						   double startTemp,
																						   unsigned int runsStep,
																						   unsigned int runsTemp,
																						   double cooling,
																						   unsigned int maxItt)
	{
		/*
		 this->startTemp = 1000000;
		 this->runsStep = 10; // Number of runs at each step size
		 this->runsTemp = 20; // Number of times step is changed at each temperature
		 this->cooling = 0.85; // Cooling factor
		 this->minEnergy = minEnergy; // Set the target energy
		 this->maxItt = 100000; // Maximum number of itterations*/
		
		this->startTemp = startTemp;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsTemp = runsTemp; // Number of times step is changed at each temperature
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
	int RSGISEstimationSimulatedAnnealing2Var3Data::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		// Check for no-data
		unsigned int nData = 3;
		for(unsigned int i = 0;i < nData; i++)
		{
			if (boost::math::isnan(gsl_vector_get(inData, i))) 
			{
				for(unsigned int j = 0; j < nPar; j++) 
				{
					gsl_vector_set(outParError, j, gsl_vector_get(initialPar, j));
				}
				gsl_vector_set(outParError, nPar, 999);
				return 0;
			}
		}
		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double temp = startTemp;
		double stepRand = 0.0;
		double newEnergy = 0.0;
		double boltzmanProb = 0;
		
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
		
		/* Set maximum number of temperature runs
		 Divide by runsStep * runsTemp * 2 so temperature will be reduced in maximum number of itterations
		 */
		
		unsigned int tRuns = maxItt / (runsStep * runsTemp * 2); 
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease tempreature
			if((t == (tRuns - 1)) | (numItt < (runsStep * runsTemp) )) // Last Run
			{
				// Set temperature to zero
				temp = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				temp = cooling * temp; //pow(this->cooling,double(t)) * startTemp;
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
			
			for(unsigned int n = 0; n < runsTemp; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					bool withinLimits = true;
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{
						withinLimits = true;
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if (testPar[j] < lowerLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
						else if(testPar[j] > upperLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
					}
					if (withinLimits) 
					{
						// Calculate energy
						newEnergy = leastSquares->calcFunction(testPar[0], testPar[1]);
						
						// Calculate rel error (Test)
						/*double realCDepth = 1.2474;
						 double realDensity = 0.505;
						 
						 double bestcDepthError = sqrt(pow((realCDepth - bestParError[0]),2)) / realCDepth; 
						 double bestdensError = sqrt(pow((realDensity - bestParError[1]),2)) / realDensity;
						 
						 double currentcDepthError = sqrt(pow((realCDepth - currentParError[0]),2)) / realCDepth; 
						 double currentdensError = sqrt(pow((realDensity - currentParError[1]),2)) / realDensity;
						 
						 outTxtFile << numItt << "," << bestcDepthError << "," << bestdensError << "," << currentcDepthError << "," << currentdensError << "," << bestParError[nPar] << endl;*/
						
						//cout << numItt << "," << newEnergy << endl;
						
						
						//cout << "newEnergy = " << newEnergy << " old energy = " << currentParError[nPar] << endl;
						
						//cout << "testPar = " << testPar[j] << " currentParError = " << currentParError[j] << endl;
						double prevEnergy = currentParError[nPar];
						if(newEnergy < currentParError[nPar]) // If new energy is lower accept
						{
							for (unsigned int j = 0; j < nPar; j++)
							{
								currentParError[j] = testPar[j];
								accepted[j]++;
							}
							
							currentParError[nPar] = newEnergy;
							
							if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
							{
								for(unsigned int par = 0; par < nPar; par++)
								{
									bestParError[par] = currentParError[par];
								}
								bestParError[nPar] = currentParError[nPar];
								
								if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
								{
									//cout << "itterations = " << numItt << endl;
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
							boltzmanProb = exp((-1*(newEnergy - currentParError[nPar]))/temp);
							//cout << "boltzmanProb = " << boltzmanProb << endl;
							if (boltzmanProb > gsl_rng_uniform(randgsl)) 
							{
								for (unsigned int j = 0; j < nPar; j++)
								{
									currentParError[j] = testPar[j];
									accepted[j]++;
								}
								currentParError[nPar] = newEnergy;
							}
						}
						if(abs(newEnergy - prevEnergy)  < 10e-7)
						{
							temp = startTemp;
							t = 0;
							for (unsigned int j = 0; j < nPar; j++) 
							{
								currentParError[j] = gsl_vector_get(initialPar, j);
							}
						}
						
					}
					numItt++;
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < nPar; j++)
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
				
				//cout << "step size = " << stepSize[0] << " " << stepSize[1] << endl;
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
	RSGISEstimationSimulatedAnnealing2Var3Data::~RSGISEstimationSimulatedAnnealing2Var3Data()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
	}
	
	RSGISEstimationSimulatedAnnealing2Var3DataWithAP::RSGISEstimationSimulatedAnnealing2Var3DataWithAP(
																									   rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
																									   rsgis::math::RSGISMathTwoVariableFunction *functionHV,
																									   rsgis::math::RSGISMathTwoVariableFunction *functionVV,
																									   double *minMaxIntervalA,
																									   double *minMaxIntervalB,
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
		/*
		 this->startTemp = 1000000;
		 this->runsStep = 10; // Number of runs at each step size
		 this->runsTemp = 20; // Number of times step is changed at each temperature
		 this->cooling = 0.85; // Cooling factor
		 this->minEnergy = minEnergy; // Set the target energy
		 this->maxItt = 100000; // Maximum number of itterations*/
		rsgis::math::RSGISMatrices matrixUtils;
		
		this->startTemp = startTemp;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsTemp = runsTemp; // Number of times step is changed at each temperature
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
	int RSGISEstimationSimulatedAnnealing2Var3DataWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		// Check for no-data
		unsigned int nData = 3;
		for(unsigned int i = 0;i < nData; i++)
		{
			if (boost::math::isnan(gsl_vector_get(inData, i))) 
			{
				for(unsigned int j = 0; j < nPar; j++) 
				{
					gsl_vector_set(outParError, j, gsl_vector_get(initialPar, j));
				}
				gsl_vector_set(outParError, nPar, 999);
				return 0;
			}
		}
		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double temp = startTemp;
		double stepRand = 0.0;
		double newEnergy = 0.0;
		double boltzmanProb = 0;
		
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
		
		/* Set maximum number of temperature runs
		 Divide by runsStep * runsTemp * 2 so temperature will be reduced in maximum number of itterations
		 */
		
		
		unsigned int tRuns = maxItt / (runsStep * runsTemp * 5);
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease tempreature
			if((t == (tRuns - 1)) | (numItt < (runsStep * runsTemp) )) // Last Run
			{
				// Set temperature to zero
				temp = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				temp = cooling * temp; //pow(this->cooling,double(t)) * startTemp;
			}
			
			// Reset step size
			for (unsigned int j = 0; j < nPar; j++) 
			{
				stepSize[j] = initialStepSize[j] / 2;
			}
			
			for(unsigned int n = 0; n < runsTemp; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					if(numItt > maxItt)
					{
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
						return 0;
					}
					
					bool withinLimits = true;
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{
						withinLimits = true;
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if (testPar[j] < lowerLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
						else if(testPar[j] > upperLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
					}
					if (withinLimits) 
					{
						// Calculate energy
						newEnergy = leastSquares->calcFunction(testPar[0], testPar[1]);
						
						// Calculate rel error (Test)
						/*double realCDepth = 1.2474;
						 double realDensity = 0.505;
						 
						 double bestcDepthError = sqrt(pow((realCDepth - bestParError[0]),2)) / realCDepth; 
						 double bestdensError = sqrt(pow((realDensity - bestParError[1]),2)) / realDensity;
						 
						 double currentcDepthError = sqrt(pow((realCDepth - currentParError[0]),2)) / realCDepth; 
						 double currentdensError = sqrt(pow((realDensity - currentParError[1]),2)) / realDensity;
						 
						 outTxtFile << numItt << "," << bestcDepthError << "," << bestdensError << "," << currentcDepthError << "," << currentdensError << "," << bestParError[nPar] << endl;*/
						
						//cout << numItt << "," << newEnergy << endl;
						
						
						//cout << "newEnergy = " << newEnergy << " old energy = " << currentParError[nPar] << endl;
						
						//cout << "testPar = " << testPar[j] << " currentParError = " << currentParError[j] << endl;
						double prevEnergy = currentParError[nPar];
						if(newEnergy < currentParError[nPar]) // If new energy is lower accept
						{
							for (unsigned int j = 0; j < nPar; j++)
							{
								currentParError[j] = testPar[j];
								accepted[j]++;
							}
							
							currentParError[nPar] = newEnergy;
							
							if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
							{
								for(unsigned int par = 0; par < nPar; par++)
								{
									bestParError[par] = currentParError[par];
								}
								bestParError[nPar] = currentParError[nPar];
								
								if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
								{
									//cout << "itterations = " << numItt << endl;
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
							boltzmanProb = exp((-1*(newEnergy - currentParError[nPar]))/temp);
							//cout << "boltzmanProb = " << boltzmanProb << endl;
							if (boltzmanProb > gsl_rng_uniform(randgsl)) 
							{
								for (unsigned int j = 0; j < nPar; j++)
								{
									currentParError[j] = testPar[j];
									accepted[j]++;
								}
								currentParError[nPar] = newEnergy;
							}
						}
						if(abs(newEnergy - prevEnergy) < 1e-10)
						{
							temp = startTemp;
							t = 0;
							for (unsigned int j = 0; j < nPar; j++) 
							{
								currentParError[j] = gsl_vector_get(initialPar, j);
							}
						}
						
					}
					numItt++;
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < nPar; j++)
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
				
				//cout << "step size = " << stepSize[0] << " " << stepSize[1] << endl;
			}
			
			if (t == (tRuns - 1)) 
			{
				temp = startTemp;
				t = 0;
				for (unsigned int j = 0; j < nPar; j++) 
				{
					currentParError[j] = gsl_vector_get(initialPar, j);
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
	RSGISEstimationSimulatedAnnealing2Var3DataWithAP::~RSGISEstimationSimulatedAnnealing2Var3DataWithAP()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
		gsl_matrix_free(invCovMatrixP);
	}
	
	RSGISEstimationSimulatedAnnealing3Var3DataWithAP::RSGISEstimationSimulatedAnnealing3Var3DataWithAP(
																									   rsgis::math::RSGISMathThreeVariableFunction *functionHH, 
																									   rsgis::math::RSGISMathThreeVariableFunction *functionHV,
																									   rsgis::math::RSGISMathThreeVariableFunction *functionVV,
																									   double *minMaxIntervalA,
																									   double *minMaxIntervalB,
																									   double *minMaxIntervalC,
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
		/*
		 this->startTemp = 1000000;
		 this->runsStep = 10; // Number of runs at each step size
		 this->runsTemp = 20; // Number of times step is changed at each temperature
		 this->cooling = 0.85; // Cooling factor
		 this->minEnergy = minEnergy; // Set the target energy
		 this->maxItt = 100000; // Maximum number of itterations*/
		
		this->startTemp = startTemp;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsTemp = runsTemp; // Number of times step is changed at each temperature
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
	int RSGISEstimationSimulatedAnnealing3Var3DataWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		
		// Check for no-data
		unsigned int nData = 3;
		for(unsigned int i = 0;i < nData; i++)
		{
			if (boost::math::isnan(gsl_vector_get(inData, i))) 
			{
				for(unsigned int j = 0; j < nPar; j++) 
				{
					gsl_vector_set(outParError, j, gsl_vector_get(initialPar, j));
				}
				gsl_vector_set(outParError, nPar, 999);
				return 0;
			}
		}
		
		// Open text file to write errors to
		/*ofstream outTxtFile;
		 outTxtFile.open("/Users/danclewley/Documents/Research/PhD/Inversion/AIRSAR/PBX/WithDielectric/SensitivityAnalysis/SAConvTests/SAIttTest.csv");
		 outTxtFile << "itt,bestHErr,CurrHErr,bestDErr,CurrDErr,bestEpsErr,CurrEpsErr" << endl;*/
		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double temp = startTemp;
		double stepRand = 0.0;
		double newEnergy = 0.0;
		double boltzmanProb = 0;
		
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
		
		/* Set maximum number of temperature runs
		 Divide by runsStep * runsTemp * nPar so temperature will be reduced in maximum number of itterations
		 */
		
		unsigned int tRuns = maxItt / (runsStep * runsTemp * 5);

		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease tempreature
			if((t == (tRuns - 1)) | (numItt < (runsStep * runsTemp) )) // Last Run
			{
				// Set temperature to zero
				temp = 0;
				for (unsigned int l = 0; l < nPar; l++) 
				{
					currentParError[l] = bestParError[l];
				}
			}
			else 
			{
				temp = cooling * temp; //pow(this->cooling,double(t)) * startTemp;
				// Reset step size
				for (unsigned int j = 0; j < nPar; j++) 
				{
					stepSize[j] = initialStepSize[j] / 2;
				}
			}
			
			for(unsigned int n = 0; n < runsTemp; n++)
			{
				for(unsigned int m = 0; m < runsStep; m++)
				{
					if (numItt > maxItt) 
					{
						double error = 0.0;
						error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // Predicted - Measured data (HH)
						error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (HV)
						error = error + pow((gsl_vector_get(inData, 2) - functionVV->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (VV)
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
						return 0;
					}
					bool withinLimits = true;
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{
						withinLimits = true;
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if (testPar[j] < lowerLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
						else if(testPar[j] > upperLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
					}
					if (withinLimits) 
					{
						// Calculate energy
						newEnergy = leastSquares->calcFunction(testPar[0], testPar[1], testPar[2]);
						
						double prevEnergy = currentParError[nPar];
						if(newEnergy < currentParError[nPar]) // If new energy is lower accept
						{
							for (unsigned int j = 0; j < nPar; j++)
							{
								currentParError[j] = testPar[j];
								accepted[j]++;
							}
							
							currentParError[nPar] = newEnergy;
							
							if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
							{
								for(unsigned int par = 0; par < nPar; par++)
								{
									bestParError[par] = currentParError[par];
								}
								bestParError[nPar] = currentParError[nPar];
								
								if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
								{
									//cout << "itterations = " << numItt << endl;
									// Calculate normalised error (differnt to least squares difference)
									double error = 0.0;
									error = pow((gsl_vector_get(inData, 0) - functionHH->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // Predicted - Measured data (HH)
									error = error + pow((gsl_vector_get(inData, 1) - functionHV->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (HV)
									error = error + pow((gsl_vector_get(inData, 2) - functionVV->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (VV)
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
							boltzmanProb = exp((-1*(newEnergy - currentParError[nPar]))/temp);
							//cout << "boltzmanProb = " << boltzmanProb << endl;
							if (boltzmanProb > gsl_rng_uniform(randgsl)) 
							{
								for (unsigned int j = 0; j < nPar; j++)
								{
									currentParError[j] = testPar[j];
									accepted[j]++;
								}
								currentParError[nPar] = newEnergy;
							}
						}
						if(abs(newEnergy - prevEnergy)  < 10e-7)
						{							
							temp = startTemp;
							t = 0;
						}
					}
					numItt++;
					
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < nPar; j++)
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
				
				
				//cout << "step size = " << stepSize[0] << " " << stepSize[1] << endl;
			}
			
			if (t == (tRuns - 1)) 
			{
				temp = startTemp;
				t = 0;
				for (unsigned int j = 0; j < nPar; j++) 
				{
					currentParError[j] = gsl_vector_get(initialPar, j);
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
		
		//cout << "..nItt = " << numItt << "..";
		
		// Exit
		return 0;
		
	}
	RSGISEstimationSimulatedAnnealing3Var3DataWithAP::~RSGISEstimationSimulatedAnnealing3Var3DataWithAP()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
		gsl_matrix_free(invCovMatrixP);
	}
	
	RSGISEstimationSimulatedAnnealing3Var4DataWithAP::RSGISEstimationSimulatedAnnealing3Var4DataWithAP(
																									   rsgis::math::RSGISMathThreeVariableFunction *function1, 
																									   rsgis::math::RSGISMathThreeVariableFunction *function2, 
																									   rsgis::math::RSGISMathThreeVariableFunction *function3, 
																									   rsgis::math::RSGISMathThreeVariableFunction *function4, 
																									   double *minMaxIntervalA,
																									   double *minMaxIntervalB,
																									   double *minMaxIntervalC,
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
		/*
		 this->startTemp = 1000000;
		 this->runsStep = 10; // Number of runs at each step size
		 this->runsTemp = 20; // Number of times step is changed at each temperature
		 this->cooling = 0.85; // Cooling factor
		 this->minEnergy = minEnergy; // Set the target energy
		 this->maxItt = 100000; // Maximum number of itterations*/
		
		this->startTemp = startTemp;
		this->runsStep = runsStep; // Number of runs at each step size
		this->runsTemp = runsTemp; // Number of times step is changed at each temperature
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
	int RSGISEstimationSimulatedAnnealing3Var4DataWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{
		// Check for no-data
		unsigned int nData = 4;
		for(unsigned int i = 0;i < nData; i++)
		{
			if (boost::math::isnan(gsl_vector_get(inData, i))) 
			{
				for(unsigned int j = 0; j < nPar; j++) 
				{
					gsl_vector_set(outParError, j, gsl_vector_get(initialPar, j));
				}
				gsl_vector_set(outParError, nPar, 999);
				return 0;
			}
		}
		
		// Open text file to write errors to
		/*ofstream outTxtFile;
		 outTxtFile.open("/Users/danclewley/Documents/Research/PhD/Inversion/AIRSAR/PBX/WithDielectric/SensitivityAnalysis/SAConvTests/SAIttTest_coeff.csv");
		 outTxtFile << "itt,bestHErr,CurrHErr,bestDErr,CurrDErr,bestEpsErr,CurrEpsErr" << endl;*/
		
		/*ofstream outTxtFile;
		 outTxtFile.open("/Users/danclewley/Documents/Research/PhD/Inversion/AIRSAR/PBX/WithDielectric/SensitivityAnalysis/SAConvTests/SAPath.csv");
		 outTxtFile << "itt,height,dens,dielectric,lSq" << endl;*/
		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int numItt = 0;
		double temp = startTemp;
		double stepRand = 0.0;
		double newEnergy = 0.0;
		double boltzmanProb = 0;
		
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
		
		/*for(double height = minMaxIntervalA[0]; height < minMaxIntervalA[1]; height = height + minMaxIntervalA[2])
		 {
		 
		 for(double dens = minMaxIntervalB[0]; dens < minMaxIntervalB[1]; dens = dens + minMaxIntervalB[2])
		 {
		 cout << height << "," << dens << "," << leastSquares->calcFunction(height, dens, 40) << endl;
		 }
		 
		 }*/
		
		/* Set maximum number of temperature runs
		 Divide by runsStep * runsTemp * nPar so temperature will be reduced in maximum number of itterations
		 */
		unsigned int tRuns = maxItt / (runsStep * runsTemp * 5);
		
		for(unsigned int t = 0; t < tRuns; t++)
		{
			// Decrease tempreature
			if((t == (tRuns - 1)) | (numItt < (runsStep * runsTemp) )) // Last Run
			{
				// Set temperature to zero
				temp = 0;
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
				
				// Reset current parameters 
				/*for (unsigned int j = 0; j < nPar; j++) 
				 {
				 double upperVal = upperLimit[j] - lowerLimit[j];
				 currentParError[j] =  (gsl_rng_uniform(randgsl)*upperVal) + lowerLimit[j];
				 }*/
				temp = cooling * temp; //pow(this->cooling,double(t)) * startTemp;
				
				currentParError[0] = (minMaxIntervalA[1] - minMaxIntervalA[0]) * gsl_rng_uniform(randgsl);
				currentParError[1] = (minMaxIntervalB[1] - minMaxIntervalB[0]) * gsl_rng_uniform(randgsl);
				currentParError[2] = (minMaxIntervalC[1] - minMaxIntervalC[0]) * gsl_rng_uniform(randgsl);
				
			}
			
			for(unsigned int n = 0; n < runsTemp; n++)
			{
				for (unsigned int j = 0; j < nPar; j++) 
				{
					accepted[j] = 0;
				}
				
				for(unsigned int m = 0; m < runsStep; m++)
				{
					bool withinLimits = true;
					// Loop through parameters
					for(unsigned int j = 0; j < nPar; j++)
					{
						if (numItt > maxItt) 
						{
							// Calculate normalised error (differnt to least squares difference)
							double error = 0.0;
							error = pow((gsl_vector_get(inData, 0) - function1->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // Predicted - Measured data (1)
							error = error + pow((gsl_vector_get(inData, 1) - function2->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (2)
							error = error + pow((gsl_vector_get(inData, 2) - function3->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (3)
							error = error + pow((gsl_vector_get(inData, 3) - function4->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (4)
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
							return 0;
						}
						withinLimits = true;
						// Select new vector
						stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
						testPar[j] = currentParError[j] + (stepRand * stepSize[j]);
						
						if (testPar[j] < lowerLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
						else if(testPar[j] > upperLimit[j])
						{
							testPar[j] = currentParError[j];
							withinLimits = false;
						}
					}
					if (withinLimits) 
					{
						// Calculate energy
						newEnergy = leastSquares->calcFunction(testPar[0], testPar[1], testPar[2]);
						
						double prevEnergy = currentParError[nPar];
						if(newEnergy < currentParError[nPar]) // If new energy is lower accept
						{
							for (unsigned int j = 0; j < nPar; j++)
							{
								currentParError[j] = testPar[j];
								accepted[j]++;
							}
							
							currentParError[nPar] = newEnergy;
							
							if(currentParError[nPar] < bestParError[nPar]) // If new energy is less than best, update best.
							{
								for(unsigned int par = 0; par < nPar; par++)
								{
									bestParError[par] = currentParError[par];
								}
								bestParError[nPar] = currentParError[nPar];
								
								if (bestParError[nPar] < minEnergy) // If new energy is less than threashold return.
								{
									//cout << "itterations = " << numItt << endl;
									// Calculate normalised error (differnt to least squares difference)
									double error = 0.0;
									error = pow((gsl_vector_get(inData, 0) - function1->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // Predicted - Measured data (1)
									error = error + pow((gsl_vector_get(inData, 1) - function2->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (2)
									error = error + pow((gsl_vector_get(inData, 2) - function3->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (3)
									error = error + pow((gsl_vector_get(inData, 3) - function4->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (4)
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
							boltzmanProb = exp((-1*(newEnergy - currentParError[nPar]))/temp);
							//cout << "boltzmanProb = " << boltzmanProb << endl;
							if (boltzmanProb > gsl_rng_uniform(randgsl)) 
							{
								for (unsigned int j = 0; j < nPar; j++)
								{
									currentParError[j] = testPar[j];
									accepted[j]++;
								}
								currentParError[nPar] = newEnergy;
							}
						}
						if(abs(newEnergy - prevEnergy)  < 10e-7)
						{
							temp = startTemp;
							t = 0;
							for (unsigned int j = 0; j < nPar; j++) 
							{
								currentParError[j] = gsl_vector_get(initialPar, j);
							}
						}
					}
					numItt++;
					
				}
				
				// Change step size
				// Uses formula of Corana et al.
				for(unsigned int j = 0; j < nPar; j++)
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
				
				//cout << "step size = " << stepSize[0] << " " << stepSize[1] << endl;
			}
			if (t == (tRuns - 1)) 
			{
				temp = startTemp;
				t = 0;
				for (unsigned int j = 0; j < nPar; j++) 
				{
					currentParError[j] = gsl_vector_get(initialPar, j);
				}
			}
		}
		
		// Calculate normalised error (differnt to least squares difference)
		double error = 0.0;
		error = pow((gsl_vector_get(inData, 0) - function1->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // Predicted - Measured data (1)
		error = error + pow((gsl_vector_get(inData, 1) - function2->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (2)
		error = error + pow((gsl_vector_get(inData, 2) - function3->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (3)
		error = error + pow((gsl_vector_get(inData, 3) - function4->calcFunction(bestParError[0], bestParError[1],bestParError[2])),2); // + Predicted - Measured data (4)
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
		
		//cout << "..nItt = " << numItt << "..";
		
		// Exit
		return 0;
		
	}
	RSGISEstimationSimulatedAnnealing3Var4DataWithAP::~RSGISEstimationSimulatedAnnealing3Var4DataWithAP()
	{
		delete[] initialStepSize;
		gsl_rng_free(randgsl);
		gsl_matrix_free(invCovMatrixP);
	}
    
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
        
        this->allFunctions = allFunctions;
        
        this->minMaxIntervalAll = minMaxIntervalAll; // minA, maxA, minStepSizeA
       
        this->nPar = allFunctions->at(0)->numVariables();
        this->nData = invCovMatrixD->size1;
        
        for(unsigned int i = 0; i < allFunctions->size(); ++i)
        {
            if(allFunctions->at(i)->numVariables() != this->nPar)
            {
                throw RSGISException("All functions must have the same number of variables");
            }
        }
        
        // Set up random number generator
        this->randgsl = gsl_rng_alloc (gsl_rng_taus2);
        double seed = 0; // time(0) + rand();
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
            if(gsl_matrix_get(covMatrixP, i, i) > 1e8){this->useAP = false;}
            else{this->useAP = true;}
            gsl_matrix_set(this->invCovMatrixP, i, i, 1. / gsl_matrix_get(covMatrixP, i, i));
        }
        
        // Set up vectors for least squares calculation
        this->deltaD = gsl_vector_alloc(this->nData);
        this->deltaX = gsl_vector_alloc(this->nPar);
        this->tempD = gsl_vector_alloc(this->nData);
        this->tempX = gsl_vector_alloc(this->nPar);
        
    }
    int RSGISEstimationSimulatedAnnealingWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
    {
        this->inData = inData;
        // Check for no-data
        for(unsigned int i = 0;i < this->nData; i++)
        {
            if (boost::math::isnan(gsl_vector_get(inData, i))) 
            {
                for(unsigned int j = 0; j < this->nPar; j++) 
                {
                    gsl_vector_set(outParError, j, gsl_vector_get(initialPar, j));
                }
                gsl_vector_set(outParError, nPar, 999);
                return 0;
            }
        }
        
        // Open text file to write errors to
        /*ofstream outTxtFile;
         outTxtFile.open("/Users/danclewley/Documents/Research/USC/Inversion/AIRSAR/CP/SensitivityAnalysis/SAConvTests/SAIttTest.csv");
         outTxtFile << "itt,bestHErr,CurrHErr,bestDErr,CurrDErr,bestEpsErr,CurrEpsErr" << endl;*/
        
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
            dataPow = dataPow + pow(gsl_vector_get(inData, d),2);
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
        
        unsigned int tRuns = maxItt;// / (runsStep * runsTemp * 5);
        unsigned int t = 0;
        for(unsigned int i = 0; i < tRuns; ++i)
        {
            // Decrease tempreature
            if(i == (tRuns - 1)) // Last Run
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
                temp = pow(this->cooling,double(t)) * startTemp;
                // Reset step size
                for (unsigned int j = 0; j < this->nPar; j++) 
                {
                    stepSize[j] = initialStepSize[j];
                }
            }
            
            for(unsigned int n = 0; n < runsTemp; n++)
            {
                for (unsigned int j = 0; j < this->nPar; j++) 
                {
                    // Reset number of accepted steps;
                    accepted[j] = 0;
                }
                for(unsigned int m = 0; m < runsStep; m++)
                {	
                    bool withinLimits = true;
                    // Loop through parameters
                    //cout << "~~~~~~~~~~~~" << endl;
                    for(unsigned int j = 0; j < this->nPar; j++)
                    {
                        withinLimits = true;
                        // Select new vector
                        stepRand = (gsl_rng_uniform(randgsl)*2.0) - 1.0;
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
                        
                        if (withinLimits) 
                        {							
							double prevEnergy = currentParError->at(this->nPar);
                            // Calculate energy
                            newEnergy = this->calcLeastSquares(testPar);
                            double error = newEnergy;
                            if(newEnergy < currentParError->at(this->nPar)) // If new energy is lower accept
                            {
                                // Calculate rel error (Test)
                                /*double realCDepth = 5.74304;
                                 double realDensity = 0.0578171;
                                 double realEps = 25.8736;
                                 
                                 double bestcDepthError = sqrt(pow((realCDepth - bestParError[0]),2)) / realCDepth; 
                                 double bestdensError = sqrt(pow((realDensity - bestParError[1]),2)) / realDensity;
                                 double bestEpsError = sqrt(pow((realEps - bestParError[2]),2)) / realEps;
                                 
                                 double currentcDepthError = sqrt(pow((realCDepth - currentParError[0]),2)) / realCDepth; 
                                 double currentdensError = sqrt(pow((realDensity - currentParError[1]),2)) / realDensity;
                                 double currentEpsError = sqrt(pow((realEps - currentParError[2]),2)) / realEps;
                                 
                                 outTxtFile << numItt << "," << bestcDepthError << "," << bestdensError << "," << currentcDepthError << "," << currentdensError << "," << bestEpsError << "," << currentEpsError << endl;*/
                                
                                currentParError->at(j) = testPar->at(j);
                                accepted[j]++;
                                
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
                                if (boltzmanProb > gsl_rng_uniform(randgsl)) 
                                {
                                    currentParError->at(j) = testPar->at(j);
                                    accepted[j]++;
                                    currentParError->at(this->nPar) = newEnergy;
                                }
                            }
                            if(abs(newEnergy - prevEnergy)  < 10e-10)
                            {							
                                currentParError->at(j) = gsl_vector_get(initialPar, j);
                                temp = startTemp;
                                //t = 0;
                            }
                        }
                        
                    }
                    numItt++;
                    
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
            ++t;
            
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
        //cout << "..nItt = " << numItt << "..";
        
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
        for(unsigned int i = 0; i < allFunctions->size(); ++i)
        {
            //cout << "Channel " << i << ", inData = " << gsl_vector_get(this->inData, i) << ", outData = " <<  allFunctions->at(i)->calcFunction(values) << endl;
            dataDiff = gsl_vector_get(this->inData, i) - allFunctions->at(i)->calcFunction(values);
            gsl_vector_set(this->deltaD, i, dataDiff);
        }
        
        matrixUtils.productMatrixVectorGSL(invCovMatrixD, deltaD, tempD);
        
        double diffD = vectorUtils.dotProductVectorVectorGSL(tempD, deltaD);
        
        // || X - Xap || ^2 
        if (this->useAP) 
        {
            for(unsigned int i = 0; i < values->size(); ++i)
            {
                valueDiff = values->at(i) - gsl_vector_get(aPrioriPar, i);
                gsl_vector_set(this->deltaX, i, valueDiff);
            }
            
            matrixUtils.productMatrixVectorGSL(invCovMatrixP, deltaX, tempX);
            
            diffX = vectorUtils.dotProductVectorVectorGSL(tempX, deltaX);
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
    }
    
}}



