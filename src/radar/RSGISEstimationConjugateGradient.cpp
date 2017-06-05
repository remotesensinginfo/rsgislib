/*
 *  RSGISEstimationConjugateGradient.cpp
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

#include "RSGISEstimationConjugateGradient.h"

namespace rsgis {namespace radar
	{
		RSGISEstimationConjugateGradient::RSGISEstimationConjugateGradient()
		{

		}
		void RSGISEstimationConjugateGradient::estimateTwoDimensionalPolyTwoChannel(gsl_vector *inSigma0dB, gsl_matrix *coeffHH, gsl_matrix *coeffVV, gsl_vector *initialPar, gsl_vector *outParError, gsl_vector *predicted, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD, int ittmax)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISVectors vectorUtils;

			gsl_vector *estimatedPar; // Vector to hold the estimated parameters
			gsl_matrix *invCovMatrixP; // Inverse covarence matrix for prior estimates
			gsl_matrix *frechet; // Vector to hold the Frechet derivative operator
			gsl_matrix *frechetT; // Vector to hold the transpose of the Frechet derivative operator
			gsl_vector *dPredictMeas; // Vector to hold predicted - measured data
			gsl_vector *gamma; // Vector to hold gamma values
			gsl_vector *xPowers, *yPowers; // Vectors to hold powers of x, y and z
			gsl_vector *dxPowers, *dyPowers; // Vectors to hold differential powers of x, y and z
			gsl_vector *aux1, *aux2, *aux3; // Tempory vectors
			double alpha = 0;

			int nData = 2; // Data channels
			int nPar = 2; // Parameters to be retrieved
			int order = coeffHH->size1; // Get polynomial order
			if(coeffHH -> size2 != order)
			{
				throw RSGISException("Different order polynomials for x and y terms are not supported!");
			}

			// Allocate vectors and matrices
			estimatedPar = gsl_vector_alloc(nPar);
			frechet = gsl_matrix_alloc(nPar,nData);
			frechetT = gsl_matrix_alloc(nData,nPar);
			dPredictMeas = gsl_vector_alloc(nData);
			invCovMatrixP = gsl_matrix_alloc(nPar,nPar);
			gamma = gsl_vector_alloc(nPar);
			aux1 = gsl_vector_alloc(nData);
			aux2 = gsl_vector_alloc(nPar);
			aux3 = gsl_vector_alloc(nData);
			xPowers = gsl_vector_alloc(order);
			yPowers = gsl_vector_alloc(order);
			dxPowers = gsl_vector_alloc(order);
			dyPowers = gsl_vector_alloc(order);

			double height = 0.0;
			double density = 0.0;
			double predictSigma0 = 0.0, predictSigma0F = 0.0, predictSigma0F2 = 0.0;
			double xPow = 0.0, dxPow = 0.0, aCoeff = 0.0, aCoeffF = 0.0;
			double yPow = 0.0, dyPow = 0.0, bcoeff = 0.0, bcoeffYPow = 0.0, bcoeffYPowF = 0.0;
			double acoeffXPow = 0.0, acoeffXPowF = 0.0, acoeffXPowF2 = 0.0;
			double predictSq = 0.0, error = 0.0, predictMeasElement = 0.0;
			double gammaElement = 0.0, currPriorDiff = 0.0, predictParElement = 0.0;
			double s1 = 0.0, s2 = 0.0, s1DivS2 = 0.0;

			// Polulate vector holding a priori  estimates
			double initialHeight = gsl_vector_get(initialPar, 0);
			double initialDensity = gsl_vector_get(initialPar, 1);

			// Set canopy depth and stem density to initial estimates.
			gsl_vector_set(estimatedPar, 0, initialHeight);
			gsl_vector_set(estimatedPar, 1, initialDensity);

			// Set up inverse covarience Matrices
			matrixUtils.inv2x2GSLMatrix(covMatrixP, invCovMatrixP);

			// Calculate Sigma Squared (used to calculate error)
			double sigmaSq = 0.0;

			for(int s = 0; s < nData; s++)
			{
				sigmaSq = sigmaSq + pow(gsl_vector_get(inSigma0dB, s),2);
			}

			// START ITTERATING
			for(int itt = 0; itt < ittmax;itt++)
			{
				// Zero matrices
				gsl_vector_set_zero(aux1);
				gsl_vector_set_zero(aux2);
				gsl_vector_set_zero(aux3);
				gsl_vector_set_zero(predicted);
				gsl_matrix_set_zero(frechet);
				gsl_matrix_set_zero(frechetT);
				gsl_vector_set_zero(gamma);
				gsl_vector_set_zero(xPowers);
				gsl_vector_set_zero(yPowers);
				gsl_vector_set_zero(dxPowers);
				gsl_vector_set_zero(dyPowers);

				// Set up equations and Frechet derivative operator
				height = gsl_vector_get(estimatedPar, 0);
				density = gsl_vector_get(estimatedPar, 1);

				// Set up vectors to hold powers
				for(int i = 0; i < order ; i++)
				{
					xPow = pow(height, i);
					dxPow = i * pow(height, i - 1);
					gsl_vector_set(xPowers, i, xPow);
					gsl_vector_set(dxPowers, i, dxPow);
					yPow = pow(density, i);
					dyPow = i * pow(density, i - 1);
					gsl_vector_set(yPowers, i, yPow);
					gsl_vector_set(dyPowers, i, dyPow);
				}

				////////
				// HH //
				////////

				predictSigma0 = 0.0;
				predictSigma0F = 0.0;
				predictSigma0F2 = 0.0;

				for(int x = 0; x < order; x ++)
				{
					xPow = gsl_vector_get(xPowers, x); // height^n;
					dxPow = gsl_vector_get(dxPowers, x); // height^(n-1);

					aCoeff = 0.0;
					aCoeffF = 0.0;

					for(int y = 0; y < order ; y++) // Calculate a_n(density)
					{
						yPow = gsl_vector_get(yPowers, y); // density^n;
						dyPow = gsl_vector_get(dyPowers, y); // density^(n-1)
						bcoeff = gsl_matrix_get(coeffHH, x, y); // b_n
						bcoeffYPow = bcoeff * yPow; // b_n * y^n
						bcoeffYPowF = bcoeff * dyPow; // n * b_n * y^(n-1)
						aCoeff = aCoeff + bcoeffYPow;
						aCoeffF = aCoeffF + bcoeffYPowF;
					}

					acoeffXPow = xPow * aCoeff;
					acoeffXPowF = dxPow * aCoeff; // d(f_nl)/ dx (partial derivative with respect to height)
					acoeffXPowF2 = xPow * aCoeffF;  // d(f_nl)/ dy (partial derivative with respect to density)

					predictSigma0 = predictSigma0 + acoeffXPow;
					predictSigma0F = predictSigma0F + acoeffXPowF;
					predictSigma0F2 = predictSigma0F2 + acoeffXPowF2;
				}

				// Save values to matrixes.
				gsl_vector_set(predicted, 0, predictSigma0);
				gsl_matrix_set(frechet, 0, 0, predictSigma0F);
				gsl_matrix_set(frechet, 1, 0, predictSigma0F2);

				////////
				// VV //
				////////

				predictSigma0 = 0.0;
				predictSigma0F = 0.0;
				predictSigma0F2 = 0.0;

				for(int x = 0; x < order; x ++)
				{
					xPow = gsl_vector_get(xPowers, x); // height^n;
					dxPow = gsl_vector_get(dxPowers, x); // height^(n-1);

					aCoeff = 0.0;
					aCoeffF = 0.0;

					for(int y = 0; y < order ; y++) // Calculate a_n(density)
					{
						yPow = gsl_vector_get(yPowers, y); // density^n;
						dyPow = gsl_vector_get(dyPowers, y); // density^(n-1)
						bcoeff = gsl_matrix_get(coeffVV, x, y); // b_n
						bcoeffYPow = bcoeff * yPow; // b_n * y^n
						bcoeffYPowF = bcoeff * dyPow; // n * b_n * y^(n-1)
						aCoeff = aCoeff + bcoeffYPow;
						aCoeffF = aCoeffF + bcoeffYPowF;
					}

					acoeffXPow = xPow * aCoeff;
					acoeffXPowF = dxPow * aCoeff; // d(f_nl)/ dx (partial derivative with respect to height)
					acoeffXPowF2 = xPow * aCoeffF;  // d(f_nl)/ dy (partial derivative with respect to density)

					predictSigma0 = predictSigma0 + acoeffXPow;
					predictSigma0F = predictSigma0F + acoeffXPowF;
					predictSigma0F2 = predictSigma0F2 + acoeffXPowF2;
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 1, predictSigma0);
				gsl_matrix_set(frechet, 0, 1, predictSigma0F);
				gsl_matrix_set(frechet, 1, 1, predictSigma0F2);

				// CALCULATE ERROR
				predictSq = 0.0;
				for(int i = 0; i < nData; i++)
				{
					predictMeasElement = gsl_vector_get(predicted, i) - gsl_vector_get(inSigma0dB, i); // Predicted - Measured data
					gsl_vector_set(dPredictMeas, i, predictMeasElement);
					predictSq = predictSq + pow(predictMeasElement,2); // Predicted data squared.
				}
				error = 0.0;
				error = sqrt(predictSq / sigmaSq); // Square Root [Sum(Predicted Squared) / Sum(Measured Squared)]

				if(error < 0.00001)
				{
					break;
				}

				// FIND GRADIENT DIRECTION
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, dPredictMeas, aux1); // aux1 = invCovMatrixD . dPredictMeas
				matrixUtils.productMatrixVectorGSL(frechet, aux1, aux2); // aux2 = Frechet . aux1
                matrixUtils.productMatrixVectorGSL(covMatrixP, aux2, gamma); // gamma = covMatrixP . aux2

				for(int i = 0; i < nPar; i++)
				{
					gammaElement = gsl_vector_get(gamma, i);
					currPriorDiff = gsl_vector_get(estimatedPar, i) - gsl_vector_get(initialPar, i); // X_i - X_ap
					gsl_vector_set(gamma, i, (gammaElement + currPriorDiff));
				}

				// FIND STEP LENGTH
				matrixUtils.transposeGSL(frechet, frechetT); // Transpose of Frechet derivative operator
				matrixUtils.productMatrixVectorGSL(frechetT, gamma, aux1); // aux1 = frechetT . gamma
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, aux1, aux3); // aux3 = invCovMatrixD . aux1
				s1 = vectorUtils.dotProductVectorVectorGSL(aux1, aux3); // s1 = aux1 . aux3
				matrixUtils.productMatrixVectorGSL(invCovMatrixP, gamma, aux2); // aux2 = invCovMatrixP . gamma
				s2 = vectorUtils.dotProductVectorVectorGSL(gamma, aux2); // s2 = gamma . aux2
				s1DivS2 = s1 / s2;
				alpha = 1 / (1 + s1DivS2); // 1 / (1 + (s1/s2))

				// UPDATE VALUES
				for(int i = 0; i < nPar; i++)
				{
					predictParElement = gsl_vector_get(estimatedPar, i);
					gammaElement = gsl_vector_get(gamma, i);
					predictParElement = predictParElement - (alpha * gammaElement);
					gsl_vector_set(estimatedPar, i, predictParElement);
					gsl_vector_set(outParError, i, predictParElement);
				}
				gsl_vector_set(outParError, nPar, error);
			}

			// FREE MATRICES
			gsl_vector_free(estimatedPar);
			gsl_matrix_free(frechet);
			gsl_matrix_free(frechetT);
			gsl_vector_free(dPredictMeas);
			gsl_matrix_free(invCovMatrixP);
			gsl_vector_free(gamma);
			gsl_vector_free(aux1);
			gsl_vector_free(aux2);
			gsl_vector_free(aux3);
			gsl_vector_free(xPowers);
			gsl_vector_free(yPowers);
			gsl_vector_free(dxPowers);
			gsl_vector_free(dyPowers);
		}
		void RSGISEstimationConjugateGradient::estimateTwoDimensionalPolyThreeChannel(gsl_vector *inSigma0dB, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV, gsl_vector *initialPar, gsl_vector *outParError, gsl_vector *predicted, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD, int ittmax)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISVectors vectorUtils;

			gsl_vector *estimatedPar; // Vector to hold the estimated parameters
			gsl_matrix *invCovMatrixP; // Inverse covarence matrix for prior estimates
			gsl_matrix *frechet; // Vector to hold the Frechet derivative operator
			gsl_matrix *frechetT; // Vector to hold the transpose of the Frechet derivative operator
			gsl_vector *dPredictMeas; // Vector to hold predicted - measured data
			gsl_vector *gamma; // Vector to hold gamma values
			gsl_vector *xPowers, *yPowers; // Vectors to hold powers of x, y and z
			gsl_vector *dxPowers, *dyPowers; // Vectors to hold differential powers of x, y and z
			gsl_vector *aux1, *aux2, *aux3; // Tempory vectors
			double alpha = 0;

			int nData = 3; // Data channels
			int nPar = 2; // Parameters to be retrieved
			int order = coeffHH->size1; // Get polynomial order
			if(coeffHH -> size2 != order)
			{
				throw RSGISException("Different order polynomials for x and y terms are not supported!");
			}

			// Allocate vectors and matrices
			estimatedPar = gsl_vector_alloc(nPar);
			frechet = gsl_matrix_alloc(nPar,nData);
			frechetT = gsl_matrix_alloc(nData,nPar);
			dPredictMeas = gsl_vector_alloc(nData);
			invCovMatrixP = gsl_matrix_alloc(nPar,nPar);
			gamma = gsl_vector_alloc(nPar);
			aux1 = gsl_vector_alloc(nData);
			aux2 = gsl_vector_alloc(nPar);
			aux3 = gsl_vector_alloc(nData);
			xPowers = gsl_vector_alloc(order);
			yPowers = gsl_vector_alloc(order);
			dxPowers = gsl_vector_alloc(order);
			dyPowers = gsl_vector_alloc(order);

			double height = 0.0;
			double density = 0.0;
			double predictSigma0 = 0.0, predictSigma0F = 0.0, predictSigma0F2 = 0.0;
			double xPow = 0.0, dxPow = 0.0, aCoeff = 0.0, aCoeffF = 0.0;
			double yPow = 0.0, dyPow = 0.0, bcoeff = 0.0, bcoeffYPow = 0.0, bcoeffYPowF = 0.0;
			double acoeffXPow = 0.0, acoeffXPowF = 0.0, acoeffXPowF2 = 0.0;
			double predictSq = 0.0, error = 0.0, predictMeasElement = 0.0;
			double gammaElement = 0.0, currPriorDiff = 0.0, predictParElement = 0.0;
			double s1 = 0.0, s2 = 0.0, s1DivS2 = 0.0;

			// Polulate vector holding a priori  estimates
			double initialHeight = gsl_vector_get(initialPar, 0);
			double initialDensity = gsl_vector_get(initialPar, 1);

			// Set canopy depth and stem density to initial estimates.
			gsl_vector_set(estimatedPar, 0, initialHeight);
			gsl_vector_set(estimatedPar, 1, initialDensity);

			// Set up inverse covarience Matrices
			matrixUtils.inv2x2GSLMatrix(covMatrixP, invCovMatrixP);

			// Calculate Sigma Squared (used to calculate error)
			double sigmaSq = 0.0;

			for(int s = 0; s < nData; s++)
			{
				sigmaSq = sigmaSq + pow(gsl_vector_get(inSigma0dB, s),2);
			}

			// START ITERATING
			for(int itt = 0; itt < ittmax;itt++)
			{
				// Set up equations and Frechet derivative operator
				height = gsl_vector_get(estimatedPar, 0);
				density = gsl_vector_get(estimatedPar, 1);

				// Set up vectors to hold powers
				for(int i = 0; i < order ; i++)
				{
					xPow = pow(height, i);
					dxPow = i * pow(height, i - 1);
					gsl_vector_set(xPowers, i, xPow);
					gsl_vector_set(dxPowers, i, dxPow);
					yPow = pow(density, i);
					dyPow = i * pow(density, i - 1);
					gsl_vector_set(yPowers, i, yPow);
					gsl_vector_set(dyPowers, i, dyPow);
				}

				////////
				// HH //
				////////

				predictSigma0 = 0.0;
				predictSigma0F = 0.0;
				predictSigma0F2 = 0.0;

				for(int x = 0; x < order; x ++)
				{
					xPow = gsl_vector_get(xPowers, x); // height^n;
					dxPow = gsl_vector_get(dxPowers, x); // height^(n-1);

					aCoeff = 0.0;
					aCoeffF = 0.0;

					for(int y = 0; y < order ; y++) // Calculate a_n(density)
					{
						yPow = gsl_vector_get(yPowers, y); // density^n;
						dyPow = gsl_vector_get(dyPowers, y); // density^(n-1)
						bcoeff = gsl_matrix_get(coeffHH, x, y); // b_n
						bcoeffYPow = bcoeff * yPow; // b_n * y^n
						bcoeffYPowF = y * bcoeff * dyPow; // n * b_n * y^(n-1)
						aCoeff = aCoeff + bcoeffYPow;
						aCoeffF = aCoeffF + bcoeffYPowF;
					}

					acoeffXPow = xPow * aCoeff;
					acoeffXPowF = x * dxPow * aCoeff; // d(f_nl)/ dx (partial derivative with respect to height)
					acoeffXPowF2 = xPow * aCoeffF;  // d(f_nl)/ dy (partial derivative with respect to density)

					predictSigma0 = predictSigma0 + acoeffXPow;
					predictSigma0F = predictSigma0F + acoeffXPowF;
					predictSigma0F2 = predictSigma0F2 + acoeffXPowF2;
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 0, predictSigma0);
				gsl_matrix_set(frechet, 0, 0, predictSigma0F);
				gsl_matrix_set(frechet, 1, 0, predictSigma0F2);

				////////
				// HV //
				////////

				predictSigma0 = 0.0;
				predictSigma0F = 0.0;
				predictSigma0F2 = 0.0;

				for(int x = 0; x < order; x ++)
				{
					xPow = gsl_vector_get(xPowers, x); // height^n;
					dxPow = gsl_vector_get(dxPowers, x); // height^(n-1);

					aCoeff = 0.0;
					aCoeffF = 0.0;

					for(int y = 0; y < order ; y++) // Calculate a_n(density)
					{
						yPow = gsl_vector_get(yPowers, y); // density^n;
						dyPow = gsl_vector_get(dyPowers, y); // density^(n-1)
						bcoeff = gsl_matrix_get(coeffHV, x, y); // b_n
						bcoeffYPow = bcoeff * yPow; // b_n * y^n
						bcoeffYPowF = y * bcoeff * dyPow; // n * b_n * y^(n-1)
						aCoeff = aCoeff + bcoeffYPow;
						aCoeffF = aCoeffF + bcoeffYPowF;
					}

					acoeffXPow = xPow * aCoeff;
					acoeffXPowF = x * dxPow * aCoeff; // d(f_nl)/ dx (partial derivative with respect to height)
					acoeffXPowF2 = xPow * aCoeffF;  // d(f_nl)/ dy (partial derivative with respect to density)

					predictSigma0 = predictSigma0 + acoeffXPow;
					predictSigma0F = predictSigma0F + acoeffXPowF;
					predictSigma0F2 = predictSigma0F2 + acoeffXPowF2;
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 1, predictSigma0);
				gsl_matrix_set(frechet, 0, 1, predictSigma0F);
				gsl_matrix_set(frechet, 1, 1, predictSigma0F2);

				////////
				// VV //
				////////

				predictSigma0 = 0.0;
				predictSigma0F = 0.0;
				predictSigma0F2 = 0.0;

				for(int x = 0; x < order; x ++)
				{
					xPow = gsl_vector_get(xPowers, x); // height^n;
					dxPow = gsl_vector_get(dxPowers, x); // height^(n-1);

					aCoeff = 0.0;
					aCoeffF = 0.0;

					for(int y = 0; y < order ; y++) // Calculate a_n(density)
					{
						yPow = gsl_vector_get(yPowers, y); // density^n;
						dyPow = gsl_vector_get(dyPowers, y); // density^(n-1)
						bcoeff = gsl_matrix_get(coeffVV, x, y); // b_n
						bcoeffYPow = bcoeff * yPow; // b_n * y^n
						bcoeffYPowF = y * bcoeff * dyPow; // n * b_n * y^(n-1)
						aCoeff = aCoeff + bcoeffYPow;
						aCoeffF = aCoeffF + bcoeffYPowF;
					}

					acoeffXPow = xPow * aCoeff;
					acoeffXPowF = x * dxPow * aCoeff; // d(f_nl)/ dx (partial derivative with respect to height)
					acoeffXPowF2 = xPow * aCoeffF;  // d(f_nl)/ dy (partial derivative with respect to density)

					predictSigma0 = predictSigma0 + acoeffXPow;
					predictSigma0F = predictSigma0F + acoeffXPowF;
					predictSigma0F2 = predictSigma0F2 + acoeffXPowF2;
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 2, predictSigma0);
				gsl_matrix_set(frechet, 0, 2, predictSigma0F);
				gsl_matrix_set(frechet, 1, 2, predictSigma0F2);

				// CALCULATE ERROR
				predictSq = 0.0;
				for(int i = 0; i < nData; i++)
				{
					predictMeasElement = gsl_vector_get(predicted, i) - gsl_vector_get(inSigma0dB, i); // Predicted - Measured data
					gsl_vector_set(dPredictMeas, i, predictMeasElement);
					predictSq = predictSq + pow(predictMeasElement,2); // Predicted data squared.
				}
				error = 0.0;
				error = sqrt(predictSq / sigmaSq); // Square Root [Sum(Predicted Squared) / Sum(Measured Squared)]

				if(error < 0.00001)
				{
					break;
				}

				// FIND GRADIENT DIRECTION
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, dPredictMeas, aux1); // aux1 = invCovMatrixD . dPredictMeas
				matrixUtils.productMatrixVectorGSL(frechet, aux1, aux2); // aux2 = Frechet . aux1
				matrixUtils.productMatrixVectorGSL(covMatrixP, aux2, gamma); // gamma = covMatrixP . aux2

				for(int i = 0; i < nPar; i++)
				{
					gammaElement = gsl_vector_get(gamma, i);
					currPriorDiff = gsl_vector_get(estimatedPar, i) - gsl_vector_get(initialPar, i); // X_i - X_ap
					gsl_vector_set(gamma, i, (gammaElement + currPriorDiff));
				}

				// FIND STEP LENGTH

				matrixUtils.transposeNonSquareGSL(frechet, frechetT); // Transpose of Frechet derivative operator
				matrixUtils.productMatrixVectorGSL(frechetT, gamma, aux1); // aux1 = frechetT . gamma
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, aux1, aux3); // aux3 = invCovMatrixD . aux1
				s1 = vectorUtils.dotProductVectorVectorGSL(aux1, aux3); // s1 = aux1 . aux2
				matrixUtils.productMatrixVectorGSL(invCovMatrixP, gamma, aux2); // aux2 = invCovMatrixP . gamma
				s2 = vectorUtils.dotProductVectorVectorGSL(gamma, aux2); // s2 = gamma . aux2
				s1DivS2 = s1 / s2;
				alpha = 1 / (1 + s1DivS2); // 1 / (1 + (s1/s2))

				// UPDATE VALUES
				for(int i = 0; i < nPar; i++)
				{
					predictParElement = gsl_vector_get(estimatedPar, i);
					gammaElement = gsl_vector_get(gamma, i);
					predictParElement = predictParElement - (alpha * gammaElement);
					gsl_vector_set(estimatedPar, i, predictParElement);
					gsl_vector_set(outParError, i, predictParElement);
				}

				gsl_vector_set(outParError, nPar, error);
			}

            // FREE MATRICES
			gsl_vector_free(estimatedPar);
			gsl_matrix_free(frechet);
			gsl_matrix_free(frechetT);
			gsl_vector_free(dPredictMeas);
			gsl_matrix_free(invCovMatrixP);
			gsl_vector_free(gamma);
			gsl_vector_free(aux1);
			gsl_vector_free(aux2);
			gsl_vector_free(aux3);
			gsl_vector_free(xPowers);
			gsl_vector_free(yPowers);
			gsl_vector_free(dxPowers);
			gsl_vector_free(dyPowers);
		}
		void RSGISEstimationConjugateGradient::estimateThreeDimensionalPolyThreeChannel(gsl_vector *inSigma0dB, gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV, gsl_vector *initialPar, gsl_vector *outParError, gsl_vector *predicted, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD, int ittmax)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISVectors vectorUtils;

			gsl_vector *estimatedPar; // Vector to hold the estimated parameters
			gsl_matrix *invCovMatrixP; // Inverse covarence matrix for prior estimates
			gsl_matrix *frechet; // Vector to hold the Frechet derivative operator
			gsl_matrix *frechetT; // Vector to hold the transpose of the Frechet derivative operator
			gsl_vector *dPredictMeas; // Vector to hold predicted - measured data
			gsl_vector *gamma; // Vector to hold gamma values
			gsl_vector *xPowers, *yPowers, *zPowers; // Vectors to hold powers of x, y and z
			gsl_vector *dxPowers, *dyPowers, *dzPowers; // Vectors to hold differential powers of x, y and z
			gsl_vector *aux1, *aux2, *aux3; // Tempory vectors
			double alpha = 0;

			int nData = 3; // Data channels
			int nPar = 3; // Parameters to be retrieved
			int order = coeffHH->size2; // Get polynomial order
			if(coeffHH -> size2 != order)
			{
				throw RSGISException("Different order polynomials for x and y terms are not supported!");
			}

			// Allocate vectors and matrices
			estimatedPar = gsl_vector_alloc(nPar);
			frechet = gsl_matrix_alloc(nPar,nData);
			frechetT = gsl_matrix_alloc(nData,nPar);
			dPredictMeas = gsl_vector_alloc(nData);
			invCovMatrixP = gsl_matrix_alloc(nPar,nPar);
			gamma = gsl_vector_alloc(nPar);
			xPowers = gsl_vector_alloc(order);
			yPowers = gsl_vector_alloc(order);
			zPowers = gsl_vector_alloc(order);
			dxPowers = gsl_vector_alloc(order);
			dyPowers = gsl_vector_alloc(order);
			dzPowers = gsl_vector_alloc(order);
			aux1 = gsl_vector_alloc(nData);
			aux2 = gsl_vector_alloc(nPar);
			aux3 = gsl_vector_alloc(nData);

			// Set vectors and matrices to zero
			gsl_vector_set_zero(estimatedPar);
			gsl_vector_set_zero(predicted);
			gsl_matrix_set_zero(frechet);
			gsl_matrix_set_zero(frechetT);
			gsl_vector_set_zero(dPredictMeas);
			gsl_matrix_set_zero(invCovMatrixP);
			gsl_vector_set_zero(gamma);
			gsl_vector_set_zero(xPowers);
			gsl_vector_set_zero(yPowers);
			gsl_vector_set_zero(zPowers);
			gsl_vector_set_zero(dxPowers);
			gsl_vector_set_zero(dyPowers);
			gsl_vector_set_zero(dzPowers);
			gsl_vector_set_zero(aux1);
			gsl_vector_set_zero(aux2);
			gsl_vector_set_zero(aux3);

			double dielectric = 0.0; // x
			double density = 0.0;   // y
			double height = 0.0;   // z
			double xPow = 0.0, dxPow = 0.0, yPow = 0.0, dyPow = 0.0, zPow = 0.0, dzPow = 0.0;
			double cCoeff, cCoeffPowZ, cCoeffPowZdZ;
			double bcoeffPowY, bcoeffPowYdY, bcoeffPowYdZ;
			double aCoeffPowX, aCoeffPowXdX, aCoeffPowXdY, aCoeffPowXdZ;
			double predictSq = 0.0, error = 0.0, predictMeasElement = 0.0;
			double gammaElement = 0.0, currPriorDiff = 0.0, predictParElement = 0.0;
			double s1 = 0.0, s2 = 0.0, s1DivS2 = 0.0;

			// Polulate vector holding a priori  estimates
			double initialDielectric = gsl_vector_get(initialPar, 0);
			double initialDensity = gsl_vector_get(initialPar, 1);
			double initialHeight = gsl_vector_get(initialPar, 2);

			// Set canopy depth and stem density to initial estimates.
			gsl_vector_set(estimatedPar, 0, initialDielectric);
			gsl_vector_set(estimatedPar, 1, initialDensity);
			gsl_vector_set(estimatedPar, 2, initialHeight);

			// Calculat the inverse covarience matrices
			matrixUtils.inv2x2GSLMatrix(covMatrixP, invCovMatrixP);

			// Calculate Sigma Squared (used to calculate error)
			double sigmaSq = 0.0;

			for(int s = 0; s < nData; s++)
			{
				sigmaSq = sigmaSq + pow(gsl_vector_get(inSigma0dB, s),2);
			}

			// START ITTERATING
			for(int itt = 0; itt < ittmax;itt++)
			{
				// Set up equations and Frechet derivative operator
				dielectric = gsl_vector_get(estimatedPar, 0);
				density = gsl_vector_get(estimatedPar, 1);
				height = gsl_vector_get(estimatedPar, 2);

				// Set up vectors to hold powers
				for(int i = 0; i < order ; i++)
				{
					xPow = pow(dielectric, i);
					dxPow = i * pow(dielectric, i - 1);
					gsl_vector_set(xPowers, i, xPow);
					gsl_vector_set(dxPowers, i, dxPow);
					yPow = pow(density, i);
					dyPow = i * pow(density, i - 1);
					gsl_vector_set(yPowers, i, yPow);
					gsl_vector_set(dyPowers, i, dyPow);
					zPow = pow(height, i);
					dzPow = i * pow(height, i - 1);
					gsl_vector_set(zPowers, i, zPow);
					gsl_vector_set(dzPowers, i, dzPow);
				}

				////////
				// HH //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				for(int x = 0; x < order; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;
					for(int y = 0; y < order; y++)
					{

						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < order; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(coeffHH, y + (x * order), z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
					}
					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 0, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 0, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 0, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 0, aCoeffPowXdZ);

				////////
				// HV //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				for(int x = 0; x < order; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;

					for(int y = 0; y < order; y++)
					{

						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < order; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(coeffHV, y + (x * order), z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
					}

					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 1, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 1, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 1, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 1, aCoeffPowXdZ);

				////////
				// VV //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				for(int x = 0; x < order; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;

					for(int y = 0; y < order; y++)
					{

						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < order; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(coeffVV, y + (x * order), z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
					}

					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 2, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 2, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 2, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 2, aCoeffPowXdZ);

				// CALCULATE ERROR
				predictSq = 0.0;
				for(int i = 0; i < nData; i++)
				{
					predictMeasElement = gsl_vector_get(predicted, i) - gsl_vector_get(inSigma0dB, i); // Predicted - Measured data
					gsl_vector_set(dPredictMeas, i, predictMeasElement);
					predictSq = predictSq + pow(predictMeasElement,2); // Predicted data squared.
				}
				error = 0.0;
				error = sqrt(predictSq / sigmaSq); // Square Root [Sum(Predicted Squared) / Sum(Measured Squared)]

				if(error < 0.00001)
				{
					break;
				}

				// FIND GRADIENT DIRECTION
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, dPredictMeas, aux1); // aux1 = invCovMatrixD . dPredictMeas
				matrixUtils.productMatrixVectorGSL(frechet, aux1, aux2); // aux2 = Frechet . aux1
				matrixUtils.productMatrixVectorGSL(covMatrixP, aux2, gamma); // gamma = covMatrixP . aux2

				for(int i = 0; i < nPar; i++)
				{
					gammaElement = gsl_vector_get(gamma, i);
					currPriorDiff = gsl_vector_get(estimatedPar, i) - gsl_vector_get(initialPar, i); // X_i - X_ap
					gsl_vector_set(gamma, i, (gammaElement + currPriorDiff));
				}

				// FIND STEP LENGTH
				matrixUtils.transposeNonSquareGSL(frechet, frechetT); // Transpose of Frechet derivative operator
				matrixUtils.productMatrixVectorGSL(frechetT, gamma, aux1); // aux1 = frechetT . gamma
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, aux1, aux3); // aux3 = invCovMatrixD . aux1
				s1 = vectorUtils.dotProductVectorVectorGSL(aux1, aux3); // s1 = aux1 . aux2
				matrixUtils.productMatrixVectorGSL(invCovMatrixP, gamma, aux2); // aux2 = invCovMatrixP . gamma
				s2 = vectorUtils.dotProductVectorVectorGSL(gamma, aux2); // s2 = gamma . aux2
				s1DivS2 = s1 / s2;
				alpha = 1 / (1 + s1DivS2); // 1 / (1 + (s1/s2))


				// UPDATE VALUES
				for(int i = 0; i < nPar; i++)
				{
					predictParElement = gsl_vector_get(estimatedPar, i);
					gammaElement = gsl_vector_get(gamma, i);
					predictParElement = predictParElement - (alpha * gammaElement);
					gsl_vector_set(estimatedPar, i, predictParElement);
					gsl_vector_set(outParError, i, predictParElement);
				}

				gsl_vector_set(outParError, nPar, error);
			}

			// FREE MATRICES
			gsl_vector_free(estimatedPar);
			gsl_matrix_free(frechet);
			gsl_matrix_free(frechetT);
			gsl_vector_free(dPredictMeas);
			gsl_matrix_free(invCovMatrixP);
			gsl_vector_free(gamma);
			gsl_vector_free(xPowers);
			gsl_vector_free(yPowers);
			gsl_vector_free(zPowers);
			gsl_vector_free(dxPowers);
			gsl_vector_free(dyPowers);
			gsl_vector_free(dzPowers);
			gsl_vector_free(aux1);
			gsl_vector_free(aux2);
			gsl_vector_free(aux3);
		}
		RSGISEstimationConjugateGradient::~RSGISEstimationConjugateGradient()
		{

		}


		RSGISEstimationConjugateGradient2DPoly2Channel::RSGISEstimationConjugateGradient2DPoly2Channel(gsl_matrix *coeffHH, gsl_matrix *coeffHV,
													   gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD,
													   int ittmax)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			this->coeffHH = coeffHH;
			this->coeffHV = coeffHV;
			this->covMatrixP = covMatrixP;
			this->invCovMatrixD = invCovMatrixD;
			this->ittmax = ittmax;
			this->nData = 2; // Data channels
			this->nPar = 2; // Parameters to be retrieved
			this->order = coeffHH->size1; // Get polynomial order

			// Set up inverse covarience Matrices
			this->invCovMatrixP = gsl_matrix_alloc(nPar,nPar);
			matrixUtils.inv2x2GSLMatrix(covMatrixP, this->invCovMatrixP);
		}
		int RSGISEstimationConjugateGradient2DPoly2Channel::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
		{

			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISVectors vectorUtils;

			gsl_vector *estimatedPar; // Vector to hold the estimated parameters
			gsl_matrix *frechet; // Vector to hold the Frechet derivative operator
			gsl_matrix *frechetT; // Vector to hold the transpose of the Frechet derivative operator
			gsl_vector *dPredictMeas; // Vector to hold predicted - measured data
			gsl_vector *predicted; // Vector to hold predicted values of backscatter
			gsl_vector *gamma; // Vector to hold gamma values
			gsl_vector *xPowers, *yPowers; // Vectors to hold powers of x, y and z
			gsl_vector *dxPowers, *dyPowers; // Vectors to hold differential powers of x, y and z
			gsl_vector *aux1, *aux2, *aux3; // Tempory vectors
			double alpha = 0;

			// Allocate vectors and matrices
			estimatedPar = gsl_vector_alloc(nPar);
			frechet = gsl_matrix_alloc(nPar,nData);
			frechetT = gsl_matrix_alloc(nData,nPar);
			dPredictMeas = gsl_vector_alloc(nData);
			predicted = gsl_vector_alloc(nData);
			gamma = gsl_vector_alloc(nPar);
			aux1 = gsl_vector_alloc(nData);
			aux2 = gsl_vector_alloc(nPar);
			aux3 = gsl_vector_alloc(nData);
			xPowers = gsl_vector_alloc(order);
			yPowers = gsl_vector_alloc(order);
			dxPowers = gsl_vector_alloc(order);
			dyPowers = gsl_vector_alloc(order);

			double height = 0.0;
			double density = 0.0;
			double predictSigma0 = 0.0, predictSigma0F = 0.0, predictSigma0F2 = 0.0;
			double xPow = 0.0, dxPow = 0.0, aCoeff = 0.0, aCoeffF = 0.0;
			double yPow = 0.0, dyPow = 0.0, bcoeff = 0.0, bcoeffYPow = 0.0, bcoeffYPowF = 0.0;
			double acoeffXPow = 0.0, acoeffXPowF = 0.0, acoeffXPowF2 = 0.0;
			double predictSq = 0.0, error = 0.0, predictMeasElement = 0.0;
			double gammaElement = 0.0, currPriorDiff = 0.0, predictParElement = 0.0;
			double s1 = 0.0, s2 = 0.0, s1DivS2 = 0.0;

			// Polulate vector holding a priori  estimates
			double initialHeight = gsl_vector_get(initialPar, 0);
			double initialDensity = gsl_vector_get(initialPar, 1);

			// Set canopy depth and stem density to initial estimates.
			gsl_vector_set(estimatedPar, 0, initialHeight);
			gsl_vector_set(estimatedPar, 1, initialDensity);

			// Calculate Sigma Squared (used to calculate error)
			double sigmaSq = 0.0;

			for(int s = 0; s < nData; s++)
			{
				sigmaSq = sigmaSq + pow(gsl_vector_get(inData, s),2);
			}

			// START ITERATING
			for(int itt = 0; itt < ittmax;itt++)
			{
				// Zero matrices
				gsl_vector_set_zero(aux1);
				gsl_vector_set_zero(aux2);
				gsl_vector_set_zero(aux3);
				gsl_vector_set_zero(predicted);
				gsl_matrix_set_zero(frechet);
				gsl_matrix_set_zero(frechetT);
				gsl_vector_set_zero(gamma);
				gsl_vector_set_zero(xPowers);
				gsl_vector_set_zero(yPowers);
				gsl_vector_set_zero(dxPowers);
				gsl_vector_set_zero(dyPowers);

				// Set up equations and Frechet derivative operator
				height = gsl_vector_get(estimatedPar, 0);
				density = gsl_vector_get(estimatedPar, 1);

				// Set up vectors to hold powers
				for(int i = 0; i < order ; i++)
				{
					xPow = pow(height, i);
					dxPow = i * pow(height, i - 1);
					gsl_vector_set(xPowers, i, xPow);
					gsl_vector_set(dxPowers, i, dxPow);
					yPow = pow(density, i);
					dyPow = i * pow(density, i - 1);
					gsl_vector_set(yPowers, i, yPow);
					gsl_vector_set(dyPowers, i, dyPow);
				}

				////////
				// HH //
				////////

				predictSigma0 = 0.0;
				predictSigma0F = 0.0;
				predictSigma0F2 = 0.0;

				for(int x = 0; x < order; x ++)
				{
					xPow = gsl_vector_get(xPowers, x); // height^n;
					dxPow = gsl_vector_get(dxPowers, x); // height^(n-1);

					aCoeff = 0.0;
					aCoeffF = 0.0;

					for(int y = 0; y < order ; y++) // Calculate a_n(density)
					{
						yPow = gsl_vector_get(yPowers, y); // density^n;
						dyPow = gsl_vector_get(dyPowers, y); // density^(n-1)
						bcoeff = gsl_matrix_get(coeffHH, x, y); // b_n
						bcoeffYPow = bcoeff * yPow; // b_n * y^n
						bcoeffYPowF = bcoeff * dyPow; // n * b_n * y^(n-1)
						aCoeff = aCoeff + bcoeffYPow;
						aCoeffF = aCoeffF + bcoeffYPowF;
					}

					acoeffXPow = xPow * aCoeff;
					acoeffXPowF = dxPow * aCoeff; // d(f_nl)/ dx (partial derivative with respect to height)
					acoeffXPowF2 = xPow * aCoeffF;  // d(f_nl)/ dy (partial derivative with respect to density)

					predictSigma0 = predictSigma0 + acoeffXPow;
					predictSigma0F = predictSigma0F + acoeffXPowF;
					predictSigma0F2 = predictSigma0F2 + acoeffXPowF2;
				}

				// Save values to matrixes.
				gsl_vector_set(predicted, 0, predictSigma0);
				gsl_matrix_set(frechet, 0, 0, predictSigma0F);
				gsl_matrix_set(frechet, 1, 0, predictSigma0F2);

				////////
				// HV //
				////////

				predictSigma0 = 0.0;
				predictSigma0F = 0.0;
				predictSigma0F2 = 0.0;

				for(int x = 0; x < order; x ++)
				{
					xPow = gsl_vector_get(xPowers, x); // height^n;
					dxPow = gsl_vector_get(dxPowers, x); // height^(n-1);

					aCoeff = 0.0;
					aCoeffF = 0.0;

					for(int y = 0; y < order ; y++) // Calculate a_n(density)
					{
						yPow = gsl_vector_get(yPowers, y); // density^n;
						dyPow = gsl_vector_get(dyPowers, y); // density^(n-1)
						bcoeff = gsl_matrix_get(coeffHV, x, y); // b_n
						bcoeffYPow = bcoeff * yPow; // b_n * y^n
						bcoeffYPowF = bcoeff * dyPow; // n * b_n * y^(n-1)
						aCoeff = aCoeff + bcoeffYPow;
						aCoeffF = aCoeffF + bcoeffYPowF;
					}

					acoeffXPow = xPow * aCoeff;
					acoeffXPowF = dxPow * aCoeff; // d(f_nl)/ dx (partial derivative with respect to height)
					acoeffXPowF2 = xPow * aCoeffF;  // d(f_nl)/ dy (partial derivative with respect to density)

					predictSigma0 = predictSigma0 + acoeffXPow;
					predictSigma0F = predictSigma0F + acoeffXPowF;
					predictSigma0F2 = predictSigma0F2 + acoeffXPowF2;
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 1, predictSigma0);
				gsl_matrix_set(frechet, 0, 1, predictSigma0F);
				gsl_matrix_set(frechet, 1, 1, predictSigma0F2);

				// CALCULATE ERROR
				predictSq = 0.0;
				for(int i = 0; i < nData; i++)
				{
					predictMeasElement = gsl_vector_get(predicted, i) - gsl_vector_get(inData, i); // Predicted - Measured data
					gsl_vector_set(dPredictMeas, i, predictMeasElement);
					predictSq = predictSq + pow(predictMeasElement,2); // Predicted data squared.
				}
				error = 0.0;
				error = sqrt(predictSq / sigmaSq); // Square Root [Sum(Predicted Squared) / Sum(Measured Squared)]

				if(error < 0.00001)
				{
					break;
				}

				// FIND GRADIENT DIRECTION
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, dPredictMeas, aux1); // aux1 = invCovMatrixD . dPredictMeas
				matrixUtils.productMatrixVectorGSL(frechet, aux1, aux2); // aux2 = Frechet . aux1
				matrixUtils.productMatrixVectorGSL(covMatrixP, aux2, gamma); // gamma = covMatrixP . aux2

				for(int i = 0; i < nPar; i++)
				{
					gammaElement = gsl_vector_get(gamma, i);
					currPriorDiff = gsl_vector_get(estimatedPar, i) - gsl_vector_get(initialPar, i); // X_i - X_ap
					gsl_vector_set(gamma, i, (gammaElement + currPriorDiff));
				}

				// FIND STEP LENGTH
				matrixUtils.transposeGSL(frechet, frechetT); // Transpose of Frechet derivative operator
				matrixUtils.productMatrixVectorGSL(frechetT, gamma, aux1); // aux1 = frechetT . gamma
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, aux1, aux3); // aux3 = invCovMatrixD . aux1
				s1 = vectorUtils.dotProductVectorVectorGSL(aux1, aux3); // s1 = aux1 . aux3
				matrixUtils.productMatrixVectorGSL(invCovMatrixP, gamma, aux2); // aux2 = invCovMatrixP . gamma
				s2 = vectorUtils.dotProductVectorVectorGSL(gamma, aux2); // s2 = gamma . aux2
				s1DivS2 = s1 / s2;
				alpha = 1 / (1 + s1DivS2); // 1 / (1 + (s1/s2))

                // UPDATE VALUES
				for(int i = 0; i < nPar; i++)
				{
					predictParElement = gsl_vector_get(estimatedPar, i);
					gammaElement = gsl_vector_get(gamma, i);
					predictParElement = predictParElement - (alpha * gammaElement);
					gsl_vector_set(estimatedPar, i, predictParElement);
					gsl_vector_set(outParError, i, predictParElement);
				}
				gsl_vector_set(outParError, nPar, error);
			}

			// FREE MATRICES
			gsl_vector_free(estimatedPar);
			gsl_matrix_free(frechet);
			gsl_matrix_free(frechetT);
			gsl_vector_free(dPredictMeas);
			gsl_vector_free(predicted);
			gsl_vector_free(gamma);
			gsl_vector_free(aux1);
			gsl_vector_free(aux2);
			gsl_vector_free(aux3);
			gsl_vector_free(xPowers);
			gsl_vector_free(yPowers);
			gsl_vector_free(dxPowers);
			gsl_vector_free(dyPowers);

			return 1;
		}
		RSGISEstimationConjugateGradient2DPoly2Channel::~RSGISEstimationConjugateGradient2DPoly2Channel()
		{
			gsl_matrix_free(invCovMatrixP);
		}


		RSGISEstimationConjugateGradient3DPoly3Channel::RSGISEstimationConjugateGradient3DPoly3Channel(gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV,
																									   int orderX, int orderY, int orderZ,
																									   gsl_vector *aPrioriPar, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD,
																									   double minError, int ittmax)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			this->coeffHH = coeffHH;
			this->coeffHV = coeffHV;
			this->coeffVV = coeffVV;

			this->aPrioriPar = aPrioriPar;
			this->covMatrixP = covMatrixP;
			this->invCovMatrixD = invCovMatrixD;
			this->ittmax = ittmax;
			this->minError = minError;
			this->nData = 3; // Data channels
			this->nPar = 3; // Parameters to be retrieved
			this->orderX = orderX;
			this->orderY = orderY;
			this->orderZ = orderZ;
			// Set up inverse covarience Matrices
			this->invCovMatrixP = gsl_matrix_alloc(nPar,nPar);
			gsl_matrix_set_zero(this->invCovMatrixP);
			gsl_matrix_set(this->invCovMatrixP, 0, 0, 1 / gsl_matrix_get(covMatrixP, 0, 0));
			gsl_matrix_set(this->invCovMatrixP, 1, 1, 1 / gsl_matrix_get(covMatrixP, 1, 1));
			gsl_matrix_set(this->invCovMatrixP, 2, 2, 1 / gsl_matrix_get(covMatrixP, 2, 2));

		}
		int RSGISEstimationConjugateGradient3DPoly3Channel::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISVectors vectorUtils;

			gsl_vector *estimatedPar; // Vector to hold the estimated parameters
			gsl_vector *predicted; // Vector to hold predicted values of backscatter
			gsl_matrix *frechet; // Vector to hold the Frechet derivative operator
			gsl_matrix *frechetT; // Vector to hold the transpose of the Frechet derivative operator
			gsl_vector *dPredictMeas; // Vector to hold predicted - measured data
			gsl_vector *gamma; // Vector to hold gamma values
			gsl_vector *xPowers, *yPowers, *zPowers; // Vectors to hold powers of x, y and z
			gsl_vector *dxPowers, *dyPowers, *dzPowers; // Vectors to hold differential powers of x, y and z
			gsl_vector *aux1, *aux2, *aux3; // Tempory vectors
			double alpha = 0;

			// Allocate vectors and matrices
			estimatedPar = gsl_vector_alloc(nPar);
			frechet = gsl_matrix_alloc(nPar,nData);
			frechetT = gsl_matrix_alloc(nData,nPar);
			dPredictMeas = gsl_vector_alloc(nData);
			predicted = gsl_vector_alloc(nData);
			gamma = gsl_vector_alloc(nPar);
			xPowers = gsl_vector_alloc(orderX);
			yPowers = gsl_vector_alloc(orderY);
			zPowers = gsl_vector_alloc(orderZ);
			dxPowers = gsl_vector_alloc(orderX);
			dyPowers = gsl_vector_alloc(orderY);
			dzPowers = gsl_vector_alloc(orderZ);
			aux1 = gsl_vector_alloc(nData);
			aux2 = gsl_vector_alloc(nPar);
			aux3 = gsl_vector_alloc(nData);

			// Set vectors and matrices to zero
			gsl_vector_set_zero(estimatedPar);
			gsl_vector_set_zero(predicted);
			gsl_matrix_set_zero(frechet);
			gsl_matrix_set_zero(frechetT);
			gsl_vector_set_zero(dPredictMeas);
			gsl_vector_set_zero(gamma);
			gsl_vector_set_zero(xPowers);
			gsl_vector_set_zero(yPowers);
			gsl_vector_set_zero(zPowers);
			gsl_vector_set_zero(dxPowers);
			gsl_vector_set_zero(dyPowers);
			gsl_vector_set_zero(dzPowers);
			gsl_vector_set_zero(aux1);
			gsl_vector_set_zero(aux2);
			gsl_vector_set_zero(aux3);

			double dielectric = 0.0; // x
			double density = 0.0;   // y
			double height = 0.0;   // z
			double xPow = 0.0, dxPow = 0.0, yPow = 0.0, dyPow = 0.0, zPow = 0.0, dzPow = 0.0;
			double cCoeff, cCoeffPowZ, cCoeffPowZdZ;
			double bcoeffPowY, bcoeffPowYdY, bcoeffPowYdZ;
			double aCoeffPowX, aCoeffPowXdX, aCoeffPowXdY, aCoeffPowXdZ;
			double error = 0.0;
			double gammaElement = 0.0, currPriorDiff = 0.0, predictParElement = 0.0;
			double s1 = 0.0, s2 = 0.0, s1DivS2 = 0.0;

			// Polulate vector holding a priori  estimates
			double initialDielectric = gsl_vector_get(initialPar, 0);
			double initialDensity = gsl_vector_get(initialPar, 1);
			double initialHeight = gsl_vector_get(initialPar, 2);

			// Set canopy depth and stem density to initial estimates.
			gsl_vector_set(estimatedPar, 0, initialDielectric);
			gsl_vector_set(estimatedPar, 1, initialDensity);
			gsl_vector_set(estimatedPar, 2, initialHeight);

			// Set initial error
			gsl_vector_set(outParError, nPar, + std::numeric_limits<double>::infinity());//+INFINITY);

			// Calculate Sigma Squared (used to calculate error)
			double sigmaSq = 0.0;

			for(int s = 0; s < nData; s++)
			{
				sigmaSq = sigmaSq + pow(gsl_vector_get(inData, s),2);
			}

			// START ITTERATING
			for(int itt = 0; itt < ittmax;itt++)
			{
				// Set up equations and Frechet derivative operator
				dielectric = gsl_vector_get(estimatedPar, 0);
				density = gsl_vector_get(estimatedPar, 1);
				height = gsl_vector_get(estimatedPar, 2);

				// Set up vectors to hold powers
				for(int i = 0; i < orderX ; i++)
				{
					xPow = pow(dielectric, i);
					dxPow = i * pow(dielectric, i - 1);
					gsl_vector_set(xPowers, i, xPow);
					gsl_vector_set(dxPowers, i, dxPow);
				}
				for(int i = 0; i < orderY ; i++)
				{
					yPow = pow(density, i);
					dyPow = i * pow(density, i - 1);
					gsl_vector_set(yPowers, i, yPow);
					gsl_vector_set(dyPowers, i, dyPow);
				}
				for(int i = 0; i < orderZ ; i++)
				{
					zPow = pow(height, i);
					dzPow = i * pow(height, i - 1);
					gsl_vector_set(zPowers, i, zPow);
					gsl_vector_set(dzPowers, i, dzPow);
				}

				////////
				// HH //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				unsigned int c = 0;

				for(int x = 0; x < this->orderX; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;
					for(int y = 0; y < this->orderY; y++)
					{
						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < this->orderZ; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(this->coeffHH, c, z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
						c++;
					}
					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 0, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 0, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 0, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 0, aCoeffPowXdZ);

				////////
				// HV //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				c = 0;

				for(int x = 0; x < this->orderX; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;

					for(int y = 0; y < this->orderY; y++)
					{

						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < this->orderZ; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(this->coeffHV, c, z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
						c++;
					}

					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 1, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 1, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 1, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 1, aCoeffPowXdZ);

				////////
				// VV //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				c = 0;

				for(int x = 0; x < orderX; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;

					for(int y = 0; y < orderY; y++)
					{
						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < orderZ; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(this->coeffVV, c, z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
						c++;
					}

					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 2, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 2, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 2, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 2, aCoeffPowXdZ);

				for(int i = 0; i < nData; i++)
				{
					double predictMeasElement = gsl_vector_get(predicted, i) - gsl_vector_get(inData, i); // Predicted - Measured data
					gsl_vector_set(dPredictMeas, i, predictMeasElement);
				}

				error = 0.0;

				// CALCULATE ERROR
				for(int i = 0; i < nData; i++)
				{
					double diff =  pow((gsl_vector_get(inData, i) - gsl_vector_get(predicted, i)),2); // Predicted - Measured data
					error	= error + diff;
				}
				error = error / sigmaSq;

				if((error < minError) | (boost::math::isnan(error)))
				{
					for(int i = 0; i < nPar; i++)
					{
						gsl_vector_set(outParError, i, gsl_vector_get(estimatedPar, i));
					}
					gsl_vector_set(outParError, nPar, error);

					gsl_vector_free(estimatedPar);
					gsl_matrix_free(frechet);
					gsl_matrix_free(frechetT);
					gsl_vector_free(predicted);
					gsl_vector_free(dPredictMeas);
					gsl_vector_free(gamma);
					gsl_vector_free(xPowers);
					gsl_vector_free(yPowers);
					gsl_vector_free(zPowers);
					gsl_vector_free(dxPowers);
					gsl_vector_free(dyPowers);
					gsl_vector_free(dzPowers);
					gsl_vector_free(aux1);
					gsl_vector_free(aux2);
					gsl_vector_free(aux3);

					if (error < minError)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					// FIND GRADIENT DIRECTION
					matrixUtils.productMatrixVectorGSL(this->invCovMatrixD, dPredictMeas, aux1); // aux1 = invCovMatrixD . dPredictMeas
					matrixUtils.productMatrixVectorGSL(frechet, aux1, aux2); // aux2 = Frechet . aux1
					matrixUtils.productMatrixVectorGSL(this->covMatrixP, aux2, gamma); // gamma = covMatrixP . aux2

					for(int i = 0; i < nPar; i++)
					{
						gammaElement = gsl_vector_get(gamma, i);
						currPriorDiff = gsl_vector_get(estimatedPar, i) - gsl_vector_get(initialPar, i); // X_i - X_ap
						gsl_vector_set(gamma, i, (gammaElement + currPriorDiff));
					}

					// FIND STEP LENGTH
					matrixUtils.transposeNonSquareGSL(frechet, frechetT); // Transpose of Frechet derivative operator
					matrixUtils.productMatrixVectorGSL(frechetT, gamma, aux1); // aux1 = frechetT . gamma
					matrixUtils.productMatrixVectorGSL(invCovMatrixD, aux1, aux3); // aux3 = invCovMatrixD . aux1
					s1 = vectorUtils.dotProductVectorVectorGSL(aux1, aux3); // s1 = aux1 . aux2
					matrixUtils.productMatrixVectorGSL(this->invCovMatrixP, gamma, aux2); // aux2 = invCovMatrixP . gamma
					s2 = vectorUtils.dotProductVectorVectorGSL(gamma, aux2); // s2 = gamma . aux2
					s1DivS2 = s1 / s2;
					alpha = 1 / (1 + s1DivS2); // 1 / (1 + (s1/s2))


					// UPDATE VALUES
					for(int i = 0; i < nPar; i++)
					{
						predictParElement = gsl_vector_get(estimatedPar, i);
						if (error < gsl_vector_get(outParError, nPar)) // Update best to current values
						{
							gsl_vector_set(outParError, i, predictParElement);
						}
						// Calculate new values
						gammaElement = gsl_vector_get(gamma, i);
						predictParElement = predictParElement - (alpha * gammaElement);
						gsl_vector_set(estimatedPar, i, predictParElement);
					}

					if (error < gsl_vector_get(outParError, nPar))
					{
						gsl_vector_set(outParError, nPar, error);
					}

				}
			}

			// FREE MATRICES
			gsl_vector_free(estimatedPar);
			gsl_matrix_free(frechet);
			gsl_matrix_free(frechetT);
			gsl_vector_free(predicted);
			gsl_vector_free(dPredictMeas);
			gsl_vector_free(gamma);
			gsl_vector_free(xPowers);
			gsl_vector_free(yPowers);
			gsl_vector_free(zPowers);
			gsl_vector_free(dxPowers);
			gsl_vector_free(dyPowers);
			gsl_vector_free(dzPowers);
			gsl_vector_free(aux1);
			gsl_vector_free(aux2);
			gsl_vector_free(aux3);

			return 0;
		}
		RSGISEstimationConjugateGradient3DPoly3Channel::~RSGISEstimationConjugateGradient3DPoly3Channel()
		{
			gsl_matrix_free(invCovMatrixP);
		}

		RSGISEstimationConjugateGradient3DPoly4Channel::RSGISEstimationConjugateGradient3DPoly4Channel(gsl_matrix *coeffA, gsl_matrix *coeffB, gsl_matrix *coeffC, gsl_matrix *coeffD,
																									   int orderX, int orderY, int orderZ,
																									   gsl_vector *aPrioriPar, gsl_matrix *covMatrixP, gsl_matrix *invCovMatrixD,
																									   double minError, int ittmax)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			this->coeffA = coeffA;
			this->coeffB = coeffB;
			this->coeffC = coeffC;
			this->coeffD = coeffD;
			this->aPrioriPar = aPrioriPar;
			this->covMatrixP = covMatrixP;
			this->invCovMatrixD = invCovMatrixD;
			this->ittmax = ittmax;
			this->minError = minError;
			this->nData = 4; // Data channels
			this->nPar = 3; // Parameters to be retrieved
			this->orderX = orderX;
			this->orderY = orderY;
			this->orderZ = orderZ;
			// Set up inverse covarience Matrices
			this->invCovMatrixP = gsl_matrix_alloc(nPar,nPar);
			gsl_matrix_set_zero(this->invCovMatrixP);
			gsl_matrix_set(this->invCovMatrixP, 0, 0, 1 / gsl_matrix_get(covMatrixP, 0, 0));
			gsl_matrix_set(this->invCovMatrixP, 1, 1, 1 / gsl_matrix_get(covMatrixP, 1, 1));
			gsl_matrix_set(this->invCovMatrixP, 2, 2, 1 / gsl_matrix_get(covMatrixP, 2, 2));
		}
        
		int RSGISEstimationConjugateGradient3DPoly4Channel::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISVectors vectorUtils;

			gsl_vector *estimatedPar; // Vector to hold the estimated parameters
			gsl_vector *predicted; // Vector to hold predicted values of backscatter
			gsl_matrix *frechet; // Vector to hold the Frechet derivative operator
			gsl_matrix *frechetT; // Vector to hold the transpose of the Frechet derivative operator
			gsl_vector *dPredictMeas; // Vector to hold predicted - measured data
			gsl_vector *gamma; // Vector to hold gamma values
			gsl_vector *xPowers, *yPowers, *zPowers; // Vectors to hold powers of x, y and z
			gsl_vector *dxPowers, *dyPowers, *dzPowers; // Vectors to hold differential powers of x, y and z
			gsl_vector *aux1, *aux2, *aux3; // Tempory vectors
			double alpha = 0;

			// Allocate vectors and matrices
			estimatedPar = gsl_vector_alloc(nPar);
			frechet = gsl_matrix_alloc(nPar,nData);
			frechetT = gsl_matrix_alloc(nData,nPar);
			dPredictMeas = gsl_vector_alloc(nData);
			predicted = gsl_vector_alloc(nData);
			gamma = gsl_vector_alloc(nPar);
			xPowers = gsl_vector_alloc(orderX);
			yPowers = gsl_vector_alloc(orderY);
			zPowers = gsl_vector_alloc(orderZ);
			dxPowers = gsl_vector_alloc(orderX);
			dyPowers = gsl_vector_alloc(orderY);
			dzPowers = gsl_vector_alloc(orderZ);
			aux1 = gsl_vector_alloc(nData);
			aux2 = gsl_vector_alloc(nPar);
			aux3 = gsl_vector_alloc(nData);

			// Set vectors and matrices to zero
			gsl_vector_set_zero(estimatedPar);
			gsl_vector_set_zero(predicted);
			gsl_matrix_set_zero(frechet);
			gsl_matrix_set_zero(frechetT);
			gsl_vector_set_zero(dPredictMeas);
			gsl_vector_set_zero(gamma);
			gsl_vector_set_zero(xPowers);
			gsl_vector_set_zero(yPowers);
			gsl_vector_set_zero(zPowers);
			gsl_vector_set_zero(dxPowers);
			gsl_vector_set_zero(dyPowers);
			gsl_vector_set_zero(dzPowers);
			gsl_vector_set_zero(aux1);
			gsl_vector_set_zero(aux2);
			gsl_vector_set_zero(aux3);

			double dielectric = 0.0; // x
			double density = 0.0;    // y
			double height = 0.0;     // z
			double xPow = 0.0, dxPow = 0.0, yPow = 0.0, dyPow = 0.0, zPow = 0.0, dzPow = 0.0;
			double cCoeff, cCoeffPowZ, cCoeffPowZdZ;
			double bcoeffPowY, bcoeffPowYdY, bcoeffPowYdZ;
			double aCoeffPowX, aCoeffPowXdX, aCoeffPowXdY, aCoeffPowXdZ;
			double error = 0.0;
			double gammaElement = 0.0, currPriorDiff = 0.0, predictParElement = 0.0;
			double s1 = 0.0, s2 = 0.0, s1DivS2 = 0.0;

			// Polulate vector holding a priori  estimates
			double initialDielectric = gsl_vector_get(initialPar, 0);
			double initialDensity = gsl_vector_get(initialPar, 1);
			double initialHeight = gsl_vector_get(initialPar, 2);

			// Set canopy depth and stem density to initial estimates.
			gsl_vector_set(estimatedPar, 0, initialDielectric);
			gsl_vector_set(estimatedPar, 1, initialDensity);
			gsl_vector_set(estimatedPar, 2, initialHeight);

			// Set initial error
			gsl_vector_set(outParError, nPar, + std::numeric_limits<double>::infinity());//+INFINITY);

			// Calculate Sigma Squared (used to calculate error)
			double sigmaSq = 0.0;

			for(int s = 0; s < nData; s++)
			{
				sigmaSq = sigmaSq + pow(gsl_vector_get(inData, s),2);
			}

			// START ITTERATING
			for(int itt = 0; itt < ittmax;itt++)
			{
				// Set up equations and Frechet derivative operator
				dielectric = gsl_vector_get(estimatedPar, 0);
				density = gsl_vector_get(estimatedPar, 1);
				height = gsl_vector_get(estimatedPar, 2);

				// Set up vectors to hold powers
				for(int i = 0; i < orderX ; i++)
				{
					xPow = pow(dielectric, i);
					dxPow = i * pow(dielectric, i - 1);
					gsl_vector_set(xPowers, i, xPow);
					gsl_vector_set(dxPowers, i, dxPow);
				}
				for(int i = 0; i < orderY ; i++)
				{
					yPow = pow(density, i);
					dyPow = i * pow(density, i - 1);
					gsl_vector_set(yPowers, i, yPow);
					gsl_vector_set(dyPowers, i, dyPow);
				}
				for(int i = 0; i < orderZ ; i++)
				{
					zPow = pow(height, i);
					dzPow = i * pow(height, i - 1);
					gsl_vector_set(zPowers, i, zPow);
					gsl_vector_set(dzPowers, i, dzPow);
				}

				////////
				// A  //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				unsigned int c = 0;

				for(int x = 0; x < orderX; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;
					for(int y = 0; y < orderY; y++)
					{

						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < orderZ; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(coeffA, c, z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
						c++;
					}
					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 0, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 0, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 0, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 0, aCoeffPowXdZ);

				////////
				// B  //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				c = 0;

				for(int x = 0; x < orderX; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;

					for(int y = 0; y < orderY; y++)
					{

						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < orderZ; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(coeffB, c, z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
						c++;
					}

					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 1, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 1, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 1, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 1, aCoeffPowXdZ);

				////////
				// C  //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				c = 0;

				for(int x = 0; x < orderX; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;

					for(int y = 0; y < orderY; y++)
					{
						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < orderZ; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(coeffC, c, z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
						c++;
					}

					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 2, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 2, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 2, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 2, aCoeffPowXdZ);


                ////////
				// D  //
				////////
				aCoeffPowX = 0.0;
				aCoeffPowXdX = 0.0;
				aCoeffPowXdY = 0.0;
				aCoeffPowXdZ = 0.0;
				c = 0;

				for(int x = 0; x < orderX; x ++)
				{
					bcoeffPowY = 0.0;
					bcoeffPowYdY = 0.0;
					bcoeffPowYdZ = 0.0;

					for(int y = 0; y < orderY; y++)
					{
						cCoeffPowZ = 0.0;
						cCoeffPowZdZ = 0.0;
						for(int z = 0; z < orderZ; z++)
						{
							zPow = gsl_vector_get(zPowers, z); // z^n;
							dzPow = gsl_vector_get(dzPowers, z); // n * z^(n -1)
							cCoeff = gsl_matrix_get(coeffD, c, z);
							cCoeffPowZ = cCoeffPowZ + (cCoeff * zPow);
							cCoeffPowZdZ = cCoeffPowZdZ + (cCoeff * dzPow);
						}
						yPow = gsl_vector_get(yPowers, y); // y^n;
						dyPow = gsl_vector_get(dyPowers, y); // n * y^(n -1);
						bcoeffPowY = bcoeffPowY + (cCoeffPowZ * yPow); // c_n * y^n
						bcoeffPowYdY = bcoeffPowYdY + (cCoeffPowZ * dyPow); // n * c_n * y^(n-1)
						bcoeffPowYdZ = bcoeffPowYdZ + (cCoeffPowZdZ * yPow);
						c++;
					}

					xPow = gsl_vector_get(xPowers, x); // dielectric^n;
					dxPow = gsl_vector_get(dxPowers, x); // n * dielectric^(n -1);
					aCoeffPowX = aCoeffPowX + (bcoeffPowY * xPow);
					aCoeffPowXdX = aCoeffPowXdX + (bcoeffPowY * dxPow);
					aCoeffPowXdY = aCoeffPowXdY + (bcoeffPowYdY * xPow);
					aCoeffPowXdZ = aCoeffPowXdZ + (bcoeffPowYdZ * xPow);
				}
				// Save values to matrixes.
				gsl_vector_set(predicted, 3, aCoeffPowX);
				gsl_matrix_set(frechet, 0, 3, aCoeffPowXdX);
				gsl_matrix_set(frechet, 1, 3, aCoeffPowXdY);
				gsl_matrix_set(frechet, 2, 3, aCoeffPowXdZ);


				for(int i = 0; i < nData; i++)
				{
					double predictMeasElement = gsl_vector_get(predicted, i) - gsl_vector_get(inData, i); // Predicted - Measured data
					gsl_vector_set(dPredictMeas, i, predictMeasElement);
				}

				error = 0.0;

				// CALCULATE ERROR
				for(int i = 0; i < nData; i++)
				{
					double diff =  pow((gsl_vector_get(inData, i) - gsl_vector_get(predicted, i)),2); // Predicted - Measured data
					error	= error + diff;
				}
				error = error / sigmaSq;

				if((error < minError) | (boost::math::isnan(error))) //((error < 10e-5) && (error / prevError > 0.8) | isnan(error))
				{
					for(int i = 0; i < nPar; i++)
					{
						gsl_vector_set(outParError, i, gsl_vector_get(estimatedPar, i));
					}
					gsl_vector_set(outParError, nPar, error);

					gsl_vector_free(estimatedPar);
					gsl_matrix_free(frechet);
					gsl_vector_free(predicted);
					gsl_matrix_free(frechetT);
					gsl_vector_free(dPredictMeas);
					gsl_vector_free(gamma);
					gsl_vector_free(xPowers);
					gsl_vector_free(yPowers);
					gsl_vector_free(zPowers);
					gsl_vector_free(dxPowers);
					gsl_vector_free(dyPowers);
					gsl_vector_free(dzPowers);
					gsl_vector_free(aux1);
					gsl_vector_free(aux2);
					gsl_vector_free(aux3);

					if (error < minError)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					// FIND GRADIENT DIRECTION
					matrixUtils.productMatrixVectorGSL(this->invCovMatrixD, dPredictMeas, aux1); // aux1 = invCovMatrixD . dPredictMeas
					matrixUtils.productMatrixVectorGSL(frechet, aux1, aux2); // aux2 = Frechet . aux1
					matrixUtils.productMatrixVectorGSL(this->covMatrixP, aux2, gamma); // gamma = covMatrixP . aux2

					for(int i = 0; i < nPar; i++)
					{
						gammaElement = gsl_vector_get(gamma, i);
						currPriorDiff = gsl_vector_get(estimatedPar, i) - gsl_vector_get(initialPar, i); // X_i - X_ap
						gsl_vector_set(gamma, i, (gammaElement + currPriorDiff));
					}

					// FIND STEP LENGTH
					matrixUtils.transposeNonSquareGSL(frechet, frechetT); // Transpose of Frechet derivative operator
					matrixUtils.productMatrixVectorGSL(frechetT, gamma, aux1); // aux1 = frechetT . gamma
					matrixUtils.productMatrixVectorGSL(invCovMatrixD, aux1, aux3); // aux3 = invCovMatrixD . aux1
					s1 = vectorUtils.dotProductVectorVectorGSL(aux1, aux3); // s1 = aux1 . aux2
					matrixUtils.productMatrixVectorGSL(this->invCovMatrixP, gamma, aux2); // aux2 = invCovMatrixP . gamma
					s2 = vectorUtils.dotProductVectorVectorGSL(gamma, aux2); // s2 = gamma . aux2
					s1DivS2 = s1 / s2;
					alpha = 1 / (1 + s1DivS2); // 1 / (1 + (s1/s2))

					// UPDATE VALUES
					for(int i = 0; i < nPar; i++)
					{
						predictParElement = gsl_vector_get(estimatedPar, i);
						if (error < gsl_vector_get(outParError, nPar)) // Update best to current values
						{
							gsl_vector_set(outParError, i, predictParElement);
						}
						// Calculate new values
						gammaElement = gsl_vector_get(gamma, i);
						predictParElement = predictParElement - (alpha * gammaElement);
						gsl_vector_set(estimatedPar, i, predictParElement);
					}

					if (error < gsl_vector_get(outParError, nPar))
					{
						gsl_vector_set(outParError, nPar, error);
					}
				}
			}

			// FREE MATRICES
			gsl_vector_free(estimatedPar);
			gsl_matrix_free(frechet);
			gsl_matrix_free(frechetT);
			gsl_vector_free(predicted);
			gsl_vector_free(dPredictMeas);
			gsl_vector_free(gamma);
			gsl_vector_free(xPowers);
			gsl_vector_free(yPowers);
			gsl_vector_free(zPowers);
			gsl_vector_free(dxPowers);
			gsl_vector_free(dyPowers);
			gsl_vector_free(dzPowers);
			gsl_vector_free(aux1);
			gsl_vector_free(aux2);
			gsl_vector_free(aux3);

			return 0;
		}
		RSGISEstimationConjugateGradient3DPoly4Channel::~RSGISEstimationConjugateGradient3DPoly4Channel()
		{
			gsl_matrix_free(invCovMatrixP);
		}

		RSGISEstimationConjugateGradient2Var2Data::RSGISEstimationConjugateGradient2Var2Data(
																							 rsgis::math::RSGISMathTwoVariableFunction *functionA,
																							 rsgis::math::RSGISMathTwoVariableFunction *functionB,
																							 gsl_vector *aPrioriPar,
																							 gsl_matrix *covMatrixP,
																							 gsl_matrix *invCovMatrixD,
																							 double minError,
																							 int ittmax
		)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			this->functionA = functionA;
			this->functionB = functionB;
			this->aPrioriPar = gsl_vector_alloc(2);
			gsl_vector_memcpy(this->aPrioriPar, aPrioriPar);
			this->covMatrixP = covMatrixP;
			this->invCovMatrixD = invCovMatrixD;
			this->ittmax = ittmax;
			this->minError = minError;

			// Invert covariance matrix
			this->invCovMatrixP = gsl_matrix_alloc(2, 2);
			matrixUtils.inv2x2GSLMatrix(covMatrixP, this->invCovMatrixP);
		}
		int RSGISEstimationConjugateGradient2Var2Data::minimise(gsl_vector *inSigma0dB, gsl_vector *initialPar, gsl_vector *outParError)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISVectors vectorUtils;

			int nData = 2; // Data channels
			int nPar = 2; // Parameters to be retrieved
			gsl_vector *estimatedPar; // Vector to hold the estimated parameters
			gsl_matrix *frechet; // Vector to hold the Frechet derivative operator
			gsl_matrix *frechetT; // Vector to hold the transpose of the Frechet derivative operator
			gsl_vector *dPredictMeas; // Vector to hold predicted - measured data
			gsl_vector *gamma; // Vector to hold gamma values
			gsl_vector *aux1, *aux2, *aux3; // Tempory vectors
			gsl_vector *predicted;
			double alpha = 0;

			// Allocate vectors and matrices
			estimatedPar = gsl_vector_alloc(nPar);
			predicted = gsl_vector_alloc(nData);
			frechet = gsl_matrix_alloc(nPar,nData);
			frechetT = gsl_matrix_alloc(nData,nPar);
			dPredictMeas = gsl_vector_alloc(nData);
			gamma = gsl_vector_alloc(nPar);
			aux1 = gsl_vector_alloc(nData);
			aux2 = gsl_vector_alloc(nPar);
			aux3 = gsl_vector_alloc(nData);

			double height = 0.0;
			double density = 0.0;
			double error = 0.0, predictMeasElement = 0.0;
			double gammaElement = 0.0, currPriorDiff = 0.0, predictParElement = 0.0;
			double s1 = 0.0, s2 = 0.0, s1DivS2 = 0.0;

			// Set canopy depth and stem density to initial estimates.
			gsl_vector_set(estimatedPar, 0, gsl_vector_get(initialPar, 0));
			gsl_vector_set(estimatedPar, 1, gsl_vector_get(initialPar, 1));

			// Set initial error to 1 (100%)
			gsl_vector_set(outParError, nPar, 1);

			// START ITTERATING
			for(int itt = 0; itt < ittmax;itt++)
			{
				// Zero matrices
				gsl_vector_set_zero(aux1);
				gsl_vector_set_zero(aux2);
				gsl_vector_set_zero(aux3);
				gsl_vector_set_zero(predicted);
				gsl_matrix_set_zero(frechet);
				gsl_matrix_set_zero(frechetT);
				gsl_vector_set_zero(gamma);

				// Set up equations and Frechet derivative operator
				height = gsl_vector_get(estimatedPar, 0);
				density = gsl_vector_get(estimatedPar, 1);

				////////
				// HH //
				////////

				gsl_vector_set(predicted, 0, functionA->calcFunction(height, density));
				gsl_matrix_set(frechet, 0, 0,  functionA->dX(height,density));
				gsl_matrix_set(frechet, 1, 0,  functionA->dY(height,density));

				////////
				// HV //
				////////

				gsl_vector_set(predicted, 1, functionB->calcFunction(height,density));
				gsl_matrix_set(frechet, 0, 1,  functionB->dX(height,density));
				gsl_matrix_set(frechet, 1, 1,  functionB->dY(height,density));

				// CALCULATE ERROR
				for(int i = 0; i < nData; i++)
				{
					predictMeasElement = gsl_vector_get(predicted, i) - gsl_vector_get(inSigma0dB, i); // Predicted - Measured data
					gsl_vector_set(dPredictMeas, i, predictMeasElement);
				}

				double prevError = error;
				error = 0.0;

				for(int i = 0; i < nData; i++)
				{
					double diff =  pow((gsl_vector_get(inSigma0dB, i) - gsl_vector_get(predicted, i)),2); // Predicted - Measured data
					error	= error + diff;
				}
				error = error / (pow(gsl_vector_get(inSigma0dB, 0),2)+pow(gsl_vector_get(inSigma0dB, 1),2));

				if((error < minError) | (fabs(prevError - error) < 10e-10) | boost::math::isnan(error))
				{
					for(int i = 0; i < nPar; i++)
					{
						gsl_vector_set(outParError, i, gsl_vector_get(estimatedPar, i));
					}
					gsl_vector_set(outParError, nPar, error);

					if(boost::math::isnan(error))
                    {
                        gsl_vector_set(outParError, nPar,9999);
                    }

					gsl_vector_free(estimatedPar);
					gsl_vector_free(predicted);
					gsl_matrix_free(frechet);
					gsl_matrix_free(frechetT);
					gsl_vector_free(dPredictMeas);
					gsl_vector_free(gamma);
					gsl_vector_free(aux1);
					gsl_vector_free(aux2);
					gsl_vector_free(aux3);

					if (error < minError)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}

				// FIND GRADIENT DIRECTION
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, dPredictMeas, aux1); // aux1 = invCovMatrixD . dPredictMeas
				matrixUtils.productMatrixVectorGSL(frechet, aux1, aux2); // aux2 = Frechet . aux1
				matrixUtils.productMatrixVectorGSL(covMatrixP, aux2, gamma); // gamma = covMatrixP . aux2

				for(int i = 0; i < nPar; i++)
				{
					gammaElement = gsl_vector_get(gamma, i);
					currPriorDiff = gsl_vector_get(estimatedPar, i) - gsl_vector_get(aPrioriPar, i); // X_i - X_ap
					gsl_vector_set(gamma, i, (gammaElement + currPriorDiff));
				}

				// FIND STEP LENGTH
				matrixUtils.transposeGSL(frechet, frechetT); // Transpose of Frechet derivative operator
				matrixUtils.productMatrixVectorGSL(frechetT, gamma, aux1); // aux1 = frechetT . gamma
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, aux1, aux3); // aux3 = invCovMatrixD . aux1
				s1 = vectorUtils.dotProductVectorVectorGSL(aux1, aux3); // s1 = aux1 . aux3
				matrixUtils.productMatrixVectorGSL(invCovMatrixP, gamma, aux2); // aux2 = invCovMatrixP . gamma
				s2 = vectorUtils.dotProductVectorVectorGSL(gamma, aux2); // s2 = gamma . aux2
				s1DivS2 = s1 / s2;
				alpha = 1 / (1 + s1DivS2); // 1 / (1 + (s1/s2))

				// UPDATE VALUES
				for(int i = 0; i < nPar; i++)
				{
					predictParElement = gsl_vector_get(estimatedPar, i);
					gammaElement = gsl_vector_get(gamma, i);
					predictParElement = predictParElement - (alpha * gammaElement);
					gsl_vector_set(estimatedPar, i, predictParElement);
					if (error < gsl_vector_get(outParError, nPar))
					{
						gsl_vector_set(outParError, i, predictParElement);
					}

				}

				if (error < gsl_vector_get(outParError, nPar))
				{
					gsl_vector_set(outParError, nPar, error);
				}
			}

            // FREE MATRICES
			gsl_vector_free(estimatedPar);
			gsl_vector_free(predicted);
			gsl_matrix_free(frechet);
			gsl_matrix_free(frechetT);
			gsl_vector_free(dPredictMeas);
			gsl_vector_free(gamma);
			gsl_vector_free(aux1);
			gsl_vector_free(aux2);
			gsl_vector_free(aux3);

			return 0;
		}

		RSGISEstimationConjugateGradient2Var2Data::~RSGISEstimationConjugateGradient2Var2Data()
		{
			gsl_matrix_free(this->invCovMatrixP);
			gsl_vector_free(this->aPrioriPar);
		}

		RSGISEstimationConjugateGradient2Var3Data::RSGISEstimationConjugateGradient2Var3Data(
																							 rsgis::math::RSGISMathTwoVariableFunction *functionA,
																							 rsgis::math::RSGISMathTwoVariableFunction *functionB,
																							 rsgis::math::RSGISMathTwoVariableFunction *functionC,
																							 gsl_vector *aPrioriPar,
																							 gsl_matrix *covMatrixP,
																							 gsl_matrix *invCovMatrixD,
																							 double minError,
																							 int ittmax
																							 )
		{
			rsgis::math::RSGISMatrices matrixUtils;
			this->functionA = functionA;
			this->functionB = functionB;
			this->functionC = functionC;
			//this->aPrioriPar = aPrioriPar;
			this->aPrioriPar = gsl_vector_alloc(2);
			gsl_vector_memcpy(this->aPrioriPar, aPrioriPar);
			this->covMatrixP = covMatrixP;
			this->invCovMatrixD = invCovMatrixD;
			this->ittmax = ittmax;
			this->minError = minError;

			// Invert covariance matrix
			this->invCovMatrixP = gsl_matrix_alloc(2, 2);
			matrixUtils.inv2x2GSLMatrix(covMatrixP, invCovMatrixP);

		}
		int RSGISEstimationConjugateGradient2Var3Data::minimise(gsl_vector *inSigma0dB, gsl_vector *initialPar, gsl_vector *outParError)
		{
			rsgis::math::RSGISMatrices matrixUtils;
			rsgis::math::RSGISVectors vectorUtils;

			int nData = 3; // Data channels
			int nPar = 2; // Parameters to be retrieved
			gsl_vector *estimatedPar; // Vector to hold the estimated parameters
			gsl_matrix *frechet; // Vector to hold the Frechet derivative operator
			gsl_matrix *frechetT; // Vector to hold the transpose of the Frechet derivative operator
			gsl_vector *dPredictMeas; // Vector to hold predicted - measured data
			gsl_vector *gamma; // Vector to hold gamma values
			gsl_vector *aux1, *aux2, *aux3; // Tempory vectors
			gsl_vector *predicted;
			double alpha = 0;

			// Allocate vectors and matrices
			estimatedPar = gsl_vector_alloc(nPar);
			predicted = gsl_vector_alloc(nData);
			frechet = gsl_matrix_alloc(nPar,nData);
			frechetT = gsl_matrix_alloc(nData,nPar);
			dPredictMeas = gsl_vector_alloc(nData);
			gamma = gsl_vector_alloc(nPar);
			aux1 = gsl_vector_alloc(nData);
			aux2 = gsl_vector_alloc(nPar);
			aux3 = gsl_vector_alloc(nData);

			double height = 0.0;
			double density = 0.0;
			double error = 0.0, predictMeasElement = 0.0;
			double gammaElement = 0.0, currPriorDiff = 0.0, predictParElement = 0.0;
			double s1 = 0.0, s2 = 0.0, s1DivS2 = 0.0;

			// Set canopy depth and stem density to initial estimates.
			gsl_vector_set(estimatedPar, 0, gsl_vector_get(initialPar, 0));
			gsl_vector_set(estimatedPar, 1, gsl_vector_get(initialPar, 1));

			// Set initial error to 1 (100%)
			gsl_vector_set(outParError, nPar, 1);

			// START ITTERATING
			for(int itt = 0; itt < ittmax;itt++)
			{
				// Zero matrices
				gsl_vector_set_zero(aux1);
				gsl_vector_set_zero(aux2);
				gsl_vector_set_zero(aux3);
				gsl_vector_set_zero(predicted);
				gsl_matrix_set_zero(frechet);
				gsl_matrix_set_zero(frechetT);
				gsl_vector_set_zero(gamma);

				// Set up equations and Frechet derivative operator
				height = gsl_vector_get(estimatedPar, 0);
				density = gsl_vector_get(estimatedPar, 1);

				// Data channel A
				gsl_vector_set(predicted, 0, functionA->calcFunction(height, density));
				gsl_matrix_set(frechet, 0, 0,  functionA->dX(height,density));
				gsl_matrix_set(frechet, 1, 0,  functionA->dY(height,density));

				// Data channel B
				gsl_vector_set(predicted, 1, functionB->calcFunction(height,density));
				gsl_matrix_set(frechet, 0, 1,  functionB->dX(height,density));
				gsl_matrix_set(frechet, 1, 1,  functionB->dY(height,density));

				// Data channel C
				gsl_vector_set(predicted, 2, functionC->calcFunction(height,density));
				gsl_matrix_set(frechet, 0, 2,  functionC->dX(height,density));
				gsl_matrix_set(frechet, 1, 2,  functionC->dY(height,density));

				// CALCULATE ERROR
				for(int i = 0; i < nData; i++)
				{
					predictMeasElement = gsl_vector_get(predicted, i) - gsl_vector_get(inSigma0dB, i); // Predicted - Measured data
					gsl_vector_set(dPredictMeas, i, predictMeasElement);
				}

				double prevError = error;
				error = 0.0;

				for(int i = 0; i < nData; i++)
				{
					double diff =  pow((gsl_vector_get(inSigma0dB, i) - gsl_vector_get(predicted, i)),2); // Predicted - Measured data
					error	= error + diff;
				}
				error = error / (pow(gsl_vector_get(inSigma0dB, 0),2)+pow(gsl_vector_get(inSigma0dB, 1),2)+pow(gsl_vector_get(inSigma0dB, 2),2));

				if((error < minError) | (fabs(prevError - error) < 10e-10) | boost::math::isnan(error))
				{
					for(int i = 0; i < nPar; i++)
					{
						gsl_vector_set(outParError, i, gsl_vector_get(estimatedPar, i));
					}
					gsl_vector_set(outParError, nPar, error);

					gsl_vector_free(estimatedPar);
					gsl_vector_free(predicted);
					gsl_matrix_free(frechet);
					gsl_matrix_free(frechetT);
					gsl_vector_free(dPredictMeas);
					gsl_vector_free(gamma);
					gsl_vector_free(aux1);
					gsl_vector_free(aux2);
					gsl_vector_free(aux3);

					if (error < minError)
					{
						return 1;
					}
					else
					{
						return 0;
					}

				}

				// FIND GRADIENT DIRECTION
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, dPredictMeas, aux1); // aux1 = invCovMatrixD . dPredictMeas
				matrixUtils.productMatrixVectorGSL(frechet, aux1, aux2); // aux2 = Frechet . aux1
				matrixUtils.productMatrixVectorGSL(covMatrixP, aux2, gamma); // gamma = covMatrixP . aux2

				for(int i = 0; i < nPar; i++)
				{
					gammaElement = gsl_vector_get(gamma, i);
					currPriorDiff = gsl_vector_get(estimatedPar, i) - gsl_vector_get(aPrioriPar, i); // X_i - X_ap
					gsl_vector_set(gamma, i, (gammaElement + currPriorDiff));
				}

				// FIND STEP LENGTH
				matrixUtils.transposeNonSquareGSL(frechet, frechetT); // Transpose of Frechet derivative operator
				matrixUtils.productMatrixVectorGSL(frechetT, gamma, aux1); // aux1 = frechetT . gamma
				matrixUtils.productMatrixVectorGSL(invCovMatrixD, aux1, aux3); // aux3 = invCovMatrixD . aux1
				s1 = vectorUtils.dotProductVectorVectorGSL(aux1, aux3); // s1 = aux1 . aux3
				matrixUtils.productMatrixVectorGSL(invCovMatrixP, gamma, aux2); // aux2 = invCovMatrixP . gamma
				s2 = vectorUtils.dotProductVectorVectorGSL(gamma, aux2); // s2 = gamma . aux2
				s1DivS2 = s1 / s2;
				alpha = 1 / (1 + s1DivS2); // 1 / (1 + (s1/s2))

				// UPDATE VALUES
				for(int i = 0; i < nPar; i++)
				{
					predictParElement = gsl_vector_get(estimatedPar, i);
					gammaElement = gsl_vector_get(gamma, i);
					predictParElement = predictParElement - (alpha * gammaElement);
					gsl_vector_set(estimatedPar, i, predictParElement);
					if (error < gsl_vector_get(outParError, nPar))
					{
						gsl_vector_set(outParError, i, predictParElement);
					}
				}

				if (error < gsl_vector_get(outParError, nPar))
				{
					gsl_vector_set(outParError, nPar, error);
				}
			}

			// FREE MATRICES
			gsl_vector_free(estimatedPar);
			gsl_vector_free(predicted);
			gsl_matrix_free(frechet);
			gsl_matrix_free(frechetT);
			gsl_vector_free(dPredictMeas);
			gsl_vector_free(gamma);
			gsl_vector_free(aux1);
			gsl_vector_free(aux2);
			gsl_vector_free(aux3);

			return 0;
		}
		RSGISEstimationConjugateGradient2Var3Data::~RSGISEstimationConjugateGradient2Var3Data()
		{
			gsl_matrix_free(this->invCovMatrixP);
			gsl_vector_free(this->aPrioriPar);
		}

		RSGISEstimationConjugateGradient2Var2DataWithRestarts::RSGISEstimationConjugateGradient2Var2DataWithRestarts(
																													 rsgis::math::RSGISMathTwoVariableFunction *functionA,
																													 rsgis::math::RSGISMathTwoVariableFunction *functionB,
																													 double *minMaxIntervalA,
																													 double *minMaxIntervalB,
																													 gsl_vector *aPrioriPar,
																													 gsl_matrix *covMatrixP,
																													 gsl_matrix *invCovMatrixD,
																													 double minError,
																													 int ittmax,
																													 int nRestarts
																													 )
		{
			rsgis::math::RSGISMatrices matrixUtils;
			this->functionA = functionA;
			this->functionB = functionB;
			this->minMaxIntervalA = minMaxIntervalA;
			this->minMaxIntervalB = minMaxIntervalB;
			this->aPrioriPar = aPrioriPar;
			this->covMatrixP = covMatrixP;
			this->invCovMatrixD = invCovMatrixD;
			this->ittmax = ittmax;
			this->nRestarts = nRestarts;
			this->minError = minError;

			// Invert covariance matrix
			this->invCovMatrixP = gsl_matrix_alloc(2,2);
			matrixUtils.inv2x2GSLMatrix(covMatrixP, this->invCovMatrixP);

			this->conjGradOpt = new RSGISEstimationConjugateGradient2Var2Data(functionA, functionB, aPrioriPar, covMatrixP, invCovMatrixD, minError, ittmax);

		}
		int RSGISEstimationConjugateGradient2Var2DataWithRestarts::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
		{
			gsl_vector *currentParError;
			gsl_vector *testInitialPar;

			currentParError = gsl_vector_alloc(3);
			testInitialPar = gsl_vector_alloc(2);

			gsl_vector_set(outParError, 0, gsl_vector_get(initialPar, 0));
			gsl_vector_set(outParError, 1, gsl_vector_get(initialPar, 1));
			gsl_vector_set(outParError, 2, +std::numeric_limits<double>::infinity()); // Set initial error to upper numerical limit
			// Try one run with inital parameters
			this->conjGradOpt->minimise(inData, initialPar, currentParError);

			if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) && (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]))
			{
				gsl_vector_memcpy(outParError, currentParError);
			}

			if (gsl_vector_get(outParError, 2) < minError)
			{
				// If initial parameters are less than target error only one
				// run is required.
				gsl_vector_free(currentParError);
				gsl_vector_free(testInitialPar);
				return 1;
			}

			double stepA = (minMaxIntervalA[1] - minMaxIntervalA[0]) / sqrt(float(nRestarts));
			double stepB =  (minMaxIntervalB[1] - minMaxIntervalB[0]) / sqrt(float(nRestarts));

			double a = minMaxIntervalA[0];
			while (a < minMaxIntervalA[1])
			{
				double b = minMaxIntervalB[0];
				while(b < minMaxIntervalB[1])
				{
					gsl_vector_set(testInitialPar, 0, a);
					gsl_vector_set(testInitialPar, 1, b);

					conjGradOpt->minimise(inData, testInitialPar, currentParError);

					if (gsl_vector_get(currentParError, 2) < gsl_vector_get(outParError, 2)) // If new values are better than current best, update
					{
						if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) && (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]))
						{
							gsl_vector_memcpy(outParError, currentParError);

							if (gsl_vector_get(outParError, 2) < minError)
							{
								// free memory and terminate if target error is reached.
								gsl_vector_free(currentParError);
								gsl_vector_free(testInitialPar);
								return 1;
							}
						}
					}
					b= b + stepB;
				}
				a = a + stepA;
			}

			// One last run with best estimate
			gsl_vector_set(testInitialPar, 0, gsl_vector_get(outParError, 0));
			gsl_vector_set(testInitialPar, 1, gsl_vector_get(outParError, 1));

			conjGradOpt->minimise(inData, testInitialPar, currentParError);

			if (gsl_vector_get(currentParError, 2) < gsl_vector_get(outParError, 2)) // If new values are better than current best, update
			{
				if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) && (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]))
				{
					gsl_vector_memcpy(outParError, currentParError);

					if (gsl_vector_get(outParError, 2) < minError)
					{
						// free memory and terminate if target error is reached.
						gsl_vector_free(currentParError);
						gsl_vector_free(testInitialPar);
						return 1;
					}
				}
			}

			gsl_vector_free(currentParError);
			gsl_vector_free(testInitialPar);

			return 0;
		}

		RSGISEstimationConjugateGradient2Var2DataWithRestarts::~RSGISEstimationConjugateGradient2Var2DataWithRestarts()
		{
			delete conjGradOpt;
			gsl_matrix_free(invCovMatrixP);
		}

		RSGISEstimationConjugateGradient2Var3DataWithRestarts::RSGISEstimationConjugateGradient2Var3DataWithRestarts(
																													 rsgis::math::RSGISMathTwoVariableFunction *functionA,
																													 rsgis::math::RSGISMathTwoVariableFunction *functionB,
																													 rsgis::math::RSGISMathTwoVariableFunction *functionC,
																													 double *minMaxIntervalA,
																													 double *minMaxIntervalB,
																													 gsl_vector *aPrioriPar,
																													 gsl_matrix *covMatrixP,
																													 gsl_matrix *invCovMatrixD,
																													 double minError,
																													 int ittmax,
																													 int nRestarts
																													 )
		{
			rsgis::math::RSGISMatrices matrixUtils;
			this->functionA = functionA;
			this->functionB = functionB;
			this->functionC = functionC;
			this->minMaxIntervalA = minMaxIntervalA;
			this->minMaxIntervalB = minMaxIntervalB;
			this->aPrioriPar = aPrioriPar;
			this->covMatrixP = covMatrixP;
			this->invCovMatrixD = invCovMatrixD;
			this->ittmax = ittmax;
			this->nRestarts = nRestarts;
			this->minError = minError;

			// Invert covariance matrix
			this->invCovMatrixP = gsl_matrix_alloc(2,2);
			matrixUtils.inv2x2GSLMatrix(covMatrixP, this->invCovMatrixP);
			this->conjGradOpt = new RSGISEstimationConjugateGradient2Var3Data(functionA, functionB, functionC, aPrioriPar, covMatrixP, invCovMatrixD, minError, ittmax);

		}
		int RSGISEstimationConjugateGradient2Var3DataWithRestarts::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
		{
			gsl_vector *currentParError;
			gsl_vector *testInitialPar;

			currentParError = gsl_vector_alloc(3);
			testInitialPar = gsl_vector_alloc(2);

			gsl_vector_set(outParError, 0, gsl_vector_get(initialPar, 0));
			gsl_vector_set(outParError, 1, gsl_vector_get(initialPar, 1));
			gsl_vector_set(outParError, 2, +std::numeric_limits<double>::infinity()); // Set initial error to upper numerical limit

			// Try one run with inital parameters
			this->conjGradOpt->minimise(inData, initialPar, currentParError);

			if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) &&
                (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]))
			{
				gsl_vector_memcpy(outParError, currentParError);
			}

			if (gsl_vector_get(outParError, 2) < minError)
			{
				// If initial parameters are less than target error only one
				// run is required.
				gsl_vector_free(currentParError);
				gsl_vector_free(testInitialPar);
				return 1;
			}

			double stepA = (minMaxIntervalA[1] - minMaxIntervalA[0]) / sqrt(float(nRestarts));
			double stepB =  (minMaxIntervalB[1] - minMaxIntervalB[0]) / sqrt(float(nRestarts));

			double a = minMaxIntervalA[0];
			while (a < minMaxIntervalA[1])
			{
				double b = minMaxIntervalB[0];
				while(b < minMaxIntervalB[1])
				{
					gsl_vector_set(testInitialPar, 0, a);
					gsl_vector_set(testInitialPar, 1, b);

					conjGradOpt->minimise(inData, testInitialPar, currentParError);

					if (gsl_vector_get(currentParError, 2) < gsl_vector_get(outParError, 2)) // If new values are better than current best, update
					{
						if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) &&
                            (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]))
						{
							gsl_vector_memcpy(outParError, currentParError);

							if (gsl_vector_get(outParError, 2) < minError)
							{
								// free memory and terminate if target error is reached.
								gsl_vector_free(currentParError);
								gsl_vector_free(testInitialPar);
								return 1;
							}
						}
					}
					b= b + stepB;
				}
				a = a + stepA;
			}

			// One last run with best estimate
			gsl_vector_set(testInitialPar, 0, gsl_vector_get(outParError, 0));
			gsl_vector_set(testInitialPar, 1, gsl_vector_get(outParError, 1));

			conjGradOpt->minimise(inData, testInitialPar, currentParError);

			if (gsl_vector_get(currentParError, 2) < gsl_vector_get(outParError, 2)) // If new values are better than current best, update
			{
				if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) &&
                    (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]))
				{
					gsl_vector_memcpy(outParError, currentParError);

					if (gsl_vector_get(outParError, 2) < minError)
					{
						// free memory and terminate if target error is reached.
						gsl_vector_free(currentParError);
						gsl_vector_free(testInitialPar);
						return 1;
					}
				}
			}

			gsl_vector_free(currentParError);
			gsl_vector_free(testInitialPar);

			return 0;

		}
		RSGISEstimationConjugateGradient2Var3DataWithRestarts::~RSGISEstimationConjugateGradient2Var3DataWithRestarts()
		{
			delete conjGradOpt;
			gsl_matrix_free(invCovMatrixP);
		}

		RSGISEstimationConjugateGradient3Var3DataWithRestarts::RSGISEstimationConjugateGradient3Var3DataWithRestarts(gsl_matrix *coeffHH, gsl_matrix *coeffHV, gsl_matrix *coeffVV,
																													 int orderX, int orderY, int orderZ,
																													 double *minMaxIntervalA,
																													 double *minMaxIntervalB,
																													 double *minMaxIntervalC,
																													 gsl_vector *aPrioriPar,
																													 gsl_matrix *covMatrixP,
																													 gsl_matrix *invCovMatrixD,
																													 double minError,
																													 int ittmax,
																													 int nRestarts
																													 )
		{
			rsgis::math::RSGISMatrices matrixUtils;
			this->minMaxIntervalA = minMaxIntervalA;
			this->minMaxIntervalB = minMaxIntervalB;
			this->minMaxIntervalC = minMaxIntervalC;
			this->aPrioriPar = aPrioriPar;
			this->covMatrixP = covMatrixP;
			this->invCovMatrixD = invCovMatrixD;
			this->ittmax = ittmax;
			this->nRestarts = nRestarts;
			this->minError = minError;

			this->conjGradOpt = new RSGISEstimationConjugateGradient3DPoly3Channel(coeffHH, coeffHV, coeffVV, orderX, orderY, orderZ, aPrioriPar, covMatrixP, invCovMatrixD, minError, ittmax);

		}
		int RSGISEstimationConjugateGradient3Var3DataWithRestarts::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
		{
			gsl_vector *currentParError;
			gsl_vector *testInitialPar;

			currentParError = gsl_vector_alloc(4);
			testInitialPar = gsl_vector_alloc(3);

			gsl_vector_set(outParError, 0, gsl_vector_get(initialPar, 0));
			gsl_vector_set(outParError, 1, gsl_vector_get(initialPar, 1));
			gsl_vector_set(outParError, 2, gsl_vector_get(initialPar, 2));
			gsl_vector_set(outParError, 3, +std::numeric_limits<double>::infinity()); // Set initial error to upper numerical limit

			double stepA = (minMaxIntervalA[1] - minMaxIntervalA[0]) / pow(nRestarts, 1.0/3.0);
			double stepB =  (minMaxIntervalB[1] - minMaxIntervalB[0]) / pow(nRestarts, 1.0/3.0);
			double stepC = (minMaxIntervalC[1] - minMaxIntervalC[0]) / pow(nRestarts, 1.0/3.0);

			// Try one run with initial parameters
			this->conjGradOpt->minimise(inData, initialPar, currentParError);

            // Check within limits of equation
            if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) &&
                (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]) &&
                (gsl_vector_get(currentParError, 2) > minMaxIntervalC[0]) && (gsl_vector_get(currentParError, 2) < minMaxIntervalC[1]))
            {
                gsl_vector_memcpy(outParError, currentParError);

                if (gsl_vector_get(outParError, 3) < minError)
                {
                    // free memory and terminate if target error is reached.
                    gsl_vector_free(currentParError);
                    gsl_vector_free(testInitialPar);
                    return 1;
                }
            }


			// LOOP THROUGH DIFFERNT STARTING VALUES
			double a = minMaxIntervalA[0];
			while (a < minMaxIntervalA[1])
			{
			    double b = minMaxIntervalB[0];
				while(b < minMaxIntervalB[1])
				{
					double c = minMaxIntervalC[0];
					while (c < minMaxIntervalC[1])
					{

                        gsl_vector_set(testInitialPar, 0, a);
						gsl_vector_set(testInitialPar, 1, b);
						gsl_vector_set(testInitialPar, 2, c);

						conjGradOpt->minimise(inData, testInitialPar, currentParError);

						if (gsl_vector_get(currentParError, 3) < gsl_vector_get(outParError, 3)) // Check if resuduals are lower
						{
							// Check within limits of equation
							if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) &&
                                (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]) &&
                                (gsl_vector_get(currentParError, 2) > minMaxIntervalC[0]) && (gsl_vector_get(currentParError, 2) < minMaxIntervalC[1]))
							{
                                // If new values are better than current best, update
								gsl_vector_memcpy(outParError, currentParError);

								if (gsl_vector_get(outParError, 3) < minError)
								{
									// free memory and terminate if target error is reached.
									gsl_vector_free(currentParError);
									gsl_vector_free(testInitialPar);
									return 1;
								}
							}
						}
						c = c + stepC;
					}
					b = b + stepB;
				}
				a = a + stepA;
			}

			// One last run with best estimate
			gsl_vector_set(testInitialPar, 0, gsl_vector_get(outParError, 0));
			gsl_vector_set(testInitialPar, 1, gsl_vector_get(outParError, 1));
			gsl_vector_set(testInitialPar, 2, gsl_vector_get(outParError, 2));

			conjGradOpt->minimise(inData, testInitialPar, currentParError);

			if (gsl_vector_get(currentParError, 3) < gsl_vector_get(outParError, 3)) // If new values are better than current best, update
			{
				if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) &&
                    (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]) &&
                    (gsl_vector_get(currentParError, 2) > minMaxIntervalC[0]) && (gsl_vector_get(currentParError, 2) < minMaxIntervalC[1]))
				{
					gsl_vector_memcpy(outParError, currentParError);

					if (gsl_vector_get(outParError, 3) < minError)
					{
						// free memory and terminate if target error is reached.
						gsl_vector_free(currentParError);
						gsl_vector_free(testInitialPar);
						return 1;
					}
				}
			}

			gsl_vector_free(currentParError);
			gsl_vector_free(testInitialPar);

			return 0;
		}
		RSGISEstimationConjugateGradient3Var3DataWithRestarts::~RSGISEstimationConjugateGradient3Var3DataWithRestarts()
		{
			delete conjGradOpt;
		}

		RSGISEstimationConjugateGradient3Var4DataWithRestarts::RSGISEstimationConjugateGradient3Var4DataWithRestarts(gsl_matrix *coeffA, gsl_matrix *coeffB, gsl_matrix *coeffC, gsl_matrix *coeffD,
																													 int orderX, int orderY, int orderZ,
																													 double *minMaxIntervalA,
																													 double *minMaxIntervalB,
																													 double *minMaxIntervalC,
																													 gsl_vector *aPrioriPar,
																													 gsl_matrix *covMatrixP,
																													 gsl_matrix *invCovMatrixD,
																													 double minError,
																													 int ittmax,
																													 int nRestarts
																													 )
		{
			rsgis::math::RSGISMatrices matrixUtils;
			this->minMaxIntervalA = minMaxIntervalA;
			this->minMaxIntervalB = minMaxIntervalB;
			this->minMaxIntervalC = minMaxIntervalC;
			this->aPrioriPar = aPrioriPar;
			this->covMatrixP = covMatrixP;
			this->invCovMatrixD = invCovMatrixD;
			this->ittmax = ittmax;
			this->nRestarts = nRestarts;
			this->minError = minError;

			this->conjGradOpt = new RSGISEstimationConjugateGradient3DPoly4Channel(coeffA, coeffB, coeffC, coeffD, orderX, orderY, orderZ, aPrioriPar, covMatrixP, invCovMatrixD, minError, ittmax);

		}
		int RSGISEstimationConjugateGradient3Var4DataWithRestarts::minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError)
		{
			gsl_vector *currentParError;
			gsl_vector *testInitialPar;

			currentParError = gsl_vector_alloc(4);
			testInitialPar = gsl_vector_alloc(3);

			gsl_vector_set(outParError, 0, gsl_vector_get(initialPar, 0));
			gsl_vector_set(outParError, 1, gsl_vector_get(initialPar, 1));
			gsl_vector_set(outParError, 2, gsl_vector_get(initialPar, 2));
			gsl_vector_set(outParError, 3, +std::numeric_limits<double>::infinity()); // Set initial error to upper numerical limit

			double stepA = (minMaxIntervalA[1] - minMaxIntervalA[0]) / pow(nRestarts, 1.0/3.0);
			double stepB =  (minMaxIntervalB[1] - minMaxIntervalB[0]) / pow(nRestarts, 1.0/3.0);
			double stepC = (minMaxIntervalC[1] - minMaxIntervalC[0]) / pow(nRestarts, 1.0/3.0);

			// Try one run with inital parameters
			this->conjGradOpt->minimise(inData, initialPar, currentParError);

            // Check within limits of equation
            if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) &&
                (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]) &&
                (gsl_vector_get(currentParError, 2) > minMaxIntervalC[0]) && (gsl_vector_get(currentParError, 2) < minMaxIntervalC[1]))
            {
                // If new values are better than current best, update
                gsl_vector_memcpy(outParError, currentParError);

                if (gsl_vector_get(outParError, 3) < minError)
                {
                    // free memory and terminate if target error is reached.
                    gsl_vector_free(currentParError);
                    gsl_vector_free(testInitialPar);
                    return 1;
                }
            }

			// LOOP THROUGH DIFFERNT STARTING VALUES
			double a = minMaxIntervalA[0];
			while (a < minMaxIntervalA[1])
			{
			    double b = minMaxIntervalB[0];
				while(b < minMaxIntervalB[1])
				{
					double c = minMaxIntervalC[0];
					while (c < minMaxIntervalC[1])
					{
						gsl_vector_set(testInitialPar, 0, a);
						gsl_vector_set(testInitialPar, 1, b);
						gsl_vector_set(testInitialPar, 2, c);

						conjGradOpt->minimise(inData, testInitialPar, currentParError);

						if (gsl_vector_get(currentParError, 3) < gsl_vector_get(outParError, 3)) // Check if resuduals are lower
						{
							// Check within limits of equation
							if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) &&
                                (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]) &&
                                (gsl_vector_get(currentParError, 2) > minMaxIntervalC[0]) && (gsl_vector_get(currentParError, 2) < minMaxIntervalC[1]))
							{
								// If new values are better than current best, update
								gsl_vector_memcpy(outParError, currentParError);

								if (gsl_vector_get(outParError, 3) < minError)
								{
									// free memory and terminate if target error is reached.
									gsl_vector_free(currentParError);
									gsl_vector_free(testInitialPar);
									return 1;
								}
							}
						}
						c = c + stepC;
					}
					b = b + stepB;
				}
				a = a + stepA;
			}

			// One last run with best estimate
			gsl_vector_set(testInitialPar, 0, gsl_vector_get(outParError, 0));
			gsl_vector_set(testInitialPar, 1, gsl_vector_get(outParError, 1));
			gsl_vector_set(testInitialPar, 2, gsl_vector_get(outParError, 2));

			conjGradOpt->minimise(inData, testInitialPar, currentParError);

			if (gsl_vector_get(currentParError, 2) < gsl_vector_get(outParError, 2)) // If new values are better than current best, update
			{
				if ((gsl_vector_get(currentParError, 0) > minMaxIntervalA[0]) && (gsl_vector_get(currentParError, 0) < minMaxIntervalA[1]) &&
                    (gsl_vector_get(currentParError, 1) > minMaxIntervalB[0]) && (gsl_vector_get(currentParError, 1) < minMaxIntervalB[1]) &&
                    (gsl_vector_get(currentParError, 2) > minMaxIntervalC[0]) && (gsl_vector_get(currentParError, 2) < minMaxIntervalC[1]))
				{
					gsl_vector_memcpy(outParError, currentParError);

					if (gsl_vector_get(outParError, 2) < minError)
					{
						// free memory and terminate if target error is reached.
						gsl_vector_free(currentParError);
						gsl_vector_free(testInitialPar);
						return 1;
					}
				}
			}

			gsl_vector_free(currentParError);
			gsl_vector_free(testInitialPar);

			return 0;
		}
		RSGISEstimationConjugateGradient3Var4DataWithRestarts::~RSGISEstimationConjugateGradient3Var4DataWithRestarts()
		{
			delete conjGradOpt;
		}

	}}

