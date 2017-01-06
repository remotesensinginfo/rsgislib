/*
 *  RSGISZonalCountStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/03/2009.
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

#ifndef RSGISZonalCountStats_H
#define RSGISZonalCountStats_H

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

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_vec_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace vec{
	
	struct DllExport CountAttributes
	{
        std::string name;
		int numBands;
		int *bands;
		float *thresholds;
	};
	
	class DllExport RSGISZonalCountStats : public RSGISProcessOGRFeature
		{
		public:
			RSGISZonalCountStats(GDALDataset *image, GDALDataset *rasterFeatures, CountAttributes** attributes, int numAttributes, bool outPxlCount, bool outEmptyCount);
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
			virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			virtual ~RSGISZonalCountStats();
		protected:
			GDALDataset **datasets;
			CountAttributes** attributes;
			int numAttributes;
			int dataSize;
			double *data;
			bool outPxlCount;
			bool outEmptyCount;
            rsgis::img::RSGISCalcImageSingle *calcImage;
			rsgis::img::RSGISCalcImageSingleValue *calcValue;
		};
	
	
	class DllExport RSGISCalcZonalCountFromRasterPolygon : public rsgis::img::RSGISCalcImageSingleValue
		{
		public: 
			RSGISCalcZonalCountFromRasterPolygon(int numOutputValues, CountAttributes **attributes, int numAttributes);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(rsgis::img::RSGISImageCalcException);
			void calcImageValue(float *bandValuesImage, int numBands, int band) throw(rsgis::img::RSGISImageCalcException);
			void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) throw(rsgis::img::RSGISImageCalcException);
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img::RSGISImageCalcException);
			double* getOutputValues() throw(rsgis::img::RSGISImageCalcException);
			void reset();
			~RSGISCalcZonalCountFromRasterPolygon();
		protected:
			CountAttributes **attributes;
			int *count;
			int totalPxl;
			int emptyPxl;
			int numAttributes;
		};
}}

#endif


