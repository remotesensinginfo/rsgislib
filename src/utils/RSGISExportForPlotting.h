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
        
    class DllExport RSGISExportForPlotting
        {
        public: 
            static RSGISExportForPlotting* getInstance(std::string filepath);
            static RSGISExportForPlotting* getInstance() throw(rsgis::RSGISException);
            std::string getFilePath();
            void export2DFreq(std::string filename, std::list<double> *values) throw(rsgis::RSGISOutputStreamException);
            void export3DFreq(std::string filename, std::list<double> *values1, std::list<double> *values2) throw(rsgis::RSGISOutputStreamException);
            void export2DLines(std::string filename, std::list<geos::geom::LineSegment> *lines) throw(rsgis::RSGISOutputStreamException);
            void export3DLines(std::string filename, std::list<geos::geom::LineSegment> *lines) throw(rsgis::RSGISOutputStreamException);
            void exportDensity(std::string filename, double *x, double *y, int numPts) throw(rsgis::RSGISOutputStreamException);
            void export2DScatter(std::string filename, double *x, double *y, int numPts) throw(rsgis::RSGISOutputStreamException);
            void export3DScatter(std::string filename, double *x, double *y, double *z, int numPts) throw(rsgis::RSGISOutputStreamException);
            void export2DColourScatter(std::string filename, double *x, double *y, double *c, int numPts) throw(rsgis::RSGISOutputStreamException);
            void export3DColourScatter(std::string filename, double *x, double *y, double *z, double *c, int numPts) throw(rsgis::RSGISOutputStreamException);
            void exportSurface(std::string filename, double *x, double *y, double *z, int numPts) throw(rsgis::RSGISOutputStreamException);
            void exportColourSurface(std::string filename, double *x, double *y, double *z, double *c, int numPts) throw(rsgis::RSGISOutputStreamException);
            void exportTriangles2d(std::string filename, double *x, double *y, unsigned long numPts) throw(rsgis::RSGISOutputStreamException);
            void exportTriangles3d(std::string filename, double *x, double *y, double *z, unsigned long numPts) throw(rsgis::RSGISOutputStreamException);
            ~RSGISExportForPlotting();
        private:
            RSGISExportForPlotting(std::string filepath);
            RSGISExportForPlotting(const RSGISExportForPlotting&){};
            RSGISExportForPlotting& operator=(const RSGISExportForPlotting&){return *instance;};
            std::string filepath;
            static RSGISExportForPlotting *instance;
        };
    
}}

#endif

