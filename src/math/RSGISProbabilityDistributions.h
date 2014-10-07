/*
 *  RSGISProbabilityDistributions.h
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

#ifndef RSGISProbabilityDistributions_H
#define RSGISProbabilityDistributions_H
#include <math.h>
#include <time.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_rng.h>
#include "math/RSGISProbDistro.h"
#include "math/RSGISMathException.h"

#include "common/RSGISCommons.h"


namespace rsgis{namespace math{
    	
	class DllExport RSGISProbDistGamma : public RSGISProbDistro
	{
		/// Statistical function for a Gamma distribution.
		/**
		 * Calculates the probability density function for a gamma distribution.
		 *  \f$
		 *	 f(x) = \frac{1}{s^a \Gamma a} x^{a-1} \mbox{e} ^{- \frac{e}{s}}
		 *  \f$ 
		 *  Where a is the shape and s is the scale.
		 */ 
	public:
		RSGISProbDistGamma(double shape, double scale);
		virtual double calcProb(double predictVal) throw(RSGISMathException);
		virtual double calcRand(){std::cout << "Not Implemented" << std::endl; return 0;};
		~RSGISProbDistGamma();
	protected:
		double shape;
		double scale;
		double gammaShape;
		double invShpSclGammaShp;
		float value;
	};
	class DllExport RSGISProbDistExponential : public RSGISProbDistro
	{			
		/// Statistical function for an Exponential distribution.
		/**
		 * Calculates the probability density function for an exponential distribution.
		 *  \f$
		 *	 f(x) = \lambda \mbox{e} ^{-\lambda x}
		 *  \f$ 
		 *  Where \f$\lambda \f$ is the rate
		 */ 
	public:
		RSGISProbDistExponential(double rate);
		virtual double calcProb(double predictVal) throw(RSGISMathException);
		virtual double calcRand(){std::cout << "Not Implemented" << std::endl; return 0;};
		~RSGISProbDistExponential();
	protected:
		double rate;
		float value;
	};
	class DllExport RSGISProbDistNormal : public RSGISProbDistro
	{			
		/// Statistical function for a Normal (Gaussian) distribution.
		/**
		 * Calculates the probability density function for a normal distribution.
		 *  \f$
		 *	 f(x) = \frac{1}{\sigma \sqrt{2 \pi}}\exp \left( \frac{- \left( x - \bar{x} \right) ^2 }{2 \sigma ^2} \right)
		 *  \f$ 
		 *  Where \f$ \sigma \f$ is the standard deviation and \f$\bar{x} \f$ is the mean value of x.
		 */ 
		
	public:
		RSGISProbDistNormal(double mean, double stdev);
		virtual double calcProb (double predictVal) throw(RSGISMathException);
		virtual double calcRand(){std::cout << "Not Implemented" << std::endl; return 0;};
		virtual ~RSGISProbDistNormal();
	protected:
		double mean;
		double stdev;
		float value;
	};
	class DllExport RSGISProbDistNeymanTypeA : public RSGISProbDistro
	{			
		/// Statistical function for a Neyman Type A distribution.
	public:
		RSGISProbDistNeymanTypeA(double m1, double m2, unsigned int maxVal = 0);
		virtual double calcProb(double iVal) throw(RSGISMathException);
		virtual double calcRand();
		~RSGISProbDistNeymanTypeA();
	protected:
		double m1;
		double m2;
		unsigned int maxVal;
		double *valsTable;
		gsl_rng *gslRand;
		float value;
	};
	
	class DllExport RSGISProbDistReturnVal : public RSGISProbDistro
	{			
		/// Returns value (use to provide constant in place of distrobution)
	public:
		RSGISProbDistReturnVal(double val);
		virtual double calcProb(double iVal) throw(RSGISMathException);
		virtual double calcRand();
		~RSGISProbDistReturnVal();
	protected:
		double val;
	};
	
	
}}

#endif

