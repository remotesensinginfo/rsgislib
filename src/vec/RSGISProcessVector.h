/*
 *  RSGISProcessVector.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/03/2009.
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


#ifndef RSGISProcessVector_H
#define RSGISProcessVector_H

#include <iostream>
#include <string>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "math/RSGISMathsUtils.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISVectorUtils.h"

#include "geos/geom/Envelope.h"

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
	
	class DllExport RSGISProcessVector
		{
		public:
			RSGISProcessVector(RSGISProcessOGRFeature *processFeatures);
			void processVectors(OGRLayer *inputLayer, OGRLayer *outputLayer, bool copyData, bool outVertical, bool newFirst) throw(RSGISVectorOutputException,RSGISVectorException);
			void processVectors(OGRLayer *inputLayer, bool outVertical) throw(RSGISVectorOutputException,RSGISVectorException);
			void processVectorsNoOutput(OGRLayer *inputLayer, bool outVertical) throw(RSGISVectorOutputException,RSGISVectorException);
			~RSGISProcessVector();
		protected:
			void copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			void copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn);
            void printGeometry(OGRGeometry *geometry);
            void printRing(OGRLinearRing *inGeomRing);
			RSGISProcessOGRFeature *processFeatures;
		};
}}

#endif




