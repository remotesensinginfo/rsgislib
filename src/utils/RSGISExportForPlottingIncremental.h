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

using namespace std;
using namespace rsgis;
using namespace rsgis::math;

namespace rsgis{namespace utils{
	
	class RSGISExportForPlottingIncremental
	{
	public:
		RSGISExportForPlottingIncremental();
		bool openFile(string file, PlotTypes inType) throw(RSGISOutputStreamException);
		void writeFrequency2DLine(double value) throw(RSGISOutputStreamException);
		void writeFrequency3DLine(double value1, double value2) throw(RSGISOutputStreamException);
		void writeScatterNDLine(vector<double> *vals) throw(RSGISOutputStreamException);
		void writeScatter2DLine(double x, double y) throw(RSGISOutputStreamException);
		void writeScatter3DLine(double x, double y, double z) throw(RSGISOutputStreamException);
		void writeCScatter2DLine(double x, double y, double c) throw(RSGISOutputStreamException);
		void writeCScatter3DLine(double x, double y, double z, double c) throw(RSGISOutputStreamException);
		void writeDensityLine(double x, double y) throw(RSGISOutputStreamException);
		void writeSurfaceLine(double x, double y, double z) throw(RSGISOutputStreamException);
		void writeCSurfaceLine(double x, double y, double z, double c) throw(RSGISOutputStreamException);
		void writeLines2DLine(double x1, double y1, double x2, double y2) throw(RSGISOutputStreamException);
		void writeLines3DLine(double x1, double y1, double z1, double x2, double y2, double z2) throw(RSGISOutputStreamException);
		void writeTriangle2D(double x1, double y1, double x2, double y2, double x3, double y3) throw(RSGISOutputStreamException);
		void writeTriangle3D(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3) throw(RSGISOutputStreamException);
		PlotTypes getPlotType();
		void close();
		~RSGISExportForPlottingIncremental();
	private:
		ofstream *outputFileStream;
		bool open;
		PlotTypes type;
	};
}}

#endif



