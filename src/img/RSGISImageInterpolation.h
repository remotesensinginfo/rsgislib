/*
 *  RSGISImageInterpolation.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/05/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISImageInterpolation_H
#define RSGISImageInterpolation_H

#include <iostream>
#include <string>

#include "gdal_priv.h"

#include "common/RSGISFileException.h"
#include "common/RSGISImageException.h"

#include "img/RSGISImageInterpolator.h"

#include "math/RSGIS2DInterpolation.h"

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

namespace rsgis{namespace img{
	
	class DllExport RSGISImageInterpolation
		{
		public:
			RSGISImageInterpolation(RSGISInterpolator *interpolator);
			void interpolateNewImage(GDALDataset *data,
											 double outputXResolution, 
											 double outputYResolution, 
                                             std::string filename)
			throw(rsgis::RSGISFileException, rsgis::RSGISImageException);
			void interpolateNewImage(GDALDataset *data,
											 double outputXResolution, 
											 double outputYResolution, 
											 std::string filename,
											 int band)
			throw(rsgis::RSGISFileException, rsgis::RSGISImageException);
			void findOutputResolution(GDALDataset *dataset, float scale, int *outResolutionX, int *outResolutionY);
			~RSGISImageInterpolation();
		protected:
			double findFloatingPointComponent(double floatingPointNum, int *integer); 
			RSGISInterpolator *interpolator;
		};
    
    
    class DllExport RSGISPopulateImageFromInterpolator
    {
    public:
        RSGISPopulateImageFromInterpolator();
        void populateImage(rsgis::math::RSGIS2DInterpolator *interpolator, GDALDataset *image)throw(rsgis::RSGISImageException, rsgis::math::RSGISInterpolationException);
        ~RSGISPopulateImageFromInterpolator();
    };
    
}}

#endif
