/*
 *  RSGISExeElevationDataTools.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2011.
 *  Copyright 2011 RSGISLib.
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


#ifndef RSGISExeElevationDataTools_H
#define RSGISExeElevationDataTools_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISRegistrationException.h"

#include "calibration/RSGISDEMTools.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "utils/RSGISFileUtils.h"
#include "utils/RSGISGEOSFactoryGenerator.h"

#include "math/RSGISMathsUtils.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::utils;
using namespace rsgis::math;
using namespace rsgis::img;
using namespace rsgis::calib;

class RSGISExeElevationDataTools : public RSGISAlgorithmParameters
{
public:
	enum options 
	{
		none,
        slope,
        aspect,
        slopeaspect,
        hillshade,
        shadowmask,
        incidenceangle,
        exitanceangle,
        incidenceexistanceangles
    };
	
	RSGISExeElevationDataTools();
	virtual RSGISAlgorithmParameters* getInstance();
	virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
	virtual void runAlgorithm() throw(RSGISException);
	virtual void printParameters();
	virtual string getDescription();
	virtual string getXMLSchema();
	virtual void help();
	~RSGISExeElevationDataTools();
protected:
	options option;
    string outputImage;
    string inputImage;
    unsigned int imageBand;
    float solarZenith;
    float solarAzimuth;
    float viewZenith;
    float viewAzimuth;
    int slopeOutputType;
    float maxElevHeight;
};
    
}

#endif


