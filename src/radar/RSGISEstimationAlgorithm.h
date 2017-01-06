/*
 *  RSGISEstimationAlgorithm.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 16/02/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#ifndef RSGISEstimationAlgorithm_H
#define RSGISEstimationAlgorithm_H

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISFunctions.h"
#include "math/RSGISMathFunction.h"

#include "radar/RSGISEstimationFPC.h"
#include "radar/RSGISEstimationConjugateGradient.h"
#include "radar/RSGISEstimationParameters.h"
#include "radar/RSGISSoilDielectricMixingModel.h"
#include "radar/RSGISEstimationOptimiser.h"

#include "utils/RSGISAllometricEquations.h"
#include "utils/RSGISAllometricSpecies.h"

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

namespace rsgis 
{
	namespace radar
	{
		class DllExport RSGISEstimationAlgorithmFullPolSingleSpeciesPoly : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from full-pol SAR data for a single species
			public: 
				RSGISEstimationAlgorithmFullPolSingleSpeciesPoly(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV, estParameters parameters, gsl_vector *initialPar, int ittmax);
				virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};									
				~RSGISEstimationAlgorithmFullPolSingleSpeciesPoly();
			protected:
				estParameters parameters;
				rsgis::utils::treeSpecies species;
				gsl_matrix *coeffHH;
				gsl_matrix *coeffHV;
				gsl_matrix *coeffVV;
				gsl_vector *initialPar;
				int ittmax;
			};
		
		class DllExport RSGISEstimationAlgorithmDualPolSingleSpeciesPoly : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from dual-pol SAR data for a single species
			public: 
				RSGISEstimationAlgorithmDualPolSingleSpeciesPoly(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffVV, estParameters parameters, gsl_vector *initialPar,  int ittmax);
				virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
				~RSGISEstimationAlgorithmDualPolSingleSpeciesPoly();
			protected:
				estParameters parameters;
				rsgis::utils::treeSpecies species;
				gsl_matrix *coeffHH;
				gsl_matrix *coeffVV;
				gsl_vector *initialPar;
				int ittmax;
			};
		
		class DllExport RSGISEstimationAlgorithmDualPolFPCSingleSpecies : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from dual-pol SAR data, FPC for a single species
			/**
			 * Input is a 3 band image:<br>
			 * FPC<b>
			 * HH<br>
			 * HV<br>
			 */
		public: 
			RSGISEstimationAlgorithmDualPolFPCSingleSpecies(int numOutputBands, double nonForestThreshold, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_vector *coeffFPCHH, gsl_vector *coeffFPCHV, gsl_vector *coeffFPCAttenuationH, gsl_vector *coeffFPCAttenuationV, estParameters parameters, rsgis::utils::treeSpecies species, int ittmax);
			virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
			~RSGISEstimationAlgorithmDualPolFPCSingleSpecies();
		protected:
			estParameters parameters;
			rsgis::utils::treeSpecies species;
			gsl_matrix *coeffHH;
			gsl_matrix *coeffHV;
			gsl_vector *coeffFPCHH;
			gsl_vector *coeffFPCHV;
			gsl_vector *coeffFPCAttenuationH;
			gsl_vector *coeffFPCAttenuationV;
			double nonForestThreshold;
			int order;
			int fpcOrder;
			int ittmax;
		};
		
		class DllExport RSGISEstimationAlgorithmFullPolSingleSpeciesPolyMask : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from full-pol SAR data for a single species defined using a mask
			public: 
				RSGISEstimationAlgorithmFullPolSingleSpeciesPolyMask(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV, estParameters parameters, gsl_vector *initialPar, int ittmax);
				virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
				~RSGISEstimationAlgorithmFullPolSingleSpeciesPolyMask();
			protected:
				estParameters parameters;
				rsgis::utils::treeSpecies species;
				gsl_matrix *coeffHH;
				gsl_matrix *coeffHV;
				gsl_matrix *coeffVV;
				gsl_vector *initialPar;
				int ittmax;
			};
		
		class DllExport RSGISEstimationAlgorithmDualPolSingleSpeciesPolyMask : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from dual-pol SAR data for a single species defined using a mask
			public: 
				RSGISEstimationAlgorithmDualPolSingleSpeciesPolyMask(int numOutputBands, double nonForestThreshold, gsl_matrix *coeffHH, gsl_matrix *coeffVV, estParameters parameters, gsl_vector *initialPar, int ittmax);
				virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
				~RSGISEstimationAlgorithmDualPolSingleSpeciesPolyMask();
			protected:
				estParameters parameters;
				rsgis::utils::treeSpecies species;
				gsl_matrix *coeffHH;
				gsl_matrix *coeffVV;
				gsl_vector *initialPar;
				int ittmax;
				double nonForestThreshold;
			};
		
		class DllExport RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from dual-pol SAR data, FPC and moisture for a single species
			/**
			 * Input is a 4 band image:<br>
			 * HH<br>
			 * HV<br>
			 * FPC<br>
			 * Volumetric soil moisture<br>
			 * Where there is no moisture information available, the 4th Image band must contain
			 * Average / estimated soil moisture
			 */
		public: 
			RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_vector *coeffFPCHH, gsl_vector *coeffFPCHV, gsl_vector *coeffFPCAttenuationH, gsl_vector *coeffFPCAttenuationV, estParameters parameters, rsgis::utils::treeSpecies species, int ittmax);
			virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
			~RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies();
		protected:
			estParameters parameters;
			rsgis::utils::treeSpecies species;
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
		
		class DllExport RSGISEstimationAlgorithmSingleSpecies : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from SAR data for a single species.
			/** 
			 * A class of the abstract type 'RSGISEstimationOptimiser' is passed in to provide an apropriately
			 * parameterised method of optimisation
			 * This is the most up to date version of the Estimation algorithm and is recomended for most applications.
			 * The older algorithms may give slightly faster performance as funcitions and first order derivatives are calculated simultaneously.  
			 */
		public: 
			RSGISEstimationAlgorithmSingleSpecies(int numOutputBands, 
																	  gsl_vector *initialPar,
																	  RSGISEstimationOptimiser *estOptimiser, 
																	  estParameters parameters,
																      double **minMaxVals = NULL);
			virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
			~RSGISEstimationAlgorithmSingleSpecies();
		protected:
			gsl_vector *initialPar;
			estParameters parameters;
			rsgis::utils::treeSpecies species;
			RSGISEstimationOptimiser *estOptimiser;
			int numOutputPar; // Number of output parameters
			int numOutputBands;
			double **minMaxVals;  // Array of arrays to hold min, max values for estimation
			bool useDefaultMinMax; // Use default min-max values for parameters
		};
		
		class DllExport RSGISEstimationAlgorithmSingleSpeciesMask : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from SAR data for a single species. Takes an D+1 band image (where D is the total number of data channels)
			/// where the first band is used to classify forest / non-forest areas.
			/** 
			 * A class of the abstract type 'RSGISEstimationOptimiser' is passed in to provide an apropriately
			 * parameterised method of optimisation
			 * This is the most up to date version of the Estimation algorithm and is recomended for most applications.
			 * The older algorithms may give slightly faster performance as funcitions and first order derivatives are calculated simultaneously.  
			 */
			
		public: 
			RSGISEstimationAlgorithmSingleSpeciesMask(int numOutputBands, 
																	  double nonForestThreshold,
																	  gsl_vector *initialPar,
																	  RSGISEstimationOptimiser *estOptimiser, 
																	  estParameters parameters,
																	  double **minMaxVals = NULL);
			virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
			~RSGISEstimationAlgorithmSingleSpeciesMask();
		protected:
			double nonForestThreshold;
			gsl_vector *initialPar;
			estParameters parameters;
			rsgis::utils::treeSpecies species;
			RSGISEstimationOptimiser *estOptimiser;
			int numOutputBands;
			double **minMaxVals; // Array of arrays to hold min, max values for estimation
			bool useDefaultMinMax; // Use default min-max values for parameters
		};
		
		class DllExport RSGISEstimationAlgorithmSingleSpeciesPixAP : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from SAR data for a single species. Takes pixel based a priori estimates.
			/** 
			 * A class of the abstract type 'RSGISEstimationOptimiser' is passed in to provide an apropriately
			 * parameterised method of optimisation
			 * This is the most up to date version of the Estimation algorithm and is recomended for most applications.
			 * The older algorithms may give slightly faster performance as funcitions and first order derivatives are calculated simultaneously.  
			 */
		public: 
			RSGISEstimationAlgorithmSingleSpeciesPixAP(int numOutputBands, 
												  gsl_vector *initialPar,
												  RSGISEstimationOptimiser *estOptimiser, 
												  estParameters parameters,
												  double **minMaxVals = NULL);
			virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
			~RSGISEstimationAlgorithmSingleSpeciesPixAP();
		protected:
			gsl_vector *initialPar;
			estParameters parameters;
			rsgis::utils::treeSpecies species;
			RSGISEstimationOptimiser *estOptimiser;
			int numOutputBands;
			int nPar;
			double **minMaxVals;  // Array of arrays to hold min, max values for estimation
			bool useDefaultMinMax; // Use default min-max values for parameters
		};
		
		class DllExport RSGISEstimationAlgorithmDualPolSingleSpeciesMaskPixAP : public rsgis::img::RSGISCalcImageValue
		{
			/** Estimates parameters from dual-pol SAR data for a single species. <br>
			 * A priori estimates for each pixel are passed in as image bands.<br>
			 * Band ordering is:<br>
			 * - Mask Layer<br>
			 * - Data1<br>
			 * - Data2<br>
			 * - AP1<br>
			 * - AP2<br>
			 * A class of the abstract type 'RSGISEstimationOptimiser' is passed in to provide an apropriately<br>
			 * parameterised method of optimisation<br>
			 */
		public: 
			RSGISEstimationAlgorithmDualPolSingleSpeciesMaskPixAP(int numOutputBands, 
																	  double nonForestThreshold,
																	  gsl_vector *initialPar,
																	  RSGISEstimationOptimiser *estOptimiser, 
																	  estParameters parameters);
			virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
			~RSGISEstimationAlgorithmDualPolSingleSpeciesMaskPixAP();
		protected:
			double nonForestThreshold;
			gsl_vector *initialPar;
			estParameters parameters;
			rsgis::utils::treeSpecies species;
			RSGISEstimationOptimiser *estOptimiser;
			gsl_vector *aPrioriPar;
		};
		
		class DllExport RSGISEstimationAlgorithmDualPolMultiSpeciesClassification : public rsgis::img::RSGISCalcImageValue
		{
			/// Estimates parameters from dual-pol SAR data for a single species. Takes a 3 band image
			/// where the first band is a classification.
			/** 
			 * Identical to 'RSGISEstimationAlgorithmDualPolSingleSpeciesMask' however rather<br>
			 * than using a binary mask (process / don't process) classes are used.
			 * For each class, inital parameters, an optimiser and species are passed in.
			 */
		public: 
			RSGISEstimationAlgorithmDualPolMultiSpeciesClassification(int numOutputBands, 
																				std::vector<gsl_vector*> *initialPar,
																				std::vector<RSGISEstimationOptimiser*> *estOptimiser, 
																				estParameters parameters,
																				std::vector<rsgis::utils::treeSpecies> *species);
			virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
			~RSGISEstimationAlgorithmDualPolMultiSpeciesClassification();
		protected:
			std::vector<gsl_vector*> *initialPar;
			estParameters parameters;
			std::vector<rsgis::utils::treeSpecies> *species;
			std::vector<RSGISEstimationOptimiser*> *estOptimiser;
		};
		
		class DllExport RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data : public rsgis::img::RSGISCalcImageValue
		{
			/// Generates simulated data
		public: 
			RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data(int numOutputBands, rsgis::math::RSGISMathTwoVariableFunction *hhFunction, rsgis::math::RSGISMathTwoVariableFunction *hvFunction);
			virtual void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
			virtual void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
			virtual void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
            void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
			virtual bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};													
			~RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data();
		protected:
			rsgis::math::RSGISMathTwoVariableFunction *hhFunction;
			rsgis::math::RSGISMathTwoVariableFunction *hvFunction;
		};
	}
}

#endif

