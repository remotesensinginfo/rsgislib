/*
 *  RSGISGenerateFilter.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/05/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISGenerateFilter_H
#define RSGISGenerateFilter_H

#include <iostream>
#include "math.h"
#include "filtering/RSGISCalcFilter.h"
#include "filtering/RSGISImageFilter.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_filter_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace filter{

	class DllExport RSGISGenerateFilter
		{
		public: 
			RSGISGenerateFilter(RSGISCalcFilter *calcFilter);
			ImageFilter* generateFilter(int size) throw(RSGISImageFilterException);
			~RSGISGenerateFilter();
		protected:
			RSGISCalcFilter *calcFilter;
		};
	
}}
#endif
