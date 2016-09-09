/*
 *  RSGISRandomDistro.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 13/11/2009.
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

#include "RSGISRandomDistro.h"


namespace rsgis{ namespace math {

	RSGISRandDistroGaussian::RSGISRandDistroGaussian(double mean, double stDev)
	{
		this->mean = mean;
		this->stDev = stDev;
		
		// Seed the random number generator using the system time plus a random number
		double seed = time(0) + rand();
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
	}
	double RSGISRandDistroGaussian::calcRand ()
	{
		double randProb;
		randProb = gsl_ran_gaussian(gslRand, stDev) + mean;
		return randProb;
	}
	RSGISRandDistroGaussian::~RSGISRandDistroGaussian()
	{
		gsl_rng_free(gslRand);
	}

	RSGISPsudoRandDistroGaussian::RSGISPsudoRandDistroGaussian(double mean, double stDev)
	{
		
		this->mean = mean;
		this->stDev = stDev;
		
		// Seed the random number generator using the system time plus a random number
		double seed = 1;
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
	}
	double RSGISPsudoRandDistroGaussian::calcRand ()
	{
		double randProb;
		randProb = gsl_ran_gaussian(gslRand, stDev) + mean;
		return randProb;
	}
	RSGISPsudoRandDistroGaussian::~RSGISPsudoRandDistroGaussian()
	{
		gsl_rng_free(gslRand);
	}
	
	RSGISRandDistroGamma::RSGISRandDistroGamma(double scale, double shape)
	{
		this->scale = scale;
		this->shape = shape;
		
		// Seed the random number generator using the system time plus a random number
		double seed = time(0) + rand();
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
	}
	double RSGISRandDistroGamma::calcRand ()
	{
		double randProb;
		randProb = gsl_ran_gamma(gslRand, this->scale, this->shape);
		return randProb;
	}
	RSGISRandDistroGamma::~RSGISRandDistroGamma()
	{
		gsl_rng_free(gslRand);
	}
	
	RSGISPsudoRandDistroGamma::RSGISPsudoRandDistroGamma(double scale, double shape)
	{
		this->scale = scale;
		this->shape = shape;
		
		// Seed the random number generator using the system time plus a random number
		double seed = 1;
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
	}
	double RSGISPsudoRandDistroGamma::calcRand()
	{
		double randProb;
		randProb = gsl_ran_gamma(gslRand, this->scale, this->shape);

		return randProb;
	}
	RSGISPsudoRandDistroGamma::~RSGISPsudoRandDistroGamma()
	{
		gsl_rng_free(gslRand);
	}
		
	RSGISRandDistroUniformInt::RSGISRandDistroUniformInt(int minVal, int maxVal)
	{
		this->minVal = minVal;
		this->maxVal = maxVal;
		
		// Seed the random number generator using the system time plus a random number
		double seed = time(0) + rand();
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
	}
	double RSGISRandDistroUniformInt::calcRand ()
	{
		double randProb;
		double upperVal = maxVal - minVal;
		randProb = gsl_rng_uniform_int(gslRand, upperVal);
		randProb = randProb + minVal;
		return randProb;
	}
	RSGISRandDistroUniformInt::~RSGISRandDistroUniformInt()
	{
		gsl_rng_free(gslRand);
	}
	
	RSGISPsudoRandDistroUniformInt::RSGISPsudoRandDistroUniformInt(int minVal, int maxVal)
	{
		this->minVal = minVal;
		this->maxVal = maxVal;
		
		// Seed the random number generator using the system time plus a random number
		double seed = 1;
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
	}
	double RSGISPsudoRandDistroUniformInt::calcRand ()
	{
		double randProb;
		double upperVal = maxVal - minVal;
		randProb = gsl_rng_uniform_int(gslRand, upperVal);
		randProb = randProb + minVal;
		return randProb;
	}
	RSGISPsudoRandDistroUniformInt::~RSGISPsudoRandDistroUniformInt()
	{
		gsl_rng_free(gslRand);
	}
	
	RSGISRandDistroUniformDouble::RSGISRandDistroUniformDouble(double minVal, double maxVal)
	{
		this->minVal = minVal;
		this->maxVal = maxVal;
		
		// Seed the random number generator using the system time plus a random number
		double seed = time(0) + rand();
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
	}
	double RSGISRandDistroUniformDouble::calcRand ()
	{
		double randProb;
		double upperVal = maxVal - minVal;
		randProb = gsl_rng_uniform(gslRand)*upperVal;
		randProb = randProb + minVal;
		return randProb;
	}
	RSGISRandDistroUniformDouble::~RSGISRandDistroUniformDouble()
	{
		gsl_rng_free(gslRand);
	}
	
	RSGISPsudoRandDistroUniformDouble::RSGISPsudoRandDistroUniformDouble(double minVal, double maxVal)
	{
		this->minVal = minVal;
		this->maxVal = maxVal;
		
		// Seed the random number generator using the system time plus a random number
		double seed = 1;
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
	}
	double RSGISPsudoRandDistroUniformDouble::calcRand ()
	{
		double randProb;
		double upperVal = maxVal - minVal;
		randProb = gsl_rng_uniform(gslRand)*upperVal;
		randProb = randProb + minVal;
		return randProb;
	}
	RSGISPsudoRandDistroUniformDouble::~RSGISPsudoRandDistroUniformDouble()
	{
		gsl_rng_free(gslRand);
	}
	
	RSGISPsudoRandDistNeymanTypeA::RSGISPsudoRandDistNeymanTypeA(double m1, double m2, unsigned int maxVal)
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
		double seed = 0;
		
		// Set up the GSL random number generator
		this->gslRand = gsl_rng_alloc (gsl_rng_taus2);
		gsl_rng_set (gslRand, seed);
		
	}
	double RSGISPsudoRandDistNeymanTypeA::calcProb(double iVal)
	{
		double partA = exp(-m1) * (pow(m2, iVal) / gsl_sf_fact(iVal));
		double partB = 0;
		for (unsigned int j = 0; j < 10; j++) 
		{
			partB = partB + (( (pow((m1 * exp(-m2)),((double)j)) / gsl_sf_fact(j))) * pow(j, iVal));
		}
		double pVal = partA * partB;
		return pVal;
	}
	
	double RSGISPsudoRandDistNeymanTypeA::calcRand()
	{
		double pRand = 0;
		double rand = gsl_rng_uniform(this->gslRand);
		
		for(unsigned int i = 0; i < (maxVal - 1); ++i)
		{
			if((rand >= this->valsTable[i]) && (rand <= this->valsTable[i+1])){pRand = i;} // Find value in cdf table, closest to random number
			
		}
		return pRand;
	}
	
	RSGISPsudoRandDistNeymanTypeA::~RSGISPsudoRandDistNeymanTypeA()
	{
		delete[] this->valsTable;
		gsl_rng_free(this->gslRand);
	}
}}
