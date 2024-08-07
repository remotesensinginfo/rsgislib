/*
 *  RSGISCalcImageStatsAndPyramids.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/05/2014.
 *  Copyright 2014 RSGISLib.
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

#ifndef RSGISCalcImageStatsAndPyramids_H
#define RSGISCalcImageStatsAndPyramids_H

#include <string>
#include <vector>
#include <cmath>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"
#include "rastergis/RSGISRasterAttUtils.h"

#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

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
    
    
    class DllExport RSGISPopulateWithImageStats
    {
    public:
        RSGISPopulateWithImageStats();
        void populateImageWithRasterGISStats(GDALDataset *clumpsDataset, bool addColourTable, bool calcImagePyramids, bool ignoreZero, unsigned int ratBand);
        void populateImageWithRasterGISStats(GDALDataset *clumpsDataset, bool addColourTable, bool ignoreZero, unsigned int ratBand);
        void calcPyramids(GDALDataset *clumpsDataset);
        ~RSGISPopulateWithImageStats();
    };
    
    
    class DllExport RSGISGetClumpsHistogram : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISGetClumpsHistogram(size_t *histogram, size_t maxVal, unsigned int band);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals);
		~RSGISGetClumpsHistogram();
    private:
        size_t *histogram;
        size_t maxVal;
        unsigned int band;
	};
    
    
    
    
}}

#endif

