 /*
 *  RSGISPolynomialImageWarp.h
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


#ifndef RSGISPolynomialImageWarp_H
#define RSGISPolynomialImageWarp_H

#include <iostream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>

#include "geos/geom/Envelope.h"

#include "registration/RSGISImageWarpException.h"
#include "registration/RSGISWarpImage.h"
#include "registration/RSGISGCPImg2MapNode.h"

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
	
	class DllExport RSGISPolynomialImageWarp : public RSGISWarpImage
	{
	public:
		RSGISPolynomialImageWarp(std::string inputImage, std::string outputImage, std::string outProjWKT, std::string gcpFilePath, float outImgRes, RSGISWarpImageInterpolator *interpolator, unsigned int polyOrder = 2, std::string gdalFormat = "ENVI");
		void initWarp()throw(RSGISImageWarpException);
		~RSGISPolynomialImageWarp();
	protected:
		geos::geom::Envelope* newImageExtent(unsigned int width, unsigned int height) throw(RSGISImageWarpException);
		void findNearestPixel(double eastings, double northings, unsigned int *x, unsigned int *y, float inImgRes) throw(RSGISImageWarpException);
        int polyOrder; // Polynominal order
        gsl_vector *aX;
        gsl_vector *aY;
        gsl_vector *aE;
        gsl_vector *aN;
	};
	
}}

#endif





