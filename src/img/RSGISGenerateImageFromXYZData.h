/*
 *  RSGISGenerateImageFromXYZData.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/02/2011.
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

#ifndef RSGISGenerateImageFromXYZData_H
#define RSGISGenerateImageFromXYZData_H

#include <iostream>
#include <fstream>

#include "gdal_priv.h"

#include "common/RSGISImageException.h"
#include "common/RSGISFileException.h"

#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageUtils.h"

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

namespace rsgis { namespace img {	
		
	class DllExport RSGISGenerateImageFromXYZData
	{
		struct XYZData
		{
			double x;
			double y;
			double z;
		};
		
	public:
		RSGISGenerateImageFromXYZData(std::string inputDataFile, std::string outputFile, char delimiter, std::string proj4, bool xyOrder, float resolution);
		void createImageRepresentingXYZData() throw(rsgis::RSGISFileException, rsgis::RSGISImageException);
		void readInputData() throw(rsgis::RSGISFileException);
		~RSGISGenerateImageFromXYZData();
	protected:
        std::string inputFile;
        std::string outputFile;
		char delimiter;
		bool xyOrder;
        std::string proj4;
        std::vector<XYZData*> *data;
		double *bbox;
		float resolution;
	};
	
}}

#endif

