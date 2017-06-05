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
#include <limits>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMathFunction.h"
#include "math/RSGISFunctions.h"
#include "math/RSGISProbDistro.h"
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

	class DllExport RSGISEstimationSimulatedAnnealingWithAP : public RSGISEstimationOptimiser
    {
        /// Simulated Annealing to use in Estimation algorithm
    public:
        RSGISEstimationSimulatedAnnealingWithAP(std::vector<rsgis::math::RSGISMathNVariableFunction*> *allFunctions,
                                                double **minMaxIntervalAll,
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
        virtual void printOptimiser(){std::cout << "Simulated Annealing" << std::endl;};
        double calcLeastSquares(std::vector<double> *values);
        ~RSGISEstimationSimulatedAnnealingWithAP();
    private:
        double startTemp;
        unsigned int runsStep; // Number of runs at each step size
        unsigned int runsTemp; // Number of times step is changed at each temperature
        double cooling; // Cooling factor
        unsigned int nPar;
        unsigned int nData;
        double **minMaxIntervalAll;
        double minEnergy; // Set the target energy
        unsigned int maxItt; // Maximum number of itterations
        double *initialStepSize;
        gsl_rng *randgsl;
        std::vector <rsgis::math::RSGISMathNVariableFunction*> *allFunctions;
        gsl_matrix *covMatrixP;
        gsl_matrix *invCovMatrixD;
        gsl_matrix *invCovMatrixP;
        gsl_vector *aPrioriPar;
        gsl_vector *deltaD;
        gsl_vector *deltaX;
        gsl_vector *tempD;
        gsl_vector *tempX;
        gsl_vector *inputData;
        bool useAP;
    };
}}

#endif

