/*
 *  RSGISEstimationAlgorithm.cpp
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

#include "RSGISEstimationAlgorithm.h"

namespace rsgis {namespace radar
	{
		/***************************
		 * FULL-POL SINGLE SPECIES *
		 ***************************/
		RSGISEstimationAlgorithmFullPolSingleSpeciesPoly::RSGISEstimationAlgorithmFullPolSingleSpeciesPoly(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV, estParameters parameters, gsl_vector *initialPar, int ittmax) : RSGISCalcImageValue(numOutputBands)
		{
			this->coeffHH = coeffHH;
			this->coeffHV = coeffHV;
			this->coeffVV = coeffVV;
			this->initialPar = initialPar;
			this->ittmax = ittmax;
			this->parameters = parameters;
		}
		void RSGISEstimationAlgorithmFullPolSingleSpeciesPoly::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			RSGISEstimationConjugateGradient conjGrad;
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			species = rsgis::utils::aHarpophylla;
			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(numOutBands);
			inSigma0dB = gsl_vector_alloc(3);

			// Check for no data (image borders)
			if(bandValues[1] < -100)
			{
				for(int i = 0; i < numBands; i++)
				{
					output[i] = 0;
				}
			}
			else if(bandValues[1] >= 0 )
			{
				for(int i = 0; i < numBands; i++)
				{
					output[i] = 0;
				}
			}
			else // Start estimation
			{
				for(int i = 0; i < 3; i++)
				{
					gsl_vector_set(inSigma0dB, i, bandValues[i]);
				}
				if(parameters == cDepthDensity) // Retrieve Canopy Depth and Stem densty
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);

					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(2,2);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
					double pCov1 = 1e5;
					double pCov2 = 1e4;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateTwoDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double height = 0.0;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(cDepth > 3)
					{
						cDepth = 3;
					}
					else if(cDepth < 0.25)
					{
						cDepth = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass (currenly hardcoded using Scanlan's equations for Brigalow)

					//double height = cDepth * 240; // convert to cm
					// Convert canopy depth to height
					double aCoeff = 0.724416488101816;
					double bCoeff = 0.025097647604632307;
					height = 2 * (exp(log(cDepth)/aCoeff)-bCoeff) * 100;
					double tMass = allometric.calculateTotalBiomassHeight(height, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = cDepth;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

					gsl_vector_free(predictSigma);
					gsl_matrix_free(covMatrixP);
					gsl_matrix_free(invCovMatrixD);
				}
				else if(parameters == cDepthDensityReturnPredictSigma) // Retrieve Canopy Depth and Stem densty and write out predicted backscatter
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);

					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(2,2);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
					double pCov1 = 1e5;
					double pCov2 = 1e4;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateTwoDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double height = 0.0;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(cDepth > 3)
					{
						cDepth = 3;
					}
					else if(cDepth < 0.25)
					{
						cDepth = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass (currenly hardcoded using Scanlan's equations for Brigalow)
					height = cDepth * 240; // convert to cm
					double tMass = allometric.calculateTotalBiomassHeight(height, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = cDepth;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
					output[4] = gsl_vector_get(predictSigma, 0); // HH
					output[5] = gsl_vector_get(predictSigma, 1); // HV
					output[6] = gsl_vector_get(predictSigma, 2); // VV

					gsl_vector_free(predictSigma);
					gsl_matrix_free(covMatrixP);
					gsl_matrix_free(invCovMatrixD);
				}
				else if(parameters == diameterDensity) // Retrieve stem diameter and density
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);

					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(2,2);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
					double pCov1 = 1e5;
					double pCov2 = 1e4;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateTwoDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double diameter = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(diameter > 4.7)
					{
						diameter = 4.7;
					}
					else if(diameter < 0.7)
					{
						diameter = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					double diametercm = diameter * 10;
					double tMass = allometric.calculateTotalBiomassDiameter(diametercm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = diameter;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

					gsl_vector_free(predictSigma);
					gsl_matrix_free(covMatrixP);
					gsl_matrix_free(invCovMatrixD);
				}
				else if(parameters == diameterDensityReturnPredictSigma) // Retrieve stem diameter and density and write out predicted backscatter
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);

					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(2,2);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
					double pCov1 = 1e5;
					double pCov2 = 1e4;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateTwoDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double diameter = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(diameter > 4.7)
					{
						diameter = 4.7;
					}
					else if(diameter < 0.7)
					{
						diameter = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					double diametercm = diameter * 10;
					double tMass = allometric.calculateTotalBiomassDiameter(diametercm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = diameter;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
					output[4] = gsl_vector_get(predictSigma, 0); // HH
					output[5] = gsl_vector_get(predictSigma, 1); // HV
					output[6] = gsl_vector_get(predictSigma, 2); // VV

					gsl_vector_free(predictSigma);
					gsl_matrix_free(covMatrixP);
					gsl_matrix_free(invCovMatrixD);
				}
				else if(parameters == dielectricDensityHeight) // Retrieve dielectric constant, density and height
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);

					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(3,3);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.

					//double pCov1 = 1;
					//double pCov2 = 1;
					//double pCov3 = 1;
					double pCov1 = (gsl_vector_get(initialPar,0)/gsl_vector_get(initialPar,2)) * 4e5;
					double pCov2 = 1e7;
					double pCov3 = gsl_vector_get(initialPar,1)/gsl_vector_get(initialPar,2) * 1e5;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);
					gsl_matrix_set(covMatrixP, 2, 2, pCov3);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateThreeDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double dielectric = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double cDepth = gsl_vector_get(outPar, 2);
					double error = gsl_vector_get(outPar, 3);
					// Set parameters to limits of equation
					if(dielectric > 60)
					 {
					 dielectric = 60;
					 }
					 else if(dielectric < 1)
					 {
					 dielectric = 1;
					 }
					 if(density > 2)
					 {
					 density = 2;
					 }
					 else if(density < 0.1)
					 {
					 density = 0;
					 }
					if(cDepth > 15)
					{
						cDepth = 1;
					}
					else if(cDepth < 0)
					{
						cDepth = 0;
					}

					/*
					 // Calculate Biomass
					 double diametercm = diameter * 10;
					 double tMass = allometric.calculateTotalBiomassDiameter(diametercm, species);
					 double biomass = ((tMass*(density*10000)))/1000000;*/
					double biomass = 0;

					// Write out
					output[0] = dielectric;
					output[1] = density;
					output[2] = cDepth;
					output[3] = biomass;
					output[4] = error;

					gsl_vector_free(predictSigma);
					gsl_matrix_free(covMatrixP);
					gsl_matrix_free(invCovMatrixD);
				}
				else if(parameters == dielectricDensityHeightPredictSigma) // Retrieve dielectric constant, density and height and write out predicted backscatter
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);

					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(3,3);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.

					//double pCov1 = 1;
					//double pCov2 = 1;
					//double pCov3 = 1;

					double pCov1 = (gsl_vector_get(initialPar,0)/gsl_vector_get(initialPar,2)) * 4e8;
					double pCov2 = 1e7;
					double pCov3 = gsl_vector_get(initialPar,1)/gsl_vector_get(initialPar,2) * 1e8;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);
					gsl_matrix_set(covMatrixP, 2, 2, pCov3);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateThreeDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double dielectric = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double cDepth = gsl_vector_get(outPar, 2);
					double error = gsl_vector_get(outPar, 3);
					// Set parameters to limits of equation
					if(dielectric > 60)
					{
						dielectric = 60;
					}
					else if(dielectric < 1)
					{
						dielectric = 1;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					if(cDepth > 15)
					{
						cDepth = 1;
					}
					else if(cDepth < 0)
					{
						cDepth = 0;
					}

					/*
					 // Calculate Biomass
					 double diametercm = diameter * 10;
					 double tMass = allometric.calculateTotalBiomassDiameter(diametercm, species);
					 double biomass = ((tMass*(density*10000)))/1000000;*/
					double biomass = 0;

					// Write out
					output[0] = dielectric;
					output[1] = density;
					output[2] = cDepth;
					output[3] = biomass;
					output[4] = error;
					output[5] = gsl_vector_get(predictSigma, 0); // HH
					output[6] = gsl_vector_get(predictSigma, 1); // HV
					output[7] = gsl_vector_get(predictSigma, 2); // VV

					gsl_vector_free(predictSigma);
				}
				else
				{
					std::cout << "Parameters not recognised, cannot calculate biomass.";
				}

			}

			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
		}
		RSGISEstimationAlgorithmFullPolSingleSpeciesPoly::~RSGISEstimationAlgorithmFullPolSingleSpeciesPoly()
		{

		}

		/***************************
		 * DUAL-POL SINGLE SPECIES *
		 ***************************/
		RSGISEstimationAlgorithmDualPolSingleSpeciesPoly::RSGISEstimationAlgorithmDualPolSingleSpeciesPoly(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffVV,  estParameters parameters, gsl_vector *initialPar, int ittmax) : RSGISCalcImageValue(numOutputBands)
		{
			this->coeffHH = coeffHH;
			this->coeffVV = coeffVV;
			this->initialPar = initialPar;
			this->ittmax = ittmax;
			this->parameters = parameters;
		}
		void RSGISEstimationAlgorithmDualPolSingleSpeciesPoly::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			RSGISEstimationConjugateGradient conjGrad;
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			species = rsgis::utils::aHarpophylla;
			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(numOutBands);
			inSigma0dB = gsl_vector_alloc(numBands);

			/********************************
			 * SET UP COVARIENCE MATRICES    *
			 *********************************/

			gsl_matrix *covMatrixP;
			gsl_matrix *invCovMatrixD;
			covMatrixP = gsl_matrix_alloc(2,2);
			invCovMatrixD = gsl_matrix_alloc(2, 2);
			gsl_matrix_set_zero(covMatrixP);
			gsl_matrix_set_zero(invCovMatrixD);

			// Set the covarience matrix and inverse covatience matrix for the a prior estimates.

			double pCov1 = 1; //(gsl_vector_get(initialPar, 0))/(gsl_vector_get(initialPar, 1)) * 1e5;
			double pCov2 = 1; //1e5;

			gsl_matrix_set(covMatrixP, 0, 0, pCov1);
			gsl_matrix_set(covMatrixP, 1, 1, pCov2);

			// Set the inverse covarience matrix for the data (only use inverse matrix)
			double dCovInv1 = 1;
			double dCovInv2 = 1;
			gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
			gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

			/***********************************/

			// Check for no data (image borders)
			if(bandValues[1] < -100)
			{
				for(int i = 0; i < numBands; i++)
				{
					output[i] = 0;
				}
			}
			else if(bandValues[1] >= 0 )
			{
				for(int i = 0; i < numBands; i++)
				{
					output[i] = 0;
				}
			}
			else // Start Estimation
			{
				for(int i = 0; i < 2; i++)
				{
					gsl_vector_set(inSigma0dB, i, bandValues[i]);
				}

				if(parameters == cDepthDensity)  // Retrieve Canopy Depth and Stem densty
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(2);
					gsl_vector_set_zero(predictSigma);

					conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double height = 0;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);

					// Set parameters to limits of equation
					if(cDepth > 3)
					{
						cDepth = 3;
					}
					else if(cDepth < 0.25)
					{
						cDepth = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}

					// Calculate Biomass (currenly hardcoded using Scanlan's equations for Brigalow)
					height = cDepth * 200; // convert to cm

                    double tMass = allometric.calculateTotalBiomassHeight(height, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = cDepth;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

					gsl_vector_free(predictSigma);

				}
				else if(parameters == cDepthDensityReturnPredictSigma) // Retrieve Canopy Depth and Stem densty and write out predicted backscatter
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(2);
					gsl_vector_set_zero(predictSigma);

					conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double height;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(cDepth > 3)
					{
						cDepth = 3;
					}
					else if(cDepth < 0.25)
					{
						cDepth = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass (currenly hardcoded using Scanlan's equations for Brigalow)
					height = cDepth * 200; // convert to cm
					double tMass = allometric.calculateTotalBiomassHeight(height, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = cDepth;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
					output[4] = gsl_vector_get(predictSigma, 0); // HH
					output[5] = gsl_vector_get(predictSigma, 1); // VV

					gsl_vector_free(predictSigma);

				}
				else if(parameters == diameterDensity) // Retrieve stem diameter and density
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(2);
					gsl_vector_set_zero(predictSigma);

					conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double diameter = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(diameter > 4.7)
					{
						diameter = 4.7;
					}
					else if(diameter < 0.7)
					{
						diameter = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					double diametermm = diameter * 10;
					double tMass = allometric.calculateTotalBiomassDiameter(diametermm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = diameter;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

					gsl_vector_free(predictSigma);

				}
				else if(parameters == diameterDensityReturnPredictSigma) // Retrieve stem diameter and density and write out predicted backscatter
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(2);
					gsl_vector_set_zero(predictSigma);

					conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double diameter = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(diameter > 4.7)
					{
						diameter = 4.7;
					}
					else if(diameter < 0.7)
					{
						diameter = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					double diametercm = diameter * 10;
					double tMass = allometric.calculateTotalBiomassDiameter(diametercm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = diameter;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
					output[4] = gsl_vector_get(predictSigma, 0); // HH
					output[5] = gsl_vector_get(predictSigma, 2); // VV
					gsl_vector_free(predictSigma);

				}
				else
				{
					std::cout << "Parameters not recognised, cannot calculate biomass.";
				}

			}


			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
			gsl_matrix_free(covMatrixP);
			gsl_matrix_free(invCovMatrixD);
		}
		RSGISEstimationAlgorithmDualPolSingleSpeciesPoly::~RSGISEstimationAlgorithmDualPolSingleSpeciesPoly()
		{

		}

		/*************************************************************
		 * DUAL-POL SINGLE SPECIES WITH FPC                          *
		 * - FPC used to calculate canopy scattering and attenuation *
		 *************************************************************/
		RSGISEstimationAlgorithmDualPolFPCSingleSpecies::RSGISEstimationAlgorithmDualPolFPCSingleSpecies(int numOutputBands, double nonForestThreshold, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_vector *coeffFPCHH, gsl_vector *coeffFPCHV, gsl_vector *coeffFPCAttenuationH, gsl_vector *coeffFPCAttenuationV, estParameters parameters, rsgis::utils::treeSpecies species, int ittmax) : RSGISCalcImageValue(numOutputBands)
		{
			this->coeffHH = coeffHH;
			this->coeffHV = coeffHV;
			this->coeffFPCHH = coeffFPCHH;
			this->coeffFPCHV = coeffFPCHV;
			this->coeffFPCAttenuationH = coeffFPCAttenuationH;
			this->coeffFPCAttenuationV = coeffFPCAttenuationV;
			this->ittmax = ittmax;
			this->parameters = parameters;
			this->order = coeffHH->size2 - 1;
			this->fpcOrder = coeffFPCHV->size;
			this->nonForestThreshold = nonForestThreshold;

			std::cout << "FPC Order = " << fpcOrder << std::endl;

			this->species = species;
		}
		void RSGISEstimationAlgorithmDualPolFPCSingleSpecies::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			RSGISEstimationConjugateGradient conjGrad;
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(numOutBands);
			inSigma0dB = gsl_vector_alloc(numBands);

			/********************************************************
			 * CALCULATE CANOPY SCATTERING AND ATTENUATION FROM FPC *
			 ********************************************************/

			double fpc = bandValues[0];

			double *sigmaTrunkGround = new double[2];

			rsgis::math::RSGISFunctionPolynomialGSL *canopyScatteringFunctionHH;
			rsgis::math::RSGISFunctionPolynomialGSL *canopyScatteringFunctionHV;

			rsgis::math::RSGISFunctionPolynomialGSL *canopyAttenuationFunctionH;
			rsgis::math::RSGISFunctionPolynomialGSL *canopyAttenuationFunctionV;

			canopyScatteringFunctionHH = new rsgis::math::RSGISFunctionPolynomialGSL(coeffFPCHH, fpcOrder);
			canopyScatteringFunctionHV = new rsgis::math::RSGISFunctionPolynomialGSL(coeffFPCHV, fpcOrder);

			canopyAttenuationFunctionH = new rsgis::math::RSGISFunctionPolynomialGSL(coeffFPCAttenuationH, fpcOrder);
			canopyAttenuationFunctionV = new rsgis::math::RSGISFunctionPolynomialGSL(coeffFPCAttenuationV, fpcOrder);

			RSGISEstimationFPCDualPolTrunkGround *calcTrunkGround;

			calcTrunkGround = new RSGISEstimationFPCDualPolTrunkGround(2, canopyScatteringFunctionHH, canopyScatteringFunctionHV, canopyAttenuationFunctionH, canopyAttenuationFunctionV);

			calcTrunkGround->calcValue(fpc, bandValues[1], bandValues[2], sigmaTrunkGround);

			double sigmaHHTrunkGround = sigmaTrunkGround[0];
			double sigmaHVTrunkGround = sigmaTrunkGround[1];

			/************************
			 * SET INPUT PARAMETERS *
			 ************************/

			gsl_vector *initialPar;
			initialPar = gsl_vector_alloc(2);
			double initialDiameter = 2; // Set diameter (cm)
			double initialDensity = 0.7; // Set stem density (stems / ha)

			gsl_vector_set(initialPar, 0, initialDiameter);
			gsl_vector_set(initialPar, 1, initialDensity);

			/******************************
			 * SET UP COVARIENCE MATRICES *
			 ******************************/

			gsl_matrix *covMatrixP;
			gsl_matrix *invCovMatrixD;
			covMatrixP = gsl_matrix_alloc(2,2);
			invCovMatrixD = gsl_matrix_alloc(2, 2);
			gsl_matrix_set_zero(covMatrixP);
			gsl_matrix_set_zero(invCovMatrixD);

			// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
			double pCov1 = 1; //(initialDiameter/initialDensity)*1e3;
			double pCov2 = 1; //1e7;

			gsl_matrix_set(covMatrixP, 0, 0, pCov1);
			gsl_matrix_set(covMatrixP, 1, 1, pCov2);

			// Set the inverse covarience matrix for the data (only use inverse matrix)
			double dCovInv1 = 1;
			double dCovInv2 = 1;
			gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
			gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

			/***********************************/

			gsl_vector_set(inSigma0dB, 0, sigmaHHTrunkGround); // HH Trunk-Ground Scattering
			gsl_vector_set(inSigma0dB, 1, sigmaHVTrunkGround); // HV Trunk-Ground Scattering

			if(parameters == diameterDensity) // Retrieve stem diameter and density
			{

				if (bandValues[0] < this->nonForestThreshold) // If FPC < non-forest mask set to zero.
				{
					output[0] = 0;
					output[1] = 0;
					output[2] = 0;
					output[3] = -999;
				}
				else
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(2);
					gsl_vector_set_zero(predictSigma);


					double bestError = 999;
					double diameter;
					double density;
					double error;

					for(int runs = 0; runs < 10; runs++)
					{

						gsl_vector_set(initialPar,0,runs+1);
						gsl_vector_set(initialPar,0,(2.2 -(runs+1 * 0.2)));

						conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffHV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

						if(gsl_vector_get(outPar, 2) < bestError)
						{
							diameter = gsl_vector_get(outPar, 0);
							density = gsl_vector_get(outPar, 1);
							error = gsl_vector_get(outPar, 2);
							bestError = error;
						}


					}


					conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffHV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					// Set parameters to limits of equation
					if(diameter > 10.2)
					{
						diameter = 10.2;
					}
                    
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}

					// Calculate Biomass
					double tMass = allometric.calculateTotalBiomassDiameter(diameter * 10, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = diameter;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

					gsl_vector_free(predictSigma);

				}
			}
			else if(parameters == diameterDensityReturnPredictSigma) // Retrieve stem diameter and density and write out predicted backscatter
			{
				gsl_vector *predictSigma;
				predictSigma = gsl_vector_alloc(2);
				gsl_vector_set_zero(predictSigma);

				conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffHV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

				double diameter = gsl_vector_get(outPar, 0);
				double density = gsl_vector_get(outPar, 1);
				double error = gsl_vector_get(outPar, 2);
				// Set parameters to limits of equation
				if(diameter > 10.2)
				{
					diameter = 10.2;
				}
				else if(diameter < 1.38)
				{
					diameter = 0;
				}
				if(density > 2)
				{
					density = 2;
				}
				else if(density < 0.1)
				{
					density = 0;
				}

				// Calculate Biomass
				//double diametercm = diameter * 10;
				double tMass = allometric.calculateTotalBiomassDiameter(diameter * 10, species);
				double biomass = ((tMass*(density*10000)))/1000000;

				// Write out
				output[0] = diameter;
				output[1] = density;
				output[2] = biomass;
				output[3] = error;
				output[4] = gsl_vector_get(predictSigma, 0); // HH
				output[5] = gsl_vector_get(predictSigma, 1); // HV
				gsl_vector_free(predictSigma);

			}
			else
			{
				std::cout << "Parameters not recognised, cannot calculate biomass.";
			}

			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
			gsl_matrix_free(covMatrixP);
			gsl_matrix_free(invCovMatrixD);
			gsl_vector_free(initialPar);
		}
		RSGISEstimationAlgorithmDualPolFPCSingleSpecies::~RSGISEstimationAlgorithmDualPolFPCSingleSpecies()
		{

		}

		/*******************************************************
		 * FULL-POL SINGLE SPECIES WITH MASK                   *
		 * - Mask used to determine areas estimation is run on *
		 *******************************************************/
		RSGISEstimationAlgorithmFullPolSingleSpeciesPolyMask::RSGISEstimationAlgorithmFullPolSingleSpeciesPolyMask(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV, estParameters parameters, gsl_vector *initialPar, int ittmax) : RSGISCalcImageValue(numOutputBands)
		{
			this->coeffHH = coeffHH;
			this->coeffHV = coeffHV;
			this->coeffVV = coeffVV;
			this->initialPar = initialPar;
			this->ittmax = ittmax;
			this->parameters = parameters;
		}
		void RSGISEstimationAlgorithmFullPolSingleSpeciesPolyMask::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			RSGISEstimationConjugateGradient conjGrad;
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			species = rsgis::utils::aHarpophylla;
			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(numOutBands);
			inSigma0dB = gsl_vector_alloc(numBands);
			if(bandValues[0] == 1) // Mask
			{
				for(int i = 0; i < 3; i++)
				{
					gsl_vector_set(inSigma0dB, i, bandValues[i+1]);
				}

				if(parameters == cDepthDensity) // Retrieve Canopy Depth and Stem densty
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);


					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(2,2);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
					double pCov1 = 1e5;
					double pCov2 = 1e4;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateTwoDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double height;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(cDepth > 3)
					{
						cDepth = 3;
					}
					else if(cDepth < 0.25)
					{
						cDepth = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass (currenly hardcoded using Scanlan's equations for Brigalow)
					height = cDepth * 240; // convert to cm
					double tMass = allometric.calculateTotalBiomassHeight(height, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = cDepth;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

					gsl_vector_free(predictSigma);
				}
				else if(parameters == cDepthDensityReturnPredictSigma) // Retrieve Canopy Depth and Stem densty and write out predicted backscatter
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);

					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(2,2);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
					double pCov1 = 1e5;
					double pCov2 = 1e4;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateTwoDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double height;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(cDepth > 3)
					{
						cDepth = 3;
					}
					else if(cDepth < 0.25)
					{
						cDepth = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass (currenly hardcoded using Scanlan's equations for Brigalow)
					height = cDepth * 240; // convert to cm
					double tMass = allometric.calculateTotalBiomassHeight(height, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = cDepth;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
					output[4] = gsl_vector_get(predictSigma, 0); // HH
					output[5] = gsl_vector_get(predictSigma, 1); // HV
					output[6] = gsl_vector_get(predictSigma, 2); // VV
					gsl_vector_free(predictSigma);
				}
				else if(parameters == diameterDensity) // Retrieve stem diameter and density
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);

					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(2,2);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
					double pCov1 = 1e5;
					double pCov2 = 1e4;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateTwoDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double diameter = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(diameter > 4.7)
					{
						diameter = 4.7;
					}
					else if(diameter < 0.7)
					{
						diameter = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					double diametercm = diameter * 10;
					double tMass = allometric.calculateTotalBiomassDiameter(diametercm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = diameter;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

					gsl_vector_free(predictSigma);
				}
				else if(parameters == diameterDensityReturnPredictSigma) // Retrieve stem diameter and density and write out predicted backscatter
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(3);
					gsl_vector_set_zero(predictSigma);

					/********************************
					 * SET UP COVARIENCE MATRICES    *
					 *********************************/

					gsl_matrix *covMatrixP;
					gsl_matrix *invCovMatrixD;
					covMatrixP = gsl_matrix_alloc(2,2);
					invCovMatrixD = gsl_matrix_alloc(3, 3);
					gsl_matrix_set_zero(covMatrixP);
					gsl_matrix_set_zero(invCovMatrixD);

					// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
					double pCov1 = 1e5;
					double pCov2 = 1e4;

					gsl_matrix_set(covMatrixP, 0, 0, pCov1);
					gsl_matrix_set(covMatrixP, 1, 1, pCov2);

					// Set the inverse covarience matrix for the data (only use inverse matrix)
					double dCovInv1 = 1;
					double dCovInv2 = 1;
					double dCovInv3 = 1;

					gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
					gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
					gsl_matrix_set(invCovMatrixD, 2, 2, dCovInv3);

					/***********************************/

					conjGrad.estimateTwoDimensionalPolyThreeChannel(inSigma0dB, coeffHH, coeffHV, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);


					double diameter = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(diameter > 4.7)
					{
						diameter = 4.7;
					}
					else if(diameter < 0.7)
					{
						diameter = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					double diametercm = diameter * 10;
					double tMass = allometric.calculateTotalBiomassDiameter(diametercm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = diameter;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
					output[4] = gsl_vector_get(predictSigma, 0); // HH
					output[5] = gsl_vector_get(predictSigma, 1); // HV
					output[6] = gsl_vector_get(predictSigma, 2); // VV
					gsl_vector_free(predictSigma);
				}
				else
				{
					std::cout << "Parameters not recognised, cannot calculate biomass.";
				}
			}
			else // Not mask
			{
				for(int i = 0; i < this->numOutBands; i++)
				{
					output[i] = 0; // Set output to zero where not in mask.
				}
			}

			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
		}
		RSGISEstimationAlgorithmFullPolSingleSpeciesPolyMask::~RSGISEstimationAlgorithmFullPolSingleSpeciesPolyMask()
		{

		}

		/*******************************************************
		 * DUAL-POL SINGLE SPECIES WITH MASK                   *
		 * - Mask used to determine areas estimation is run on *
		 *******************************************************/
		RSGISEstimationAlgorithmDualPolSingleSpeciesPolyMask::RSGISEstimationAlgorithmDualPolSingleSpeciesPolyMask(int numOutputBands, double nonForestThreshold, gsl_matrix *coeffHH, gsl_matrix *coeffVV, estParameters parameters, gsl_vector *initialPar, int ittmax) : RSGISCalcImageValue(numOutputBands)
		{
			this->coeffHH = coeffHH;
			this->coeffVV = coeffVV;
			this->initialPar = initialPar;
			this->ittmax = ittmax;
			this->parameters = parameters;
			this->nonForestThreshold = nonForestThreshold;
			std::cout << "numOutputBands " << numOutputBands << std::endl;
		}
		void RSGISEstimationAlgorithmDualPolSingleSpeciesPolyMask::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			RSGISEstimationConjugateGradient conjGrad;
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			species = rsgis::utils::aHarpophylla;
			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(numOutBands-1);
			inSigma0dB = gsl_vector_alloc(2);

			/********************************
			 * SET UP COVARIENCE MATRICES   *
			 ********************************/

			gsl_matrix *covMatrixP;
			gsl_matrix *invCovMatrixD;
			covMatrixP = gsl_matrix_alloc(2,2);
			invCovMatrixD = gsl_matrix_alloc(2, 2);
			gsl_matrix_set_zero(covMatrixP);
			gsl_matrix_set_zero(invCovMatrixD);

			// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
			double pCov1 = 1e10;//(gsl_vector_get(initialPar, 0))/(gsl_vector_get(initialPar, 1)) * 1e5;
			double pCov2 = 1e10;//1e5;

			gsl_matrix_set(covMatrixP, 0, 0, pCov1);
			gsl_matrix_set(covMatrixP, 1, 1, pCov2);

			// Set the inverse covarience matrix for the data (only use inverse matrix)
			double dCovInv1 = 1;
			double dCovInv2 = 1;
			gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
			gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);
			/***********************************/

			if(bandValues[0] > this->nonForestThreshold) // Mask
			{
				for(int i = 0; i < 2; i++)
				{
					gsl_vector_set(inSigma0dB, i, bandValues[i+1]);
				}

				if(parameters == cDepthDensity)  // Retrieve Canopy Depth and Stem densty
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(2);
					gsl_vector_set_zero(predictSigma);

					conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double height;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(cDepth > 3)
					{
						cDepth = 3;
					}
					else if(cDepth < 0.25)
					{
						cDepth = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
                    
					// Calculate Biomass
                    height = cDepth / 0.3295;
					double heightcm = height * 100;
					double tMass = allometric.calculateTotalBiomassHeight(heightcm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					//output[0] = cDepth;
					output[0] = height;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
				}
				else if(parameters == cDepthDensityReturnPredictSigma) // Retrieve Canopy Depth and Stem densty and write out predicted backscatter
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(2);
					gsl_vector_set_zero(predictSigma);

					conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double height;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					height = cDepth / 0.3295;
					double heightcm = height * 100;
					double tMass = allometric.calculateTotalBiomassHeight(heightcm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = cDepth;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
					output[4] = gsl_vector_get(predictSigma, 0); // HH
					output[5] = gsl_vector_get(predictSigma, 1); // VV
					gsl_vector_free(predictSigma);
				}
				else if(parameters == diameterDensity) // Retrieve stem diameter and density
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(2);
					gsl_vector_set_zero(predictSigma);

					conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double diameter = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(diameter > 4.7)
					{
						diameter = 4.7;
					}
					else if(diameter < 0.7)
					{
						diameter = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					double diametercm = diameter * 10;
					double tMass = allometric.calculateTotalBiomassDiameter(diametercm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = diameter;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
				}
				else if(parameters == diameterDensityReturnPredictSigma) // Retrieve stem diameter and density and write out predicted backscatter
				{
					gsl_vector *predictSigma;
					predictSigma = gsl_vector_alloc(2);
					gsl_vector_set_zero(predictSigma);

					conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffVV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

					double diameter = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(diameter > 4.7)
					{
						diameter = 4.7;
					}
					else if(diameter < 0.7)
					{
						diameter = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					double diametercm = diameter * 10;
					double tMass = allometric.calculateTotalBiomassDiameter(diametercm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = diameter;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
					output[4] = gsl_vector_get(predictSigma, 0); // HH
					output[5] = gsl_vector_get(predictSigma, 2); // VV

					gsl_vector_free(predictSigma);
				}
				else
				{
					std::cout << "Parameters not recognised, cannot calculate biomass.";
				}
			}
			else // Not mask
			{
				for(int i = 0; i < numOutBands; i++)
				{
					output[i] = 0; // Set output to zero where not in mask.
				}
			}
			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
		}
		RSGISEstimationAlgorithmDualPolSingleSpeciesPolyMask::~RSGISEstimationAlgorithmDualPolSingleSpeciesPolyMask()
		{
		}

		/**************************************************************
		 * DUAL-POL SINGLE SPECIES WITH FPC AND SOIL MOISTURE         *
		 * - FPC used to calculate canopy scattering and attenuation  *
		 * - Soil moisture information used                           *
		 **************************************************************/
		RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies::RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_vector *coeffFPCHH, gsl_vector *coeffFPCHV, gsl_vector *coeffFPCAttenuationH, gsl_vector *coeffFPCAttenuationV, estParameters parameters, rsgis::utils::treeSpecies species, int ittmax) : RSGISCalcImageValue(numOutputBands)
		{
			this->coeffHH = coeffHH;
			this->coeffHV = coeffHV;
			this->coeffFPCHH = coeffFPCHH;
			this->coeffFPCHV = coeffFPCHV;
			this->coeffFPCAttenuationH = coeffFPCAttenuationH;
			this->coeffFPCAttenuationV = coeffFPCAttenuationV;
			this->ittmax = ittmax;
			this->parameters = parameters;
			this->order = coeffHH->size2 - 1;
			this->fpcOrder = coeffFPCHV->size - 1;
			this->species = species;
		}
		void RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			RSGISEstimationConjugateGradient conjGrad;
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(numOutBands);
			inSigma0dB = gsl_vector_alloc(numBands);

			/********************************************************
			 * CALCULATE CANOPY SCATTERING AND ATTENUATION FROM FPC *
			 ********************************************************/

			double fpc = bandValues[0];

			double *sigmaTrunkGround = new double[2];

			rsgis::math::RSGISFunctionPolynomialGSL *canopyScatteringFunctionHH;
			rsgis::math::RSGISFunctionPolynomialGSL *canopyScatteringFunctionHV;

			rsgis::math::RSGISFunctionPolynomialGSL *canopyAttenuationFunctionH;
			rsgis::math::RSGISFunctionPolynomialGSL *canopyAttenuationFunctionV;

			canopyScatteringFunctionHH = new rsgis::math::RSGISFunctionPolynomialGSL(coeffFPCHH, fpcOrder);
			canopyScatteringFunctionHV = new rsgis::math::RSGISFunctionPolynomialGSL(coeffFPCHV, fpcOrder);

			canopyAttenuationFunctionH = new rsgis::math::RSGISFunctionPolynomialGSL(coeffFPCAttenuationH, fpcOrder);
			canopyAttenuationFunctionV = new rsgis::math::RSGISFunctionPolynomialGSL(coeffFPCAttenuationV, fpcOrder);

			RSGISEstimationFPCDualPolTrunkGround *calcTrunkGround;

			calcTrunkGround = new RSGISEstimationFPCDualPolTrunkGround(2, canopyScatteringFunctionHH, canopyScatteringFunctionHV, canopyAttenuationFunctionH, canopyAttenuationFunctionV);

			calcTrunkGround->calcValue(fpc, bandValues[1], bandValues[2], sigmaTrunkGround);

			double sigmaHHTrunkGround = sigmaTrunkGround[0];
			double sigmaHVTrunkGround = sigmaTrunkGround[1];

			/***************************
			 * CALCULATE SOIL MOISTURE *
			 ***************************/

			double realDielectric = 0;

			if (bandValues[3] > 0)
			{
				// Assume soil is Vertisol

				double frequency = 1.25e9; // L-Band
				double temperature = 20;
				double bulkDensity = 1.55;
				double clay = 0.5;
				double sand = 0.4;
				double soilMoisture = bandValues[3] / 100; // Convert from percent to fraction

				RSGISSoilDielectricMixingModel *calcDielectric = NULL;
				calcDielectric = new RSGISSoilDielectricMixingModel(frequency, sand, clay, temperature, bulkDensity, soilMoisture);
				realDielectric = calcDielectric->calcRealDielectric();
				delete calcDielectric;
			}
			else
			{
				realDielectric = 5.0; // Assume a soil moisture of 5 %.
			}

			/*************************************
			 * ADJUST COEFFICIENTS BASED ON SOIL *
			 * MOISTURE							 *
			 *************************************/

			gsl_matrix *newCoeffHH; // Matrix to hold new HH coefficients, once epsilon has been fixed
			gsl_matrix *newCoeffHV; // Matric to hold new HV coefficients, once epsilon has been fixed
			gsl_vector *epsPow; // Vector to hold powers of dielectric constant

			newCoeffHH = gsl_matrix_alloc(order, order);
			newCoeffHV = gsl_matrix_alloc(order, order);
			epsPow = gsl_vector_alloc(order);

			double zPow, cCoeff, cCoeffPowZ;

			//Set up vector to hold powers of epsilon
			for(int z = 0; z < order;z++)
			{
				gsl_vector_set(epsPow, z, pow(realDielectric, z));
			}

			// HH
			int c = 0;
			for (int x = 0; x < order;x++)
			{
				for (int y = 0; y < order;y++)
				{
					cCoeffPowZ = 0;
					for (int z = 0; z < order; z++)
					{
						zPow = gsl_vector_get(epsPow, z);
						cCoeff = gsl_matrix_get(coeffHH, c, z);
						cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
					}
					gsl_matrix_set(newCoeffHH, x, y, cCoeffPowZ);
					c++; // Go through lines in input coefficients
				}
			}

			// HV
			c = 0;
			for (int x = 0; x < order;x++)
			{
				for (int y = 0; y < order;y++)
				{
					cCoeffPowZ = 0;
					for (int z = 0; z < order; z++)
					{
						zPow = gsl_vector_get(epsPow, z);
						cCoeff = gsl_matrix_get(coeffHV, c, z);
						cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
					}
					gsl_matrix_set(newCoeffHV, x, y, cCoeffPowZ);
					c++; // Go through lines in in coefficients
				}
			}

			/************************
			 * SET INPUT PARAMETERS *
			 ************************/

			gsl_vector *initialPar;
			initialPar = gsl_vector_alloc(2);
			double initialDiameter = 5; // Set diameter (cm)
			double initialDensity = 0.5; // Set stem density (stems / ha)

			gsl_vector_set(initialPar, 0, initialDiameter);
			gsl_vector_set(initialPar, 1, initialDensity);

			/******************************
			 * SET UP COVARIENCE MATRICES *
			 ******************************/

			gsl_matrix *covMatrixP;
			gsl_matrix *invCovMatrixD;
			covMatrixP = gsl_matrix_alloc(2,2);
			invCovMatrixD = gsl_matrix_alloc(2, 2);
			gsl_matrix_set_zero(covMatrixP);
			gsl_matrix_set_zero(invCovMatrixD);

			// Set the covarience matrix and inverse covatience matrix for the a prior estimates.
			double pCov1 = (initialDiameter/initialDensity)*1e3;
			double pCov2 = 1e7;

			gsl_matrix_set(covMatrixP, 0, 0, pCov1);
			gsl_matrix_set(covMatrixP, 1, 1, pCov2);

			// Set the inverse covarience matrix for the data (only use inverse matrix)
			double dCovInv1 = 1;
			double dCovInv2 = 1;
			gsl_matrix_set(invCovMatrixD, 0, 0, dCovInv1); // Set diagonal elements of the matrix
			gsl_matrix_set(invCovMatrixD, 1, 1, dCovInv2);

			/***********************************/

			gsl_vector_set(inSigma0dB, 0, sigmaHHTrunkGround); // HH Trunk-Ground Scattering
			gsl_vector_set(inSigma0dB, 1, sigmaHVTrunkGround); // HV Trunk-Ground Scattering

			if(parameters == diameterDensity) // Retrieve stem diameter and density
			{
				gsl_vector *predictSigma;
				predictSigma = gsl_vector_alloc(2);
				gsl_vector_set_zero(predictSigma);

				conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, newCoeffHH, newCoeffHV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

				double diameter = gsl_vector_get(outPar, 0);
				double density = gsl_vector_get(outPar, 1);
				double error = gsl_vector_get(outPar, 2);
				// Set parameters to limits of equation
				if(diameter > 10.2)
				{
					diameter = 10.2;
				}
				else if(diameter < 1.38)
				{
					diameter = 0;
				}
				if(density > 2)
				{
					density = 2;
				}
				else if(density < 0.1)
				{
					density = 0;
				}

				// Calculate Biomass
				double tMass = allometric.calculateTotalBiomassDiameter(diameter * 10, species);
				double biomass = ((tMass*(density*10000)))/1000000;

				// Write out
				output[0] = diameter;
				output[1] = density;
				output[2] = biomass;
				output[3] = error;

				gsl_vector_free(predictSigma);

			}
			else if(parameters == diameterDensityReturnPredictSigma) // Retrieve stem diameter and density and write out predicted backscatter
			{
				gsl_vector *predictSigma;
				predictSigma = gsl_vector_alloc(2);
				gsl_vector_set_zero(predictSigma);

				conjGrad.estimateTwoDimensionalPolyTwoChannel(inSigma0dB, coeffHH, coeffHV, initialPar, outPar, predictSigma, covMatrixP, invCovMatrixD, ittmax);

				double diameter = gsl_vector_get(outPar, 0);
				double density = gsl_vector_get(outPar, 1);
				double error = gsl_vector_get(outPar, 2);
				// Set parameters to limits of equation
				if(diameter > 10.2)
				{
					diameter = 10.2;
				}
				else if(diameter < 1.38)
				{
					diameter = 0;
				}
				if(density > 2)
				{
					density = 2;
				}
				else if(density < 0.1)
				{
					density = 0;
				}

				// Calculate Biomass
				//double diametercm = diameter * 10;
				double tMass = allometric.calculateTotalBiomassDiameter(diameter * 10, species);
				double biomass = ((tMass*(density*10000)))/1000000;

				// Write out
				output[0] = diameter;
				output[1] = density;
				output[2] = biomass;
				output[3] = error;
				output[4] = gsl_vector_get(predictSigma, 0); // HH
				output[5] = gsl_vector_get(predictSigma, 1); // HV
				gsl_vector_free(predictSigma);

			}
			else
			{
				std::cout << "Parameters not recognised, cannot calculate biomass.";
			}

			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
			gsl_matrix_free(covMatrixP);
			gsl_matrix_free(invCovMatrixD);
			gsl_vector_free(initialPar);
			gsl_matrix_free(newCoeffHH);
			gsl_matrix_free(newCoeffHV);
			gsl_vector_free(epsPow);
		}
		RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies::~RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies()
		{

		}

		/***********************************
		 * SINGLE SPECIES                  *
		 * May use any formula for         *
		 * fits (not limited to polynomial)*
		 ***********************************/
		RSGISEstimationAlgorithmSingleSpecies::RSGISEstimationAlgorithmSingleSpecies(int numOutputBands,
																					 gsl_vector *initialPar,
																					 RSGISEstimationOptimiser *estOptimiser,
																					 estParameters parameters,
																					 double **minMaxVals) : RSGISCalcImageValue(numOutputBands)
		{
			this->initialPar = initialPar;
			this->estOptimiser = estOptimiser;
			this->parameters = parameters;

			if (this->parameters == cDepthDensity || this->parameters == heightDensity ) // Set number of output parameters
			{
				this->numOutputPar = 2;
			}
			else if(this->parameters == dielectricDensityHeight)
			{
				this->numOutputPar = 3;
			}
			this->numOutputBands = this->numOutputPar + 2;
			this->minMaxVals = minMaxVals;
			this->useDefaultMinMax = true; // Initialise at true

			if (this->minMaxVals == NULL)
			{
				throw RSGISException("Must specify min / max values.");
			}

		}
		void RSGISEstimationAlgorithmSingleSpecies::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			species = rsgis::utils::aHarpophylla;
			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(this->numOutputPar + 1); // Output parameters + error
			inSigma0dB = gsl_vector_alloc(numBands);

			// Check for no data (image borders)
			if((bandValues[1] < -100) | (boost::math::isnan(bandValues[1])))
			{
				for(int i = 0; i < numOutputBands; i++)
				{
					output[i] = 0;
				}
			}
			else // Start Estimation
			{
				for(int i = 0; i < numBands; i++)
				{
					gsl_vector_set(inSigma0dB, i, bandValues[i]);
				}

				if(parameters == heightDensity) // Retrieve stem diameter and density
				{

					estOptimiser->minimise(inSigma0dB, initialPar, outPar);

					double height = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);



					if ((boost::math::isnan(height)) | (boost::math::isnan(density)))
					{
						try
						{
							height = gsl_vector_get(estOptimiser->getAPrioriPar(), 0);
							density = gsl_vector_get(estOptimiser->getAPrioriPar(), 1);
						}
						catch (RSGISException)
						{
							height = gsl_vector_get(initialPar, 0);
							density = gsl_vector_get(initialPar, 1);
						}

					}

                    // Set to limits of min/max values
                    if (height < this->minMaxVals[0][0]){height = this->minMaxVals[0][0];}
                    if (height > this->minMaxVals[0][1]){height = this->minMaxVals[0][1];}
                    if (density < this->minMaxVals[1][0]){density = this->minMaxVals[1][0];}
                    if (density > this->minMaxVals[1][1]){density = this->minMaxVals[1][1];}

                    // Calculate Biomass
					double heightcm = height * 100;
					double tMass = allometric.calculateTotalBiomassHeight(heightcm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					if (boost::math::isnan(height)){output[0] = 0;}
					else{output[0] = height;}
					if (boost::math::isnan(density)){output[1] = 0;}
					else{output[1] = density;}
					if (boost::math::isnan(biomass)){output[2] = 0;}
					else{output[2] = biomass;}
					if (boost::math::isnan(error)){output[3] = 9999;}
					else{output[3] = error;}
				}
				else if(parameters == cDepthDensity)  // Retrieve Canopy Depth and Stem densty
				{
					estOptimiser->minimise(inSigma0dB, initialPar, outPar);

					double height;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);

					if (error > 1) // If error is greater than 1, hasn't converged, write out a priori par (if available) or initial par.
					{
						try
						{
							cDepth = gsl_vector_get(estOptimiser->getAPrioriPar(), 0);
							density = gsl_vector_get(estOptimiser->getAPrioriPar(), 1);
						}
						catch (RSGISException)
						{
							cDepth = gsl_vector_get(initialPar, 0);
							density = gsl_vector_get(initialPar, 1);
						}

					}

                    // Set to limits of min/max values
                    if (cDepth < this->minMaxVals[0][0]){cDepth = this->minMaxVals[0][0];}
                    if (cDepth > this->minMaxVals[0][1]){cDepth = this->minMaxVals[0][1];}
                    if (density < this->minMaxVals[1][0]){density = this->minMaxVals[1][0];}
                    if (density > this->minMaxVals[1][1]){density = this->minMaxVals[1][1];}

                    // Calculate Biomass
					height = cDepth * 240; // convert to cm
					double tMass = allometric.calculateTotalBiomassHeight(height, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					if (boost::math::isnan(cDepth)){output[0] = 0;}
					else{output[0] = cDepth;}
					if (boost::math::isnan(density)){output[1] = 0;}
					else{output[1] = density;}
					if (boost::math::isnan(biomass)){output[2] = 0;}
					else{output[2] = biomass;}
					if (boost::math::isnan(error)){output[3] = 9999;}
					else{output[3] = error;}

				}
				else if(parameters == dielectricDensityHeight)
				{
					estOptimiser->minimise(inSigma0dB, initialPar, outPar);

					double height = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double dielectric = gsl_vector_get(outPar, 2);
					double error = gsl_vector_get(outPar, 3);

                    // Set to limits of min/max values
                    if (height < this->minMaxVals[0][0]){height = this->minMaxVals[0][0];}
                    if (height > this->minMaxVals[0][1]){height = this->minMaxVals[0][1];}
                    if (density < this->minMaxVals[1][0]){density = this->minMaxVals[1][0];}
                    if (density > this->minMaxVals[1][1]){density = this->minMaxVals[1][1];}
                    if (dielectric < this->minMaxVals[2][0]){dielectric = this->minMaxVals[2][0];}
                    if (dielectric > this->minMaxVals[2][1]){dielectric = this->minMaxVals[2][1];}

                    // Write out
					if (boost::math::isnan(height)){output[0] = 0;}
					else{output[0] = height;}
					if (boost::math::isnan(density)){output[1] = 0;}
					else{output[1] = density;}
					if (boost::math::isnan(dielectric)){output[2] = 0;}
					else{output[2] = dielectric;}
					if (boost::math::isnan(error)){output[4] = 9999;}
					else{output[4] = error;}
					output[3] = 9999;
				}
				else
				{
					std::cout << "Parameters not recognised, cannot calculate biomass." << std::endl;
				}

			}

			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);

		}
		RSGISEstimationAlgorithmSingleSpecies::~RSGISEstimationAlgorithmSingleSpecies()
		{

		}

		/***********************************
		 * SINGLE SPECIES Mask             *
		 * May use any formula for         *
		 * fits (not limited to polynomial)*
		 ***********************************/
		RSGISEstimationAlgorithmSingleSpeciesMask::RSGISEstimationAlgorithmSingleSpeciesMask(int numOutputBands,
																							 double nonForestThreshold,
																							 gsl_vector *initialPar,
																							 RSGISEstimationOptimiser *estOptimiser,
																							 estParameters parameters,
																							 double **minMaxVals) : RSGISCalcImageValue(numOutputBands)
		{
			this->initialPar = initialPar;
			this->nonForestThreshold = nonForestThreshold;
			this->estOptimiser = estOptimiser;
			this->parameters = parameters;
			this->numOutputBands = numOutputBands;
			this->minMaxVals = minMaxVals;
			this->useDefaultMinMax = true; // Initialise at true
			if (this->minMaxVals != NULL)
			{
				this->useDefaultMinMax = false; // If minimum and maximum values are passed in use these insead
				if (this->parameters > general) // For general parameters must specify minimum and maximum values.
				{
					throw RSGISException("Must specify min / max values for general parameters");
				}
			}
		}
		void RSGISEstimationAlgorithmSingleSpeciesMask::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			species = rsgis::utils::aHarpophylla;
			unsigned int nData = numBands - 1;
			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(3);
			inSigma0dB = gsl_vector_alloc(nData);

			// Check within mask and not border
			if((bandValues[0] > this->nonForestThreshold) && (bandValues[1] > -100) && (bandValues[1] < 0) )
			{
				for(unsigned int i = 0; i < nData; i++)
				{
					gsl_vector_set(inSigma0dB, i, bandValues[i+1]);
				}
				if(parameters == heightDensity) // Retrieve stem diameter and density
				{
					estOptimiser->minimise(inSigma0dB, initialPar, outPar);

					double height = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);

					// Set parameters to limits of equation
					if (useDefaultMinMax)
					{
						if(height < 1.5)
						{
							height = 0;
						}
						if (height > 20)
						{
							height = 20;
						}
						if(density > 2)
						{
							density = 2;
						}
						if(density < 0)
						{
							density = 0;
						}
					}
					else
					{
						// If density or height are below threashold, assume no veg.
						if(height < this->minMaxVals[0][0])
						{
							height = 0;
							density = 0;
						}
						if (height > this->minMaxVals[0][1])
						{
							height = this->minMaxVals[0][1];
						}
						if(density < this->minMaxVals[1][0])
						{
							density = 0;
							height = 0;
						}
						if(density > this->minMaxVals[1][1])
						{
							density = this->minMaxVals[0][0];
						}
					}

					if (error > 1) // If error is greater than 1, hasn't converged, write out a priori par (if available) or initial par.
					{
						try
						{
							height = gsl_vector_get(estOptimiser->getAPrioriPar(), 0);
							density = gsl_vector_get(estOptimiser->getAPrioriPar(), 1);
						}
						catch (RSGISException)
						{
							height = gsl_vector_get(initialPar, 0);
							density = gsl_vector_get(initialPar, 1);
						}

					}

					// Calculate Biomass
					double heightcm = height * 100;
					double tMass = allometric.calculateTotalBiomassHeight(heightcm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = height;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

				}
				else if(parameters == cDepthDensity)  // Retrieve Canopy Depth and Stem densty
				{
					estOptimiser->minimise(inSigma0dB, initialPar, outPar);

					double height;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);

					// Set parameters to limits of equation
					if (useDefaultMinMax)
					{
						if(cDepth > 3)
						{
							cDepth = 3;
						}
						else if(cDepth < 0.25)
						{
							cDepth = 0;
						}
						if(density > 2)
						{
							density = 2;
						}
						else if(density < 0.1)
						{
							density = 0;
						}
					}
					else
					{
						if(cDepth < this->minMaxVals[0][0])
						{
							cDepth = 0;
						}
						if (cDepth > this->minMaxVals[0][1])
						{
							cDepth = this->minMaxVals[0][1];
						}
						if(density < this->minMaxVals[1][0])
						{
							density = 0;
						}
						if(density > this->minMaxVals[1][1])
						{
							density = this->minMaxVals[0][0];
						}
					}

					if (error > 1) // If error is greater than 1, hasn't converged, write out a priori par (if available) or initial par.
					{
						try
						{
							height = gsl_vector_get(estOptimiser->getAPrioriPar(), 0);
							density = gsl_vector_get(estOptimiser->getAPrioriPar(), 1);
						}
						catch (RSGISException)
						{
							height = gsl_vector_get(initialPar, 0);
							density = gsl_vector_get(initialPar, 1);
						}

					}

					// Calculate Biomass
					height = cDepth / 0.3295;
					double heightcm = height * 100;
					double tMass = allometric.calculateTotalBiomassHeight(heightcm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = cDepth;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
				}
				else
				{
					std::cout << "Parameters not recognised, cannot calculate biomass.";
				}

			}
			else
			{
				for(int i = 0; i < numOutputBands; i++)
				{
					output[i] = 0;
				}
			}


			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
		}
		RSGISEstimationAlgorithmSingleSpeciesMask::~RSGISEstimationAlgorithmSingleSpeciesMask()
		{

		}

		/***********************************
		 * SINGLE SPECIES, PIXAP           *
		 * Per-pixel AP values used		   *
		 * May use any formula for         *
		 * fits (not limited to polynomial)*
		 ***********************************/
		RSGISEstimationAlgorithmSingleSpeciesPixAP::RSGISEstimationAlgorithmSingleSpeciesPixAP(int numOutputBands,
																					 gsl_vector *initialPar,
																					 RSGISEstimationOptimiser *estOptimiser,
																					 estParameters parameters,
																					 double **minMaxVals) : RSGISCalcImageValue(numOutputBands)
		{
			this->initialPar = initialPar;
			this->estOptimiser = estOptimiser;
			this->parameters = parameters;
			this->numOutputBands = 4;
			this->minMaxVals = minMaxVals;
			this->useDefaultMinMax = true; // Initialise at true
			this->nPar = 2; // TO DO: Add in to calculate from estParameters

			if (this->minMaxVals != NULL)
			{
				this->useDefaultMinMax = false; // If minimum and maximum values are passed in use these insead
				if (this->parameters > general) // For general parameters must specify minimum and maximum values.
				{
					throw RSGISException("Must specify min / max values for general parameters");
				}
			}

		}
		void RSGISEstimationAlgorithmSingleSpeciesPixAP::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			species = rsgis::utils::aHarpophylla;
			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(nPar + 1); // Number of parameters + error
			inSigma0dB = gsl_vector_alloc(numBands - nPar); // Number of input bands - nPar bands (a priori estimates)

			// Check for no data (image borders)
			if(bandValues[3] < -100 || boost::math::isnan(bandValues[1]))
			{
				for(int i = 0; i < numOutputBands; i++)
				{
					output[i] = 0;
				}
			}
			if (bandValues[0] == 0) // A priori estimate of 0
			{
				for(int i = 0; i < numOutputBands; i++)
				{
					output[i] = 0;
				}
			}
			else // Start Estimation
			{
				for(int i = 0; i < nPar; i++)
				{
					gsl_vector_set(initialPar, i, bandValues[i]);
				}

				for(int i = 0; i < numBands - nPar; i++)
				{
					gsl_vector_set(inSigma0dB, i, bandValues[i+nPar]);
				}
				if(parameters == heightDensity) // Retrieve stem diameter and density
				{
					estOptimiser->modifyAPriori(initialPar);
					estOptimiser->minimise(inSigma0dB, initialPar, outPar);

					double height = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);

					// Set parameters to limits of equation
					if (useDefaultMinMax)
					{
						if(height < 1.5)
						{
							height = 0;
						}
						if (height > 20)
						{
							height = 20;
						}
						if(density > 2)
						{
							density = 2;
						}
						if(density < 0)
						{
							density = 0;
						}
					}
					else
					{
						if(height < this->minMaxVals[0][0])
						{
							height = 0;
							density = 0;
						}
						if (height > this->minMaxVals[0][1])
						{
							height = this->minMaxVals[0][1];
						}
						if(density < this->minMaxVals[1][0])
						{
							height = 0;
							density = 0;
						}
						if(density > this->minMaxVals[1][1])
						{
							density = this->minMaxVals[1][1];
						}
					}

					if (error > 1) // If error is greater than 1, hasn't converged, write out a priori par (if available) or initial par.
					{
						try
						{
							height = gsl_vector_get(estOptimiser->getAPrioriPar(), 0);
							density = gsl_vector_get(estOptimiser->getAPrioriPar(), 1);
						}
						catch (RSGISException)
						{
							height = gsl_vector_get(initialPar, 0);
							density = gsl_vector_get(initialPar, 1);
						}

					}

					// Calculate Biomass
					double heightcm = height * 100;
					double tMass = allometric.calculateTotalBiomassHeight(heightcm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = height;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

					//gsl_vector_set(this->initialPar, 0, height);
					//gsl_vector_set(this->initialPar, 1, density);

				}
				else if(parameters == cDepthDensity)  // Retrieve Canopy Depth and Stem densty
				{
					estOptimiser->minimise(inSigma0dB, initialPar, outPar);

					double height;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);

					// Set parameters to limits of equation
					if (useDefaultMinMax)
					{
						if(cDepth > 3)
						{
							cDepth = 3;
						}
						else if(cDepth < 0.25)
						{
							cDepth = 0;
						}
						if(density > 2)
						{
							density = 2;
						}
						else if(density < 0.1)
						{
							density = 0;
						}
					}
					else
					{
						if(cDepth < this->minMaxVals[0][0])
						{
							cDepth = 0;
						}
						if (cDepth > this->minMaxVals[0][1])
						{
							cDepth = this->minMaxVals[0][1];
						}
						if(density < this->minMaxVals[1][0])
						{
							density = 0;
						}
						if(density > this->minMaxVals[1][1])
						{
							density = this->minMaxVals[1][1];
						}
					}

					if (error > 1) // If error is greater than 1, hasn't converged, write out a priori par (if available) or initial par.
					{
						try
						{
							cDepth = gsl_vector_get(estOptimiser->getAPrioriPar(), 0);
							density = gsl_vector_get(estOptimiser->getAPrioriPar(), 1);
						}
						catch (RSGISException)
						{
							cDepth = gsl_vector_get(initialPar, 0);
							density = gsl_vector_get(initialPar, 1);
						}

					}

					// Calculate Biomass
					height = cDepth * 240; // convert to cm
					double tMass = allometric.calculateTotalBiomassHeight(height, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = cDepth;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;

				}
				else
				{
					std::cout << "Parameters not recognised, cannot calculate biomass.";
				}

			}

			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
		}
		RSGISEstimationAlgorithmSingleSpeciesPixAP::~RSGISEstimationAlgorithmSingleSpeciesPixAP()
		{

		}

		/***********************************
		 * DUAL-POL SINGLE SPECIES Mask AP *
		 * Per-pixel AP values used		   *
		 * May use any formula for         *
		 * fits (not limited to polynomial)*
		 ***********************************/
		RSGISEstimationAlgorithmDualPolSingleSpeciesMaskPixAP::RSGISEstimationAlgorithmDualPolSingleSpeciesMaskPixAP(int numOutputBands,
																															 double nonForestThreshold,
																															 gsl_vector *initialPar,
																															 RSGISEstimationOptimiser *estOptimiser,
																															 estParameters parameters) : RSGISCalcImageValue(numOutputBands)
		{
			this->initialPar = initialPar;
			this->nonForestThreshold = nonForestThreshold;
			this->estOptimiser = estOptimiser;
			this->parameters = parameters;
			this->aPrioriPar = gsl_vector_alloc(2);
		}
		void RSGISEstimationAlgorithmDualPolSingleSpeciesMaskPixAP::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			species = rsgis::utils::aHarpophylla;
			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(3);
			inSigma0dB = gsl_vector_alloc(2);

			// Check for no data (image borders)
			if(bandValues[1] < -100)
			{
				for(int i = 0; i < numBands; i++)
				{
					output[i] = 0;
				}
			}
			else if(bandValues[1] >= 0 )
			{
				for(int i = 0; i < numBands; i++)
				{
					output[i] = 0;
				}
			}
			else // Start Estimation
			{
				for(int i = 0; i < 2; i++)
				{
					gsl_vector_set(inSigma0dB, i, bandValues[i+1]);
				}
				if(parameters == heightDensity) // Retrieve stem diameter and density
				{
					if (bandValues[0] < this->nonForestThreshold) // If FPC < non-forest mask set to zero.
					{
						output[0] = 0;
						output[1] = 0;
						output[2] = 0;
						output[3] = -999;
					}
					else
					{
						/* If optimiser user a Priori information, modify based on pixel values
						 * if not set inital par to a Priori values.
						 * Conventionally the optimiser will start at a Priori values so the two will give the same effect.
						 * However, I wanted the flexibility to start at different places to a Priori estimates
						 */
						if (estOptimiser->getOptimiserType() == conjugateGradient)
						{
							gsl_vector_set(aPrioriPar, 0, bandValues[3]);
							gsl_vector_set(aPrioriPar, 1, bandValues[4]);
							estOptimiser->modifyAPriori(aPrioriPar);
						}
						else
						{
							gsl_vector_set(this->initialPar, 0, bandValues[3]);
							gsl_vector_set(this->initialPar, 1, bandValues[4]);
						}

						estOptimiser->minimise(inSigma0dB, initialPar, outPar);

						double height = gsl_vector_get(outPar, 0);
						double density = gsl_vector_get(outPar, 1);
						double error = gsl_vector_get(outPar, 2);

						// Set parameters to limits of equation
						if(height < 0.5)
						{
							height = 0;
						}
						if(density > 2)
						{
							density = 2;
						}
						else if(density < 0.1)
						{
							density = 0;
						}
						// Calculate Biomass
						double heightcm = height * 100;
						double tMass = allometric.calculateTotalBiomassHeight(heightcm, species);
						double biomass = ((tMass*(density*10000)))/1000000;

						// Write out
						output[0] = height;
						output[1] = density;
						output[2] = biomass;
						output[3] = error;

						//gsl_vector_set(this->initialPar, 0, height);
						//gsl_vector_set(this->initialPar, 1, density);

					}
				}
				else if(parameters == cDepthDensity)  // Retrieve Canopy Depth and Stem densty
				{

					/* If optimiser user a Priori information, modify based on pixel values
					 * if not set inital par to a Priori values.
					 * Conventionally the optimiser will start at a Priori values so the two will give the same effect.
					 * However, I wanted the flexibility to start at different places to a Priori estimates
					 */
					if (estOptimiser->getOptimiserType() == conjugateGradient)
					{
						gsl_vector_set(aPrioriPar, 0, bandValues[3]);
						gsl_vector_set(aPrioriPar, 1, bandValues[4]);
						estOptimiser->modifyAPriori(aPrioriPar);
					}
					else
					{
						gsl_vector_set(this->initialPar, 0, bandValues[3]);
						gsl_vector_set(this->initialPar, 1, bandValues[4]);
					}

					estOptimiser->minimise(inSigma0dB, initialPar, outPar);

					double height;
					double cDepth = gsl_vector_get(outPar, 0);
					double density = gsl_vector_get(outPar, 1);
					double error = gsl_vector_get(outPar, 2);
					// Set parameters to limits of equation
					if(cDepth > 3)
					{
						cDepth = 3;
					}
					else if(cDepth < 0.25)
					{
						cDepth = 0;
					}
					if(density > 2)
					{
						density = 2;
					}
					else if(density < 0.1)
					{
						density = 0;
					}
					// Calculate Biomass
					height = cDepth / 0.3295;
					double heightcm = height * 100;
					double tMass = allometric.calculateTotalBiomassHeight(heightcm, species);
					double biomass = ((tMass*(density*10000)))/1000000;

					// Write out
					output[0] = height;
					output[1] = density;
					output[2] = biomass;
					output[3] = error;
				}
				else
				{
					std::cout << "Parameters not recognised, cannot calculate biomass.";
				}

			}

			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
		}
		RSGISEstimationAlgorithmDualPolSingleSpeciesMaskPixAP::~RSGISEstimationAlgorithmDualPolSingleSpeciesMaskPixAP()
		{
			gsl_vector_free(this->aPrioriPar);
		}

		/*********************************************
		 * DUAL-POL MULTI SPECIES Classification     *
		 * May use any formula for                   *
		 * fits (not limited to polynomial)          *
		 * Takes classifiction image to determine    *
		 * initial estimates					     *
		 *********************************************/
		RSGISEstimationAlgorithmDualPolMultiSpeciesClassification::RSGISEstimationAlgorithmDualPolMultiSpeciesClassification(int numOutputBands,
																															 std::vector <gsl_vector*> *initialPar,
																															 std::vector <RSGISEstimationOptimiser*> *estOptimiser,
																															 estParameters parameters,
																															 std::vector<rsgis::utils::treeSpecies> *species
																															  ) : RSGISCalcImageValue(numOutputBands)
		{
			this->initialPar = initialPar;
			this->parameters = parameters;
			this->species = species;
			this->estOptimiser = estOptimiser;

			if(initialPar->size() != estOptimiser->size())
			{
				throw RSGISException("Number of optimisers is not equal to number of parameters");
			}
			if(initialPar->size() != species->size())
			{
				throw RSGISException("Number of species is not equal to number of parameters");
			}
		}
		void RSGISEstimationAlgorithmDualPolMultiSpeciesClassification::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{

			/**
			 Error Codes:
			   0 - out
			   -999 - Class 0 (mask)
			   -998 - No parameters provided for class
			   -997 - One or more values outside limits
			 */

			rsgis::math::RSGISVectors vectorUtils;
			rsgis::utils::RSGISAllometricEquations allometric = rsgis::utils::RSGISAllometricEquations();

			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(3);
			inSigma0dB = gsl_vector_alloc(2);

			// Check for no data (image borders)
			if(bandValues[1] < -100)
			{
				for(int i = 0; i < numBands; i++)
				{
					output[i] = 0;
				}
			}
			else if(bandValues[1] >= 0 )
			{
				for(int i = 0; i < numBands; i++)
				{
					output[i] = 0;
				}
			}
			else // Start Estimation
			{
				for(int i = 0; i < 2; i++)
				{
					gsl_vector_set(inSigma0dB, i, bandValues[i+1]);
				}
				if(parameters == heightDensity) // Retrieve stem diameter and density
				{
					if (bandValues[0] == 0) // No classification
					{
						output[0] = 0;
						output[1] = 0;
						output[2] = 0;
						output[3] = -999;
					}
					else if (bandValues[0] > initialPar->size()) // No parameters provided for class
					{
						output[0] = 0;
						output[1] = 0;
						output[2] = 0;
						output[3] = -998;
					}
					else
					{
						unsigned int classification = bandValues[0] - 1;

						estOptimiser->at(classification)->minimise(inSigma0dB, initialPar->at(classification), outPar);

						double height = gsl_vector_get(outPar, 0);
						double density = gsl_vector_get(outPar, 1);
						double error = gsl_vector_get(outPar, 2);

						// Set parameters to limits of equation
						if(height < 0)
						{
							height = 0;
							error = -997;
						}
						if (height > 20)
						{
							height = 20;
							error = -997;
						}
						if(density > 3)
						{
							density = 3;
							error = -997;
						}
						else if(density < 0)
						{
							density = 0;
							error = -997;
						}
						// Calculate Biomass
						double heightcm = height * 100;
						double tMass = allometric.calculateTotalBiomassHeight(heightcm, species->at(classification));
						double biomass = ((tMass*(density*10000)))/1000000;

						// Write out
						output[0] = height;
						output[1] = density;
						output[2] = biomass;
						output[3] = error;

					}
				}
				else
				{
					std::cout << "Parameters not recognised, cannot calculate biomass.";
				}
			}


			gsl_vector_free(inSigma0dB);
			gsl_vector_free(outPar);
		}
		RSGISEstimationAlgorithmDualPolMultiSpeciesClassification::~RSGISEstimationAlgorithmDualPolMultiSpeciesClassification()
		{

		}

		/********************************************************************
		 * GENERATE SIMULATED DATA - 2VAR2DATA                              *
		 * Generates simulated data suitible for testing algorithm with     *
		 ********************************************************************/

		RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data::RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data(int numOutputBands, rsgis::math::RSGISMathTwoVariableFunction *hhFunction, rsgis::math::RSGISMathTwoVariableFunction *hvFunction) : RSGISCalcImageValue(numOutputBands)
		{
			this->hhFunction = hhFunction;
			this->hvFunction = hvFunction;
		}
		void RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
		{
			if (bandValues[0] > 0)
			{
				output[0] = hhFunction->calcFunction(bandValues[0], bandValues[1]);
				output[1] = hvFunction->calcFunction(bandValues[0], bandValues[1]);
			}
			else
			{
				output[0] = 0;
				output[1] = 0;
			}

		}
		RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data::~RSGISEstimationAlgorithmGenerateSimulatedData2Var2Data()
		{

		}

	}}
