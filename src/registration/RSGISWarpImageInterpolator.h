 /*
 *  RSGISWarpImageInterpolator.h
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

#ifndef RSGISWarpImageInterpolator_H
#define RSGISWarpImageInterpolator_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageBandException.h"

#include "registration/RSGISImageWarpException.h"

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
	
	class DllExport RSGISWarpImageInterpolator
	{
	public:
		virtual void calcValue(GDALDataset *image, float *outValues, unsigned int numOutVals, double eastings, double northings, unsigned int xPxl, unsigned int yPxl, float inImgRes, float outImgRes) = 0;
		virtual ~RSGISWarpImageInterpolator(){};
	};
		
	class DllExport RSGISWarpImageNNInterpolator : public RSGISWarpImageInterpolator
	{
	public:
		RSGISWarpImageNNInterpolator(){};
		void calcValue(GDALDataset *image, float *outValues, unsigned int numOutVals, double eastings, double northings, unsigned int xPxl, unsigned int yPxl, float inImgRes, float outImgRes);
		~RSGISWarpImageNNInterpolator(){};
	};
	
}}

#endif





