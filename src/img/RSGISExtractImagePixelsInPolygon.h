/*
 *  RSGISExtractImagePixelsInPolygon.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/08/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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


#ifndef RSGISExtractImagePixelsInPolygon_h
#define RSGISExtractImagePixelsInPolygon_h

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "common/RSGISImageException.h"
#include "common/RSGISOutputStreamException.h"

#include "utils/RSGISGEOSFactoryGenerator.h"
#include "utils/RSGISExportForPlottingIncremental.h"

#include "img/RSGISImageBandException.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

#include <boost/cstdint.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "geos/geom/Envelope.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/PrecisionModel.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_img_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis 
{
	namespace img
	{        
        class DllExport RSGISExtractImagePixelsOnLine
        {
        public:
            RSGISExtractImagePixelsOnLine();
            std::vector<ImagePixelValuePt*>* getImagePixelValues(GDALDataset *image, unsigned int imageBand, geos::geom::Coordinate *pt1, float azimuthRad, float zenithRad, float rayElevThreshold) throw(RSGISImageCalcException);
            void populateWithImageValues(GDALDataset *image, unsigned int imageBand, std::vector<ImagePixelValuePt*> *ptPxlValues) throw(rsgis::RSGISImageException);
            ~RSGISExtractImagePixelsOnLine();
        };
    }
}


#endif
