/*
 *  RSGISNearestNeighbourClassifier.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/08/2008.
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

#ifndef RSGISNearestNeighbourClassifier_H
#define RSGISNearestNeighbourClassifier_H

#include <iostream>
#include <string>
#include "classifier/RSGISClassifier.h"
#include "math/RSGISMatrices.h"
#include "common/RSGISClassificationException.h"
#include <math.h>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_classify_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace classifier{
    
	class DllExport RSGISNearestNeighbourClassifier : public RSGISClassifier
		{
		public:
			RSGISNearestNeighbourClassifier(ClassData **trainingData, int numClasses);
			virtual int getClassID(float *variables, int numVars);
			virtual std::string getClassName(float *variables, int numVars);
			~RSGISNearestNeighbourClassifier();
		protected:
			ClassData* findClass(float *variables, int numVars);
			double findClosestPointInClass(ClassData *data, float *variables, int numVars);
		};

}}

#endif

