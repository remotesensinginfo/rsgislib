/*
 *  RSGISPlotPolygonsCSVParse.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/09/2009.
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

#ifndef RSGISPlotPolygonsCSVParse_H
#define RSGISPlotPolygonsCSVParse_H

#include <iostream>
#include <fstream>

#include <string>
#include <vector>

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISTextException.h"

#include "common/RSGISInputStreamException.h"

#include "math/RSGISMathsUtils.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_utils_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace utils{
    
	struct DllExport PlotPoly
	{
		int fid;
		int zone;
		double eastings;
		double northings;
		double sw_Eastings;
		double sw_Northings;
		double eastSide;
		double northSide;
		double orientation;
		std::string siteName;
		std::string study;
		std::string site;
		int extraID;
		std::string issues;
	};
	
	class DllExport RSGISPlotPolygonsCSVParse
	{
		public:
			RSGISPlotPolygonsCSVParse();
			std::vector<PlotPoly*>* parsePolyPlots(std::string inputfile);
			std::string formattedString(PlotPoly *poly);
			~RSGISPlotPolygonsCSVParse();
	};
}}

#endif


