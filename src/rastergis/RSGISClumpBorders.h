/*
 *  RSGISClumpBorders.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/03/2013.
 *  Copyright 2013 RSGISLib.
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

#ifndef RSGISClumpBorders_H
#define RSGISClumpBorders_H

#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "rastergis/RSGISRasterAttUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_rastergis_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace rastergis{
    
    class DllExport RSGISClumpBorders
    {
    public:
        RSGISClumpBorders();
        void calcClumpBorderLength(GDALDataset *clumpImage, bool includeZeroEdges, std::string colName);
        void calcClumpRelBorderLen2Class(GDALDataset *clumpImage, bool includeZeroEdges, std::string colName, std::string classColName, std::string className);
        ~RSGISClumpBorders();

    };
    
    
    class DllExport RSGISCalcBorderLenInPixels : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCalcBorderLenInPixels(double *borderLen, size_t numRows, double xRes, double yRes, bool includeZeros=false);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
		~RSGISCalcBorderLenInPixels();
    protected:
        double *borderLen;
        size_t numRows;
        bool includeZeros;
        double xRes;
        double yRes;
	};
    
    class DllExport RSGISCalcBorderLenInPixelsWithClass : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCalcBorderLenInPixelsWithClass(double *borderLen, double *classBorderLen, std::string *classNames, size_t numRows, double xRes, double yRes, std::string className, bool includeZeros=false);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output);
		~RSGISCalcBorderLenInPixelsWithClass();
    protected:
        double *classBorderLen;
        double *borderLen;
        std::string *classNames;
        size_t numRows;
        bool includeZeros;
        double xRes;
        double yRes;
        std::string className;
	};
    
}}

#endif
