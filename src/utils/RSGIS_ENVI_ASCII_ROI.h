/*
 *  RSGIS_ENVI_ASCII_ROI.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/08/2008.
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

#ifndef RSGIS_ENVI_ASCII_ROI_H
#define RSGIS_ENVI_ASCII_ROI_H

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "utils/RSGISColour.h"
#include "utils/RSGISTextUtils.h"
#include "common/RSGISInputStreamException.h"
#include "math/RSGISMatrices.h"
#include "utils/RSGISENVIROIException.h"

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
    
	struct DllExport enviroi
	{
		std::string name;
		int samples;
		RSGISColour *colour;
		rsgis::math::Matrix *data;
	};
	
	class DllExport RSGISReadENVIASCIIROI
		{
		public:
			RSGISReadENVIASCIIROI(std::string file);
			void printROIs();
			int getNumROIs();
			std::string* getName(int i);
			rsgis::math::Matrix* getMatrix(int i);
			int getNumSamples(int i);
			RSGISColour* getColour(int i);
			int getNumVariables();
			enviroi* getENVIROI(int i);
			~RSGISReadENVIASCIIROI();
		protected:
			void parsefile();
			std::string inputfile;
			enviroi *rois;
			int numrois;
			int numVariables;
		};
}}

#endif
