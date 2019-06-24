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
	
	struct DllExport MeanAttributes
	{
		std::string name;
		int numBands;
		int *bands;
		float *minThresholds;
		float *maxThresholds;
		
	};
	
	class DllExport RSGISZonalMeanStats : public RSGISProcessOGRFeature
		{
		public:
			RSGISZonalMeanStats(GDALDataset *image, GDALDataset *rasterFeatures, MeanAttributes** attributes, int numAttributes, bool outPxlCount);
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
			virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid){throw RSGISVectorException("Not Implemented");};
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
			virtual ~RSGISZonalMeanStats();
		protected:
			GDALDataset **datasets;
			MeanAttributes** attributes;
			int numAttributes;
			int dataSize;
			double *data;
			bool outPxlCount;
            rsgis::img::RSGISCalcImageSingle *calcImage;
			rsgis::img::RSGISCalcImageSingleValue *calcValue;
		};
	
	class DllExport RSGISCalcZonalMeanFromRasterPolygon : public rsgis::img::RSGISCalcImageSingleValue
		{
		public: 
			RSGISCalcZonalMeanFromRasterPolygon(int numOutputValues, MeanAttributes **attributes, int numAttributes);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
			void calcImageValue(float *bandValuesImage, int numBands, int band);
			void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
			double* getOutputValues();
			void reset();
			~RSGISCalcZonalMeanFromRasterPolygon();
		protected:
			MeanAttributes **attributes;
			double *sum;
			double *pxlCount;
			int totalPxl;
			int numAttributes;
		};
	
	class DllExport RSGISZonalMeanStatsPoly : public RSGISProcessOGRFeature
	{
	public:
		RSGISZonalMeanStatsPoly(GDALDataset *image, MeanAttributes** attributes, int numAttributes, bool outPxlCount, rsgis::img::pixelInPolyOption method, bool dB = false);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
		virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid){throw RSGISVectorException("Not Implemented");};
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
		virtual ~RSGISZonalMeanStatsPoly();
	protected:
		GDALDataset **datasets;
		MeanAttributes** attributes;
		int numAttributes;
		int dataSize;
		double *data;
		bool outPxlCount;
		rsgis::img::RSGISCalcImageSingle *calcImage;
		rsgis::img::RSGISCalcImageSingleValue *calcValue;
		rsgis::img::pixelInPolyOption method; 
	};
	
	class DllExport RSGISCalcZonalMeanFromPolygon : public rsgis::img::RSGISCalcImageSingleValue
	{
	public: 
		RSGISCalcZonalMeanFromPolygon(int numOutputValues, MeanAttributes **attributes, int numAttributes);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt);
		double* getOutputValues();
		void reset();
		~RSGISCalcZonalMeanFromPolygon();
	protected:
		MeanAttributes **attributes;
		double *sum;
		double *pxlCount;
		int totalPxl;
		int numAttributes;
	};
	
	class DllExport RSGISCalcZonalMeanFromPolygondB : public rsgis::img::RSGISCalcImageSingleValue
	{
	public: 
		RSGISCalcZonalMeanFromPolygondB(int numOutputValues, MeanAttributes **attributes, int numAttributes);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt);
		double* getOutputValues();
		void reset();
		~RSGISCalcZonalMeanFromPolygondB();
	protected:
		MeanAttributes **attributes;
		double *sum;
		double *pxlCount;
		int totalPxl;
		int numAttributes;
	};
	
	class DllExport RSGISZonalWeightedMeanStatsPoly : public RSGISProcessOGRFeature
	{
	public:
		RSGISZonalWeightedMeanStatsPoly(GDALDataset *image, MeanAttributes** attributes, int numAttributes, bool outPxlCount);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
		virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid){throw RSGISVectorException("Not Implemented");};
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
		virtual ~RSGISZonalWeightedMeanStatsPoly();
	protected:
		GDALDataset **datasets;
		MeanAttributes** attributes;
		int numAttributes;
		int dataSize;
		double *data;
		bool outPxlCount;
		rsgis::img::RSGISCalcImageSingle *calcImage;
		rsgis::img::RSGISCalcImageSingleValue *calcValue;
	};
	
	class DllExport RSGISCalcZonalWeightedMeanStatsPoly : public rsgis::img::RSGISCalcImageSingleValue
	{
	public: 
		RSGISCalcZonalWeightedMeanStatsPoly(int numOutputValues, MeanAttributes **attributes, int numAttributes);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) {throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt);
		double* getOutputValues();
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


