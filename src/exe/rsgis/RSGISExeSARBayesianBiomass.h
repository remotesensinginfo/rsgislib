/*
 *  RSGISExeSARBayesianBiomass.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 16/12/2008.
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

#ifndef RSGISExeSARBayesianBiomass_H
#define RSGISExeSARBayesianBiomass_H

#include <string>
#include <iostream>
#include <cstdlib>

#include "radar/RSGISDefaultSplitBiomassFunction.h"
#include "img/RSGISImageCalcValueBaysianNoPrior.h"
#include "img/RSGISImageCalcValueBaysianPrior.h"
#include "img/RSGISCalcImage.h"
#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"
#include "math/RSGISMathsUtils.h"
#include "math/RSGISMathFunction.h"
#include "math/RSGISProbabilityDistributions.h"
#include "math/RSGISProbDistro.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

namespace rsgisexe{
    
using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::radar;
using namespace rsgis::math;

/// Biomass estimation from single band SAR image using the algorithm of LeToan

/**
* This is an implanentation of biomass retrieval from a single band \f$ \gamma^0 \f$ image based on the method of Le Toan (2008). \n
* The method uses Baysian statistics to predict a biomass map with associated confidence limits. \n
* \n
* Biomass can be related to backscatter using the formula:
* \f$
* \log (B) = \left\{ \begin{array}{l c l}
* - \ln \left( \frac{- \gamma ^ 0 - a_{lt}}{b_{lt}} \right)&\mbox{for }\gamma ^ 0 < \gamma_{\mbox{split}}\\
* a_{gt} + b_{gt} . \gamma ^0&\mbox{for }\gamma ^ 0 > \gamma_{\mbox{split}}\\ 
* \end{array} \right
* \f$ 
* \n 
* Where \f$-a_{lt} >  \gamma_{\mbox{split}} \f$ \n
* 
* The Baysian method for biomass retrieval uses the probability of a biomass (B) for a given value of backscatter ($\gamma ^0$). \n
* From Baysian theory this can be writen as:\n
* \f$
* P(B | \gamma^0) \propto \frac{P(\gamma^0 | B).P(B)}{P(\gamma ^0)}
* \f$
* Where P(B) is prior information on the distrobution of biomass, \f$P(\gamma ^0)\f$ is the prior distrobution of \f$\gamma ^0\f$.
* In the absenve of prior information these distrobution can be assumed to be uniform, i.e. any value is equally likely. \n
* The probability of biomass for a given backscatter is therefore given as: \n
* \f$
* P(B | \gamma^0) \propto P(\gamma^0 | B) 
* \f$
* \n 
* This can be written as: \n
* \f$
* P(B | \gamma^0) = e^{- \left( \frac{ ( \gamma^0 - f(B) )^2}{2 \sigma ^2} \right)
* \f$
* Where f(B) is the inverse of the first equation: \n
* $
* \gamma ^ 0 = \left\{ \begin{array}{l c l}
* -b_{lt}.\mbox{e}^{-\log B}-a_{lt}& \mbox{for } B ^ 0 < B_{\mbox{split}}\\
* \frac{\log B -a_{gt}}{b_{gt}} & \mbox{for }B > B_{\mbox{split}}\\ 
* \end{array} \right
* $
* \n 
* When there is known information about the distribution of biomass this can be used to give: \n
* \f$
* P(B | \gamma^0) \propto P(\gamma^0 | B).P(B) 
* \f$<br>
* <b>Coefficients</b><br>
* <i>Closed Forest</i> 
* LHH (noPriorClosedLHH)<br>
* coefAGT="4.112887686" coefBGT="0.35805" coefALT="3.53" coefBLT="15.58" split="-8.0" <br>
* LVV (noPriorClosedLVV)<br>
* coefAGT="4.51355858" coefBGT="0.44021" coefALT="3.38" coefBLT="15.15"  split="-7.0" <br>
* LHV (noPriorClosedLHV)<br>
* coefAGT="5.421108362" coefBGT="0.31216" coefALT="7.45" coefBLT="24.31" split="-12.0"<br>
* PHH (noPriorClosedPHH)<br>
* coefAGT="3.539210149" coefBGT="0.20507" coefALT="4.77" coefBLT="18.25"  split="-7.0" <br>
* PVV (noPriorClosedPVV)<br>
* coefAGT="4.769536659" coefBGT="0.36644" coefALT="5.12" coefBLT="17.08" split="-7.0" <br>
* PHV (noPriorClosedPHV)<br>
* coefAGT="4.880450875" coefBGT="0.18876" coefALT="9.81" coefBLT="30.00" split="-12.0" <br>
* PHV - LeToan Coefficients (noPriorClosedPHVLeToan) <br>           
* coefAGT="4.5563" coefBGT="0.18" coefALT="10" coefBLT="32.00"  split="-15.0" <br>
* <br>
* <i>Open Forest</i>
* LHH (noPriorOpenLHH)<br>
* coefAGT="2.9909" coefBGT="0.1174" coefALT="6.1633" coefBLT="21.403" split="-40" <br>
* LVV (noPriorOpenLVV)<br>
* coefAGT="3.7943" coefBGT="0.1393" coefALT="9.6046" coefBLT="24.103"  split="-40" <br>
* LHV (noPriorOpenLHV)<br>
* coefAGT="3.5861" coefBGT="0.0953" coefALT="12.3831" coefBLT="32.8825" split="-40"<br>
* PHH (noPriorOpenPHH)<br>
* coefAGT="2.6421" coefBGT="0.0292" coefALT="4.9357" coefBLT="20.9475"  split="-40" <br>
* PVV (noPriorOpenPVV)<br>
* coefAGT="5.8346" coefBGT="0.1273" coefALT="10.0723" coefBLT="24.3553" split="-40" <br>
* PHV (noPriorOpenPHV)<br>
* coefAGT="3.7629" coefBGT="0.0917" coefALT="14.5024" coefBLT="35.69" split="-40" <br>
*/


class DllExport RSGISExeSARBayesianBiomass : public RSGISAlgorithmParameters
	{
		enum options
		{
			none,
			noPrior, // No prior
			priorNormal, // With normal prior distrobution
			priorExp, // With exponential prior distrobution
			priorGamma, // With gamma prior distrobution
			noPriorOpenLHH, // Hardcoded with coefficients for open forests
			noPriorOpenLHV,
			noPriorOpenLVV,
			noPriorOpenPHH,
			noPriorOpenPHV,
			noPriorOpenPVV,
			noPriorClosedLHH, // Hardcoded with coefficients for closed forests
			noPriorClosedLHV,
			noPriorClosedLVV,
			noPriorClosedPHH,
			noPriorClosedPHV,
			noPriorClosedPHVLeToan,
			noPriorClosedPVV
		};
		
	public:
		RSGISExeSARBayesianBiomass();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeSARBayesianBiomass();
	protected:
		string inputImage;
		string outputImage;
		options option;
		double coefAGT;
		double coefBGT;
		double coefALT;
		double coefBLT;
		double split;
		double variance;
		double interval;
		double upperLimit;
		double lowerLimit;
		double minVal;
		double maxVal;
		double meanB;
		double stdevB;
		double rateB;
		double shapeB;
		double scaleB;
		deltatypedef deltatype;
	};
}

#endif
