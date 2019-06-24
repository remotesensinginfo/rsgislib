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

namespace rsgis{namespace utils{
    
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
	
	class DllExport RSGISParseExportForPlotting
		{
		public:
			RSGISParseExportForPlotting();
			PlotTypes identify(std::string file);	
			void parseFrequency2D(std::string file, std::list<double> *value);
			void parseFrequency3D(std::string file, std::list<double> *value1, std::list<double> *value2);
			void parseScatter2D(std::string file, std::list<double> *x, std::list<double> *y);
			void parseScatter3D(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z);
			void parseCScatter2D(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *c);
			void parseCScatter3D(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z, std::list<double> *c);
			void parseDensity(std::string file, std::list<double> *x, std::list<double> *y);
			void parseSurface(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z);
			void parseCSurface(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z, std::list<double> *c);
			void parseLines2D(std::string file, std::list<double> *x1, std::list<double> *y1, std::list<double> *x2, std::list<double> *y2);
			void parseLines3D(std::string file, std::list<double> *x1, std::list<double> *y1, std::list<double> *z1, std::list<double> *x2, std::list<double> *y2, std::list<double> *z2);
			void parseTriangles2D(std::string file, std::list<double> *x, std::list<double> *y);
			void parseTriangles3D(std::string file, std::list<double> *x, std::list<double> *y, std::list<double> *z);
			~RSGISParseExportForPlotting();
		};
}}

#endif

