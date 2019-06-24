 /*
 *  RSGISBasicNNGCPImageWarp.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/09/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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


#ifndef RSGISBasicNNGCPImageWarp_H
#define RSGISBasicNNGCPImageWarp_H

#include <iostream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "geos/geom/Envelope.h"
#include "geos/index/quadtree/Quadtree.h" 

#include "registration/RSGISImageWarpException.h"
#include "registration/RSGISWarpImage.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
    	
	class DllExport RSGISBasicNNGCPImageWarp : public RSGISWarpImage
	{
	public:
		RSGISBasicNNGCPImageWarp(std::string inputImage, std::string outputImage, std::string outProjWKT, std::string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, std::string gdalFormat);
		void initWarp();
		~RSGISBasicNNGCPImageWarp();
	protected:
		geos::geom::Envelope* newImageExtent(unsigned int width, unsigned int height);
		void findNearestPixel(double eastings, double northings, unsigned int *x, unsigned int *y, float inImgRes);
        geos::index::quadtree::Quadtree *pointIndex;
	};
	
}}

#endif





