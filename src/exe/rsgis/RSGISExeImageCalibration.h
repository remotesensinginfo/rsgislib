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
//#include "calibration/RSGISCalculateTopOfAtmosphereReflectance.h"
#include "calibration/RSGISApply6SCoefficients.h"

#include "cmds/RSGISCmdImageCalibration.h"

#include "utils/RSGISFileUtils.h"
#include "math/RSGISMathsUtils.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

#include "boost/date_time/gregorian/gregorian.hpp"

namespace rsgisexe{

class DllExport RSGISExeImageCalibration : public rsgis::RSGISAlgorithmParameters
{
public:
	enum options 
	{
		none,
		landsatradcal,
        landsatradcalmultiadd,
        spotradcal,
        ikonosradcal,
        asterradcal,
        irsradcal,
        quickbird16bitradcal,
        quickbird8bitradcal,
        worldview2radcal,
        topatmosrefl,
        apply6ssingle,
        apply6s
    };
	
	RSGISExeImageCalibration();
	virtual rsgis::RSGISAlgorithmParameters* getInstance();
	virtual void retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
	virtual void runAlgorithm() throw(rsgis::RSGISException);
	virtual void printParameters();
	virtual std::string getDescription();
	virtual std::string getXMLSchema();
	virtual void help();
	~RSGISExeImageCalibration();
protected:
	options option;
    std::string outputImage;
    std::string inputImage;
	std::string inputDEM;
    std::string *inputImages;
    unsigned int numBands;
	bool useTopo6S;
    std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsets> landsatRadGainOffs;
    std::vector<rsgis::cmds::CmdsLandsatRadianceGainsOffsetsMultiAdd> landsatRadGainOffsMultiAdd;
    rsgis::calib::SPOTRadianceGainsOffsets *spotRadGainOffs;
    rsgis::calib::IkonosRadianceGainsOffsets *ikonosRadGainOffs;
    rsgis::calib::ASTERRadianceGainsOffsets *asterRadGainOffs;
    rsgis::calib::IRSRadianceGainsOffsets *irsRadGainOffs;
    rsgis::calib::Quickbird16bitRadianceGainsOffsets *quickbird16bitRadGainOffs;
    rsgis::calib::Quickbird8bitRadianceGainsOffsets *quickbird8bitRadGainOffs;
    rsgis::calib::WorldView2RadianceGainsOffsets *worldview2RadGainOffs;
    float *solarIrradiance;
    float solarZenith;
    unsigned int julianDay;
    unsigned int *imageBands;
    float **aX;
    float **bX;
    float **cX;
    float *aXSingle;
    float *bXSingle;
    float *cXSingle;
	float *elevationThresh;
    float scaleFactor;
    int numValues;
	unsigned int numElevation;
	std::string imageFormat;
    GDALDataType outDataType;
    rsgis::RSGISLibDataType rsgisOutDataType;
    bool useNoDataVal;
    float noDataVal;
};

}

#endif



