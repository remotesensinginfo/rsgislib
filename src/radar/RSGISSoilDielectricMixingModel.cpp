/*
 *  RSGISSoilDielectricMixingModel.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 28/08/2009.
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

#include "RSGISSoilDielectricMixingModel.h"
namespace rsgis { namespace radar {

	RSGISSoilDielectricMixingModel::RSGISSoilDielectricMixingModel(double frequency, double sand, double clay, double temperature, double bulkDensity, double volMoisture)
	{
		//double pi = 3.141592653589793;
		this->frequency = frequency;
		this->sand = sand;
		this->clay = clay;
		this->temperature = temperature;
		this->rhoB = bulkDensity;
		this->mV = volMoisture;
		this->rhoS = 2.66; // [g / cm3] Specific density of solid soil properties
		this->alpha = 0.65;
		this->epsWInf = 4.9; // High frequency limit of dielectric constant of free water
		
		
		if(temperature == 20)
		{
			this->twoPiTw = 0.58e-10; // Relaxation time for water (at 20 degrees) times 2 pi[s]
			this->epsW0 = 80.1; // Static dielectric constant for water (at 20 degrees).
		}
		else
		{
			this->twoPiTw = 1.1109e-10  - (3.824e-12 * temperature)+ (6.938e-14 * (temperature * temperature)) - (5.096e-16* pow(temperature,3));
			this->epsW0 = 88.045  - (0.4147 * temperature) + (6.295e-4 * (temperature * temperature))  + (1.075e-5 * pow(temperature, 3));
		}
	}
	
	double RSGISSoilDielectricMixingModel::calcRealDielectric()
	{
		double betaReal = 1.2748 - (0.519 * sand) - (0.152 * clay); // Real part of soil dependence constant
		double epsS = ((1.01 + (0.44 * rhoS)) * (1.01 + (0.44 * rhoS))) - 0.062; // Dielectric constant of solid soils;
		double epsFwReal = epsWInf + (epsW0 - epsWInf) / (1 + ((frequency * twoPiTw )*(frequency * twoPiTw )));
		double epsRealPartA = 1 + (rhoB / rhoS) * (pow(epsS,alpha) - 1);
		double epsRealPartB =  (pow(mV,betaReal) * pow(epsFwReal,alpha)) - mV;
		double epsReal = pow((epsRealPartA + epsRealPartB),(1/alpha));
		return epsReal;
	}
	double RSGISSoilDielectricMixingModel::calcImaginaryDielectric()
	{
		double betaImg = 1.33797 - (0.603 * sand) - (0.166 * clay);
		double sigmaEff = 0;
		if(frequency < 1.4e9)
		{
			sigmaEff = 0.0467 + (0.2204 * rhoB) - (0.4111 * sand) + (0.6614 * clay);
		}
		else
		{
			sigmaEff = -1.645 + (1.939 * rhoB) - (2.25622 * sand) + (1.594 * clay);
		}
		
		double epsFwImgPartA = ((frequency * twoPiTw)  * (epsW0 - epsWInf)) /  (1 + ((frequency * twoPiTw )*(frequency * twoPiTw )));
		double epsFwImgPartB = (sigmaEff / (frequency * twoPiTw)) - ((rhoS - rhoB) / (rhoS * mV));
		double epsFwImg = epsFwImgPartA + epsFwImgPartB;
		double epsImgPartA = pow(mV,betaImg);
		double espImgPartB = pow(epsFwImg,alpha);
		double epsImg = pow(epsImgPartA*espImgPartB,(1/alpha));
		return epsImg;
	}
	gsl_complex RSGISSoilDielectricMixingModel::calcComplexDielectric()
	{
		double epsReal = calcRealDielectric();
		double epsImg = calcImaginaryDielectric();
		gsl_complex epsComplex = gsl_complex_rect(epsReal,epsImg);
		return epsComplex;
	}
	RSGISSoilDielectricMixingModel::~RSGISSoilDielectricMixingModel()
	{
		
	}

}}

