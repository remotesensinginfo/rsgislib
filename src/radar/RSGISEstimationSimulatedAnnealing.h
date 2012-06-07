/*
 *  RSGISEstimationSimulatedAnnealing.h
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

#ifndef RSGISEstimationSimulatedAnnealing_H
#define RSGISEstimationSimulatedAnnealing_H

#include <math.h>
#include <time.h>
#include <iostream>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMathFunction.h"
#include "math/RSGISFunctions.h"
#include "math/RSGISProbDistro.h"
#include "radar/RSGISEstimationOptimiser.h"

namespace rsgis {namespace radar{
    
    using namespace std;
    using namespace rsgis::math;
    
	class RSGISEstimationSimulatedAnnealing2Var2Data : public RSGISEstimationOptimiser
	{
	/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationSimulatedAnnealing2Var2Data(
												   RSGISMathTwoVariableFunction *functionHH, 
												   RSGISMathTwoVariableFunction *functionHV,
												   double *minMaxIntervalA,
												   double *minMaxIntervalB,
												   double minEnergy,
												   double startTemp,
												   unsigned int runsStep,
												   unsigned int runsTemp,
												   double cooling,
												   unsigned int maxItt);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){cout << "Simulated Annealing - 2 Var 2 Data" << endl;};
		~RSGISEstimationSimulatedAnnealing2Var2Data();
	private:
		double startTemp;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsTemp; // Number of times step is changed at each temperature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		RSGISMathTwoVariableFunction *functionHH;
		RSGISMathTwoVariableFunction *functionHV;
	};
	
	class RSGISEstimationSimulatedAnnealing2Var2DataWithAP : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationSimulatedAnnealing2Var2DataWithAP(RSGISMathTwoVariableFunction *functionHH, 
														   RSGISMathTwoVariableFunction *functionHV,
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
														   gsl_vector *aPrioriPar);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){cout << "Simulated Annealing - 2 Var 2 Data (with a Priori)" << endl;};
		~RSGISEstimationSimulatedAnnealing2Var2DataWithAP();
	private:
		double startTemp;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsTemp; // Number of times step is changed at each temperature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		RSGISMathTwoVariableFunction *functionHH;
		RSGISMathTwoVariableFunction *functionHV;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
	};
	
	class RSGISEstimationSimulatedAnnealing2Var3Data : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationSimulatedAnnealing2Var3Data(
												   RSGISMathTwoVariableFunction *functionHH, 
												   RSGISMathTwoVariableFunction *functionHV,
												   RSGISMathTwoVariableFunction *functionVV,
												   double *minMaxIntervalA,
												   double *minMaxIntervalB,
												   double minEnergy,
												   double startTemp,
												   unsigned int runsStep,
												   unsigned int runsTemp,
												   double cooling,
												   unsigned int maxItt);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){cout << "Simulated Annealing - 2 Var 3 Data" << endl;};
		~RSGISEstimationSimulatedAnnealing2Var3Data();
	private:
		double startTemp;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsTemp; // Number of times step is changed at each temperature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		RSGISMathTwoVariableFunction *functionHH;
		RSGISMathTwoVariableFunction *functionHV;
		RSGISMathTwoVariableFunction *functionVV;
	};
	
	class RSGISEstimationSimulatedAnnealing2Var3DataWithAP : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationSimulatedAnnealing2Var3DataWithAP(RSGISMathTwoVariableFunction *functionHH, 
														 RSGISMathTwoVariableFunction *functionHV,
														 RSGISMathTwoVariableFunction *functionVV,
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
														 gsl_vector *aPrioriPar);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){cout << "Simulated Annealing - 2 Var 3 Data (with a Priori)" << endl;};
		~RSGISEstimationSimulatedAnnealing2Var3DataWithAP();
	private:
		double startTemp;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsTemp; // Number of times step is changed at each temperature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		RSGISMathTwoVariableFunction *functionHH;
		RSGISMathTwoVariableFunction *functionHV;
		RSGISMathTwoVariableFunction *functionVV;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
	};
	
	class RSGISEstimationSimulatedAnnealing3Var3DataWithAP : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationSimulatedAnnealing3Var3DataWithAP(RSGISMathThreeVariableFunction *functionHH, 
														 RSGISMathThreeVariableFunction *functionHV,
														 RSGISMathThreeVariableFunction *functionVV,
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
														 gsl_vector *aPrioriPar);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){cout << "Simulated Annealing - 3 Var 3 Data (with a Priori)" << endl;};
		~RSGISEstimationSimulatedAnnealing3Var3DataWithAP();
	private:
		double startTemp;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsTemp; // Number of times step is changed at each temperature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double *minMaxIntervalC;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		RSGISMathThreeVariableFunction *functionHH;
		RSGISMathThreeVariableFunction *functionHV;
		RSGISMathThreeVariableFunction *functionVV;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
	};
	
	class RSGISEstimationSimulatedAnnealing3Var4DataWithAP : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationSimulatedAnnealing3Var4DataWithAP(RSGISMathThreeVariableFunction *function1, 
														 RSGISMathThreeVariableFunction *function2,
														 RSGISMathThreeVariableFunction *function3,
														 RSGISMathThreeVariableFunction *function4,
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
														 gsl_vector *aPrioriPar);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){cout << "Simulated Annealing - 3 Var 4 Data (with a Priori)" << endl;};
		~RSGISEstimationSimulatedAnnealing3Var4DataWithAP();
	private:
		double startTemp;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsTemp; // Number of times step is changed at each temperature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double *minMaxIntervalC;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		RSGISMathThreeVariableFunction *function1;
		RSGISMathThreeVariableFunction *function2;
		RSGISMathThreeVariableFunction *function3;
		RSGISMathThreeVariableFunction *function4;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
	};
	
	
}}

#endif

