/*
 *  RSGISMaximumLikelihoodException.h
 *  RSGIS_LIB
 *
 *  Created by Sam Gillingham on 14/01/2014.
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

#ifndef RSGISMaximumLikelihoodException_H
#define RSGISMaximumLikelihoodException_H

#include "math/RSGISMathException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace math{        

    class DllExport RSGISMaximumLikelihoodException : public RSGISMathException
        {
        public:
            RSGISMaximumLikelihoodException();
            RSGISMaximumLikelihoodException(const char* message);
            RSGISMaximumLikelihoodException(std::string message);
        };
}}

#endif

