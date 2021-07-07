/*
 *  RSGISGeometryUtils.h
 *
 *
 *  Created by Pete Bunting on 07/07/2021.
 *  Copyright 2021 RSGISLib. All rights reserved.
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

#ifndef RSGISGeometryUtils_H
#define RSGISGeometryUtils_H

#include <vector>
#include <list>
#include <iostream>
#include <string>

#include "gdal_priv.h"

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

    class DllExport RSGISGeometryUtils
    {
    public:
        RSGISGeometryUtils(){};
        OGREnvelope* createOGREnvelopePointer(double minX, double maxX, double minY, double maxY);
        OGREnvelope createOGREnvelopeObject(double minX, double maxX, double minY, double maxY);
        ~RSGISGeometryUtils(){};
    };


}}




#endif
