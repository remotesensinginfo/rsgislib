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

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_radar_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis {namespace radar{
    
	class DllExport RSGISEstimationExhaustiveSearch2Var2Data : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var2Data(rsgis::math::RSGISMathTwoVariableFunction *functionHH, rsgis::math::RSGISMathTwoVariableFunction *functionHV, double *minMaxIntervalA, double *minMaxIntervalB);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;}; 
		virtual void printOptimiser(){std::cout << "Exhaustive search - 2 Var 2 Data" << std::endl;};
		~RSGISEstimationExhaustiveSearch2Var2Data();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *functionHH;
		rsgis::math::RSGISMathTwoVariableFunction *functionHV;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
	};
	
	class DllExport RSGISEstimationExhaustiveSearch2Var2DataExportPoints : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var2DataExportPoints(rsgis::math::RSGISMathTwoVariableFunction *functionHH, rsgis::math::RSGISMathTwoVariableFunction *functionHV, double *minMaxIntervalA, double *minMaxIntervalB, std::string outFilenameBase);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;};
		virtual void printOptimiser(){std::cout << "Exhaustive search - 2 Var 2 Data" << std::endl;};
		~RSGISEstimationExhaustiveSearch2Var2DataExportPoints();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *functionHH;
		rsgis::math::RSGISMathTwoVariableFunction *functionHV;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		std::string outFilenameBase;
		int nMinimiseRuns;
	};
	
	class DllExport RSGISEstimationExhaustiveSearch2Var2DataWithAP : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var2DataWithAP(rsgis::math::RSGISMathTwoVariableFunction *functionHH, rsgis::math::RSGISMathTwoVariableFunction *functionHV,
													   gsl_matrix *covMatrixP, 
													   gsl_matrix *invCovMatrixD,
													   gsl_vector *aPrioriPar,
													   double *minMaxIntervalA, double *minMaxIntervalB);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;}; 
		virtual void printOptimiser(){std::cout << "Exhaustive search - 2 Var 2 Data (with a priori estimates)" << std::endl;};
		~RSGISEstimationExhaustiveSearch2Var2DataWithAP();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *functionHH;
		rsgis::math::RSGISMathTwoVariableFunction *functionHV;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		gsl_vector *aPrioriPar;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
	};
	
	class DllExport RSGISEstimationExhaustiveSearch2Var3Data : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var3Data(rsgis::math::RSGISMathTwoVariableFunction *function1, rsgis::math::RSGISMathTwoVariableFunction *function2, rsgis::math::RSGISMathTwoVariableFunction *function3, double *minMaxIntervalA, double *minMaxIntervalB);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;};
		virtual void printOptimiser(){std::cout << "Exhaustive search - 2 Var 3 Data" << std::endl;};
		~RSGISEstimationExhaustiveSearch2Var3Data();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *function1;
		rsgis::math::RSGISMathTwoVariableFunction *function2;
		rsgis::math::RSGISMathTwoVariableFunction *function3;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
	};
	
	class DllExport RSGISEstimationExhaustiveSearch2Var3DataExportPoints : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearch2Var3DataExportPoints(rsgis::math::RSGISMathTwoVariableFunction *function1, rsgis::math::RSGISMathTwoVariableFunction *function2, rsgis::math::RSGISMathTwoVariableFunction *function3, double *minMaxIntervalA, double *minMaxIntervalB, std::string outFilenameBase);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;};
		virtual void printOptimiser(){std::cout << "Exhaustive search - 2 Var 3 Data" << std::endl;};
		~RSGISEstimationExhaustiveSearch2Var3DataExportPoints();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *function1;
		rsgis::math::RSGISMathTwoVariableFunction *function2;
		rsgis::math::RSGISMathTwoVariableFunction *function3;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		std::string outFilenameBase;
		int nMinimiseRuns;
	};
	
	class DllExport RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data(rsgis::math::RSGISMathTwoVariableFunction *functionHH, rsgis::math::RSGISMathTwoVariableFunction *functionHV, double *minMaxIntervalA, double *minMaxIntervalB);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual estOptimizerType getOptimiserType(){return exhaustiveSearch;};
		virtual void printOptimiser(){std::cout << "Exhaustive search - 2 Var 2 Data" << std::endl;};
		~RSGISEstimationExhaustiveSearchWithGSLOptimiser2Var2Data();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *functionHH;
		rsgis::math::RSGISMathTwoVariableFunction *functionHV;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
	};
}}

#endif

