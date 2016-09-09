/*
 *  RSGISPrincipalComponentAnalysis.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/08/2008.
 *  Copyright 2008 RSGISLib.
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

#include "RSGISPrincipalComponentAnalysis.h"

namespace rsgis{namespace math{

	RSGISPrincipalComponentAnalysis::RSGISPrincipalComponentAnalysis(Matrix *inputData) throw(RSGISMatricesException,RSGISMultivariantStatsException)
	{
		this->inputData = inputData;
		this->calcIntermediateSteps();
	}
	
	Matrix* RSGISPrincipalComponentAnalysis::getMeanVector()
	{
		return this->meanVec;
	}
	
	Matrix* RSGISPrincipalComponentAnalysis::getStandardisedData()
	{
		return this->stdInputData;
	}
	
	Matrix* RSGISPrincipalComponentAnalysis::getCovarianceMatrix()
	{
		return this->covariance;
	}
	
	Matrix* RSGISPrincipalComponentAnalysis::getEigenvalues()
	{
		return this->eigenvalues;
	}
	
	Matrix* RSGISPrincipalComponentAnalysis::getEigenvectors()
	{
		return this->eigenvectors;
	}
	
	Matrix* RSGISPrincipalComponentAnalysis::getComponent(int component)
	{
		/*RSGISMatrices matrixUtils;
		Matrix *compMatrix = matrixUtils.createMatrix(inputData->m, 1);
		
		int index = 0;
		for(int i = 0; i < inputData->m; i++)
		{
			index = (i * inputData->n) + component;
		}*/
		std::cout << "Matrix* RSGISPrincipalComponentAnalysis::getComponent(int component) NOT IMPLEMENTED!\n";
		return NULL;
	}
	
	Matrix* RSGISPrincipalComponentAnalysis::getComponents(int num)
	{
		std::cout << "Matrix* RSGISPrincipalComponentAnalysis::getComponents(int num) NOT IMPLEMENTED!\n";
		return NULL;
	}
	
	Matrix* RSGISPrincipalComponentAnalysis::getAllComponents()
	{
		RSGISMatrices matrixUtils;
		Matrix *pcaMatrix = matrixUtils.createMatrix(inputData->n, inputData->m);
		
		int outputIndex = 0;
		int inputIndex = 0;
		int eigenIndex = 0;
		double newValue = 0;
		double value = 0;
		
		for(int i = 0; i < eigenvectors->n; i++)
		{
			for(int j = 0; j < inputData->m; j++)
			{
				newValue = 0;
				for(int k = 0; k < inputData->n; k++)
				{
					inputIndex = (j * inputData->n) + k;
					eigenIndex = (k * eigenvectors->n) + i;
					value = stdInputData->matrix[inputIndex] * eigenvectors->matrix[eigenIndex];
					newValue += value;
				}
				outputIndex = (j * inputData->n) + i;
				pcaMatrix->matrix[outputIndex] = newValue;
			}
		}
		
		return pcaMatrix;
	}
			
	RSGISPrincipalComponentAnalysis::~RSGISPrincipalComponentAnalysis()
	{
		RSGISMatrices matrixUtils;
		matrixUtils.freeMatrix(this->meanVec);
		matrixUtils.freeMatrix(this->stdInputData);
		matrixUtils.freeMatrix(this->stdMeanVec);
		matrixUtils.freeMatrix(this->covariance);
		matrixUtils.freeMatrix(this->eigenvalues);
		matrixUtils.freeMatrix(this->eigenvectors);
	}
	
	void RSGISPrincipalComponentAnalysis::calcIntermediateSteps() throw(RSGISMatricesException,RSGISMultivariantStatsException)
	{
		RSGISMultivariantStats mvarStats;
		RSGISMatrices matrixUtils;
		
		//matrixUtils.printMatrix(this->inputData);
		
		//cout << "Mean Vector:\n";
		this->meanVec = mvarStats.findMeanVector(inputData);
		//matrixUtils.printMatrix(meanVec);
		
		//cout << "Standardised Input Data:\n";
		this->stdInputData = mvarStats.standardiseMatrix(inputData, meanVec);
		//matrixUtils.printMatrix(stdInputData);
		
		//cout << "Standardised Mean Vector:\n";
		this->stdMeanVec = mvarStats.findMeanVector(stdInputData);
		//matrixUtils.printMatrix(stdInputData);
		
		//cout << "Covariance Matrix: \n";
		this->covariance = mvarStats.calcCovarianceMatrix(stdInputData, stdMeanVec);
		//matrixUtils.printMatrix(covariance);
		
		//cout << "EigenValues: \n";
		this->eigenvalues = matrixUtils.createMatrix(1, covariance->m);
		
		//cout << "EigenVectors: \n";
		this->eigenvectors = matrixUtils.createMatrix(covariance->n, covariance->m);
		matrixUtils.calcEigenVectorValue(covariance, eigenvalues, eigenvectors);
		
		//matrixUtils.printMatrix(eigenvalues);
		
		//matrixUtils.printMatrix(eigenvectors);
	}
}}

