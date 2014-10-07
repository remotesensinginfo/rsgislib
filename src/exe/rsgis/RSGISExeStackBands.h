/*
 *  RSGISExeStackBands.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/12/2008.
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

#ifndef RSGISExeStackBands_H
#define RSGISExeStackBands_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "img/RSGISAddBands.h"
#include "utils/RSGISFileUtils.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

#include "cmds/RSGISCmdImageUtils.h"
#include "cmds/RSGISCmdException.h"
#include "cmds/RSGISCmdParent.h"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::utils;

class DllExport RSGISExeStackBands : public RSGISAlgorithmParameters
	{
	public:
		RSGISExeStackBands();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeStackBands();
	protected:
		string *inputImages;
		string outputImage;
		bool createVRT;
		bool useBandNames;
		int numImages;
        string *imageBandNames;
		float noDataValue;
		float skipValue;
        bool skipPixels;
        GDALDataType outDataType;
        string imageFormat;
        bool replaceBandNames;
	};
}
#endif

