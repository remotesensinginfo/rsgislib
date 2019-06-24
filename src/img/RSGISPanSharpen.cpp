/*
 *  RSGISPanSharpen.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 31/10/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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

#include "RSGISPanSharpen.h"


namespace rsgis { namespace img {


	
	RSGISHCSPanSharpen::RSGISHCSPanSharpen(int numberOutBands, float *imageStats) : RSGISCalcImageValue(numberOutBands)
	{
		this->numberOutBands = numberOutBands;
		this->imageStats = imageStats;
	}
	
	void RSGISHCSPanSharpen::calcImageValue(float *bandValues, int numBands, double *output) 
	{
		float meanMS = this->imageStats[0];
		float meanPAN = this->imageStats[1];
		float sdMS = this->imageStats[2];
		float sdPAN = this->imageStats[3];
		
		float pan = bandValues[this->numberOutBands];
		float pSq = ((sdMS / sdPAN) * ((pan*pan) - meanPAN + sdPAN)) + (meanMS - sdMS);
		float iAdj = sqrt(pSq);
		if(pSq < 0)
		{
			iAdj = 0;
		}
		
		float *phi = new float[this->numberOutBands - 1];
		
		// CALCULATE FORWARD TRANSFORM
		for(unsigned int i = 0; i < this->numberOutBands - 1; ++i)
		{
			float sumMSSq = 0;
			
			for(unsigned int j = i; j < this->numberOutBands; ++j) // Loop through band values
			{
				sumMSSq = sumMSSq + pow(bandValues[j],2);
			}
			
			phi[i] = atan(sqrt(sumMSSq) / bandValues[i]);
		}
		
		// APPLY REVERSE TRANSFORM
		float prodPhi = 1;
		for(unsigned int i = 0; i < (this->numberOutBands - 1); ++i)
		{
			prodPhi = 1;
			for(unsigned int j = 0; j < i; ++j) // Loop through band values
			{
				prodPhi = prodPhi*sin(phi[j]);
			}
			prodPhi = prodPhi*cos(phi[i]);
			
			output[i] = iAdj * prodPhi;
		}
		
		// Last band
		prodPhi = 1;
		for(unsigned int j = 0; j < (this->numberOutBands - 1); ++j) // Loop through band values
		{
			prodPhi = prodPhi*sin(phi[j]);
		}
		
		output[this->numberOutBands - 1] = iAdj * prodPhi;
		
		delete[] phi;
	}
	
	
	void RSGISHCSPanSharpen::calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) 
	{
		
		//dataBlock[k][i][j]; k = band; j = y axis; i = x axis
		
		float meanMS = this->imageStats[0];
		float meanPAN = this->imageStats[1];
		float sdMS = this->imageStats[2];
		float sdPAN = this->imageStats[3];
		
		// Centre pixel
		unsigned int cPix = int(winSize / 2.);
		
		float panSmoothSum = 0;
		for(int i = 0; i < winSize; ++i)
		{
			for(int j = 0; j < winSize; ++j)
			{
				panSmoothSum = panSmoothSum + dataBlock[this->numberOutBands][j][i];
			}
		}
		
		float panSmooth = panSmoothSum / float(winSize * winSize);
		
		float iSq = 0;
		
		for(unsigned int i = 0; i < (this->numberOutBands - 1); ++i)
		{
			iSq = iSq + pow(dataBlock[i][cPix][cPix],2);
			
		}
		
		float pan = dataBlock[this->numberOutBands][cPix][cPix];
		float pSq = ((sdMS / sdPAN) * ((pan*pan) - meanPAN + sdPAN)) + (meanMS - sdMS);
		float pSqSmooth = ((sdMS / sdPAN) * ((panSmooth*panSmooth) - meanPAN + sdPAN)) + (meanMS - sdMS);
		
		
		// Calculate iAdj
		float iAdj = sqrt((pSq / pSqSmooth) * iSq);
		
		float *phi = new float[this->numberOutBands - 1];
		
		// CALCULATE FORWARD TRANSFORM
		for(unsigned int i = 0; i < this->numberOutBands - 1; ++i)
		{
			float sumMSSq = 0;
			
			for(unsigned int j = i; j < this->numberOutBands; ++j) // Loop through band values
			{
				sumMSSq = sumMSSq + pow(dataBlock[j][cPix][cPix],2);
			}
			
			phi[i] = atan(sqrt(sumMSSq) / dataBlock[i][cPix][cPix]);
		}
		
		// APPLY REVERSE TRANSFORM
		float prodPhi = 1;
		for(unsigned int i = 0; i < (this->numberOutBands - 1); ++i)
		{
			prodPhi = 1;
			for(unsigned int j = 0; j < i; ++j) // Loop through band values
			{
				prodPhi = prodPhi*sin(phi[j]);
			}
			prodPhi = prodPhi*cos(phi[i]);
			
			output[i] = iAdj * prodPhi;
		}
		
		// Last band
		prodPhi = 1;
		for(unsigned int j = 0; j < (this->numberOutBands - 1); ++j) // Loop through band values
		{
			prodPhi = prodPhi*sin(phi[j]);
		}
		
		output[this->numberOutBands - 1] = iAdj * prodPhi;
		
		delete[] phi;
		
	}
	
	RSGISHCSPanSharpenCalcMeanStats::RSGISHCSPanSharpenCalcMeanStats(int numberOutBands, float *outStats) : RSGISCalcImageValue(numberOutBands)
	{
		this->numberOutBands = numberOutBands;
		this->outStats = outStats;
		this->sumMS = 0.;
		this->sumPAN = 0.;
		this->nPix = 0;
	}
	
	void RSGISHCSPanSharpenCalcMeanStats::calcImageValue(float *bandValues, int numBands) 
	{
		
		if(bandValues[0] > 0)
		{
			this->nPix = this->nPix + 1;
			this->sumPAN = this->sumPAN + (bandValues[numBands-1]*bandValues[numBands-1]); // P^2
			float pixMSSum = 0;
			
			for(int i = 0; i < (numBands - 1); ++i)
			{
				pixMSSum = pixMSSum + (bandValues[i]*bandValues[i]);
			}
			
			this->sumMS = this->sumMS + pixMSSum;
		}
	}
	
	void RSGISHCSPanSharpenCalcMeanStats::returnStats()
	{
		this->outStats[0] = this->sumMS / this->nPix;
		this->outStats[1] = this->sumPAN / this->nPix;
	}
	
	
	RSGISHCSPanSharpenCalcSDStats::RSGISHCSPanSharpenCalcSDStats(int numberOutBands, float *outStats) : RSGISCalcImageValue(numberOutBands)
	{
		this->numberOutBands = numberOutBands;
		this->outStats = outStats;
		this->sumMS = 0.;
		this->sumPAN = 0.;
		this->nPix = 0;
	}
	
	void RSGISHCSPanSharpenCalcSDStats::calcImageValue(float *bandValues, int numBands) 
	{
		if(bandValues[0] > 0)
		{
			this->nPix++;
			this->sumPAN = this->sumPAN + pow(((bandValues[numBands-1]*bandValues[numBands-1]) - outStats[1]),2); // P^2
			float pixMSSum = 0;
			
			for(int i = 0; i < (numBands - 1); ++i)
			{
				pixMSSum = pixMSSum + (bandValues[i]*bandValues[i]);
			}
			
			this->sumMS = this->sumMS + pow(pixMSSum - outStats[0],2);
		}
	}
	
	void RSGISHCSPanSharpenCalcSDStats::returnStats()
	{
		this->outStats[2] = sqrt(this->sumMS / this->nPix);
		this->outStats[3] = sqrt(this->sumPAN / this->nPix);
	}

}}
