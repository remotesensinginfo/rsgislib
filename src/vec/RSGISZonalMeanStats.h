/*
 *  RSGISZonalMeanStats.h
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

#ifndef RSGISZonalMeanStats_H
#define RSGISZonalMeanStats_H

#include <iostream>
#include <string>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
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
	
	struct MeanAttributes
	{
		string name;
		int numBands;
		int *bands;
		float *minThreasholds;
		float *maxThreasholds;
		
	};
	
	class RSGISZonalMeanStats : public RSGISProcessOGRFeature
		{
		public:
			RSGISZonalMeanStats(GDALDataset *image, GDALDataset *rasterFeatures, MeanAttributes** attributes, int numAttributes, bool outPxlCount);
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
			virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException){throw RSGISVectorException("Not Implemented");};
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
			virtual ~RSGISZonalMeanStats();
		protected:
			GDALDataset **datasets;
			MeanAttributes** attributes;
			int numAttributes;
			int dataSize;
			double *data;
			bool outPxlCount;
			RSGISCalcImageSingle *calcImage;
			RSGISCalcImageSingleValue *calcValue;
		};
	
	class RSGISCalcZonalMeanFromRasterPolygon : public RSGISCalcImageSingleValue
		{
		public: 
			RSGISCalcZonalMeanFromRasterPolygon(int numOutputValues, MeanAttributes **attributes, int numAttributes);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
			void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
			double* getOutputValues() throw(RSGISImageCalcException);
			void reset();
			~RSGISCalcZonalMeanFromRasterPolygon();
		protected:
			MeanAttributes **attributes;
			double *sum;
			double *pxlCount;
			int totalPxl;
			int numAttributes;
		};
	
	class RSGISZonalMeanStatsPoly : public RSGISProcessOGRFeature
	{
	public:
		RSGISZonalMeanStatsPoly(GDALDataset *image, MeanAttributes** attributes, int numAttributes, bool outPxlCount, pixelInPolyOption method, bool dB = false);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException){throw RSGISVectorException("Not Implemented");};
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		virtual ~RSGISZonalMeanStatsPoly();
	protected:
		GDALDataset **datasets;
		MeanAttributes** attributes;
		int numAttributes;
		int dataSize;
		double *data;
		bool outPxlCount;
		RSGISCalcImageSingle *calcImage;
		RSGISCalcImageSingleValue *calcValue;
		pixelInPolyOption method; 
	};
	
	class RSGISCalcZonalMeanFromPolygon : public RSGISCalcImageSingleValue
	{
	public: 
		RSGISCalcZonalMeanFromPolygon(int numOutputValues, MeanAttributes **attributes, int numAttributes);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException);
		double* getOutputValues() throw(RSGISImageCalcException);
		void reset();
		~RSGISCalcZonalMeanFromPolygon();
	protected:
		MeanAttributes **attributes;
		double *sum;
		double *pxlCount;
		int totalPxl;
		int numAttributes;
	};
	
	class RSGISCalcZonalMeanFromPolygondB : public RSGISCalcImageSingleValue
	{
	public: 
		RSGISCalcZonalMeanFromPolygondB(int numOutputValues, MeanAttributes **attributes, int numAttributes);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException);
		double* getOutputValues() throw(RSGISImageCalcException);
		void reset();
		~RSGISCalcZonalMeanFromPolygondB();
	protected:
		MeanAttributes **attributes;
		double *sum;
		double *pxlCount;
		int totalPxl;
		int numAttributes;
	};
	
	class RSGISZonalWeightedMeanStatsPoly : public RSGISProcessOGRFeature
	{
	public:
		RSGISZonalWeightedMeanStatsPoly(GDALDataset *image, MeanAttributes** attributes, int numAttributes, bool outPxlCount);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException){throw RSGISVectorException("Not Implemented");};
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		virtual ~RSGISZonalWeightedMeanStatsPoly();
	protected:
		GDALDataset **datasets;
		MeanAttributes** attributes;
		int numAttributes;
		int dataSize;
		double *data;
		bool outPxlCount;
		RSGISCalcImageSingle *calcImage;
		RSGISCalcImageSingleValue *calcValue;
	};
	
	class RSGISCalcZonalWeightedMeanStatsPoly : public RSGISCalcImageSingleValue
	{
	public: 
		RSGISCalcZonalWeightedMeanStatsPoly(int numOutputValues, MeanAttributes **attributes, int numAttributes);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException);
		double* getOutputValues() throw(RSGISImageCalcException);
		void reset();
		~RSGISCalcZonalWeightedMeanStatsPoly();
	protected:
		MeanAttributes **attributes;
		double *sum;
		double *weightsSum;
		int totalPxl;
		int numAttributes;
	};
}}

#endif


