/*
 *  RSGISExeFFTUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/12/2008.
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

#ifndef RSGISExeFFTUtils_H
#define RSGISExeFFTUtils_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "math/RSGISFFTWUtils.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISMathsUtils.h"
#include "img/RSGISImageBand2Matrix.h"
#include "img/RSGISFFTProcessing.h"
#include "img/RSGISCalcImageSingle.h"
#include "img/RSGISCalcImageSingleValue.h"

#include "vec/RSGISVectorIO.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "geos/geom/Polygon.h"

#include "gdal_priv.h"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::math;
using namespace rsgis::utils;
using namespace rsgis::vec;
using namespace geos::geom;

class DllExport RSGISExeFFTUtils : public RSGISAlgorithmParameters
	{
	public:
		
		enum options 
		{
			none,
			fftImage,
			fftMatrix,
			shiftFFTW,
			sizeAngleMatrix,
			sizeAngleImage,
			sizeAngleFFT
		};
		
		RSGISExeFFTUtils();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeFFTUtils();
	protected:
		string inputImage;
		string inputMatrix;
		string outputImage;
		string outputMatrix;
		int imageband;
		options option;
	};
}
#endif


