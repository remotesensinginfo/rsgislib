/*
 *  RSGISVectorProcessing.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/01/2009.
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

#ifndef RSGISVectorProcessing_H
#define RSGISVectorProcessing_H

#include <iostream>
#include <string>
#include "ogrsf_frmts.h"
#include "math.h"

#include "vec/RSGISPolygonData.h"
#include "vec/RSGISPointData.h"
#include "vec/RSGISCirclePoint.h"
#include "vec/RSGISCirclePolygon.h"
#include "vec/RSGISVectorIO.h"

#include "geom/RSGISGeometry.h"

#include "math/RSGISMathsUtils.h"

#include "utils/RSGISFileUtils.h"
#include "utils/RSGISPlotPolygonsCSVParse.h"
#include "utils/RSGISImageFootprintPolygonsCSVParse.h"
#include "utils/RSGISGEOSFactoryGenerator.h"

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

namespace rsgis{namespace vec{
	
	class DllExport RSGISVectorProcessing
		{
		public:
			RSGISVectorProcessing();
            geos::geom::Polygon* bufferPolygon(geos::geom::Polygon *polygon, double distance);
			void generateCircles(RSGISCirclePoint **points, RSGISCirclePolygon **circles, int numFeatures, float resolution);
			void listAttributes(OGRLayer *inputSHPLayer);
			void printAttribute(OGRLayer *inputLayer, std::string attribute);
			void splitFeatures(OGRLayer *inputLayer, std::string outputBase, bool force);
			void createPlotPolygons(std::vector<rsgis::utils::PlotPoly*> *polyDetails, std::string output, bool force) throw(RSGISVectorException);
			void createImageFootprintPolygons(std::vector<rsgis::utils::ImageFootPrintPoly*> *polyDetails, std::string output, bool force) throw(RSGISVectorException);
			void createGrid(std::string outputShapefile, OGRSpatialReference* spatialRef, bool deleteIfPresent, double xTLStart, double yTLStart, double resolutionX, double resolutionY, double width, double height) throw(RSGISVectorException);
            float calcMeanMinDistance(std::vector<OGRGeometry*> *geometries) throw(RSGISVectorException);
            ~RSGISVectorProcessing();
		};
}}

#endif


