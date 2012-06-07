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

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISTextException.h"

#include "common/RSGISInputStreamException.h"

#include "math/RSGISMathsUtils.h"

namespace rsgis{ namespace utils{
	
    using namespace std;
    using namespace rsgis;
    using namespace rsgis::math;
    
	struct PlotPoly
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
		string siteName;
		string study;
		string site;
		int extraID;
		string issues;
	};
	
	class RSGISPlotPolygonsCSVParse
	{
		public:
			RSGISPlotPolygonsCSVParse();
			vector<PlotPoly*>* parsePolyPlots(string inputfile) throw(RSGISInputStreamException,RSGISTextException);
			string formatedString(PlotPoly *poly);
			~RSGISPlotPolygonsCSVParse();
	};
}}

#endif


