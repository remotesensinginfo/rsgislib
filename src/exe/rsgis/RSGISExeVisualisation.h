/*
 *  RSGISExeVisualisation.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/07/2009.
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

#ifndef RSGISExeVisualisation_H
#define RSGISExeVisualisation_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"
#include "common/RSGISFileException.h"

#include "utils/RSGISFileUtils.h"
#include "utils/RSGISParseExportForPlotting.h"
#include "utils/RSGISExportForPlotting.h"

#include "math/RSGISMathsUtils.h"

#include "vec/RSGISVectorIO.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::utils;
using namespace rsgis::vec;


class DllExport RSGISExeVisualisation : public RSGISAlgorithmParameters
	{
	public:
		
		enum options 
		{
			none,
			setupplotter,
			deleteplotter,
			line2dtoshpfile
		};
		
		RSGISExeVisualisation();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeVisualisation();
	protected:
		options option;
		string inputFile;
		string outputFile;
		string outputDIR;
		bool force;
	};
}
#endif




