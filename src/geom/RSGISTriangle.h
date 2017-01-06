/*
 *  RSGISTriangle.h
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 16/06/2009.
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


#ifndef RSGISTriangle_H
#define RSGISTriangle_H

#include <string>
#include <iostream>

#include "math/RSGISVectors.h"

#include "geom/RSGISGeometryException.h"
#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISCircle.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/Envelope.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/PrecisionModel.h"

#include <gsl/gsl_vector.h>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_geom_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace geom{
	
	class DllExport RSGISTriangle
		{
		public:
			/***
			 * a, b, and c form the 3 corners of the triangle. The triangle 
			 * will be ordered a, b, c.
			 * 
			 *        a
			 *      /   \
			 *     /     \
			 *    /       \
			 *   /         \
			 *  c-----------b
			 *                   
			 */
			RSGISTriangle(RSGIS2DPoint *p1, RSGIS2DPoint *p2, RSGIS2DPoint *p3);
			void updateTriangle(RSGIS2DPoint *p1, RSGIS2DPoint *p2, RSGIS2DPoint *p3);
			bool contained(RSGIS2DPoint *pt);
			bool containedBBOX(RSGIS2DPoint *pt);
			bool containedCircle(RSGIS2DPoint *pt);
			geos::geom::Envelope* getBBox();
			RSGISCircle* getCircle();
			RSGIS2DPoint* getPointA();
			RSGIS2DPoint* getPointB();
			RSGIS2DPoint* getPointC();
			double getArea();
			geos::geom::Polygon* createPolygon();
			friend std::ostream& operator<<(std::ostream& ostr, const RSGISTriangle& tri);
            std::ostream& operator<<(std::ostream& ostr);
			virtual ~RSGISTriangle();
		protected:
			void setTriangleVertex(RSGIS2DPoint *p1, RSGIS2DPoint *p2, RSGIS2DPoint *p3);
			RSGIS2DPoint *a;
			RSGIS2DPoint *b;
			RSGIS2DPoint *c;
			geos::geom::Envelope *bbox;
			RSGISCircle *circle;
		};
}}

#endif

