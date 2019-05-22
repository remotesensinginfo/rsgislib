/*
 *  RSGISCmdCommon.h
 *
 *
 *  Created by Pete Bunting on 03/07/2013.
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

#ifndef RSGISCmdCommon_H
#define RSGISCmdCommon_H

#include <iostream>
#include <string>

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
    
    struct DllExport RSGISCmdStatsSummary
    {
        bool calcMin;
        bool calcMax;
        bool calcMean;
        bool calcSum;
        bool calcStdDev;
        bool calcMedian;
        bool calcMode;
        double min;
        double max;
        double mean;
        double sum;
        double stdDev;
        double median;
        double mode;
    };
    
}}


#endif

