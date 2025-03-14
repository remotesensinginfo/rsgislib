/*
 *  RSGISClumpPxls.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/01/2012.
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

#ifndef RSGISClumpPxls_h
#define RSGISClumpPxls_h

#include <iostream>
#include <vector>
#include <queue>
#include <cmath>

#include "gdal_priv.h"

#include "common/rsgis-tqdm.h"

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"

#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include "utils/RSGISTextUtils.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_segmentation_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace segment{

    class DllExport RSGISClumpPxls
    {
    public:
        RSGISClumpPxls();
        void performClump(GDALDataset *catagories, GDALDataset *clumps, bool noDataValProvided, unsigned int noDataVal, std::vector<unsigned int> *clumpPxlVals=NULL);
        void performClumpPosVals(GDALDataset *catagories, GDALDataset *clumps);
        void performMultiBandClump(std::vector<GDALDataset*> *catagories, std::string clumpsOutputPath, std::string outFormat, bool noDataValProvided, unsigned int noDataVal, bool addRatPxlVals=false);
        ~RSGISClumpPxls();
    protected:
        inline bool allValueEqual(unsigned int *vals, unsigned int numVals, unsigned int equalVal);
        inline bool allValueEqual(unsigned int *vals1, unsigned int *vals2, unsigned int numVals);
    };
    
    class DllExport RSGISRelabelClumps
    {
    public:
        RSGISRelabelClumps();
        void relabelClumps(GDALDataset *catagories, GDALDataset *clumps);
        void relabelClumpsCalcImg(GDALDataset *catagories, GDALDataset *clumps);
        ~RSGISRelabelClumps();
    };
    
    class DllExport RSGISCreateRelabelLookupTable : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCreateRelabelLookupTable(unsigned long *clumpIdxLookUp, unsigned long numVals);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals);
        ~RSGISCreateRelabelLookupTable();
    protected:
        unsigned long *clumpIdxLookUp;
        unsigned long numVals;
        unsigned long nextVal;
	};
    
    
    class DllExport RSGISApplyRelabelLookupTable : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISApplyRelabelLookupTable(unsigned long *clumpIdxLookUp, unsigned long numVals);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output);
        ~RSGISApplyRelabelLookupTable();
    protected:
        unsigned long *clumpIdxLookUp;
        unsigned long numVals;
	};
    
}}

#endif
