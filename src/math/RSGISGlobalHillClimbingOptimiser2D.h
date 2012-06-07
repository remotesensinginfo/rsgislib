/*
 *  RSGISGlobalHillClimbingOptimiser2D.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/08/2009.
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

#ifndef RSGISGlobalHillClimbingOptimiser2D_H
#define RSGISGlobalHillClimbingOptimiser2D_H

#include <iostream>
#include <string>
#include <math.h>

#include "math/RSGISGlobalOptimiser2D.h"
#include "math/RSGISGlobalOptimisationFunction.h"
#include "math/RSGISOptimisationException.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/Envelope.h"

namespace rsgis{namespace math{
    
    using namespace std;
    using namespace geos::geom;
	
	class RSGISGlobalHillClimbingOptimiser2D : public RSGISGlobalOptimiser2D
		{
		public:
			RSGISGlobalHillClimbingOptimiser2D(RSGISGlobalOptimisationFunction *func, bool maximise, int maxNumIterations);
			virtual vector<Coordinate*>* optimise4Neighbor(vector<Coordinate*> *coords, double step, Envelope *boundary) throw(RSGISOptimisationException);
			virtual vector<Coordinate*>* optimise8Neighbor(vector<Coordinate*> *coords, double step, Envelope *boundary) throw(RSGISOptimisationException);
			virtual ~RSGISGlobalHillClimbingOptimiser2D();
		private:
			int maxNumIterations;
		};
	
	class RSGISGlobalHillClimbingOptimiser2DVaryNumPts : public RSGISGlobalOptimiser2D
	{
	public:
		RSGISGlobalHillClimbingOptimiser2DVaryNumPts(RSGISGlobalOptimisationFunction *func, bool maximise, int maxNumIterations);
		virtual vector<Coordinate*>* optimise4Neighbor(vector<Coordinate*> *coords, double step, Envelope *boundary) throw(RSGISOptimisationException);
		virtual vector<Coordinate*>* optimise8Neighbor(vector<Coordinate*> *coords, double step, Envelope *boundary) throw(RSGISOptimisationException);
		virtual ~RSGISGlobalHillClimbingOptimiser2DVaryNumPts();
	private:
		int maxNumIterations;
		void findPointOnLine(Coordinate *p1, Coordinate *p2, float distance, Coordinate *p3);
	};
}}

#endif



