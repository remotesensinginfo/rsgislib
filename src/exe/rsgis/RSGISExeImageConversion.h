/*
 *  RSGISExeImageConversion.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/12/2008.
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

#ifndef RSGISExeImageConversion_H
#define RSGISExeImageConversion_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"

#include "utils/RSGISFileUtils.h"
#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImageSingle.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISImageBand2Matrix.h"
#include "img/RSGISGenerateImageFromXYZData.h"

#include "gdal_priv.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::img;
using namespace rsgis::utils;

class DllExport RSGISExeImageConversion : public RSGISAlgorithmParameters
	{
	public:
		
		enum options 
		{
			none,
			asciicolumn,
			format,
			splitbands,
			image2matrix,
			convertXYZ
		};
		
		RSGISExeImageConversion();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeImageConversion();
	protected:
		options option;
		string inputFile;
		string inputImage;
		string outputImage;
		string *inputImages;
		string *outputImages;
		string outputMatrix;
		string outputASCII;
		string imageFormat;
		int imageband;
		int numImages;
		char delimiter;
		bool xyOrder;
		string projWKTFile;
        bool projFromImage;
		float resolution;
	};
}
#endif


