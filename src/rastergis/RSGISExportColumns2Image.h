/*
 *  RSGISExportColumns2Image.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/09/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISExportColumns2Image_H
#define RSGISExportColumns2Image_H

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/rsgis-tqdm.h"
#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

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
    
    class DllExport RSGISExportColumns2ImageCalcImage : public rsgis::img::RSGISCalcImageValue
	{
	public: 
		RSGISExportColumns2ImageCalcImage(int numberOutBands, GDALRasterAttributeTable *attTable, unsigned int columnIndex);
        void loadColumn(int columnIndex);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
		~RSGISExportColumns2ImageCalcImage();
    protected:
        GDALRasterAttributeTable *attTable;
        unsigned int nRows;
        double *columnData;
	};
	
}}

#endif



