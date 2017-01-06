/*
 *  RSGISAddGCPsGDAL.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 06/06/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 *
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef RSGISAddGCPsGDAL_H
#define RSGISAddGCPsGDAL_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "utils/RSGISTextUtils.h"
#include "common/RSGISImageException.h"
#include "math/RSGISMathsUtils.h"

#include "registration/RSGISImageWarpException.h"
#include "registration/RSGISGCPImg2MapNode.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_registration_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace reg{
    
	class DllExport RSGISAddGCPsGDAL
	{
	public:
        RSGISAddGCPsGDAL(std::string inFileName, std::string gcpFilePath, std::string outFileName = "",  std::string gdalFormat = "KEA", GDALDataType gdalDataType = GDT_Float32);
        void readGCPFile(std::string gcpFilePath, std::vector<RSGISGCPImg2MapNode*> *gcps) throw(RSGISImageWarpException);
        void convertRSGIS2GDALGCP(std::vector<RSGISGCPImg2MapNode*> *gcps, GDAL_GCP *gdalGCPList) throw(RSGISImageWarpException);
        void copyImageWithoutSpatialRef(std::string inFileName, std::string outFileName, std::string gdalFormat, GDALDataType gdalDataType) throw(RSGISImageWarpException);
        ~RSGISAddGCPsGDAL(){};
	};
	
}}

#endif





