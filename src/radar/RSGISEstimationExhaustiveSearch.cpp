/*
 *  RSGISEstimationExhaustiveSearch.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 07/04/2010.
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

#include "RSGISEstimationExhaustiveSearch.h"

namespace rsgis {namespace radar{
	
	RSGISEstimationExhaustiveSearch2Var2Data::RSGISEstimationExhaustiveSearch2Var2Data(rsgis::math::RSGISMathTwoVariableFunction *functionHH, rsgis::math::RSGISMathTwoVariableFunction *functionHV, double *minMaxIntervalA, double *minMaxIntervalB)
	{
		this->functionHH = functionHH;
		this->functionHV = functionHV;
		this->minMaxIntervalA = minMaxIntervalA;
		this->minMaxIntervalB = minMaxIntervalB;
	}
	int RSGISEstimationExhaustiveSearch2Var2Data::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int nPar = 2;
		
		double *bestParError = new double[nPar + 1];
		double currentError;

		// Set energy;
		bestParError[nPar] = + std::numeric_limits<double>::infinity();//+INFINITY;
		currentError = + std::numeric_limits<double>::infinity();//+INFINITY;
		
		rsgis::math::RSGISMathTwoVariableFunction *leastSquares;
		//std::cout << "*********************************************************" << std::endl;
		//std::cout << "HH = " <<  gsl_vector_get(inData, 0) << " HV = " <<  gsl_vector_get(inData, 1) << std::endl;
		
		leastSquares = new rsgis::math::RSGISFunction2Var2DataLeastSquares(functionHH, functionHV, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1)); 
		
		double h = minMaxIntervalA[0];
		while (h < minMaxIntervalA[1]) 
		{
			double d = minMaxIntervalB[0];
			while(d < minMaxIntervalB[1]) 
			{
				currentError =  leastSquares->calcFunction(h, d);
				//std::cout << "Predict HH " << functionHH->calcFunction(h, d) << " Predict HV " << functionHV->calcFunction(h, d) << std::endl;
				//std::cout << "current Error = " << currentError << std::endl;
				if (currentError < bestParError[2])
				{
					bestParError[0] = h;
					bestParError[1] = d;
					bestParError[2] = currentError;
				}
				d = d + minMaxIntervalB[2];
			}
			h = h + minMaxIntervalA[2];
		}		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
			
		//std::cout << "*********************************************************" << std::endl;		
		
		// Tidy
		delete[] bestParError;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationExhaustiveSearch2Var2Data::~RSGISEstimationExhaustiveSearch2Var2Data()
	{
	}
	
	RSGISEstimationExhaustiveSearch2Var2DataExportPoints::RSGISEstimationExhaustiveSearch2Var2DataExportPoints(rsgis::math::RSGISMathTwoVariableFunction *functionHH, rsgis::math::RSGISMathTwoVariableFunction *functionHV, double *minMaxIntervalA, double *minMaxIntervalB, std::string outFilenameBase)
	{
		this->functionHH = functionHH;
		this->functionHV = functionHV;
		this->minMaxIntervalA = minMaxIntervalA;
		this->minMaxIntervalB = minMaxIntervalB;
		this->outFilenameBase = outFilenameBase;
		this->nMinimiseRuns = 0;
	}
	int RSGISEstimationExhaustiveSearch2Var2DataExportPoints::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		rsgis::math::RSGISMathsUtils mathUtils;
		
		// Update number of runs for minimiser
		this->nMinimiseRuns++;
		
		unsigned int nPar = 2;
		
		int numA = (minMaxIntervalA[1] - minMaxIntervalA[0]) / minMaxIntervalA[2];
		int numB = (minMaxIntervalB[1] - minMaxIntervalB[0]) / minMaxIntervalB[2];
		
		// set up vectors to store ouput values of h and d and least squares.
		double *aList = new double[numA * numB];
		double *bList = new double[numA * numB];
		double *lstSqList = new double[numA * numB];
		
		double *bestParError = new double[nPar + 1];
		double currentError;
		
		// Set energy;
		bestParError[nPar] = + std::numeric_limits<double>::infinity();//+INFINITY;
		currentError = + std::numeric_limits<double>::infinity();//+INFINITY;
		
		rsgis::math::RSGISMathTwoVariableFunction *leastSquares;
		
		leastSquares = new rsgis::math::RSGISFunction2Var2DataLeastSquares(functionHH, functionHV, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1)); 
		
		unsigned int i = 0;
		
		double a = minMaxIntervalA[0];
		while (a < minMaxIntervalA[1]) 
		{
			double b = minMaxIntervalB[0];
			while(b < minMaxIntervalB[1]) 
			{
				currentError =  leastSquares->calcFunction(a, b);
				if (currentError < bestParError[2])
				{
					bestParError[0] = a;
					bestParError[1] = b;
					bestParError[2] = currentError;
				}
				aList[i] = a;
				bList[i] = b;
				lstSqList[i] = currentError;
				
				b = b + minMaxIntervalB[2];
				i++;
			}
			a = a + minMaxIntervalA[2];
		}		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}

		std::string filename = this->outFilenameBase + mathUtils.inttostring(nMinimiseRuns);
		
        rsgis::utils::RSGISExportForPlotting::getInstance()->exportSurface(filename, aList, bList, lstSqList, i);
		
		// Tidy
		delete[] aList;
		delete[] bList;
		delete[] lstSqList;
		delete[] bestParError;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationExhaustiveSearch2Var2DataExportPoints::~RSGISEstimationExhaustiveSearch2Var2DataExportPoints()
	{
	}

	RSGISEstimationExhaustiveSearch2Var2DataWithAP::RSGISEstimationExhaustiveSearch2Var2DataWithAP(rsgis::math::RSGISMathTwoVariableFunction *functionHH, rsgis::math::RSGISMathTwoVariableFunction *functionHV,
																								   gsl_matrix *covMatrixP, 
																								   gsl_matrix *invCovMatrixD,
																								   gsl_vector *aPrioriPar,
																								   double *minMaxIntervalA, double *minMaxIntervalB)
	{
		rsgis::math::RSGISMatrices matrixUtils;
		this->functionHH = functionHH;
		this->functionHV = functionHV;
		this->minMaxIntervalA = minMaxIntervalA;
		this->minMaxIntervalB = minMaxIntervalB;
		this->aPrioriPar = aPrioriPar;
		this->covMatrixP = covMatrixP;
		this->invCovMatrixD = invCovMatrixD;
		// Invert covariance matrix
		this->invCovMatrixP = gsl_matrix_alloc(2,2);
		matrixUtils.inv2x2GSLMatrix(covMatrixP, this->invCovMatrixP);
		
		rsgis::math::RSGISVectors vectorUtils;
	}
	int RSGISEstimationExhaustiveSearch2Var2DataWithAP::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int nPar = 2;
		
		double *bestParError = new double[nPar + 1];
		double currentError;
		
		// Set energy;
		bestParError[nPar] = + std::numeric_limits<double>::infinity();//+INFINITY;
		currentError = + std::numeric_limits<double>::infinity();//+INFINITY;
		
		rsgis::math::RSGISFunction2Var2DataPreconditionedLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunction2Var2DataPreconditionedLeastSquares(functionHH, functionHV, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1), gsl_vector_get(this->aPrioriPar, 0), gsl_vector_get(this->aPrioriPar, 1), this->invCovMatrixP, this->invCovMatrixD);
		
		double h = minMaxIntervalA[0];
		while (h < minMaxIntervalA[1]) 
		{
			double d = minMaxIntervalB[0];
			while(d < minMaxIntervalB[1]) 
			{
				currentError =  leastSquares->calcFunction(h, d);
				
				//std::cout << "Predict HH " << functionHH->calcFunction(h, d) << " Predict HV " << functionHV->calcFunction(h, d) << std::endl;
				//std::cout << "current Error = " << currentError << std::endl;
				if (currentError < bestParError[2])
				{
					bestParError[0] = h;
					bestParError[1] = d;
					bestParError[2] = currentError;
				}
				d = d + minMaxIntervalB[2];
			}
			h = h + minMaxIntervalA[2];
		}		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
		
		//std::cout << "*********************************************************" << std::endl;		
		
		// Tidy
		delete[] bestParError;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationExhaustiveSearch2Var2DataWithAP::~RSGISEstimationExhaustiveSearch2Var2DataWithAP()
	{
		gsl_matrix_free(invCovMatrixP);
	}
	
	RSGISEstimationExhaustiveSearch2Var3Data::RSGISEstimationExhaustiveSearch2Var3Data(rsgis::math::RSGISMathTwoVariableFunction *function1, rsgis::math::RSGISMathTwoVariableFunction *function2, rsgis::math::RSGISMathTwoVariableFunction *function3, double *minMaxIntervalA, double *minMaxIntervalB)
	{
		this->function1 = function1;
		this->function2 = function2;
		this->function3 = function3;
		this->minMaxIntervalA = minMaxIntervalA;
		this->minMaxIntervalB = minMaxIntervalB;
	}
	int RSGISEstimationExhaustiveSearch2Var3Data::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int nPar = 2;
		
		double *bestParError = new double[nPar + 1];
		double currentError;
		
		// Set energy;
		bestParError[nPar] = + std::numeric_limits<double>::infinity();//+INFINITY;
		currentError = + std::numeric_limits<double>::infinity();//+INFINITY;
		
		rsgis::math::RSGISFunction2Var3DataLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunction2Var3DataLeastSquares(function1, function2, function3, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1), gsl_vector_get(inData, 2)); 
		
		double h = minMaxIntervalA[0];
		while (h < minMaxIntervalA[1]) 
		{
			double d = minMaxIntervalB[0];
			while(d < minMaxIntervalB[1]) 
			{
				currentError =  leastSquares->calcFunction(h, d);
				if (currentError < bestParError[2])
				{
					bestParError[0] = h;
					bestParError[1] = d;
					bestParError[2] = currentError;
				}
				d = d + minMaxIntervalB[2];
			}
			h = h + minMaxIntervalA[2];
		}		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
		
		// Tidy
		delete[] bestParError;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationExhaustiveSearch2Var3Data::~RSGISEstimationExhaustiveSearch2Var3Data()
	{
	}
	
	RSGISEstimationExhaustiveSearch2Var3DataExportPoints::RSGISEstimationExhaustiveSearch2Var3DataExportPoints(rsgis::math::RSGISMathTwoVariableFunction *function1, rsgis::math::RSGISMathTwoVariableFunction *function2, rsgis::math::RSGISMathTwoVariableFunction *function3, double *minMaxIntervalA, double *minMaxIntervalB, std::string outFilenameBase)
	{
		this->function1 = function1;
		this->function2 = function2;
		this->function3 = function3;
		this->minMaxIntervalA = minMaxIntervalA;
		this->minMaxIntervalB = minMaxIntervalB;
		this->outFilenameBase = outFilenameBase;
		this->nMinimiseRuns = 0;
	}
	int RSGISEstimationExhaustiveSearch2Var3DataExportPoints::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		rsgis::math::RSGISMathsUtils mathUtils;
		
		// Update number of runs for minimiser
		this->nMinimiseRuns++;
		
		unsigned int nPar = 2;
		
		int numA = (minMaxIntervalA[1] - minMaxIntervalA[0]) / minMaxIntervalA[2];
		int numB = (minMaxIntervalB[1] - minMaxIntervalB[0]) / minMaxIntervalB[2];
		
		// set up vectors to store ouput values of h and d and least squares.
		double *aList = new double[numA * numB];
		double *bList = new double[numA * numB];
		double *lstSqList = new double[numA * numB];
		
		double *bestParError = new double[nPar + 1];
		double currentError;
		
		// Set energy;
		bestParError[nPar] = + std::numeric_limits<double>::infinity();//+INFINITY;
		currentError = + std::numeric_limits<double>::infinity();//+INFINITY;
		
		rsgis::math::RSGISFunction2Var3DataLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunction2Var3DataLeastSquares(function1, function2, function3, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1), gsl_vector_get(inData, 2)); 
		
		unsigned int i = 0;
		
		double a = minMaxIntervalA[0];
		while (a < minMaxIntervalA[1]) 
		{
			double b = minMaxIntervalB[0];
			while(b < minMaxIntervalB[1]) 
			{
				currentError =  leastSquares->calcFunction(a, b);
				if (currentError < bestParError[2])
				{
					bestParError[0] = a;
					bestParError[1] = b;
					bestParError[2] = currentError;
				}
				aList[i] = a;
				bList[i] = b;
				lstSqList[i] = currentError;
				
				b = b + minMaxIntervalB[2];
				i++;
			}
			a = a + minMaxIntervalA[2];
		}		
		// Set output to best value
		for (unsigned int j = 0; j < nPar + 1; j++) 
		{
			gsl_vector_set(outParError, j, bestParError[j]);
		}
		
		std::string filename = this->outFilenameBase + mathUtils.inttostring(nMinimiseRuns);
		
        rsgis::utils::RSGISExportForPlotting::getInstance()->exportSurface(filename, aList, bList, lstSqList, i);
		
		// Tidy
		delete[] aList;
		delete[] bList;
		delete[] lstSqList;
		delete[] bestParError;
		delete leastSquares;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationExhaustiveSearch2Var3DataExportPoints::~RSGISEstimationExhaustiveSearch2Var3DataExportPoints()
	{
	}
	
	RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data::RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data(rsgis::math::RSGISMathTwoVariableFunction *functionHH, rsgis::math::RSGISMathTwoVariableFunction *functionHV, double *minMaxIntervalA, double *minMaxIntervalB)
	{
		this->functionHH = functionHH;
		this->functionHV = functionHV;
		this->minMaxIntervalA = minMaxIntervalA;
		this->minMaxIntervalB = minMaxIntervalB;
	}
	int RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
	{		
		rsgis::math::RSGISVectors vectorUtils;
		rsgis::math::RSGISMatrices matrixUtils;
		
		unsigned int nPar = 2;
		double leastSquaresThreshold = 500;
		
		rsgis::math::RSGISFunctionEstimationLeastSquares *leastSquares;
		leastSquares = new rsgis::math::RSGISFunctionEstimationLeastSquares(functionHH, functionHV, gsl_vector_get(inData, 0), gsl_vector_get(inData, 1)); 
		
		RSGISEstimationGSLOptimiser *optimiser;
		optimiser = new RSGISEstimationGSLOptimiser();
		
		gsl_vector *testPar;
		gsl_vector *testOut;
		testPar = gsl_vector_alloc(2);
		testOut = gsl_vector_alloc(3);
		
		gsl_vector_set(outParError, nPar, 9999);
		
		double h = minMaxIntervalA[0];
		while (h < minMaxIntervalA[1]) 
		{
			double d = minMaxIntervalB[0];
			while(d < minMaxIntervalB[1]) 
			{
				if (leastSquares->calcFunction(h, d) < leastSquaresThreshold) // If least squares is less than threashold run optimiser
				{
					gsl_vector_set(testPar, 0, h);
					gsl_vector_set(testPar, 1, d);
					
					optimiser->minimise(inData, testPar, testOut);
					
					if(gsl_vector_get(testOut, 2) < gsl_vector_get(outParError, 2))
					{
						gsl_vector_memcpy(outParError, testOut);
					}
					
				}
				
				d = d + minMaxIntervalB[2];
			}
			h = h + minMaxIntervalA[2];
		}		
				
		// Tidy
		gsl_vector_free(testPar);
		gsl_vector_free(testOut);
		delete leastSquares;
		delete optimiser;
		
		// Exit
		return 0;
		
	}
	RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data::~RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data()
	{
	}
}}


