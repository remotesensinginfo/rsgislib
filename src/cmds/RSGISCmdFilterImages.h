/*
 *  RSGISCmdFilterImages.h
 *
 *
 *  Created by Dan Clewley on 09/11/2013.
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

#ifndef RSGISCmdFilterImages_H
#define RSGISCmdFilterImages_H

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

    struct DllExport RSGISFilterParameters
    {
        std::string type;
        std::string fileEnding;
        std::string option;
        unsigned int size;
        unsigned int nLooks;
        float stddev;
        float stddevX;
        float stddevY;
        float angle;
    };

    /** Function to apply filters to an image */
    DllExport void executeFilter(std::string inputImage, std::vector <rsgis::cmds::RSGISFilterParameters*> *filterParameters, std::string outputImageBase, std::string imageFormat, std::string imageExt, RSGISLibDataType outDataType) throw(RSGISCmdException);

    /** Function to set up LeuncMalik Filter Band */
    DllExport std::vector<rsgis::cmds::RSGISFilterParameters*> *createLeungMalikFilterBank() throw(RSGISCmdException);
    
}}


#endif

