/*
 *  RSGISRandomDistro.h
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

#ifndef RSGISRandomDistro_H
#define RSGISRandomDistro_H

#include <time.h>
#include <iostream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf_gamma.h>
#include "math/RSGISProbDistro.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace math {
    
	class DllExport RSGISRandDistroGaussian : public RSGISProbDistro
	{
	/** Randomly sample from a gausian distrobution.
	 *  Uses GSL library, see http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html
	 *  for a full explenation.
	 *  Seeded using system time
	 */
	public:
		RSGISRandDistroGaussian(double mean, double stDev);
		virtual double calcProb (double value){return 0;};
		virtual double calcRand ();
		virtual ~RSGISRandDistroGaussian();
	private:
		double mean;
		double stDev;
		gsl_rng *gslRand;
	};
	
	class DllExport RSGISPsudoRandDistroGaussian : public RSGISProbDistro
	{
	/** Randomly sample from a gausian distrobution.
	 *  Uses GSL library, see http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html
	 *  for a full explenation.
	 *  Seeded using 1 to always produce the same sequence of numbers
	 */
	public:
		RSGISPsudoRandDistroGaussian(double mean, double stDev);
		virtual double calcProb (double value){return 0;};
		virtual double calcRand ();
		virtual ~RSGISPsudoRandDistroGaussian();
	private:
		double mean;
		double stDev;
		gsl_rng *gslRand;
	};
	
	class DllExport RSGISRandDistroGamma : public RSGISProbDistro
	{
	/** Randomly sample from a gamma distrobution.
	 *  Uses GSL library, see http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html
	 *  for a full explenation.
	 *  Seeded using system time
	 */
	public:
		RSGISRandDistroGamma(double scale, double shape);
		virtual double calcProb (double value){return 0;};
		virtual double calcRand ();
		virtual ~RSGISRandDistroGamma();
	private:
		double scale;
		double shape;
		gsl_rng *gslRand;
	};
	
	class DllExport RSGISPsudoRandDistroGamma : public RSGISProbDistro
	{
	/** Randomly sample from a gamma distrobution.
	 *  Uses GSL library, see http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html
	 *  for a full explenation.
	 *  Seeded using 1 to always produce the same sequence of numbers
	 */
	public:
		RSGISPsudoRandDistroGamma(double scale, double shape);
		virtual double calcProb (double value){return 0;};
		virtual double calcRand ();
		virtual ~RSGISPsudoRandDistroGamma();
	private:
		double scale;
		double shape;
		gsl_rng *gslRand;
	};
		
	class DllExport RSGISRandDistroUniformInt : public RSGISProbDistro
	{
	/** Returns a random integeger between minVal and maxVal
	 *  Uses GSL library, see http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Generation.html
	 *  for a full explenation.
	 *  Seeded using system time
	 */
	public:
		RSGISRandDistroUniformInt(int minVal, int maxVal);
		virtual double calcProb (double value){return 0;};
		virtual double calcRand ();
		virtual ~RSGISRandDistroUniformInt();
	private:
		int minVal;
		int maxVal;
		gsl_rng *gslRand;
	};
	
	class DllExport RSGISPsudoRandDistroUniformInt : public RSGISProbDistro
	{
	/** Returns a random integeger between minVal and maxVal
	 *  Uses GSL library, see http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Generation.html
	 *  for a full explenation.
	 *  Seeded using 1 to always produce the same sequence of numbers
	 */
	public:
		RSGISPsudoRandDistroUniformInt(int minVal, int maxVal);
		virtual double calcProb (double value){return 0;};
		virtual double calcRand ();
		virtual ~RSGISPsudoRandDistroUniformInt();
	private:
		int minVal;
		int maxVal;
		gsl_rng *gslRand;
	};
	
	class DllExport RSGISRandDistroUniformDouble : public RSGISProbDistro
	{
		/** Returns a random double between minVal and maxVal
		 *  Uses GSL library, see http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Generation.html
		 *  for a full explenation.
		 *  Seeded using system time
		 */
	public:
		RSGISRandDistroUniformDouble(double minVal, double maxVal);
		virtual double calcProb (double value){return 0;};
		virtual double calcRand ();
		virtual ~RSGISRandDistroUniformDouble();
	private:
		double minVal;
		double maxVal;
		gsl_rng *gslRand;
	};
	
	class DllExport RSGISPsudoRandDistroUniformDouble : public RSGISProbDistro
	{
		/** Returns a random double between minVal and maxVal
		 *  Uses GSL library, see http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Generation.html
		 *  for a full explenation.
		 *  Seeded using 1 to always produce the same sequence of numbers
		 */
	public:
		RSGISPsudoRandDistroUniformDouble(double minVal, double maxVal);
		virtual double calcProb (double value){return 0;};
		virtual double calcRand ();
		virtual ~RSGISPsudoRandDistroUniformDouble();
	private:
		double minVal;
		double maxVal;
		gsl_rng *gslRand;
	};
	
	class DllExport RSGISPsudoRandDistNeymanTypeA : public RSGISProbDistro
	{			
		/// Statistical function for a Neyman Type A distribution.
	public:
		RSGISPsudoRandDistNeymanTypeA(double m1, double m2, unsigned int maxVal = 0);
		virtual double calcProb(double iVal);
		virtual double calcRand();
		~RSGISPsudoRandDistNeymanTypeA();
	protected:
		double m1;
		double m2;
		unsigned int maxVal;
		double *valsTable;
		gsl_rng *gslRand;
		float value;
	};
}}

#endif
