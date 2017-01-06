/*
 *  RSGISMinimumDistanceClassifier.h
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

#ifndef RSGISMinimumDistanceClassifier_H
#define RSGISMinimumDistanceClassifier_H

#include <iostream>
#include <string>
#include "classifier/RSGISClassifier.h"
#include "math/RSGISMatrices.h"
#include "common/RSGISClassificationException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
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
    
	enum MinDistCentreType
	{
		mean = 0,
		middle = 1
	};
	
	class DllExport RSGISMinimumDistanceClassifier : public RSGISClassifier
		{
		public:
			RSGISMinimumDistanceClassifier(ClassData **trainingData, int numClasses, MinDistCentreType centreType)  throw(RSGISClassificationException);
			virtual int getClassID(float *variables, int numVars) throw(RSGISClassificationException);
			virtual std::string getClassName(float *variables, int numVars) throw(RSGISClassificationException);
			~RSGISMinimumDistanceClassifier();
		protected:
			void calcClusterCentres() throw(RSGISClassificationException);
			ClassData* findClass(float *variables, int numVars) throw(RSGISClassificationException);
			ClassData *clusterCentres;
			MinDistCentreType centreType;
		};
	
}}

#endif

