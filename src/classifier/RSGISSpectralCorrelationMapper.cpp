/*
 *  RSGISSpectralCorrelationMapper.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 14/02/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISSpectralCorrelationMapper.h"

namespace rsgis { namespace classifier {
	
	
	RSGISSpectralCorrelationMapperRule::RSGISSpectralCorrelationMapperRule(int numOutBands, gsl_matrix *refSpectra) : RSGISCalcImageValue(numOutBands)
	{
		this->refSpectra = refSpectra;
		std::cout << "Number of Refference Spectra = " << refSpectra->size2 << std::endl;
		this->imageSpecArray = new double[refSpectra->size1];
	}
	void RSGISSpectralCorrelationMapperRule::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		
		// Load pixel into memory - saves time on disk reads when comparing to a large number of refference spectra.
		for(unsigned int b = 0; b < refSpectra->size1; b++)
		{
			this->imageSpecArray[b] = bandValues[b];
		}
		
		// Loop through output spectra
		for(unsigned int i = 0; i < refSpectra->size2; i++)
		{
			
			// Calculate Pearsonian Correlation Coefficient for each spectra in reffernce library
			double sumX = 0;
			double sumY = 0;
			
			// Calc mean
			for(unsigned int b = 0;b < refSpectra->size1; b++)
			{
				sumX = sumX + imageSpecArray[b];
				sumY = sumY + gsl_matrix_get(refSpectra, b, i);
			}
			
			double xMean = sumX / refSpectra->size1;
			double yMean = sumY / refSpectra->size1;
			
			double ssXX = 0;
			double ssYY = 0;
			double ssXY = 0;
			
			for(unsigned int b = 0; b < refSpectra->size1; b++)
			{
				
				double dataX = imageSpecArray[b];
				double dataY = gsl_matrix_get(refSpectra, b, i);
				
				ssXY = ssXY + ((dataX - xMean)*(dataY - yMean));
				ssXX = ssXX + ((dataX - xMean)*(dataX - xMean));
				ssYY = ssYY + ((dataY - yMean)*(dataY - yMean));
			}
			
			double pCC = ssXY / sqrt(ssXX * ssYY);
			double scm = sqrt(pCC * pCC);
			
			output[i] = scm;
		}
	}
	RSGISSpectralCorrelationMapperRule::~RSGISSpectralCorrelationMapperRule()
	{
		delete imageSpecArray;
	}
	
	RSGISSpectralCorrelationMapperClassifier::RSGISSpectralCorrelationMapperClassifier(int numOutBands, double threashold) : RSGISCalcImageValue(numOutBands)
	{
		this->threashold = threashold;
	}
	void RSGISSpectralCorrelationMapperClassifier::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{										
		double maxCorrelation = 0;
		double maxCorrelationTemp = 0;
		int outClass = 0;
		
		for(int i = 0; i < numBands; i++)
		{
			maxCorrelationTemp = bandValues[i];
			if(maxCorrelationTemp > maxCorrelation)
			{
				maxCorrelation = maxCorrelationTemp;
				outClass = i + 1;
			}
		}
		
		if(maxCorrelation > this->threashold)
		{
			output[0] = float(outClass);
		}
		else 
		{
			output[0] = 0;
		}
		
	}
	RSGISSpectralCorrelationMapperClassifier::~RSGISSpectralCorrelationMapperClassifier()
	{
		
	}
	
}}
