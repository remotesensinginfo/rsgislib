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

#include "cmds/RSGISCmdFilterImages.h"

#include "filtering/RSGISFilterBank.h"
#include "filtering/RSGISImageFilter.h"
#include "filtering/RSGISCalcImageFilters.h"
#include "filtering/RSGISGenerateFilter.h"
#include "filtering/RSGISImageKernelFilter.h"
#include "filtering/RSGISSobelFilter.h"
#include "filtering/RSGISPrewittFilter.h"
#include "filtering/RSGISStatsFilters.h"
#include "filtering/RSGISSpeckleFilters.h"
#include "filtering/RSGISNonLocalDenoising.h"
#include "filtering/RSGISSARTextureFilters.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

namespace rsgisexe{

class DllExport RSGISExeFilterImages : public rsgis::RSGISAlgorithmParameters
	{
	public:
		
		enum options 
		{
			none,
			filter,
            nldenoising,
			exportfilterbank
		};
		
		RSGISExeFilterImages();
		virtual rsgis::RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(rsgis::RSGISException);
		virtual void printParameters();
		virtual std::string getDescription();
		virtual std::string getXMLSchema();
		virtual void help();
		~RSGISExeFilterImages();
	protected:
		options option;
		std::string inputImage;
		std::string outputImageBase;
		rsgis::filter::RSGISFilterBank *filterBank;
        std::vector<rsgis::cmds::RSGISFilterParameters*> *filterParameters;
        std::string imageFormat;
        std::string imageExt;
        GDALDataType outDataType;
        rsgis::RSGISLibDataType rsgisOutDataType;
        double aPar;
        double hPar;
	};
}

#endif


