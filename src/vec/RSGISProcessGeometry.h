/*
 *  RSGISProcessGeometry.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 14/04/2009.
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



#ifndef RSGISProcessGeometry_H
#define RSGISProcessGeometry_H

#include <iostream>
#include <string>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "math/RSGISMathsUtils.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRGeometry.h"
#include "vec/RSGISVectorUtils.h"

namespace rsgis{namespace vec{
	
	class DllExport RSGISProcessGeometry
		{
		public:
			RSGISProcessGeometry(RSGISProcessOGRGeometry *processGeom);
			void processGeometry(OGRLayer *inputLayer, OGRLayer *outputLayer, bool copyData, bool outVertical) throw(RSGISVectorOutputException,RSGISVectorException);
			void processGeometryPolygonOutput(OGRLayer *inputLayer, OGRLayer *outputLayer, bool copyData, bool outVertical) throw(RSGISVectorOutputException,RSGISVectorException);
			~RSGISProcessGeometry();
		protected:
			void copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			void copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn);
			RSGISProcessOGRGeometry *processGeom;
		};
}}

#endif


