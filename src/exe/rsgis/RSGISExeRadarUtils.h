/*
 *  RSGISExeRadarUtils.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 22/07/2009.
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

#ifndef RSGISExeRadarUtils_H
#define RSGISExeRadarUtils_H

#include <string>
#include <iostream>

#include "img/RSGISCalcImage.h"
#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"
#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"
#include "radar/RSGISRadarUtils.h"
#include "radar/RSGISSoilDielectricMixingModel.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::math;
using namespace rsgis::radar;

/// General Radar utilities

class DllExport RSGISExeRadarUtils : public RSGISAlgorithmParameters
	{
		/**
		 * General Radar Utils<br>
		 * <b>convert2dB</b> - Convert to decebels (with optional callibration factor for ALOS data<br>
		 * <b>soilDielectric</b> - calculate the dielectic constant of soil - parameters are given in XML and the output is printed to the screen<br>
		 * <b>soilDielectricList</b> - calculate the dielectric constant of soil for an input text file of the form:<br>
		 * frequency, temperature, bulkDensity, volMoisture, clay, sand<br>
		 * The output is provided as a text file
		 * <b>predictFPCList</b> - Predict FPC for given forest parameters, provided as an gmtxt format text file.<br>
		 * leafRadius,leafDensity, branchRadius, branchLenght, branchTheta, branchPhi, branchDensity, canopyDepth, res<br>
		 * The output is provided as a text file.
		*/ 
		enum options
		{
			none,
			convert2dB,
			soilDielectric,
			soilDielectricList,
			predictFPCList
		};
		
	public:
		RSGISExeRadarUtils();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeRadarUtils();
	protected:
		options option;
		string inputImage;
		string outputImage;
		string inputFile;
		string outputFile;
		double calFactor;
		double frequency;
		double temperature;
		double bulkDensity;
		double volMoisture;
		double clay;
		double sand;
		unsigned int nRuns;
	};
}
#endif


