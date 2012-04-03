/*
 *  RSGIS2DPoint.h
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

#ifndef RSGIS2DPoint_H
#define RSGIS2DPoint_H

#include <string>
#include <iostream>
#include <math.h>

#include "geom/RSGISGeometryException.h"

#include "geos/geom/Coordinate.h"

using namespace std;
using namespace geos::geom;

namespace rsgis{namespace geom{
	
	class RSGIS2DPoint
		{
		public:
			RSGIS2DPoint();
			RSGIS2DPoint(Coordinate *pt);
			RSGIS2DPoint(Coordinate *pt, int classID);
			virtual void setPoint(Coordinate *pt);
			virtual const Coordinate* getPoint();
			virtual double getX();
			virtual double getY();
			virtual double getZ();
			virtual int getIndex();
			virtual void setIndex(int index);
			virtual void setTouched(bool touched);
			virtual bool getTouched();
			virtual void setClassID(unsigned int classID);
			virtual unsigned int getClassID();
			virtual double distance(RSGIS2DPoint *pt);
			virtual double distance(Coordinate *pt);
			friend ostream& operator<<(ostream& ostr, const RSGIS2DPoint& pt);
			ostream& operator<<(ostream& ostr);
			bool operator==(RSGIS2DPoint pt) const;
			bool operator!=(RSGIS2DPoint pt) const;
			bool operator>(RSGIS2DPoint pt) const;
			bool operator<(RSGIS2DPoint pt) const;
			bool operator>=(RSGIS2DPoint pt) const;
			bool operator<=(RSGIS2DPoint pt) const;
			bool operator==(RSGIS2DPoint *pt) const;
			bool operator!=(RSGIS2DPoint *pt) const;
			bool operator>(RSGIS2DPoint *pt) const;
			bool operator<(RSGIS2DPoint *pt) const;
			bool operator>=(RSGIS2DPoint *pt) const;
			bool operator<=(RSGIS2DPoint *pt) const;
			virtual ~RSGIS2DPoint();
		protected:
			Coordinate *point;
			int index;
			bool touched;
			unsigned int classID;
		};
}}

#endif

