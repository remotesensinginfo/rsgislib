/*
 *  RSGISProbabilityDistributions.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 14/10/2009.
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

#include "RSGISProbabilityDistributions.h"

namespace rsgis { namespace math{
	
	RSGISProbDistGamma::RSGISProbDistGamma(double shape, double scale)
	{
		this->shape = shape;
		this->scale = scale;
		this->gammaShape = gsl_sf_gamma(shape);
		this->invShpSclGammaShp = 1 / (pow(scale, shape));
		
	}
	double RSGISProbDistGamma::calcProb(double predictVal) throw(RSGISMathException)
	{
		double valPowShape = pow(predictVal, (shape - 1));
		double expValScale = exp((-1) * ((predictVal / scale)));
 		double pVal = invShpSclGammaShp * (valPowShape * expValScale);
		return pVal;
	}
	RSGISProbDistGamma::~RSGISProbDistGamma()
	{
		
	}
	
	RSGISProbDistExponential::RSGISProbDistExponential(double rate)
	{
		this->rate = rate;
	}
	double RSGISProbDistExponential::calcProb(double predictVal) throw(RSGISMathException)
	{
		double valrate = (-1) * (rate * predictVal);
		double exprate = exp (valrate);
		double pVal = exprate * rate;
		return pVal;
	}
	RSGISProbDistExponential::~RSGISProbDistExponential()
	{
		
	}
	
	RSGISProbDistNormal::RSGISProbDistNormal(double mean, double stdev)
	{
		this->mean = mean;
		this->stdev = stdev;
	}
	double RSGISProbDistNormal::calcProb(double predictVal) throw(RSGISMathException)
	{
		double pi = 3.141592653589793;
		double valDiff = (predictVal - mean);
		double valDiff2 = valDiff * valDiff;
		double valDStDev = valDiff2 / (stdev * stdev);
		double expvalDStDev = exp ((-1) * valDStDev);
		double sqrtpi = sqrt(2 * pi);
		double sqrtpiStDev = stdev * sqrtpi;
		double invsqrtpiStDev = 1 / sqrtpiStDev;
		double pVal = invsqrtpiStDev * expvalDStDev;
		return pVal;
	}
	RSGISProbDistNormal::~RSGISProbDistNormal()
	{
		
	}
	
	RSGISProbDistNeymanTypeA::RSGISProbDistNeymanTypeA(double m1, double m2, unsigned int maxVal)
	{
		this->m1 = m1;
		this->m2 = m2;
		this->maxVal = maxVal;
		if(this->maxVal == 0)
		{
			this->maxVal = int(2*m1*m2)+1;
		}

		// Set up table of values
		double prob = 0;
		this->valsTable = new double[this->maxVal];
		for(unsigned int i = 0; i < this->maxVal; ++i) // Set up an array
		{
			prob = prob + this->calcProb(i);
			this->valsTable[i] = prob;
		}
		
		for(unsigned int i = 0; i < this->maxVal; ++i) // Normalise (values between 0 and 1)
		{
			this->valsTable[i] = this->valsTable[i] / prob; // Last value largest (should equal 1)
		}
		
		// Seed the random number generator using the system time plus a random number
		double seed = time(0) + rand();
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
		
	}
	double RSGISProbDistNeymanTypeA::calcProb(double iVal) throw(RSGISMathException)
	{
		double pVal = 0;

		double partA = exp(-m1) * (pow(m2, iVal) / gsl_sf_fact(iVal));
		double partB = 0;
		for (unsigned int j = 0; j < 10; j++) 
		{
			partB = partB + (( (pow((m1 * exp(-m2)),((double)j)) / ((double)gsl_sf_fact(j)))) * pow(((double)j), iVal));
		}
		pVal = partA * partB;

		return pVal;
	}
	
	double RSGISProbDistNeymanTypeA::calcRand()
	{
		double pRand = 0;
		double rand = gsl_rng_uniform(this->gslRand);
		
		for(unsigned int i = 0; i < (maxVal - 1); ++i)
		{
			if((rand >= this->valsTable[i]) && (rand <= this->valsTable[i+1])){pRand = i;} // Find value in cdf table, closest to random number
			
		}
		return pRand;
	}
	
	RSGISProbDistNeymanTypeA::~RSGISProbDistNeymanTypeA()
	{
		delete[] this->valsTable;
		gsl_rng_free(this->gslRand);
	}
	
	RSGISProbDistReturnVal::RSGISProbDistReturnVal(double val)
	{
		this->val = val;
	}
	double RSGISProbDistReturnVal::calcProb(double newVal) throw(RSGISMathException)
	{
		return val;
	}
	double RSGISProbDistReturnVal::calcRand()
	{
		return val;
	}
	RSGISProbDistReturnVal::~RSGISProbDistReturnVal()
	{
		
	}
}}

