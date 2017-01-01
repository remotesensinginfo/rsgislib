/*
 *  RSGISExeEstimationAlgorithm.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 16/02/2009.
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

#ifndef RSGISExeEstimationAlgorithm_H
#define RSGISExeEstimationAlgorithm_H

#include <string>
#include <iostream>
#include <cstdlib>
#include <gsl/gsl_matrix.h>

#include "img/RSGISCalcImage.h"
#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"
#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "utils/RSGISAllometricSpecies.h"
#include "radar/RSGISEstimationAlgorithm.h"
#include "radar/RSGISObjectBasedEstimation.h"
#include "radar/RSGISEstimationOptimiser.h"
#include "radar/RSGISEstimationConjugateGradient.h"
#include "radar/RSGISEstimationSimulatedAnnealing.h"
#include "radar/RSGISEstimationExhaustiveSearch.h"
#include "radar/RSGISEstimationLinearLeastSquares.h"
#include "radar/RSGISEstimationThresholdAccepting.h"
#include "vec/RSGISProcessVector.h"
#include "vec/RSGISProcessOGRFeature.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::math;
using namespace rsgis::radar;
using namespace rsgis::utils;

/// Executable for parameter estimation from Radar data using an estimation algorithm

class DllExport RSGISExeEstimationAlgorithm : public RSGISAlgorithmParameters
	{
		
	enum options
	{
		none,
		dualPolObject,
		fullPolObject,
		dualPolObjectObjAP,
		fullPolObjectObjAP,
		dualPolMultiSpeciesClassification,
		fullPolMultiSpeciesClassification,
		simData2Var2Data
	};
		
	public:
		RSGISExeEstimationAlgorithm();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeEstimationAlgorithm();
	protected:
		estParameters parameters;
		options option;
		string inputImage;
		string classification;
		string outputImage;
		int nPar;
		int nBands;
		gsl_matrix *coeffHH;
		gsl_matrix *coeffHV;
		gsl_matrix *coeffVV;
		gsl_matrix *coeffA;
		gsl_matrix *coeffB;
		gsl_vector *initialPar;
		gsl_vector *coeffBranchFPCHH;
		gsl_vector *coeffBranchFPCHV;
		gsl_vector *coeffAttenuationFPCH;
		gsl_vector *coeffAttenuationFPCV;
		RSGISMathTwoVariableFunction *functionHH;
		RSGISMathTwoVariableFunction *functionHV;
		RSGISMathTwoVariableFunction *functionVV;
		RSGISEstimationOptimiser *estOptimiser;
		RSGISEstimationOptimiser *estSlowOptimiser;
		RSGISEstimationOptimiser *estFastOptimiser;
		estOptimizerType estOptType;
		double maskThreshold;
		int ittmax;
		vector <gsl_vector*> *initialParClass;
		vector <RSGISEstimationOptimiser*> *estOptimiserClass;
		vector <RSGISEstimationOptimiser*> *estSlowOptimiserClass;
		vector <RSGISEstimationOptimiser*> *estFastOptimiserClass;
		vector <treeSpecies> *species;
		string inputObjPolys;
		string inputRasPolys;
		string outputSHP;
		string *objAPParField;
		bool useRasPoly;
		bool createOutSHP;
		bool force;
		bool copyAttributes;
		string classField;
		bool useClass;
		bool objAP;
		bool useDefaultMinMax;
		double **minMaxValues;
		double ***minMaxValuesClass;
		int polyOrderX;
		int polyOrderY;
		int polyOrderZ;
	};
}
#endif
