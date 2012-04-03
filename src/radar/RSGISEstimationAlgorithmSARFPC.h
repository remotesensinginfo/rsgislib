/*
 *  RSGISEstimationAlgorithmSARFPC.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 16/09/2009.
 *  Copyright 2009 Aberystwyth University. All rights reserved.
 *
 */

/*
 *  RSGISEstimationAlgorithm.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 16/02/2009.
 *  Copyright 2009 Aberystwyth University. All rights reserved.
 *
 */

#ifndef RSGISEstimationAlgorithmSARFPC_H
#define RSGISEstimationAlgorithmSARFPC_H

#include <iostream>
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

#include "math/RSGISConjugateGradient.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"

#include "radar/RSGISEstimationParameters.h"
#include "radar/RSGISFPCModel.h"
#include "radar/RSGISSoilDielectricMixingModel.h"

#include "utils/RSGISAllometricEquations.h"
#include "utils/RSGISAllometricSpecies.h"

using namespace std;
using namespace rsgis::img;
using namespace rsgis::utils;

namespace rsgis 
{
	namespace radar
	{
		class RSGISEstimationAlgorithmDualPolFPCSingleSpecies : public RSGISCalcImageValue
		{
			/// Estimates parameters from dual-pol SAR data and FPC for a single species
		public: 
			RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_vector *coeffFPCHH, gsl_vector *coeffFPCHV, gsl_vector *coeffFPCAttenuationH, gsl_vector *coeffFPCAttenuationV, estParameters parameters, treeSpecies species, int ittmax);
			virtual void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};								
			~RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies();
		protected:
			estParameters parameters;
			treeSpecies species;
			gsl_matrix *coeffHH;
			gsl_matrix *coeffHV;
			gsl_vector *coeffFPCHH;
			gsl_vector *coeffFPCHV;
			gsl_vector *coeffFPCAttenuationH;
			gsl_vector *coeffFPCAttenuationV;
			int order;
			int fpcOrder;
			
			int ittmax;
		};
	}
}

#endif

