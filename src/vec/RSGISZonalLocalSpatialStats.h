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

using namespace std;
using namespace rsgis;
using namespace rsgis::img;
using namespace geos::geom;

namespace rsgis{namespace vec{
	
	class RSGISZonalLSSMeanVar : public RSGISProcessOGRFeature
	{
	public:
		RSGISZonalLSSMeanVar(GDALDataset *image, MeanAttributes** attributes, int numAttributes, bool outPxlCount, int winSize, double offsetSize, pixelInPolyOption method);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException){throw RSGISVectorException("Not Implemented");};
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		virtual ~RSGISZonalLSSMeanVar();
	protected:
		GDALDataset **datasets;
		MeanAttributes** attributes;
		string *attributesVar;
		int numAttributes;
		int dataSize;
		int looksX;
		int looksY;
		int numOffsets;
		double offsetSize;
		double *averages;
		double **offsetAverages;
		bool outPxlCount;
		RSGISCalcImageSingle *calcImageCentre;
		RSGISCalcImageSingle *calcImageSurrounding;
		RSGISCalcImageSingleValue *calcValueCentre;
		RSGISCalcImageSingleValue *calcValueSurrounding;
		pixelInPolyOption method; 
	};
	
	class RSGISCalcZonalLSSMeanVar : public RSGISCalcImageSingleValue
	{
	public: 
		RSGISCalcZonalLSSMeanVar(int numOutputValues, MeanAttributes **attributes, int numAttributes);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException);
		double* getOutputValues() throw(RSGISImageCalcException);
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
