/*
 *  RSGISParseXMLArguments.h
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

#ifndef RSGISParseXMLArguments_H
#define RSGISParseXMLArguments_H

#include <string>
#include <iostream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include "RSGISAlgorithmParameters.h"
#include "RSGISXMLArgumentsException.h"
#include "RSGISInputStreamException.h"
#include "RSGISAlgorParamsFactory.h"

namespace rsgis
{
	class DllExport RSGISParseXMLArguments
		{
		public:
			RSGISParseXMLArguments(std::string xmlFile);
			RSGISAlgorithmParameters** parseArguments(int *numParams, RSGISAlgorParamsFactory *algorFactory) throw(RSGISXMLArgumentsException);
			~RSGISParseXMLArguments();
		protected:
            std::string xmlFile;
			int numArguments;
		};
}

#endif

