/*
 *  RSGISPolygonReader.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/07/2009.
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

#ifndef RSGISPolygonReader_H
#define RSGISPolygonReader_H

#include <iostream>
#include <string>
#include <list>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISVectorUtils.h"

#include "geom/RSGISPolygon.h"
#include "geom/RSGISGeometry.h"

#include "geos/geom/Envelope.h"

using namespace std;
using namespace rsgis::geom;
using namespace geos::geom;

namespace rsgis{namespace vec{
	
	class RSGISPolygonReader : public RSGISProcessOGRFeature
		{
		public:
			RSGISPolygonReader(list<RSGIS2DPoint*> *data);
			RSGISPolygonReader(vector<RSGIS2DPoint*> *data);
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
			virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			virtual ~RSGISPolygonReader();
		protected:
			RSGISVectorUtils *vecUtils;
			list<RSGIS2DPoint*> *dataList;
			vector<RSGIS2DPoint*> *dataVector;
			bool listtype;
		};
}}

#endif


