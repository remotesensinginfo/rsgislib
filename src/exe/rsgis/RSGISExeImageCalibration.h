/*
 *  RSGISExeImageCalibration.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/05/2011.
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


#ifndef RSGISExeImageCalibration_H
#define RSGISExeImageCalibration_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISRegistrationException.h"

#include "calibration/RSGISStandardDN2RadianceCalibration.h"
#include "calibration/RSGISCalculateTopOfAtmosphereReflectance.h"
#include "calibration/RSGISApply6SCoefficients.h"

#include "utils/RSGISFileUtils.h"
#include "math/RSGISMathsUtils.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

#include "boost/date_time/gregorian/gregorian.hpp"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::utils;
using namespace rsgis::math;
using namespace rsgis::img;
using namespace rsgis::calib;

class RSGISExeImageCalibration : public RSGISAlgorithmParameters
{
public:
	enum options 
	{
		none,
		landsatradcal,
        spotradcal,
        ikonosradcal,
        asterradcal,
        irsradcal,
        quickbird16bitradcal,
        quickbird8bitradcal,
        worldview2radcal,
        topatmosrefl,
        apply6s
    };
	
	RSGISExeImageCalibration();
	virtual RSGISAlgorithmParameters* getInstance();
	virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
	virtual void runAlgorithm() throw(RSGISException);
	virtual void printParameters();
	virtual string getDescription();
	virtual string getXMLSchema();
	virtual void help();
	~RSGISExeImageCalibration();
protected:
	options option;
    string outputImage;
    string inputImage;
	string inputDEM;
    string *inputImages;
    unsigned int numBands;
	bool useTopo6S;
    LandsatRadianceGainsOffsets *landsatRadGainOffs;
    SPOTRadianceGainsOffsets *spotRadGainOffs;
    IkonosRadianceGainsOffsets *ikonosRadGainOffs;
    ASTERRadianceGainsOffsets *asterRadGainOffs;
    IRSRadianceGainsOffsets *irsRadGainOffs;
    Quickbird16bitRadianceGainsOffsets *quickbird16bitRadGainOffs;
    Quickbird8bitRadianceGainsOffsets *quickbird8bitRadGainOffs;
    WorldView2RadianceGainsOffsets *worldview2RadGainOffs;
    float *solarIrradiance;
    float solarZenith;
    unsigned int julianDay;
    unsigned int *imageBands;
    float **aX;
    float **bX;
    float **cX;
	float *elevationThreash;
    float scaleFactor;
    int numValues;
	unsigned int numElevation;
	string imageFormat;
    GDALDataType outDataType;
};

}

#endif



