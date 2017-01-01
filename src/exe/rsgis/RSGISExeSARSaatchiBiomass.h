/*
 *  RSGISExeSARSaatchiBiomass.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 04/12/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISExeSARSaatchiBiomass_H
#define RSGISExeSARSaatchiBiomass_H

#include <string>
#include <iostream>
#include <cstdlib>

#include "radar/RSGISCalcSaatchiBiomass.h"
#include "radar/RSGISCalcGammaZero.h"
#include "img/RSGISCalcImage.h"
#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"
#include "math/RSGISMathsUtils.h"

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

/// Excecutable for biomass estimation using the algorithm of Saatchi
/**
* Available options are: \n
* - stem - Stem biomass using L or P band polametric data. Input in the form HH, HV, VV , ia, lia\n
* - crown - Crown biomass using L or P band polametric data. Input in the form HH, HV, VV, ia, lia\n
* - crownPL - Crown biomass using L and P band polametric data. Input in the from L-HH,HV,VV,P-HH,HV,VV,ia,lia \n
* - stemNIA - Stem biomass using L or P band polametric data with no correction for incidence angle. Input in the form HH, HV, VV\n
* - crownNIA - Crown biomass using L or P band polametric data with no correction for incidence angle. Input in the form HH, HV, VV \n 
* - crownPLNIA - Crown biomass using L and P band polametric data with no correction for incidence angle. Input in the from L-HH,HV,VV,P-HH,HV,VV \n
* <b>Coefficients</b><br>
* For open forest the following coefficients have been derived using data collected from the Injune Study Site, \n
* Queensland, Australia (Lucas and Carreiras, 2008). \n 
* \n
* L-band Crownn\n
* a0="-0.0494395"\n
* a1="0.120779"\n
* a2="-0.0043865"\n
* b1="0"\n
* b2="0"\n
* c1="-0.3951039"\n
* c2="-0.013844"\n
* \n
* L-band Stem\n
* a0="6.591433"\n
* a1="0.2931488"\n
* a2="0.0056944"\n
* b1="-0.2118106"\n
* b2="-0.0148108"\n
* c1="-0.0301716"\n
* c2="-0.0005826"\n
* \n
* P-band Crown"\n
* a0="3.5215194"\n
* a1="0.10149"\n
* a2="0.0000118"\n
* b1="0"\n
* b2="0"\n
* c1="-0.3141069"\n
* c2="-0.0115178"\n
* \n
* P-band Stem\n
* a0="2.4261112"\n
* a1="0.2470936"\n
* a2="0.0056361"\n
* b1="0.0134467"\n
* b2="-0.001529"\n
* c1="-0.7007589"\n
* c2="-0.0263756"\n
* \n
* LP-band Crown\n
* a0="3.2641606"\n
* a1="-0.4925952"\n
* a2="-0.0126369"\n
* b1="0.321497"\n
* b2="0.0050075"\n
* c1="0"\n
* c2="0" \n
*/

class DllExport RSGISExeSARSaatchiBiomass : public RSGISAlgorithmParameters
	{
	
	enum options
	{
		none,
		stem,
		crown,
		crownPL,
		stemNIA,
		crownNIA,
		crownPLNIA,
		openStemL, // Coefficients derived from Injune field data, with no correction for incidence angle
		openStemP, // HV term retained in stem calculations
		openCrownL,
		openCrownP,
		openCrownPL,
		closedStemL, // Saatchi published coefficients for closed forest
		closedStemP, // No correction for local incidence angle
		closedCrownL,
		closedCrownP,
		closedCrownPL,
		closedLeToanStemP, // LeToan modification to include HV returns
		closedLeToanCrownP
	};
	
	public:
		RSGISExeSARSaatchiBiomass();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeSARSaatchiBiomass();
	protected:
		string inputImage;
		string outputImage;
		options option;
		double a0;
		double a1;
		double a2;
		double b1;
		double b2;
		double c1;
		double c2;
		
	};
}

#endif

