/*
 *  RSGISVectorSQLClassification.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/02/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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


#ifndef RSGISVectorSQLClassification_H
#define RSGISVectorSQLClassification_H

#include <iostream>
#include <string>
#include <math.h>
#include <vector>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_vec_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace vec{
	
	struct DllExport sqlclass
	{
        std::string name;
		std::string sql;
	};
	
	class DllExport RSGISVectorSQLClassification
		{
		public:
			RSGISVectorSQLClassification();
			void classifyVector(GDALDataset *source, OGRLayer *inputLayer, sqlclass **rules, int numRules, std::string classAttribute) throw(RSGISVectorException);
			~RSGISVectorSQLClassification();
		};
}}

#endif

