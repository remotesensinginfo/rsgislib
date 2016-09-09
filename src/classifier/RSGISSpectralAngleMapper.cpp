/*
 *  RSGISSpectralAngleMapper.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 08/01/2010.
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

#include "RSGISSpectralAngleMapper.h"

namespace rsgis { namespace classifier {
	

	RSGISSpectralAngleMapperRule::RSGISSpectralAngleMapperRule(int numOutBands, gsl_matrix *refSpectra) : RSGISCalcImageValue(numOutBands)
	{
		this->refSpectra = refSpectra;
		std::cout << "Number of Refference Spectra = " << refSpectra->size2 << std::endl;
		this->imageSpecArray = new double[refSpectra->size1];
	}
	void RSGISSpectralAngleMapperRule::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{

		// Load pixel into memory - saves time on disk reads when comparing to a large number of refference spectra.
		for(unsigned int b = 0; b < refSpectra->size1; b++)
		{
			this->imageSpecArray[b] = bandValues[b];
		}
		
		// Loop through output spectra
		for(unsigned int i = 0; i < refSpectra->size2; i++)
		{
			
			double sumImageRef = 0;
			double sumImageImage = 0;
			double sumRefRef = 0;
			
			// Calculate angle for each spectra in reffernce library
			for(unsigned int b = 0; b < refSpectra->size1; b++)
			{
				double image = imageSpecArray[b];
				double ref = gsl_matrix_get(refSpectra, b, i);
				
				sumImageRef = sumImageRef + (image * ref);
				sumImageImage = sumImageImage + (image * image);
				sumRefRef = sumRefRef + (ref * ref);
			}
			
			double demononator = sqrt(sumRefRef) * sqrt(sumImageImage);
			double angle = acos(sumImageRef / demononator);
			
			output[i] = angle;
		}
	}
	RSGISSpectralAngleMapperRule::~RSGISSpectralAngleMapperRule()
	{
		delete imageSpecArray;
	}
	
	RSGISSpectralAngleMapperED::RSGISSpectralAngleMapperED(int numOutBands, gsl_matrix *refSpectra) : RSGISCalcImageValue(numOutBands)
	{
		this->refSpectra = refSpectra;
		std::cout << "Number of Refference Spectra = " << refSpectra->size2 << std::endl;
	}
	void RSGISSpectralAngleMapperED::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{
		
		// Loop through output spectra
		for(unsigned int i = 0; i < refSpectra->size2; i++)
		{
			
			double sumImageRef = 0;
			double sumImageImage = 0;
			double sumRefRef = 0;
			
			for(unsigned int b = 0; b < refSpectra->size1; b++)
			{
				double image = bandValues[b];
				double ref = gsl_matrix_get(refSpectra, b, i);
				
				sumImageRef = sumImageRef + (image * ref);
				sumImageImage = sumImageImage + (image * image);
				sumRefRef = sumRefRef + (ref * ref);
			}
			
			double demononator = sqrt(sumRefRef) * sqrt(sumImageImage);
			double angle = acos(sumImageRef / demononator);
			
			double euclidianDistance = 2 * sin(angle / 2);
			
			output[i] = euclidianDistance;
		}
	}
	RSGISSpectralAngleMapperED::~RSGISSpectralAngleMapperED()
	{
		
	}
	
	RSGISSpectralAngleMapperClassifier::RSGISSpectralAngleMapperClassifier(int numOutBands, double threashold) : RSGISCalcImageValue(numOutBands)
	{
		this->threashold = threashold;
	}
	void RSGISSpectralAngleMapperClassifier::calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException)
	{										
		double minAngleTemp = 100;
		double minAngle = 100;
		int outClass = 0;
		
		for(int i = 0; i < numBands; i++)
		{
			minAngleTemp = bandValues[i];
			if(minAngleTemp < minAngle)
			{
				minAngle = minAngleTemp;
				outClass = i + 1;
			}
		}
		
		if(minAngle < this->threashold)
		{
			output[0] = float(outClass);
		}
		else 
		{
			output[0] = 0;
		}

	}
	RSGISSpectralAngleMapperClassifier::~RSGISSpectralAngleMapperClassifier()
	{
		
	}
				
}}
