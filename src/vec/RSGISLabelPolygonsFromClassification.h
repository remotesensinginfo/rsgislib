/*
 *  RSGISLabelPolygonsFromClassification.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/03/2010.
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

#ifndef RSGISLabelPolygonsFromClassification_H
#define RSGISLabelPolygonsFromClassification_H

#include <iostream>
#include <string>
#include <list>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISClassPolygon.h"

#include "geom/RSGISPolygon.h"
#include "geom/RSGISGeometry.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Polygon.h"

namespace rsgis{namespace vec{
	
	enum dominantspecies
	{
		unknown,
		speciesCount,
		speciesArea
	};
	
	class DllExport RSGISLabelPolygonsFromClassification : public RSGISProcessOGRFeature
	{
	public:
		RSGISLabelPolygonsFromClassification(RSGISClassPolygon **labelsClassData, unsigned long numFeatures, dominantspecies dominant);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		virtual ~RSGISLabelPolygonsFromClassification();
	protected:
		RSGISClassPolygon **labelsClassData;
		unsigned long numFeatures;
		dominantspecies dominant;
	};
}}

#endif
