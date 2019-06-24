/*
 *  RSGISRegionGrowAttributeTable.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/02/2012.
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

#ifndef RSGISRegionGrowAttributeTable_H
#define RSGISRegionGrowAttributeTable_H

#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <list>

#include "common/RSGISImageException.h"
#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathsUtils.h"
//#include "rastergis/RSGISAttributeTable.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

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
/*
    class DllExport RSGISRegionGrowAttributeTable
    {
    public:
        RSGISRegionGrowAttributeTable();
        void growClassRegionsUsingThresholds(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, std::vector<rsgis::rastergis::RSGISIfStatement*> *statements, std::string classAttName, int classAttVal);
        ~RSGISRegionGrowAttributeTable();
    private:
        bool** createMask(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, unsigned int *width, unsigned int *height, unsigned int classFieldIdx, int classVal);
        void updateMask(bool **mask, rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, unsigned int width, unsigned int height, unsigned int classFieldIdx, int classVal);
        void getConnectedClumps(std::list<unsigned int> *connectedClumps, rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, bool **mask, unsigned int width, unsigned int height);
    };
*/
}}

#endif

