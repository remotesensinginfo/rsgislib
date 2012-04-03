/*
 *  RSGISEstimationExhaustiveSearch.h
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

#ifndef RSGISEstimationExhaustiveSearch_H
#define RSGISEstimationExhaustiveSearch_H

#include <math.h>
#include <limits>
#include <iostream>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "utils/RSGISExportForPlotting.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMathFunction.h"
#include "math/RSGISFunctions.h"
#include "math/RSGISMathsUtils.h"
#include "radar/RSGISEstimationOptimiser.h"
#include "radar/RSGISEstimationGSLOptimiser.h"

using namespace std;
using namespace rsgis::math;
using namespace rsgis::utils;

namespace rsgis {namespace radar{
	class RSGISEstimationExhaustiveSearch2Var2Data : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var2Data(RSGISMathTwoVariableFunction *functionHH, RSGISMathTwoVariableFunction *functionHV, double *minMaxIntervalA, double *minMaxIntervalB);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;}; 
		virtual void printOptimiser(){cout << "Exhaustive search - 2 Var 2 Data" << endl;};
		~RSGISEstimationExhaustiveSearch2Var2Data();
	private:
		RSGISMathTwoVariableFunction *functionHH;
		RSGISMathTwoVariableFunction *functionHV;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
	};
	
	class RSGISEstimationExhaustiveSearch2Var2DataExportPoints : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var2DataExportPoints(RSGISMathTwoVariableFunction *functionHH, RSGISMathTwoVariableFunction *functionHV, double *minMaxIntervalA, double *minMaxIntervalB, string outFilenameBase);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;};
		virtual void printOptimiser(){cout << "Exhaustive search - 2 Var 2 Data" << endl;};
		~RSGISEstimationExhaustiveSearch2Var2DataExportPoints();
	private:
		RSGISMathTwoVariableFunction *functionHH;
		RSGISMathTwoVariableFunction *functionHV;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		string outFilenameBase;
		int nMinimiseRuns;
	};
	
	class RSGISEstimationExhaustiveSearch2Var2DataWithAP : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var2DataWithAP(RSGISMathTwoVariableFunction *functionHH, RSGISMathTwoVariableFunction *functionHV,
													   gsl_matrix *covMatrixP, 
													   gsl_matrix *invCovMatrixD,
													   gsl_vector *aPrioriPar,
													   double *minMaxIntervalA, double *minMaxIntervalB);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;}; 
		virtual void printOptimiser(){cout << "Exhaustive search - 2 Var 2 Data (with a priori estimates)" << endl;};
		~RSGISEstimationExhaustiveSearch2Var2DataWithAP();
	private:
		RSGISMathTwoVariableFunction *functionHH;
		RSGISMathTwoVariableFunction *functionHV;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
	};
	
	class RSGISEstimationExhaustiveSearch2Var3Data : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var3Data(RSGISMathTwoVariableFunction *function1, RSGISMathTwoVariableFunction *function2, RSGISMathTwoVariableFunction *function3, double *minMaxIntervalA, double *minMaxIntervalB);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;};
		virtual void printOptimiser(){cout << "Exhaustive search - 2 Var 3 Data" << endl;};
		~RSGISEstimationExhaustiveSearch2Var3Data();
	private:
		RSGISMathTwoVariableFunction *function1;
		RSGISMathTwoVariableFunction *function2;
		RSGISMathTwoVariableFunction *function3;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
	};
	
	class RSGISEstimationExhaustiveSearch2Var3DataExportPoints : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var3DataExportPoints(RSGISMathTwoVariableFunction *function1, RSGISMathTwoVariableFunction *function2, RSGISMathTwoVariableFunction *function3, double *minMaxIntervalA, double *minMaxIntervalB, string outFilenameBase);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;};
		virtual void printOptimiser(){cout << "Exhaustive search - 2 Var 3 Data" << endl;};
		~RSGISEstimationExhaustiveSearch2Var3DataExportPoints();
	private:
		RSGISMathTwoVariableFunction *function1;
		RSGISMathTwoVariableFunction *function2;
		RSGISMathTwoVariableFunction *function3;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		string outFilenameBase;
		int nMinimiseRuns;
	};
	
	class RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data(RSGISMathTwoVariableFunction *functionHH, RSGISMathTwoVariableFunction *functionHV, double *minMaxIntervalA, double *minMaxIntervalB);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;};
		virtual void printOptimiser(){cout << "Exhaustive search - 2 Var 2 Data" << endl;};
		~RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data();
	private:
		RSGISMathTwoVariableFunction *functionHH;
		RSGISMathTwoVariableFunction *functionHV;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
	};
}}

#endif

