/*
 *  RSGISNearestNeighbourClassifier.cpp
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

#include "RSGISNearestNeighbourClassifier.h"

namespace rsgis{ namespace classifier{

	RSGISNearestNeighbourClassifier::RSGISNearestNeighbourClassifier(ClassData **trainingData, int numClasses) throw(RSGISClassificationException) : RSGISClassifier(trainingData, numClasses)
	{

	}
	
	int RSGISNearestNeighbourClassifier::getClassID(float *variables, int numVars) throw(RSGISClassificationException)
	{
		//cout << "int RSGISNearestNeighbourClassifier::getClassID(float *variables, int numVars) throw(RSGISClassificationException)\n";
		ClassData *classToReturn = this->findClass(variables, numVars);
		//cout << "returning class " << classToReturn->className << endl;
		return classToReturn->classID;
	}
	
	std::string RSGISNearestNeighbourClassifier::getClassName(float *variables, int numVars) throw(RSGISClassificationException)
	{
		ClassData *classToReturn = this->findClass(variables, numVars);
		return classToReturn->className;
	}
	
	ClassData* RSGISNearestNeighbourClassifier::findClass(float *variables, int numVars) throw(RSGISClassificationException)
	{
		//cout << "ClassData* RSGISNearestNeighbourClassifier::findClass(float *variables, int numVars) throw(RSGISClassificationException)\n";
		double distance = 0;
		double minDistance = 0;
		ClassData *minDistData = NULL;
		for(int i = 0; i < this->numClasses; i++)
		{
			//cout << " findClass minDistance = " << minDistance << endl;
			if(i == 0)
			{
				minDistance = this->findClosestPointInClass(trainingData[i], variables, numVars);
				minDistData = trainingData[i];
			}
			else
			{
				distance = this->findClosestPointInClass(trainingData[i], variables, numVars);
				if(distance < minDistance)
				{
					minDistance = distance;
					minDistData = trainingData[i];
				}
			}
		}
		return minDistData;
	}
	
	double RSGISNearestNeighbourClassifier::findClosestPointInClass(ClassData *data, float *variables, int numVars) throw(RSGISClassificationException)
	{
		//cout << "double RSGISNearestNeighbourClassifier::findClosestPointInClass(ClassData *data, float *variables, int numVars) throw(RSGISClassificationException)\n";
		double minDistance = 0;
		double distance = 0;
		int matrixIndex = 0;
		double sqSum = 0;
		double sumPair = 0;
		for(int i = 0; i < data->data->m; i++)
		{
			sqSum = 0;
			for(int j = 0; j < numVars; j++)
			{
				sumPair = data->data->matrix[matrixIndex] - variables[j]; 
				sqSum += (sumPair*sumPair);
				matrixIndex++;
			}
			distance = sqrt(sqSum);
			
			if(i == 0)
			{
				minDistance = distance;
			}
			else
			{
				if(distance < minDistance)
				{
					minDistance = distance;
				}
			}
		}
		return minDistance;
	}
	
	RSGISNearestNeighbourClassifier::~RSGISNearestNeighbourClassifier()
	{
		
	}
}}
