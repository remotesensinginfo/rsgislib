/*
 *  RSGISEstimationAlgorithmSARFPC.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 16/09/2009.
 *  Copyright 2009 Aberystwyth University. All rights reserved.
 *
 */

#include "RSGISEstimationAlgorithmSARFPC.h"

namespace rsgis {namespace radar
	{
		RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies::RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies(int numOutputBands, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_vector *coeffFPCHH, gsl_vector *coeffFPCHV, gsl_vector *coeffFPCAttenuationH, gsl_vector *coeffFPCAttenuationV, estParameters parameters, treeSpecies species, int ittmax) : RSGISCalcImageValue(numOutputBands)
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
		}
		void RSGISEstimationAlgorithmDualPolFPCMoistureSingleSpecies::calcImageValue(float *bandValues, int numBands, float *output) 
		{
			RSGISConjugateGradient conjGrad;
			RSGISVectors vectorUtils;
			RSGISAllometricEquations allometric = RSGISAllometricEquations();
			
			species = aHarpophylla;
			gsl_vector *inSigma0dB;
			gsl_vector *outPar;
			outPar = gsl_vector_alloc(numOutBands);
			inSigma0dB = gsl_vector_alloc(numBands);
			
			/********************************************************
			 * CALCULATE CANOPY SCATTERING AND ATTENUATION FROM FPC *
			 ********************************************************/
			
			double fpc = bandValues[0];
			gsl_vector *fpcPow;
			fpcPow = gsl_vector_alloc(fpcOrder);
			double coeff = 0;
			double coeffXPow = 0;
			
			for(int z = 0; z < fpcOrder; z++)
			{
				gsl_vector_set(fpcPow, z, pow(fpc, z));
			}
			
			// Calculate HH branch scattering from FPC data (dB)
			double sigmaHHBranch = 0; 
			for(int j = 0; j < fpcOrder; j++)
			{
				coeff = gsl_vector_get(coeffFPCHH, j); // a_n
				coeffXPow = coeff * gsl_vector_get(fpcPow, j); // a_n * x^n				
				sigmaHHBranch = sigmaHHBranch + coeffXPow;
			}
			
			// Calculate HV branch scattering from FPC data (dB)
			double sigmaHVBranch = 0;
			for(int j = 0; j < fpcOrder; j++)
			{
				coeff = gsl_vector_get(coeffFPCHV, j); // a_n
				coeffXPow = coeff * gsl_vector_get(fpcPow, j); // a_n * x^n				
				sigmaHVBranch = sigmaHVBranch + coeffXPow;
			}
			
			// Calculate H attenuation from FPC data (dB / m)
			double attenuationH = 0; 
			for(int j = 0; j < fpcOrder; j++)
			{
				coeff = gsl_vector_get(coeffFPCAttenuationH, j); // a_n
				coeffXPow = coeff * gsl_vector_get(fpcPow, j); // a_n * x^n				
				attenuationH = attenuationH + coeffXPow;
			}
			
			// Calculate V attenuation from FPC data (dB / m)
			double attenuationV = 0; 
			for(int j = 0; j < fpcOrder; j++)
			{
				coeff = gsl_vector_get(coeffFPCAttenuationV, j); // a_n
				coeffXPow = coeff * gsl_vector_get(fpcPow, j); // a_n * x^n				
				attenuationV = attenuationV + coeffXPow;
			}

			double sigmaHHTotal = bandValues[1];
			double sigmaHVTotal = bandValues[2];
			
			double sigmaHHTrunkGround = 10 * log10((pow(10,sigmaHHTotal/10) - pow(10,sigmaHHBranch/10)) / ( pow(10,attenuationH/10) + pow(10,attenuationH/10) )); // Calculate Trunk Ground Scattering (HH)
			double sigmaHVTrunkGround = 10 * log10((pow(10,sigmaHVTotal/10) - pow(10,sigmaHVBranch/10)) / ( pow(10,attenuationH/10) + pow(10,attenuationV/10) )); // Calculate Trunk Ground Scattering (HV)
			
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
				double soilMoisture = bandValues[3] / 100; 
				
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
			
			gsl_matrix *newCoeffHH;
			gsl_matrix *newCoeffHV;
			gsl_vector *epsPow;
			
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
				cout << "Parameters not recognised, cannot calculate biomass.";
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
		
	}}
