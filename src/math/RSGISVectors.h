/*
 *  RSGISVectors.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 21/01/2009.
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

#ifndef RSGISVectors_H
#define RSGISVectors_H

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <gsl/gsl_vector.h>

#include "math/RSGISVectorsException.h"
#include "common/RSGISInputStreamException.h"
#include "common/RSGISOutputStreamException.h"

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
	/// Utilities for RSGIS and GSL vectors
    
	/** 
	 * n - size
	 */
	
	struct DllExport Vector
	{
		double *vector;
		int n;
	};
	
	class DllExport RSGISVectors
		{
		public:
			RSGISVectors();
			Vector* createVector(int n) throw(RSGISVectorsException);
			void freeVector(Vector *vector);
			void printVector(Vector *vector);
			void printGSLVector(gsl_vector *vector);
			void saveVector2GridTxt(Vector *vector, std::string filepath) throw(RSGISVectorsException,RSGISOutputStreamException);
			void saveVector2txt(Vector *vector, std::string filepath) throw(RSGISVectorsException,RSGISOutputStreamException);
			void saveGSLVector2Txt(gsl_vector *gslVector, std::string filepath) throw(RSGISVectorsException,RSGISOutputStreamException);
			void saveGSLVector2GridTxt(gsl_vector *gslVector, std::string filepath) throw(RSGISVectorsException,RSGISOutputStreamException);
			Vector* readVectorFromTxt(std::string filepath) throw(RSGISVectorsException,RSGISInputStreamException);
			Vector* readVectorFromGridTxt(std::string filepath) throw(RSGISVectorsException,RSGISInputStreamException);
			gsl_vector* readGSLVectorFromTxt(std::string filepath) throw(RSGISVectorsException);
			gsl_vector* readGSLVectorFromGridTxt(std::string filepath) throw(RSGISVectorsException);
			gsl_vector* convertRSGIS2GSLVector(Vector *vector) throw(RSGISVectorsException);
			Vector* convertGSL2RSGISVector(gsl_vector *inVector) throw(RSGISVectorsException);
			double dotProductVectorVectorGSL(gsl_vector *inVectorA, gsl_vector *inVectorB) throw(RSGISVectorsException);
			gsl_vector* crossProductVectorVectorGSL(gsl_vector *inVectorA, gsl_vector *inVectorB) throw(RSGISVectorsException);
			double euclideanDistance(Vector *vecA, Vector *vecB) throw(RSGISVectorsException);
			~RSGISVectors();
		};
}}

#endif

