/*
 *  RSGISPixelAreaCountInPolygon.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/03/2009.
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

#ifndef RSGISPixelAreaCountInPolygon_H
#define RSGISPixelAreaCountInPolygon_H

#include <iostream>
#include <string>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"

#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"
#include "img/RSGISImageCalcException.h"

#include "geos/geom/Envelope.h"

using namespace std;
using namespace rsgis;
using namespace rsgis::img;
using namespace geos::geom;

namespace rsgis{namespace vec{
	
	class RSGISPixelAreaCountInPolygon : public RSGISProcessOGRFeature
		{
		public:
			RSGISPixelAreaCountInPolygon(GDALDataset *rasterFeatures);
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
			virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			virtual ~RSGISPixelAreaCountInPolygon();
		protected:
			GDALDataset **datasets;			
			int dataSize;
			double *data;
			double pxlresolution;
			RSGISCalcImageSingle *calcImage;
			RSGISCalcImageSingleValue *calcValue;
		};
	
	
	class RSGISCalcPixelCountFromRasterPolygon : public RSGISCalcImageSingleValue
		{
		public: 
			RSGISCalcPixelCountFromRasterPolygon(int numOutputValues);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException);
			double* getOutputValues() throw(RSGISImageCalcException);
			void reset();
			~RSGISCalcPixelCountFromRasterPolygon();
		protected:
			int totalPxl;
		};
}}

#endif


