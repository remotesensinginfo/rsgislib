/*
 *  RSGISExeMathsUtilities.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 08/12/2008.
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

#ifndef RSGISExeMathsUtilities_H
#define RSGISExeMathsUtilities_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMultivariantStats.h"
#include "math/RSGISPrincipalComponentAnalysis.h"
#include "math/RSGISMatricesException.h"
#include "math/RSGISMatrices.h"

#include "utils/RSGIS_ENVI_ASCII_ROI.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::utils;
using namespace rsgis::math;

class DllExport RSGISExeMathsUtilities : public RSGISAlgorithmParameters
	{
	public:
		
		enum options 
		{
			none,
			eigenvectors,
			meanvector,
			eigenmeanvector,
			matrix2image,
			duplicatematrix
		};
		
		RSGISExeMathsUtilities();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeMathsUtilities();
	protected:
		string inputMatrix;
		string outputMatrix;
		string outputMeanVector;
		string outputEigenVector;
		string outputImage;
		int xDup;
		int yDup;
		options option;
	};
}
#endif


