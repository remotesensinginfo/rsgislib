/*
 *  RSGISDelaunayTriangulation.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2009.
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

#ifndef RSGISDelaunayTriangulation_H
#define RSGISDelaunayTriangulation_H

#include <string>
#include <iostream>
#include <list>

#include "geom/RSGIS2DPoint.h"
#include "geom/RSGISGeometryException.h"
#include "geom/RSGISTriangle.h"
#include "geom/RSGISGeometry.h"

#include "geos/geom/Envelope.h"

#include "math/RSGISMathsUtils.h"

namespace rsgis{namespace geom{
	
	class DllExport RSGISDelaunayTriangulation
		{
		public:
			/***
			 * Initialise with a triangle within which the Delaunay triangulation
			 * will be formed. 
			 * 
			 *        a
			 *      /   \
			 *     /     \
			 *    /       \
			 *   /         \
			 *  c-----------b
			 *                   
			 */
			RSGISDelaunayTriangulation(RSGIS2DPoint *a, RSGIS2DPoint *b, RSGIS2DPoint *c);
			RSGISDelaunayTriangulation(RSGISTriangle *tri);
			RSGISDelaunayTriangulation(std::list<RSGIS2DPoint*> *data) throw(RSGISGeometryException);
			RSGISDelaunayTriangulation(std::vector<RSGIS2DPoint*> *data) throw(RSGISGeometryException);
			void createDelaunayTriangulation(std::list<RSGIS2DPoint*> *data) throw(RSGISGeometryException);
			void addVertex(RSGIS2DPoint *pt) throw(RSGISGeometryException);
			void finaliseTriangulation(std::list<RSGIS2DPoint*> *data);
			void finaliseTriangulation();
			std::list<RSGISTriangle*>* getTriangulation();
			void plotTriangulationAsEdges(std::string filename);
			~RSGISDelaunayTriangulation();
		protected:
			std::list<RSGIS2DPoint*>* getPtsClockwise(std::list<RSGISTriangle*> *tris, RSGIS2DPoint *pt);
			std::list<RSGISTriangle*> *triangleList;
            geos::geom::Envelope *bbox;
			RSGIS2DPoint *aOuter;
			RSGIS2DPoint *bOuter;
			RSGIS2DPoint *cOuter;
		};

}}

#endif



