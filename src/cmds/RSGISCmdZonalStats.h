/*
 *  RSGISCmdZonalStats.h
 *
 *
 *  Created by Dan Clewley on 08/08/2013.
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

#ifndef RSGISCmdZonalStats_H
#define RSGISCmdZonalStats_H

#include <iostream>
#include <string>
#include <vector>

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

    struct DllExport RSGISZonalBandAttrsCmds
    {
        int band;
        std::string baseName;
        std::string minName;
        std::string maxName;
        std::string meanName;
        std::string stdName;
        std::string countName;
        std::string modeName;
        std::string medianName;
        std::string sumName;
        bool outMin;
        bool outMax;
        bool outMean;
        bool outStDev;
        bool outCount;
        bool outMode;
        bool outMedian;
        bool outSum;
        float minThres;
        float maxThres;
    };

    struct DllExport RSGISBandAttZonalStatsCmds
    {
        float minThreshold;
        float maxThreshold;
        bool calcCount;
        bool calcMin;
        bool calcMax;
        bool calcMean;
        bool calcStdDev;
        bool calcMode;
        bool calcSum;
    };

    /** Function to extract pixel value for points and save as a shapefile or CSV */
    DllExport void executePointValue(std::string inputImage, std::string inputVecPolys, std::string outputStatsFile, bool outputToText = false, bool force = false, bool useBandNames = true, bool shortenBandNames=true);
    /** Function to extract statistics for pixels falling within a polygon */
    DllExport void executePixelStats(std::string inputImage, std::string inputVecPolys, std::string outputStatsFile, RSGISBandAttZonalStatsCmds *calcStats, std::string inputRasPolys = "", bool outputToText = false, bool force = false, bool useBandNames = true, bool ignoreProjection = false, int pixelInPolyMethodInt = 1, bool shortenBandNames = true);
    /** Function to extract pixel values for each polygon and save to a seperate text file */
    DllExport void executePixelVals2txt(std::string inputImage, std::string inputVecPolys, std::string outputTextBase, std::string polyAttribute, std::string outtxtform = "csv", bool ignoreProjection = false, int pixelInPolyMethodInt = 1);
    /** Function to extract the all the pixel values for regions to a HDF5 file */
    DllExport void executeZonesImage2HDF5(std::string inputImage, std::string inputVecPolys, std::string outputHDF, bool ignoreProjection = false, int pixelInPolyMethodInt = 1);
    /** Function to extract the average endmember's defined by polygons for linear spectral unmixing */
    DllExport void executeExtractAvgEndMembers(std::string inputImage, std::string inputVecPolys, std::string outputMatrixFile,  int pixelInPolyMethodInt = 1);
    /** Function to extract statistics for pixels falling within a polygon */
    DllExport void executePixelBandStatsVecLyr(std::string inputImage, std::string vecfile, std::string veclyr, std::vector<RSGISZonalBandAttrsCmds> *zonBandAtts, int pixelInPolyMethodInt, bool ignoreProjection=false);

}}


#endif

