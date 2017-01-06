/*
 *  RSGISProbDistro.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 09/11/2009.
 *  Copyright 2009 RSGISLib.
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

#ifndef RSGISProbDistro_H
#define RSGISProbDistro_H

#include <math.h>

#include "common/RSGISCommons.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
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

	class DllExport RSGISProbDistro
	{
	public:
		RSGISProbDistro(){};
		virtual double calcProb (double value) = 0;
		virtual double calcRand () = 0;
		virtual ~RSGISProbDistro(){};
	};
	
}}

#endif

