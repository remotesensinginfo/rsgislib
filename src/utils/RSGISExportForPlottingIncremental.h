/*
 *  RSGISExportForPlottingIncremental.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/10/2009.
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

#ifndef RSGISExportForPlottingIncremental_H
#define RSGISExportForPlottingIncremental_H

#include <string>
#include <iostream>
#include <list>

#include "common/RSGISOutputStreamException.h"

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISParseExportForPlotting.h"

#include "math/RSGISMathsUtils.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
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
    
	class DllExport RSGISExportForPlottingIncremental
	{
	public:
		RSGISExportForPlottingIncremental();
		bool openFile(std::string file, PlotTypes inType) throw(rsgis::RSGISOutputStreamException);
		void writeFrequency2DLine(double value) throw(rsgis::RSGISOutputStreamException);
		void writeFrequency3DLine(double value1, double value2) throw(rsgis::RSGISOutputStreamException);
		void writeScatterNDLine(std::vector<double> *vals) throw(rsgis::RSGISOutputStreamException);
		void writeScatter2DLine(double x, double y) throw(rsgis::RSGISOutputStreamException);
		void writeScatter3DLine(double x, double y, double z) throw(rsgis::RSGISOutputStreamException);
		void writeCScatter2DLine(double x, double y, double c) throw(rsgis::RSGISOutputStreamException);
		void writeCScatter3DLine(double x, double y, double z, double c) throw(rsgis::RSGISOutputStreamException);
		void writeDensityLine(double x, double y) throw(rsgis::RSGISOutputStreamException);
		void writeSurfaceLine(double x, double y, double z) throw(rsgis::RSGISOutputStreamException);
		void writeCSurfaceLine(double x, double y, double z, double c) throw(rsgis::RSGISOutputStreamException);
		void writeLines2DLine(double x1, double y1, double x2, double y2) throw(rsgis::RSGISOutputStreamException);
		void writeLines3DLine(double x1, double y1, double z1, double x2, double y2, double z2) throw(rsgis::RSGISOutputStreamException);
		void writeTriangle2D(double x1, double y1, double x2, double y2, double x3, double y3) throw(rsgis::RSGISOutputStreamException);
		void writeTriangle3D(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3) throw(rsgis::RSGISOutputStreamException);
		PlotTypes getPlotType();
		void close();
		~RSGISExportForPlottingIncremental();
	private:
		std::ofstream *outputFileStream;
		bool open;
		PlotTypes type;
	};
}}

#endif



