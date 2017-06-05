/*
 *  RSGISSoilDielectricMixingModel.h
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

#ifndef RSGISSoilDielectricMixingModel_H
#define RSGISSoilDielectricMixingModel_H
#include <iostream>
#include <math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

#include "math/RSGISMathFunction.h"

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
	{        
		class DllExport RSGISSoilDielectricMixingModel
			{
			/**
			 * This class implemnets the dielectic mixing model from the following refference to calculate the complex dielectic of soil:<br>
			 * Peplinski, ULaby, Dobson, Dielectric Properties of Soils in the 0.3 to 1.3 GHz Range, <br>
			 * IEEE Trans Geosci Rem Sensing, Vol 33, No. 3, pp 803-807, May 1995.<br><br>
			 *
			 * with the corrections given in:<br><br>
			 *
			 * Peplinski, ULaby, Dobson, Corrections to: Dielectric Properties of Soils in the 0.3 to 1.3 GHz Range,  <br>
			 * IEEE Trans Geosci Rem Sensing, Vol 33, No. 6, p 1340, Nov. 1995. <br>
			 * 
			 * The temperature dependent form is given in:
			 * F. T. Ulaby, R. K. Moore, and A. K. Fung, Microwave Remote Sensing, vol. III. Dedham, MA: Artech House, 1986, Appendix E.
			 * 
			*/
			public:
				RSGISSoilDielectricMixingModel(double frequency, double sand, double clay, double temperature, double bulkDensity, double volMoisture);
				double calcRealDielectric();
				double calcImaginaryDielectric();
				gsl_complex calcComplexDielectric();
				~RSGISSoilDielectricMixingModel();
			protected:
				double frequency;
				double sand;
				double clay;
				double temperature;
				double rhoB;
				double mV;
				double alpha;
				double epsWInf;
				double rhoS;
				double twoPiTw;
				double epsW0;
			};
	}
}

#endif
