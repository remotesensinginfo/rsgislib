 /*
 *  RSGISZonalLocalSpatialStats.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 29/06/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#ifndef RSGISZonalLocalSpatialStats_H
#define RSGISZonalLocalSpatialStats_H

#include <iostream>
#include <string>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISZonalMeanStats.h"
#include "vec/RSGISVectorUtils.h"

#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISPixelInPoly.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/GeometryFactory.h"

#include <boost/math/special_functions/fpclassify.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
	
	class DllExport RSGISZonalLSSMeanVar : public RSGISProcessOGRFeature
	{
	public:
		RSGISZonalLSSMeanVar(GDALDataset *image, MeanAttributes** attributes, int numAttributes, bool outPxlCount, int winSize, double offsetSize, rsgis::img::pixelInPolyOption method);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
		virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid){throw RSGISVectorException("Not Implemented");};
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
		virtual ~RSGISZonalLSSMeanVar();
	protected:
		GDALDataset **datasets;
		MeanAttributes** attributes;
		std::string *attributesVar;
		int numAttributes;
		int dataSize;
		int looksX;
		int looksY;
		int numOffsets;
		double offsetSize;
		double *averages;
		double **offsetAverages;
		bool outPxlCount;
        rsgis::img::RSGISCalcImageSingle *calcImageCentre;
		rsgis::img::RSGISCalcImageSingle *calcImageSurrounding;
		rsgis::img::RSGISCalcImageSingleValue *calcValueCentre;
		rsgis::img::RSGISCalcImageSingleValue *calcValueSurrounding;
		rsgis::img::pixelInPolyOption method; 
	};
	
	class DllExport RSGISCalcZonalLSSMeanVar : public rsgis::img::RSGISCalcImageSingleValue
	{
	public: 
		RSGISCalcZonalLSSMeanVar(int numOutputValues, MeanAttributes **attributes, int numAttributes);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt);
		double* getOutputValues();
		void reset();
		~RSGISCalcZonalLSSMeanVar();
	protected:
		MeanAttributes **attributes;
		double *sum;
		int totalPxl;
		int numAttributes;
	};

}}

#endif
