/*
 *  RSGISExeFilterImages.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/12/2008.
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

#ifndef RSGISExeFilterImages_H
#define RSGISExeFilterImages_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "math/RSGISMathsUtils.h"

#include "filtering/RSGISFilterBank.h"
#include "filtering/RSGISImageFilter.h"
#include "filtering/RSGISCalcImageFilters.h"
#include "filtering/RSGISGenerateFilter.h"
#include "filtering/RSGISImageKernelFilter.h"
#include "filtering/RSGISSobelFilter.h"
#include "filtering/RSGISPrewittFilter.h"
#include "filtering/RSGISStatsFilters.h"
#include "filtering/RSGISSpeckleFilters.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::math;
using namespace rsgis::filter;

class RSGISExeFilterImages : public RSGISAlgorithmParameters
	{
	public:
		
		enum options 
		{
			none,
			filter,
			exportfilterbank
		};
		
		RSGISExeFilterImages();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeFilterImages();
	protected:
		options option;
		string inputImage;
		string outputImageBase;
		RSGISFilterBank *filterBank;
	};

#endif


