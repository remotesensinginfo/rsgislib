/*
 *  RSGISImageFilterException.h
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

#ifndef RSGISImageFilterException_H
#define RSGISImageFilterException_H

#include "common/RSGISImageException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_filter_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis 
{
	namespace filter
	{
		class DllExport RSGISImageFilterException : public rsgis::RSGISImageException
			{
			public:
				RSGISImageFilterException();
				RSGISImageFilterException(const char* message);
				RSGISImageFilterException(std::string message);
			};
	}
}

#endif
