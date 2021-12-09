/*
 *  RSGISCmdVectorUtils.h
 *
 *
 *  Created by Pete Bunting on 04/07/2013.
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

#ifndef RSGISCmdVectorUtils_H
#define RSGISCmdVectorUtils_H

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

namespace rsgis{ namespace cmds{
    
    struct DllExport RSGISVariableFieldCmds
    {
        std::string name;
        std::string fieldName;
    };

    /** Function to calculate a maths functions between  */
    DllExport void executeVectorMaths(std::string inputVectorFile, std::string inputVectorLyr, std::string outputVectorFile, std::string outputVectorLyr, std::string outFormat, std::string outColumn, std::string expression, bool delExistVec, std::vector<RSGISVariableFieldCmds> vars);

    /** Function to convert a set of lines into regularly spaced set of points */
    DllExport void executeCreateLinesOfPoints(std::string inputVectorFile, std::string inputVectorLyr, std::string outputVectorFile, std::string outputVectorLyr, std::string outFormat, double step, bool delExistVec);

    /** Function to check and validate the geometries within the vector file */
    DllExport void executeCheckValidateGeometries(std::string inputVectorFile, std::string inputVectorLyr, std::string outputVectorFile, std::string outputVectorLyr, std::string outFormat, bool printGeomErrs, bool delExistVec);
}}


#endif

