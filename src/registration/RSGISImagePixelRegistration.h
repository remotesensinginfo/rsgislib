/*
 *  RSGISImagePixelRegistration.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/01/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
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


#ifndef RSGISImagePixelRegistration_H
#define RSGISImagePixelRegistration_H

#include <iostream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "common/RSGISRegistrationException.h"

#include "registration/RSGISImageRegistration.h"

#include "boost/math/special_functions/fpclassify.hpp"

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
    
	class DllExport RSGISImagePixelRegistration : public RSGISImageRegistration
	{
	public:
		RSGISImagePixelRegistration(GDALDataset *reference, GDALDataset *floating, std::string outputImagePath, std::string outputFormat, unsigned int windowSize, unsigned int searchArea, RSGISImageSimilarityMetric *metric, unsigned int subPixelResolution);
		void initRegistration()throw(RSGISRegistrationException);
		void executeRegistration()throw(RSGISRegistrationException);
		void finaliseRegistration()throw(RSGISRegistrationException);
		void exportTiePointsENVIImage2Map(std::string filepath)throw(RSGISRegistrationException);
		void exportTiePointsENVIImage2Image(std::string filepath)throw(RSGISRegistrationException);
		void exportTiePointsRSGISImage2Map(std::string filepath)throw(RSGISRegistrationException);
        void exportTiePointsRSGISMapOffs(std::string filepath)throw(RSGISRegistrationException);
		~RSGISImagePixelRegistration();
	private:
		std::string outputImagePath;
        std::string outputFormat;
        GDALDataset *outputImage;
		bool initExecuted;
		unsigned int windowSize;
		unsigned int searchArea;
		RSGISImageSimilarityMetric *metric;
		unsigned int subPixelResolution;
	};
}}

#endif



