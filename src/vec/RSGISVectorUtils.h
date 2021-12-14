/*
 *  RSGISVectorUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/04/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#ifndef RSGISVectorUtils_H
#define RSGISVectorUtils_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <list>

#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "common/RSGISVectorException.h"
#include "utils/RSGISFileUtils.h"

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
    
	class DllExport RSGISVectorUtils
		{
		public:
			std::string getLayerName(std::string filepath);
            bool checkDIR4SHP(std::string dir, std::string shp);
            void deleteSHP(std::string dir, std::string shp);
            OGRPolygon* createOGRPolygon(double tlX, double tlY, double brX, double brY);
            OGRPolygon* checkCloseOGRPolygon(OGRPolygon *poly);
            OGRPolygon* removeHolesOGRPolygon(OGRPolygon *poly);
            OGRPolygon* removeHolesOGRPolygon(OGRPolygon *poly, float areaThreshold);
            OGRPolygon* moveOGRPolygon(OGRPolygon *poly, double shiftX, double shiftY, double shiftZ);
            std::vector<std::string>* findUniqueVals(OGRLayer *layer, std::string attribute);
            std::vector<std::string>* getColumnNames(OGRLayer *layer);
            std::vector<std::string> getColumnNamesLitVec(OGRLayer *layer);
            std::vector<OGRPoint*>* getRegularStepPoints(std::vector<OGRLineString*> *lines, double step);
            OGREnvelope* getEnvelope(OGRGeometry *geom);
            OGREnvelope* getEnvelopePixelBuffer(OGRGeometry *geom, double imageRes);
		};

    
}}

#endif


