/*
 *  RSGISPopulateTopPowerSet.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/03/2009.
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

#ifndef RSGISPopulateTopPowerSet_H
#define RSGISPopulateTopPowerSet_H

#include <iostream>
#include <string>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISZonalCountStats.h"
#include "vec/RSGISPixelAreaCountInPolygon.h"

#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"
#include "img/RSGISImageCalcException.h"

#include "math/RSGISMathsUtils.h"

#include "geos/geom/Envelope.h"

namespace rsgis{namespace vec{
	
	class DllExport RSGISPopulateTopPowerSet : public RSGISProcessOGRFeature
		{
		public:
			RSGISPopulateTopPowerSet(GDALDataset *image, GDALDataset *rasterFeatures, CountAttributes** attributes, int numAttributes, int nTop);
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
			virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			virtual ~RSGISPopulateTopPowerSet();
		protected:
			void identifyPowerSet(CountAttributes** allAttributes, int numAttributes, std::string *topAttributeNames, int numTop, CountAttributes** powerSetAttributes, int numPSAttributes) throw(rsgis::math::RSGISMathException,RSGISVectorException);
			GDALDataset **datasets;
			CountAttributes** attributes;
			int numAttributes;
			int dataSize;
			int nTop;
			double *data;
            rsgis::img::RSGISCalcImageSingle *calcImage;
			rsgis::img::RSGISCalcImageSingleValue *calcValue;
		};
}}

#endif




