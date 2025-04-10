/*
 *  RSGISClassifier.cpp
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

#include "RSGISClassifier.h"

namespace rsgis{ namespace classifier{
	
	RSGISClassifier::RSGISClassifier(ClassData **trainingData, int numClasses)
	{
		this->trainingData = trainingData;
		this->numClasses = numClasses;
		this->numVariables = trainingData[0]->data->n;
	}
	
	int RSGISClassifier::getNumVariables()
	{
		return this->numVariables;
	}
	
	void RSGISClassifier::printClassIDs()
	{
		for(int i = 0; i < numClasses; i++)
		{
            std::cout << "Class " << trainingData[i]->className << " has ID " << trainingData[i]->classID << std::endl;
		}
	}
	
	RSGISClassifier::~RSGISClassifier()
	{
		
	}
	
	RSGISApplyClassifier::RSGISApplyClassifier(int numberOutBands, RSGISClassifier *classifier)  : rsgis::img::RSGISCalcImageValue(numberOutBands)
	{
		this->classifier = classifier;
	}
	
	void RSGISApplyClassifier::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		if(numBands != classifier->getNumVariables())
		{
			rsgis::img::RSGISImageCalcException("The number of input variables is not equal to the number of training variables.");
		}
		try
		{
			output[0] = classifier->getClassID(bandValues, numBands);
		}
		catch(RSGISClassificationException &e)
		{
			output[0] = -1;
		}
	}
	
	RSGISApplyClassifier::~RSGISApplyClassifier()
	{
		
	}
	
}}


