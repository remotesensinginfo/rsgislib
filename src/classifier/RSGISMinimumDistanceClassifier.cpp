/*
 *  RSGISMinimumDistanceClassifier.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/08/2008.
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

#include "RSGISMinimumDistanceClassifier.h"

namespace rsgis{ namespace classifier{
	
	RSGISMinimumDistanceClassifier::RSGISMinimumDistanceClassifier(ClassData **trainingData, int numClasses, MinDistCentreType centreType)  throw(RSGISClassificationException) : RSGISClassifier(trainingData, numClasses)
	{
		this->centreType = centreType;
		this->calcClusterCentres();
	}
	
	int RSGISMinimumDistanceClassifier::getClassID(float *variables, int numVars) throw(RSGISClassificationException)
	{
		return this->findClass(variables, numVars)->classID;
	}
	
	std::string RSGISMinimumDistanceClassifier::getClassName(float *variables, int numVars) throw(RSGISClassificationException)
	{
		return this->findClass(variables, numVars)->className;
	}
	
	void RSGISMinimumDistanceClassifier::calcClusterCentres() throw(RSGISClassificationException)
	{
        rsgis::math::RSGISMatrices matrixUtils;
		clusterCentres = new ClassData[numClasses];
		
		if(centreType == mean)
		{
			double sum = 0;
			int matrixIndex = 0;
			for(int i = 0; i < numClasses; i++)
			{
				clusterCentres[i].classID = trainingData[i]->classID;
				clusterCentres[i].className = trainingData[i]->className;
				clusterCentres[i].data = matrixUtils.createMatrix(trainingData[i]->data->n, 1);
				
				for(int j = 0; j < trainingData[i]->data->n; j++)
				{
					sum = 0;
					matrixIndex = j;
					for(int k = 0; k < trainingData[i]->data->m; k++)
					{
						sum += trainingData[i]->data->matrix[matrixIndex];
						matrixIndex += trainingData[i]->data->n;
					}
					clusterCentres[i].data->matrix[j] = sum/clusterCentres[i].data->n;
				}
			}
		}
		else if(centreType == middle)
		{
			int matrixIndex = 0;
			double max = 0;
			double min = 0;
			for(int i = 0; i < numClasses; i++)
			{
				clusterCentres[i].classID = trainingData[i]->classID;
				clusterCentres[i].className = trainingData[i]->className;
				clusterCentres[i].data = matrixUtils.createMatrix(trainingData[i]->data->n, 1);
				for(int j = 0; j < trainingData[i]->data->n; j++)
				{
					matrixIndex = j;
					max = 0;
					min = 0;
					for(int k = 0; k < trainingData[i]->data->m; k++)
					{
						if(k == 0)
						{
							min = trainingData[i]->data->matrix[matrixIndex];
							max = trainingData[i]->data->matrix[matrixIndex];
						}
						else
						{
							if(trainingData[i]->data->matrix[matrixIndex] < min)
							{
								min = trainingData[i]->data->matrix[matrixIndex];
							}
							else if(trainingData[i]->data->matrix[matrixIndex] > max)
							{
								max = trainingData[i]->data->matrix[matrixIndex];
							}
						}
						matrixIndex += trainingData[i]->data->n;
					}
					clusterCentres[i].data->matrix[j] = (max - min)/2;
				}
			}
		}
		else
		{
			throw RSGISClassificationException("Centre type is not defined");
		}
		
		std::cout << "CLUSTER CENTRES:\n";
		for(int i = 0; i < numClasses; i++)
		{
			std::cout << "Cluster " << clusterCentres[i].className << ":\t";
			matrixUtils.printMatrix(clusterCentres[i].data);
		}
		
		
	}
	
	ClassData* RSGISMinimumDistanceClassifier::findClass(float *variables, int numVars) throw(RSGISClassificationException)
	{
		double distance = 0;
		double minDistance = 0;
		double sqSum = 0;
		double sumPair = 0;
		ClassData *minDistClass = NULL;
		
		for(int i = 0; i < numClasses; i++)
		{
			sqSum = 0;
			for(int j = 0; j < numVars; j++)
			{
				sumPair = clusterCentres->data->matrix[i] - variables[j]; 
				sqSum += (sumPair*sumPair);
			}
			distance = sqrt(sqSum);
			
			if(i == 0)
			{
				minDistance = distance;
				minDistClass = &clusterCentres[i];
			}
			else
			{
				if(distance < minDistance)
				{
					minDistance = distance;
					minDistClass = &clusterCentres[i];
				}
			}
		}
		return minDistClass;
	}
	
	RSGISMinimumDistanceClassifier::~RSGISMinimumDistanceClassifier()
	{
		
	}
}}

