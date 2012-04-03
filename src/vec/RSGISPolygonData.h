/*
 *  RSGISPolygonData.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#ifndef RSGISPolygonData_H
#define RSGISPolygonData_H

#include <iostream>
#include <string>
#include <vector>

#include "ogrsf_frmts.h"

#include "geos/geom/GeometryFactory.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/Envelope.h"

#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISVectorOutputException.h"

#include "utils/RSGISGEOSFactoryGenerator.h"

using namespace std;
using namespace rsgis::utils;
using namespace geos::geom;
using namespace geos;

namespace rsgis{namespace vec{
	
	class RSGISPolygonData
		{
		public:
			RSGISPolygonData();
			void readPolygon(OGRPolygon *polygon);
			void readMultiPolygon(OGRMultiPolygon *multipolygon);
			void printGeometry();
			virtual void readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn)=0;
			virtual void createLayerDefinition(OGRLayer *layer)throw(RSGISVectorOutputException)=0;
			virtual void populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn)=0;
			double distance(RSGISPolygonData *data);
			bool contains(Coordinate *coord);
			bool contains(Geometry *geom);
			Envelope* getBBox();
			Polygon* getPolygon();
			void setPolygon(Polygon *polygon);
			Geometry* getGeometry();
			virtual ~RSGISPolygonData();
		protected:
			bool polygonType;
			MultiPolygon *multiPolygonGeom;
			Polygon *polygonGeom;
			Envelope *envelope;
		};
}}

#endif


