/*
 *  RSGISCmdHistoCube.h
 *
 *
 *  Created by Pete Bunting on 17/02/2017.
 *  Copyright 2017 RSGISLib.
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

#ifndef RSGISCmdHistoCube_H
#define RSGISCmdHistoCube_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
#ifdef rsgis_cmds_EXPORTS
#define DllExport   __declspec( dllexport )
#else
#define DllExport   __declspec( dllimport )
#endif
#else
#define DllExport
#endif

namespace rsgis{ namespace cmds {
    
    enum RSGISCmdsHistSummariseStats
    {
        rsgiscmds_hstat_none = 0,
        rsgiscmds_hstat_min = 1,
        rsgiscmds_hstat_max = 2,
        rsgiscmds_hstat_mean = 3,
        rsgiscmds_hstat_median = 4,
        rsgiscmds_hstat_range = 5,
        rsgiscmds_hstat_stddev = 6,
        rsgiscmds_hstat_sum = 7,
        rsgiscmds_hstat_mode = 8
    };
    
    /** A function to create an empty histocube file ready to be populated */
    DllExport void executeCreateEmptyHistoCube(std::string histCubeFile, unsigned long numFeats);
    
    /** A function to create a zero'd histocube layer */
    DllExport void executeCreateHistoCubeLayer(std::string histCubeFile, std::string layerName, int lowBin, int upBin, float scale, float offset, bool hasDateTime, std::string dataTime);
    
    /** A function to populate a single histogram layer from multiple input files */
    DllExport void executePopulateSingleHistoCubeLayer(std::string histCubeFile, std::string layerName, std::string clumpsImg, std::string valsImg, unsigned int imgBand, bool inMem);
    
    /** A function to export histogram columns as a multi-band image dataset */
    DllExport void executeExportHistBins2Img(std::string histCubeFile, std::string layerName, std::string clumpsImg, std::string outputImg, std::string gdalFormat, std::vector<unsigned int> exportBins);
    
    /** A function to get a list of all the datasets layers within a histocube file*/
    DllExport std::vector<std::string> executeExportHistBins2Img(std::string histCubeFile);
    
    /** A function to export statistics from each histogram to an output image */
    DllExport void executeExportHistStats2Img(std::string histCubeFile, std::string layerName, std::string clumpsImg, std::string outputImg, std::string gdalFormat, RSGISLibDataType outDataType, std::vector<RSGISCmdsHistSummariseStats> exportStats);

}}


#endif


