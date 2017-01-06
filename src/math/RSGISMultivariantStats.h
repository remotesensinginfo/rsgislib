/*
 *  RSGISMultivariantStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/08/2008.
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

#ifndef RSGISMultivariantStats_H
#define RSGISMultivariantStats_H

#include "RSGISMatrices.h"
#include "RSGISMultivariantStatsException.h"
#include "RSGISMatricesException.h"

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
    	
	class DllExport RSGISMultivariantStats
		{
		public:
			RSGISMultivariantStats();
			Matrix* findMeanVector(Matrix *inputData) throw(RSGISMatricesException);
			Matrix* standardiseMatrix(Matrix *inputData, Matrix *meanVector) throw(RSGISMatricesException,RSGISMultivariantStatsException);
			float calcCovariance(Matrix *inputData, int var1, int var2, Matrix *meanVector) throw(RSGISMatricesException,RSGISMultivariantStatsException); 
			Matrix* calcCovarianceMatrix(Matrix *inputData, Matrix *meanVector) throw(RSGISMatricesException,RSGISMultivariantStatsException);
			~RSGISMultivariantStats();
		};
}}

#endif

