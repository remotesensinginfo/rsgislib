/*
 *  RSGISDefaultSplitBiomassFunction.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 12/12/2008.
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

#ifndef RSGISDefaultSplitBiomassFunction_H
#define RSGISDefaultSplitBiomassFunction_H

#include <iostream>
#include <math.h>

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
        
		/// Function for calculating gamma0 for a given value of biomass.
		class DllExport RSGISDefaultSplitBiomassFunction : public rsgis::math::RSGISMathFunction
			{
				
				/**
				 * The function implemented is:
				 * \f$ 
				 *		\gamma^0 = \begin{array}{l c l}
				 *			-b_{lt}.\mbox{e}^{-\log B} - a_{lt} & \mbox{for } B < B_{\mbox{split}} \\
				 *			\frac{\log B - a_{gt}}{b_{gt}}  & \mbox{for } B > B_{\mbox{split}} \\
				 * \end{array}
				 * \f$
				 *
				 * Where \f$ B_{\mbox{split}} \f$ is calculated using \f$ \gamma _{\mbox{split}} \f$ in the inverse of the above equation:
				 *
				 * \f$ 
				 *	 \log (B) = \begin{array}{l c l}
				 *	 - \ln \left( \frac{- \gamma ^ 0 - a_{lt}}{b_{lt}} \right)&\mbox{for }\gamma ^ 0 < \gamma_{\mbox{split}}\\
				 *	 a_{gt} + b_{gt} . \gamma ^0 &\mbox{for }\gamma ^ 0 > \gamma_{\mbox{split}}\\ 
				 *	 \end{array}
				 *	\f$
				 * Where the two equations produce different values of biomass, an average is used.
				 */ 
			public:
				RSGISDefaultSplitBiomassFunction(double coefAGT, double coefBGT, double coefALT, double coefBLT, double split);
				virtual double calcFunction(double value) throw(rsgis::math::RSGISMathException);
				virtual double dX(double value) throw(rsgis::math::RSGISMathException){throw rsgis::math::RSGISMathException("Not implemented");};
				virtual int numCoefficients() throw(rsgis::math::RSGISMathException){return 5;};
				virtual void updateCoefficents(double *newCoefficents) throw(rsgis::math::RSGISMathException){throw rsgis::math::RSGISMathException("Not implemented");};
				~RSGISDefaultSplitBiomassFunction();
			protected:
				double coefAGT;
				double coefBGT;
				double coefALT;
				double coefBLT;
				double split;
				double bioSplit;
			};
	}
}

#endif


