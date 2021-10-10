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

    /** Function to extract the all the pixel values for regions to a HDF5 file */
    DllExport void executeZonesImage2HDF5(std::string inputImage, std::string inputVecFile, std::string inputVecLyr, std::string outputHDF, bool ignoreProjection = false, int pixelInPolyMethodInt = 1);

    /** Function to extract the average endmember's defined by polygons for linear spectral unmixing */
    DllExport void executeExtractAvgEndMembers(std::string inputImage, std::string inputVecFile, std::string inputVecLyr, std::string outputMatrixFile,  int pixelInPolyMethodInt = 1);

    /** A function to extract image values to a HDF file */
    DllExport void executeImageRasterZone2HDF(std::string imageFile, std::string maskImage, std::string outputHDF, float maskVal, RSGISLibDataType dataType);

    /** A function to extract image band values to a HDF file */
    DllExport void executeImageBandRasterZone2HDF(std::vector<std::pair<std::string, std::vector<unsigned int> > > imageFiles, std::string maskImage, std::string outputHDF, float maskVal, RSGISLibDataType dataType);

    /** A function to sample a list of values saved in a HDF5 file */
    DllExport void executeRandomSampleH5File(std::string inputH5, std::string outputH5, unsigned int nSample, int seed, RSGISLibDataType dataType);

    /** A function to sample a list of values saved in a HDF5 file */
    DllExport void executeSplitSampleH5File(std::string inputH5, std::string outputP1H5, std::string outputP2H5, unsigned int nSample, int seed, RSGISLibDataType dataType);


}}


#endif

