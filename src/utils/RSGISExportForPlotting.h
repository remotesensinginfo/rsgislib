/*
 *  RSGISExportForPlotting.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/07/2009.
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

#ifndef RSGISExportForPlotting_H
#define RSGISExportForPlotting_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>

#include "common/RSGISException.h"
#include "common/RSGISOutputStreamException.h"

#include "geos/geom/LineSegment.h"

namespace rsgis 
{
	namespace utils
	{
        using namespace std;
        using namespace rsgis;
        using namespace geos::geom;
        
		class RSGISExportForPlotting
			{
			public: 
				static RSGISExportForPlotting* getInstance(string filepath);
				static RSGISExportForPlotting* getInstance() throw(RSGISException);
				string getFilePath();
				void export2DFreq(string filename, list<double> *values) throw(RSGISOutputStreamException);
				void export3DFreq(string filename, list<double> *values1, list<double> *values2) throw(RSGISOutputStreamException);
				void export2DLines(string filename, list<LineSegment> *lines) throw(RSGISOutputStreamException);
				void export3DLines(string filename, list<LineSegment> *lines) throw(RSGISOutputStreamException);
				void exportDensity(string filename, double *x, double *y, int numPts) throw(RSGISOutputStreamException);
				void export2DScatter(string filename, double *x, double *y, int numPts) throw(RSGISOutputStreamException);
				void export3DScatter(string filename, double *x, double *y, double *z, int numPts) throw(RSGISOutputStreamException);
				void export2DColourScatter(string filename, double *x, double *y, double *c, int numPts) throw(RSGISOutputStreamException);
				void export3DColourScatter(string filename, double *x, double *y, double *z, double *c, int numPts) throw(RSGISOutputStreamException);
				void exportSurface(string filename, double *x, double *y, double *z, int numPts) throw(RSGISOutputStreamException);
				void exportColourSurface(string filename, double *x, double *y, double *z, double *c, int numPts) throw(RSGISOutputStreamException);
				void exportTriangles2d(string filename, double *x, double *y, unsigned long numPts) throw(RSGISOutputStreamException);
				void exportTriangles3d(string filename, double *x, double *y, double *z, unsigned long numPts) throw(RSGISOutputStreamException);
				~RSGISExportForPlotting();
			private:
				RSGISExportForPlotting(string filepath);
				RSGISExportForPlotting(const RSGISExportForPlotting&){};
				RSGISExportForPlotting& operator=(const RSGISExportForPlotting&){return *instance;};
				string filepath;
				static RSGISExportForPlotting *instance;
			};
	}
}

#endif

