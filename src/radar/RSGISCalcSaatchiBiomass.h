/*
 *  RSGISCalcSaatchiBiomass.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/04/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#ifndef RSGISCalcSaatchiBiomass_H
#define RSGISCalcSaatchiBiomass_H

#include <iostream>
#include <string>
#include <math.h>
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_radar_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis 
{
	namespace radar
	/// Radar utilities
	{
        
		class DllExport RSGISCalcSaatchiBiomassCrown : public rsgis::img::RSGISCalcImageValue
			/// Estimates Crown Biomass from L or P band data with correction for local incidence angle.			
			/** Called using:
			sartools -s -c -i \<input images\> -o \<output image\> -a0 \<a0\> -a1 \<a1\> -a2 \<a2\> -b1 \<b1\> -b2 \<b2\> -c1 \<c1\> -c2 \<c2\>
			
			Implements:
			
			\f$
				\begin{array}{rl}
					\log(W_c)= a_0+a_1 \sigma_{HV}^0 \cos(\theta_0 - \theta_l) + a_2(\sigma_{HV}\cos(\theta_0 - \theta_l))^2\\
						+b_1 \sigma_{HH}^0 \sin(\theta_0 - \theta_l)+ b_2(\sigma_{HH}\sin(\theta_0 - \theta_l))^2\\
						+c_1 \sigma_{VV}^0 \cos(\theta_0 - \theta_l) + c_2(\sigma_{VV}\cos(\theta_0 - \theta_l))^2\\
				\end{array}
			\f$
			*/
			
			{
			public: 
				RSGISCalcSaatchiBiomassCrown(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef, double c1Coef, double c2Coef);
				void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};					
			protected:
				double a0;
				double a1;
				double a2;
				double b1;
				double b2;
				double c1;
				double c2;
			};
		
		class DllExport RSGISCalcSaatchiBiomassStem : public rsgis::img::RSGISCalcImageValue
			 /// Estimates stem Biomass from L or P band data with correction for local incidence angle.
			/** Called using:
			sartools -s -st -i \<input images> -o \<output image> -a0 \<a0> -a1 \<a1> -a2 \<a2> -b1 \<b1> -b2 \<b2> -c1 \<c1> -c2 \<c2>
			
			Implements:
			
			\f$ 
				\begin{array}{rl}
					\log(W_s)= a_0+a_1 \sigma_{HV}^0 \sin(\theta_0 - \theta_l) + a_2(\sigma_{HV}\sin(\theta_0 - \theta_l))^2\\
					+b_1 \sigma_{HH}^0 \cos(\theta_0 - \theta_l)+ b_2(\sigma_{HH}\cos(\theta_0 - \theta_l))^2\\
					+c_1 \sigma_{VV}^0 \cos(\theta_0 - \theta_l) + c_2(\sigma_{VV}\cos(\theta_0 - \theta_l))^2\\
				\end{array}
			\f$
			*/
						 
			{
			public: 
				RSGISCalcSaatchiBiomassStem(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef, double c1Coef, double c2Coef);
				void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};					
			protected:
				double a0;
				double a1;
				double a2;
				double b1;
				double b2;
				double c1;
				double c2;
			};
		
		
		class DllExport RSGISCalcSaatchiBiomassCrownPL : public rsgis::img::RSGISCalcImageValue
			 /// Estimates crown Biomass from L and P band data with correction for local incidence angle.
			/** Called using:
			sartools -s -cPL -i \<input images> -o \<output image> -a0 \<a0> -a1 \<a1> -a2 \<a2> -b1 \<b1> -b2 \<b2> -c1 \<c1> -c2 \<c2>
			
			
			Implements:
			
			\f$
				\begin{array}{rl}
					\log(W_c)= a_0+a_1 \sigma_{LHV}^0 \cos(\theta_0 - \theta_l) + a_2(\sigma_{LHV}\cos(\theta_0 - \theta_l))^2\\
					+b_1 \sigma_{PHV}^0 \cos(\theta_0 - \theta_l)+ b_2(\sigma_{PHV}\cos(\theta_0 - \theta_l))^2\\
				\end{array}
			\f$
			*/

			{
			public: 
				RSGISCalcSaatchiBiomassCrownPL(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef);
				void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};					
			protected:
				double a0;
				double a1;
				double a2;
				double b1;
				double b2;
			};
		
		class DllExport RSGISCalcSaatchiNoIABiomassCrown : public rsgis::img::RSGISCalcImageValue
			 /// Estimates crown Biomass from L or P band data with no correction for local incidence angle.
			/** 
			This implements a version where the difference between incidence angle and local incidence angle is assumed to be zero,
			therefore the HH terms drop out.
			Called using:
			sartools -snia -c -i \<input images> -o \<output image> -a0 \<a0> -a1 \<a1> -a2 \<a2> -b1 \<b1> -b2 \<b2> -c1 \<c1> -c2 \<c2>
			
			Implements:
			
			\f$
			  \log(W_c)= a_0+a_1 \sigma_{HV}^0 + a_2\sigma_{HV}^2 +c_1 \sigma_{VV}^0 + c_2\sigma_{VV}^2\
			\f$
			*/
			{
			public: 
				RSGISCalcSaatchiNoIABiomassCrown(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double c1Coef, double c2Coef);
				void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};					
			protected:
				double a0;
				double a1;
				double a2;
				double c1;
				double c2;
			};
		
		class DllExport RSGISCalcSaatchiNoIABiomassStem : public rsgis::img::RSGISCalcImageValue
			 /// Estimates stem Biomass from L or P band data with no correction for local incidence angle.
			/** 
			This implements a modification of Saatchi's algorithm by LeToan whereby the difference between the incidence angle and local incidence angle is assumed to be zero, 
			however for the stem biomass the HV term is left in the account for the significant effect HV scattering has on the results.
			setting the a1 and a2 coefficents to 0 produces the same results as the unmodified algorithm with no difference between incidence angle 
			and local incidence angle.
			Called using:
			sartools -snia -st -i \<input images> -o \<output image> -a0 \<a0> -a1 \<a1> -a2 \<a2> -b1 \<b1> -b2 \<b2> -c1 \<c1> -c2 \<c2>
			
			Implements:
			
			\f$
				\begin{array}{rl}
					\log(W_s)= a_0+a_1 \sigma_{HV}^0 + a_2 \sigma_{HV}^2+b_1 \sigma_{HH}^0 + b_2\sigma_{HH}^2\\
				\end{array}
			\f$
			*/
			{
			public: 
				RSGISCalcSaatchiNoIABiomassStem(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef, double c1Coef, double c2Coef);
				void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};					
			protected:
				double a0;
				double a1;
				double a2;
				double b1;
				double b2;
				double c1;
				double c2;
			};
		
		
		class DllExport RSGISCalcSaatchiNoIABiomassCrownPL : public rsgis::img::RSGISCalcImageValue
			/// Estimates crown Biomass from L and P band data with no correction for local incidence angle.
			/** 
			This implements a version where the difference between incidence angle and local incidence angle is assumed to be zero.
			Called using:
			sartools -snia -cPL -i \<input images> -o \<output image> -a0 \<a0> -a1 \<a1> -a2 \<a2> -b1 \<b1> -b2 \<b2> -c1 \<c1> -c2 \<c2>
			
			Implements:
			\f$
			   \log(W_c)= a_0+a_1 \sigma_{LHV}^0 + a_2\sigma_{LHV}^2 +b_1 \sigma_{PHV}^0+b_2\sigma_{PHV}^2
			\f$
			*/
			{

			public: 
				RSGISCalcSaatchiNoIABiomassCrownPL(int numberOutBands, double a0Coef, double a1Coef, double a2Coef, double b1Coef, double b2Coef);
				void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
                void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};					
			protected:
				double a0;
				double a1;
				double a2;
				double b1;
				double b2;
			};
	}
}

#endif
