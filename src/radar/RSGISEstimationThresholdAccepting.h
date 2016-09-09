 /*
 *  RSGISEstimationThresholdAccepting.h
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

#ifndef RSGISEstimationThresholdAccepting_H
#define RSGISEstimationThresholdAccepting_H

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
    
	class DllExport RSGISEstimationThresholdAccepting2Var2Data : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationThresholdAccepting2Var2Data(
												   rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
												   rsgis::math::RSGISMathTwoVariableFunction *functionHV,
												   double *minMaxIntervalA,
												   double *minMaxIntervalB,
												   double minEnergy,
												   double startThreshold,
												   unsigned int runsStep,
												   unsigned int runsThreshold,
												   double cooling,
												   unsigned int maxItt);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){std::cout << "Simulated Annealing - 2 Var 2 Data" << std::endl;};
		~RSGISEstimationThresholdAccepting2Var2Data();
	private:
		double startThreshold;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsThreshold; // Number of times step is changed at each threasherature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		rsgis::math::RSGISMathTwoVariableFunction *functionHH;
		rsgis::math::RSGISMathTwoVariableFunction *functionHV;
	};
	
	class DllExport RSGISEstimationThresholdAccepting2Var2DataWithAP : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationThresholdAccepting2Var2DataWithAP(rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
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
														 gsl_vector *aPrioriPar);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){std::cout << "Simulated Annealing - 2 Var 2 Data (with a Priori)" << std::endl;};
		~RSGISEstimationThresholdAccepting2Var2DataWithAP();
	private:
		double startThreshold;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsThreshold; // Number of times step is changed at each threasherature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		rsgis::math::RSGISMathTwoVariableFunction *functionHH;
		rsgis::math::RSGISMathTwoVariableFunction *functionHV;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
	};
	
	class DllExport RSGISEstimationThresholdAccepting2Var3Data : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationThresholdAccepting2Var3Data(
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
												   unsigned int maxItt);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){std::cout << "Simulated Annealing - 2 Var 3 Data" << std::endl;};
		~RSGISEstimationThresholdAccepting2Var3Data();
	private:
		double startThreshold;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsThreshold; // Number of times step is changed at each threasherature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		rsgis::math::RSGISMathTwoVariableFunction *functionHH;
		rsgis::math::RSGISMathTwoVariableFunction *functionHV;
		rsgis::math::RSGISMathTwoVariableFunction *functionVV;
	};
	
	class DllExport RSGISEstimationThresholdAccepting2Var3DataWithAP : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationThresholdAccepting2Var3DataWithAP(rsgis::math::RSGISMathTwoVariableFunction *functionHH, 
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
														 gsl_vector *aPrioriPar);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){std::cout << "Simulated Annealing - 2 Var 3 Data (with a Priori)" << std::endl;};
		~RSGISEstimationThresholdAccepting2Var3DataWithAP();
	private:
		double startThreshold;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsThreshold; // Number of times step is changed at each threasherature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		rsgis::math::RSGISMathTwoVariableFunction *functionHH;
		rsgis::math::RSGISMathTwoVariableFunction *functionHV;
		rsgis::math::RSGISMathTwoVariableFunction *functionVV;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
	};
	
	class DllExport RSGISEstimationThresholdAccepting3Var3DataWithAP : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationThresholdAccepting3Var3DataWithAP(rsgis::math::RSGISMathThreeVariableFunction *functionHH, 
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
														 gsl_vector *aPrioriPar);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){std::cout << "Simulated Annealing - 3 Var 3 Data (with a Priori)" << std::endl;};
		~RSGISEstimationThresholdAccepting3Var3DataWithAP();
	private:
		double startThreshold;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsThreshold; // Number of times step is changed at each threasherature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double *minMaxIntervalC;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		rsgis::math::RSGISMathThreeVariableFunction *functionHH;
		rsgis::math::RSGISMathThreeVariableFunction *functionHV;
		rsgis::math::RSGISMathThreeVariableFunction *functionVV;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
	};
	
	class DllExport RSGISEstimationThresholdAccepting3Var4DataWithAP : public RSGISEstimationOptimiser
	{
		/// Simulated Annealing to use in Estimation algorithm
	public:
		RSGISEstimationThresholdAccepting3Var4DataWithAP(rsgis::math::RSGISMathThreeVariableFunction *function1, 
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
														 gsl_vector *aPrioriPar);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return simulatedAnnealing;}; 
		virtual void printOptimiser(){std::cout << "Simulated Annealing - 3 Var 4 Data (with a Priori)" << std::endl;};
		~RSGISEstimationThresholdAccepting3Var4DataWithAP();
	private:
		double startThreshold;
		unsigned int runsStep; // Number of runs at each step size
		unsigned int runsThreshold; // Number of times step is changed at each threasherature
		double cooling; // Cooling factor
		unsigned int nPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double *minMaxIntervalC;
		double minEnergy; // Set the target energy
		unsigned int maxItt; // Maximum number of itterations
		double *initialStepSize;
		gsl_rng *randgsl;
		rsgis::math::RSGISMathThreeVariableFunction *function1;
		rsgis::math::RSGISMathThreeVariableFunction *function2;
		rsgis::math::RSGISMathThreeVariableFunction *function3;
		rsgis::math::RSGISMathThreeVariableFunction *function4;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
	};
	
	
}}

#endif

