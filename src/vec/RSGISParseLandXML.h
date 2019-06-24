/*
 *  RSGISParseLandXML.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 06/07/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

/** 
* Reads information from the LandXML format. <href=http://www.landxml.org /> 
* GetLandXMLLine<br>
* Reads line segments from LandXML
* Lines are assumed to consist of two points joined together
* All lines are treated independently 
* 
*/

#ifndef RSGISParseLandXML_H
#define RSGISParseLandXML_H

#include <string>
#include <iostream>
#include <vector>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include "geos/geom/Coordinate.h"
#include "geos/geom/LineSegment.h"

#include <boost/tokenizer.hpp>

#include "math/RSGISMathsUtils.h"

#include "common/RSGISVectorException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_vec_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis { namespace vec {

	class DllExport RSGISParseLandXML
		{
		public:
			RSGISParseLandXML();
			void getLandXMLLine(std::string xmlFile, std::vector<geos::geom::LineSegment*> *outLines);
			~RSGISParseLandXML();
		protected:
            std::string xmlFile;
		};
}}

#endif
