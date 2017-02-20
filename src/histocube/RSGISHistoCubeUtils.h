/*
 *  RSGISHistoCubeUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/02/2017.
 *
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

#ifndef RSGISHistoCubeUtils_H
#define RSGISHistoCubeUtils_H


// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
#ifdef rsgis_histocube_EXPORTS
#define DllExport   __declspec( dllexport )
#else
#define DllExport   __declspec( dllimport )
#endif
#else
#define DllExport
#endif


#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include "common/RSGISHistoCubeException.h"

#include "RSGISHistoCubeFileIO.h"

namespace rsgis {namespace histocube{
    
    
    class DllExport RSGISHistoCubeUtils
    {
    public:
        RSGISHistoCubeUtils();
        long getBinsIndex(int val, std::vector<int> bins) throw(rsgis::RSGISHistoCubeException);
        ~RSGISHistoCubeUtils();
    };
    
}}

#endif

