/*
 *  RSGISPointData.h
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

#ifndef RSGISPointData_H
#define RSGISPointData_H

#include <iostream>
#include <string>
#include <vector>
#include "ogrsf_frmts.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/Point.h"
#include "geos/geom/Geometry.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISPolygonData.h"
#include "vec/RSGISVectorOutputException.h"

using namespace std;
using namespace geos::geom;
using namespace geos;

namespace rsgis{namespace vec{
	
	class RSGISPointData
		{
		public:
			RSGISPointData();
			void readPoint(OGRPoint *point);
			void printGeometry();
			virtual void readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)=0;
			virtual void createLayerDefinition(OGRLayer *layer)throw(RSGISVectorOutputException)=0;
			virtual void populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)=0;
			double distance(RSGISPolygonData *data);
			double distance(RSGISPointData *data);
			Point* getPoint();
			void setPoint(Point *point);
			virtual ~RSGISPointData();
		protected:
			Point *pointGeom;
		};
}}

#endif



