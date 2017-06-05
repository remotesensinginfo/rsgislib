/*
 *  RSGISEstimationConjugateGradient.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 31/01/2009.
 *  Copyright 2009 RSGISLib.
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

#ifndef RSGISEstimationConjugateGradient_H
#define RSGISEstimationConjugateGradient_H

#include <math.h>
#include <limits>
#include <iostream>
#include <time.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMathFunction.h"
#include "math/RSGISProbDistro.h"
#include "math/RSGISRandomDistro.h"
#include "math/RSGISFunctions.h"
#include "radar/RSGISEstimationOptimiser.h"

#include <boost/math/special_functions/fpclassify.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
    
	/** There are three classes for conjugate gradient.
	 * the first 'RSGISEstimationConjugateGradient' is the old implementation for polynomials.
	 * many parameters are hardcoded.
	 * The second 'RSGISEstimationConjugateGradient2DPoly2Channel' inherits from 'RSGISEstimationOptimiser'
	 * and is much more general.
	 * The third 'RSGISEstimationConjugateGradient2Var2Data' inherits from 'RSGISEstimationOptimiser'.
	 * it takes a function therefore any function is used, as long as partial differentials are provided.
	 */
	class DllExport RSGISEstimationConjugateGradient
		{
		public:
			RSGISEstimationConjugateGradient();
			/// Parameter estimation from two-dimensional polynomial equations.
			/**
			* See: \n
			* Moghaddam et al. Monitering Tree Moisture Using an Estimation Algorithm Applied to SAR Data from BOREAS. \n Geoscience and Remote Sensing (1999) vol. 37 (2) pp. 901 - 915
			*/ 
			void estimateTwoDimensionalPolyTwoChannel(gsl_vector *inSigma0dB, gsl_matrix *coeffHH, gsl_matrix *coeffVV, gsl_vector *initialPar, gsl_vector *outParError, gsl_vector *predicted, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD, int ittmax);
			void estimateTwoDimensionalPolyThreeChannel(gsl_vector *inSigma0dB, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV, gsl_vector *initialPar, gsl_vector *outParError, gsl_vector *predicted, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD, int ittmax);
			void estimateThreeDimensionalPolyThreeChannel(gsl_vector *inSigma0dB, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV, gsl_vector *initialPar, gsl_vector *outParError, gsl_vector *predicted, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD, int ittmax);
			~RSGISEstimationConjugateGradient();
		private:
		};

	class DllExport RSGISEstimationConjugateGradient2DPoly2Channel : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationConjugateGradient2DPoly2Channel(gsl_matrix *coeffHH, gsl_matrix *coeffHV, 
													   gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD, 
													   int ittmax);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return conjugateGradient;};
		virtual void printOptimiser(){std::cout << "Conjugate gradient (Polynomial) - 2 Var 2 Data" << std::endl;};
		~RSGISEstimationConjugateGradient2DPoly2Channel();
	private:
		gsl_matrix *coeffHH;
		gsl_matrix *coeffHV;
		gsl_matrix *covMatrixP; // Covarence matrix for prior estimates
		gsl_matrix *invCovMatrixP; // Inverse covarence matrix for prior estimates
		gsl_matrix *invCovMatrixD; // Inverse covarence matrix for data
		int ittmax;
		int order;
		int nData; // Data channels
		int nPar; // Parameters to be retrieved
	};
	
	class DllExport RSGISEstimationConjugateGradient3DPoly3Channel : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationConjugateGradient3DPoly3Channel(gsl_matrix *inCoeffHH, gsl_matrix *inCoeffHV, gsl_matrix *inCoeffVV,
													   int orderX, int orderY, int orderZ,
													    gsl_vector *aPrioriPar, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD, 
													   double minError, int ittmax);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;}
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return conjugateGradient;};
		virtual void printOptimiser(){std::cout << "Conjugate gradient (Polynomial) - 3 Var 3 Data" << std::endl;};
		~RSGISEstimationConjugateGradient3DPoly3Channel();
	private:
		gsl_matrix *coeffHH;
		gsl_matrix *coeffHV;
		gsl_matrix *coeffVV;
		gsl_vector *aPrioriPar;
		gsl_matrix *covMatrixP; // Covarence matrix for prior estimates
		gsl_matrix *invCovMatrixP; // Inverse covarence matrix for prior estimates
		gsl_matrix *invCovMatrixD; // Inverse covarence matrix for data
		int ittmax;
		double minError;
		int orderX; // Polynomial order for x term
		int orderY; // Polynomial order for y term
		int orderZ; // Polynomial order for z term
		int nData;  // Data channels
		int nPar;   // Parameters to be retrieved
	};
	
	class DllExport RSGISEstimationConjugateGradient3DPoly4Channel : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationConjugateGradient3DPoly4Channel(gsl_matrix *coeffA, gsl_matrix *coeffB, gsl_matrix *coeffC, gsl_matrix *coeffD,
													   int orderX, int orderY, int orderZ,
													   gsl_vector *aPrioriPar, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD, 
													   double minError, int ittmax);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;}
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return conjugateGradient;};
		virtual void printOptimiser(){std::cout << "Conjugate gradient (Polynomial) - 3 Var 4 Data" << std::endl;};
		~RSGISEstimationConjugateGradient3DPoly4Channel();
	private:
		gsl_matrix *coeffA;
		gsl_matrix *coeffB;
		gsl_matrix *coeffC;
		gsl_matrix *coeffD;
		gsl_vector *aPrioriPar;
		gsl_matrix *covMatrixP; // Covarence matrix for prior estimates
		gsl_matrix *invCovMatrixP; // Inverse covarence matrix for prior estimates
		gsl_matrix *invCovMatrixD; // Inverse covarence matrix for data
		int ittmax;
		double minError;
		int orderX; // Polynomial order for x term
		int orderY; // Polynomial order for y term
		int orderZ; // Polynomial order for z term
		int nData;  // Data channels
		int nPar;   // Parameters to be retrieved
	};
	
	
	class DllExport RSGISEstimationConjugateGradient2Var2Data : public RSGISEstimationOptimiser
	{
		/**
		 Preconditioned conjugate gradient optimiser, takes two 'rsgis::math::RSGISMathTwoVariableFunction' as input.
		 The functions must contain partial derivatives with respect to x and y.
		 For polynomial functions 'RSGISEstimationConjugateGradient2DPoly2Channel' is quicker
		 */
	public:
		RSGISEstimationConjugateGradient2Var2Data(rsgis::math::RSGISMathTwoVariableFunction *functionA, 
												  rsgis::math::RSGISMathTwoVariableFunction *functionB,  
												  gsl_vector *aPrioriPar,
												  gsl_matrix *covMatrixP, 
												  gsl_matrix *invCovMatrixD, 
												  double minError,
												  int ittmax);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return conjugateGradient;}; 
		virtual void printOptimiser(){std::cout << "Conjugate gradient - 2 Var 2 Data" << std::endl;};
		~RSGISEstimationConjugateGradient2Var2Data();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *functionA; 
		rsgis::math::RSGISMathTwoVariableFunction *functionB;
		gsl_vector *aPrioriPar; // a priori estimates
		gsl_matrix *covMatrixP; // Covarence matrix for a priori estimates
		gsl_matrix *invCovMatrixP; // Inverse covarence matrix for prior estimates
		gsl_matrix *invCovMatrixD; // Inverse covarence matrix for data
		int ittmax;
		double minError;
	};
	
	class DllExport RSGISEstimationConjugateGradient2Var3Data : public RSGISEstimationOptimiser
	{
		/**
		 Preconditioned conjugate gradient optimiser, takes three 'rsgis::math::RSGISMathTwoVariableFunction' as input.
		 The functions must contain partial derivatives with respect to x and y.
		 For polynomial functions 'RSGISEstimationConjugateGradient2DPoly3Channel' is quicker
		 */
	public:
		RSGISEstimationConjugateGradient2Var3Data(rsgis::math::RSGISMathTwoVariableFunction *functionA, 
												  rsgis::math::RSGISMathTwoVariableFunction *functionB,
												  rsgis::math::RSGISMathTwoVariableFunction *functionC,
												  gsl_vector *aPrioriPar,
												  gsl_matrix *covMatrixP, 
												  gsl_matrix *invCovMatrixD, 
												  double minError,
												  int ittmax);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		void modifyAPriori(gsl_vector *newAPrioriPar){this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return conjugateGradient;}; 
		virtual void printOptimiser(){std::cout << "Conjugate gradient - 2 Var 3 Data" << std::endl;};
		~RSGISEstimationConjugateGradient2Var3Data();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *functionA; 
		rsgis::math::RSGISMathTwoVariableFunction *functionB;
		rsgis::math::RSGISMathTwoVariableFunction *functionC;
		gsl_vector *aPrioriPar; // a priori estimates
		gsl_matrix *covMatrixP; // Covarence matrix for a priori estimates
		gsl_matrix *invCovMatrixP; // Inverse covarence matrix for prior estimates
		gsl_matrix *invCovMatrixD; // Inverse covarence matrix for data
		int ittmax;
		double minError;
	};
	
	class DllExport RSGISEstimationConjugateGradient2Var2DataWithRestarts : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationConjugateGradient2Var2DataWithRestarts(rsgis::math::RSGISMathTwoVariableFunction *functionA, 
															  rsgis::math::RSGISMathTwoVariableFunction *functionB,
															  double *minMaxIntervalA,
															  double *minMaxIntervalB,
															  gsl_vector *aPrioriPar,
															  gsl_matrix *covMatrixP, 
															  gsl_matrix *invCovMatrixD,
															  double minError,
															  int ittmax,
															  int nRestarts);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		void modifyAPriori(gsl_vector *newAPrioriPar){this->conjGradOpt->modifyAPriori(newAPrioriPar);this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return conjugateGradient;}; 
		virtual void printOptimiser(){std::cout << "Conjugate gradient - 2 Var 2 Data, with Restarts" << std::endl;};
		~RSGISEstimationConjugateGradient2Var2DataWithRestarts();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *functionA;
		rsgis::math::RSGISMathTwoVariableFunction *functionB;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		gsl_vector *aPrioriPar;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		double minError;
		int ittmax;
		unsigned int nRestarts;
		RSGISEstimationConjugateGradient2Var2Data *conjGradOpt;
	};
	
	class DllExport RSGISEstimationConjugateGradient2Var3DataWithRestarts : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationConjugateGradient2Var3DataWithRestarts(rsgis::math::RSGISMathTwoVariableFunction *functionA, 
															  rsgis::math::RSGISMathTwoVariableFunction *functionB,
															  rsgis::math::RSGISMathTwoVariableFunction *functionC,
															  double *minMaxIntervalA,
															  double *minMaxIntervalB,
															  gsl_vector *aPrioriPar,
															  gsl_matrix *covMatrixP, 
															  gsl_matrix *invCovMatrixD,
															  double minError,
															  int ittmax,
															  int nRestarts);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		void modifyAPriori(gsl_vector *newAPrioriPar){this->conjGradOpt->modifyAPriori(newAPrioriPar);this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return conjugateGradient;}; 
		virtual void printOptimiser(){std::cout << "Conjugate gradient - 2 Var 3 Data, with Restarts" << std::endl;};
		~RSGISEstimationConjugateGradient2Var3DataWithRestarts();
	private:
		rsgis::math::RSGISMathTwoVariableFunction *functionA;
		rsgis::math::RSGISMathTwoVariableFunction *functionB;
		rsgis::math::RSGISMathTwoVariableFunction *functionC;
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		gsl_vector *aPrioriPar;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		gsl_matrix *invCovMatrixP;
		double minError;
		int ittmax;
		unsigned int nRestarts;
		RSGISEstimationConjugateGradient2Var3Data *conjGradOpt;
	};
	
	class DllExport RSGISEstimationConjugateGradient3Var3DataWithRestarts : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationConjugateGradient3Var3DataWithRestarts(gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV,
															  int orderX, int orderY, int orderZ,
															  double *minMaxIntervalA,
															  double *minMaxIntervalB,
															  double *minMaxIntervalC,
															  gsl_vector *aPrioriPar,
															  gsl_matrix *covMatrixP, 
															  gsl_matrix *invCovMatrixD,
															  double minError,
															  int ittmax,
															  int nRestarts);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		void modifyAPriori(gsl_vector *newAPrioriPar){this->conjGradOpt->modifyAPriori(newAPrioriPar);this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return conjugateGradient;}; 
		virtual void printOptimiser(){std::cout << "Conjugate gradient - 3 Var 3 Data, with Restarts" << std::endl;};
		~RSGISEstimationConjugateGradient3Var3DataWithRestarts();
	private:
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double *minMaxIntervalC;
		gsl_vector *aPrioriPar;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		double minError;
		int ittmax;
		unsigned int nRestarts;
		RSGISEstimationConjugateGradient3DPoly3Channel *conjGradOpt;
	};
	
	class DllExport RSGISEstimationConjugateGradient3Var4DataWithRestarts : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationConjugateGradient3Var4DataWithRestarts(gsl_matrix *coeffA, gsl_matrix *coeffB, gsl_matrix *coeffC, gsl_matrix *coeffD,
															  int orderX, int orderY, int orderZ,
															  double *minMaxIntervalA,
															  double *minMaxIntervalB,
															  double *minMaxIntervalC,
															  gsl_vector *aPrioriPar,
															  gsl_matrix *covMatrixP, 
															  gsl_matrix *invCovMatrixD,
															  double minError,
															  int ittmax,
															  int nRestarts);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		void modifyAPriori(gsl_vector *newAPrioriPar){this->conjGradOpt->modifyAPriori(newAPrioriPar);this->aPrioriPar = newAPrioriPar;};
		gsl_vector* getAPrioriPar(){return this->aPrioriPar;};
		virtual estOptimizerType getOptimiserType(){return conjugateGradient;}; 
		virtual void printOptimiser(){std::cout << "Conjugate gradient - 3 Var 4 Data, with Restarts" << std::endl;};
		~RSGISEstimationConjugateGradient3Var4DataWithRestarts();
	private:
		double *minMaxIntervalA;
		double *minMaxIntervalB;
		double *minMaxIntervalC;
		gsl_vector *aPrioriPar;
		gsl_matrix *covMatrixP;
		gsl_matrix *invCovMatrixD;
		double minError;
		int ittmax;
		unsigned int nRestarts;
		RSGISEstimationConjugateGradient3DPoly4Channel *conjGradOpt;
	};

}}


#endif

