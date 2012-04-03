/*
 *  RSGISImageFootprintPolygonsCSVParse.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 26/01/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#ifndef RSGISImageFootprintPolygonsCSVParse_H
#define RSGISImageFootprintPolygonsCSVParse_H

#include <iostream>
#include <fstream>

#include <string>

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISTextException.h"

#include "common/RSGISInputStreamException.h"

#include "math/RSGISMathsUtils.h"

using namespace std;
using namespace rsgis;
using namespace rsgis::math;

namespace rsgis{ namespace utils{
	
	struct ImageFootPrintPoly
	{
		string scene;
		int fid;
		double ulE;
		double ulN;
		double urE;
		double urN;
		double lrE;
		double lrN;
		double llE;
		double llN;
	};
	
	class RSGISImageFootprintPolygonsCSVParse
	{
		public:
			RSGISImageFootprintPolygonsCSVParse();
			vector<ImageFootPrintPoly*>* parsePoly(string inputfile) throw(RSGISInputStreamException,RSGISTextException);
			string formatedString(ImageFootPrintPoly *poly);
			~RSGISImageFootprintPolygonsCSVParse();
	};
}}

#endif


