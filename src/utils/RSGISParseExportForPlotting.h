/*
 *  RSGISParseExportForPlotting.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/07/2009.
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

#ifndef RSGISParseExportForPlotting_H
#define RSGISParseExportForPlotting_H

#include <string>
#include <iostream>
#include <list>

#include "common/RSGISInputStreamException.h"

#include "utils/RSGISTextUtils.h"

#include "math/RSGISMathsUtils.h"

namespace rsgis{namespace utils{
	
    using namespace std;
    using namespace rsgis;
    using namespace rsgis::math;
    
	enum PlotTypes
	{
		unknown,
		freq2d,
		freq3d,
		scatternd,
		scatter2d,
		scatter3d,
		cscatter2d,
		cscatter3d,
		density,
		surface,
		csurface,
		lines2d,
		lines3d,
		triangle2d,
		triangle3d
	};
	
	class RSGISParseExportForPlotting
		{
		public:
			RSGISParseExportForPlotting();
			PlotTypes identify(string file) throw(RSGISInputStreamException);	
			void parseFrequency2D(string file, list<double> *value) throw(RSGISInputStreamException);
			void parseFrequency3D(string file, list<double> *value1, list<double> *value2) throw(RSGISInputStreamException);
			void parseScatter2D(string file, list<double> *x, list<double> *y) throw(RSGISInputStreamException);
			void parseScatter3D(string file, list<double> *x, list<double> *y, list<double> *z) throw(RSGISInputStreamException);
			void parseCScatter2D(string file, list<double> *x, list<double> *y, list<double> *c) throw(RSGISInputStreamException);
			void parseCScatter3D(string file, list<double> *x, list<double> *y, list<double> *z, list<double> *c) throw(RSGISInputStreamException);
			void parseDensity(string file, list<double> *x, list<double> *y) throw(RSGISInputStreamException);
			void parseSurface(string file, list<double> *x, list<double> *y, list<double> *z) throw(RSGISInputStreamException);
			void parseCSurface(string file, list<double> *x, list<double> *y, list<double> *z, list<double> *c) throw(RSGISInputStreamException);
			void parseLines2D(string file, list<double> *x1, list<double> *y1, list<double> *x2, list<double> *y2) throw(RSGISInputStreamException);
			void parseLines3D(string file, list<double> *x1, list<double> *y1, list<double> *z1, list<double> *x2, list<double> *y2, list<double> *z2) throw(RSGISInputStreamException);
			void parseTriangles2D(string file, list<double> *x, list<double> *y) throw(RSGISInputStreamException);
			void parseTriangles3D(string file, list<double> *x, list<double> *y, list<double> *z) throw(RSGISInputStreamException);
			~RSGISParseExportForPlotting();
		};
}}

#endif

