/*
 *  RSGISGCPImg2MapNode.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 06/09/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef RSGISGCPImg2MapNode_H
#define RSGISGCPImg2MapNode_H

#include <math.h>

#include "common/RSGISCommons.h"

namespace rsgis{namespace reg{
	
	class DllExport RSGISGCPImg2MapNode
	{
	public:
		RSGISGCPImg2MapNode(double eastings, double northings, float imgXIn, float imgYIn);
		double eastings() const;
		double northings() const;
		float imgX() const;
		float imgY() const;
		double distanceGeo(RSGISGCPImg2MapNode *pt);
		~RSGISGCPImg2MapNode();
	protected:
        double eastings_;
        double northings_;
		float imgX_;
		float imgY_;
	};
	
}}

#endif




