/*
 *  RSGISExeFitting.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 29/01/2009.
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

#ifndef RSGISExeFitting_H
#define RSGISExeFitting_H

#include <string>
#include <iostream>
#include <cstdlib>
#include <gsl/gsl_matrix.h>

#include "img/RSGISCalcImage.h"
#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISMathFunction.h"
#include "math/RSGISFunctions.h"
#include "math/RSGISMathsUtils.h"
#include "math/RSGISPolyFit.h"
#include "math/RSGISLinearFit.h"
#include "math/RSGISNonLinearFit.h"
#include "utils/RSGISExportForPlotting.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis::math;
using namespace rsgis::utils;

/// Executable for fitting equations
/**
* Available options are: \n
* - oneDimensionalPolyfit - for fitting a one dimensional nth order polynomial equation using /maths/RSGISPolyFit \n
* - twoDimensionalPolyfit - for fitting a two dimensional nth order polynomial equation using /maths/RSGISPolyFit \n
* - threeDimensionalPolyfit - for fitting a three dimensional nth order polynomial euqation using /maths/RSGISPolyFit \n 
*/ 

class DllExport RSGISExeFitting : public rsgis::RSGISAlgorithmParameters
	{
		
		enum options
		{
			none,
			oneDimensionalPolyfit,
			oneDimensionalPolyTest,
			twoDimensionalPolyfit,
			twoDimensionalPolyTest,
			threeDimensionalPolyfit,
			threeDimensionalPolyTest,
			linearFit,
			linearTest,
			nonLinearFit,
			nonLinearTest
		};
		
	public:
		RSGISExeFitting();
		virtual rsgis::RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(rsgis::RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeFitting();
	protected:
		options option;
		string infile;
		string outfile;
		string inCoeffFile;
		int polyOrder;
		int polyOrderX;
		int polyOrderY;
		int polyOrderZ;
		int numX;
		int numY;
		int numZ;
		double *coefficients;
		bool *fixCoefficients;
		functionType function;
	};
}
#endif

