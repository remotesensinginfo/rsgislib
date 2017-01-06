/*
 *  RSGISCirclePoint.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/01/2009.
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

#ifndef RSGISCirclePoint_H
#define RSGISCirclePoint_H

#include <iostream>
#include <string>

#include "vec/RSGISPointData.h"
#include "vec/RSGISVectorUtils.h"

#include "math/RSGISMathsUtils.h"

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
	
	class DllExport RSGISCirclePoint : public RSGISPointData
		{
		public:
			RSGISCirclePoint(std::string radiusAttribute, std::string areaAttribute, std::string heightAttribute);
			virtual void readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn);
			virtual void createLayerDefinition(OGRLayer *outputSHPLayer)throw(RSGISVectorOutputException);
			virtual void populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn);
			void setArea(float area);
			void setHeight(float height);
			void setRadius(float radius);
			float getArea();
			float getHeight();
			float getRadius();
			~RSGISCirclePoint();
		protected:
            std::string radiusAttribute;
			std::string areaAttribute;
			std::string heightAttribute;
			float radius;
			float area;
			float height;
		};
}}

#endif

