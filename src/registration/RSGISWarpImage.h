 /*
 *  RSGISWarpImage.h
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

#ifndef RSGISWarpImage_H
#define RSGISWarpImage_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "geos/geom/Envelope.h"

#include "common/RSGISImageException.h"

#include "utils/RSGISTextUtils.h"

#include "math/RSGISMathsUtils.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageBandException.h"

#include "registration/RSGISImageWarpException.h"
#include "registration/RSGISWarpImageInterpolator.h"
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
    
	class DllExport RSGISWarpImage
	{
	public:
		RSGISWarpImage(std::string inputImage, std::string outputImage, std::string outProjWKT, std::string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, std::string gdalFormat);
		void performWarp() throw(RSGISImageWarpException);
        void generateTransformImage() throw(RSGISImageWarpException);
		void readGCPFile() throw(RSGISImageWarpException);
		void createOutputImage() throw(RSGISImageWarpException);
        void createOutputTransformImage() throw(RSGISImageWarpException);
		void populateOutputImage() throw(RSGISImageWarpException);
        void populateTransformImage() throw(RSGISImageWarpException);
		virtual void initWarp() throw(RSGISImageWarpException) = 0;
		virtual ~RSGISWarpImage();
	protected:
		virtual geos::geom::Envelope* newImageExtent(unsigned int width, unsigned int height) throw(RSGISImageWarpException) = 0;
		virtual void findNearestPixel(double eastings, double northings, unsigned int *x, unsigned int *y, float inImgRes) throw(RSGISImageWarpException) = 0;
        std::string inputImage;
		std::string outputImage;
		std::string outProjWKT;
		std::string gcpFilePath;
		std::vector<RSGISGCPImg2MapNode*> *gcps;
		float outImgRes;
		RSGISWarpImageInterpolator *interpolator;
        std::string gdalFormat;
	};
	
}}

#endif





