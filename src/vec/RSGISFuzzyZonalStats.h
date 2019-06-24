/*
 *  RSGISFuzzyZonalStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/02/2009.
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

#ifndef RSGISFuzzyZonalStats_H
#define RSGISFuzzyZonalStats_H

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
	
	struct DllExport FuzzyAttributes
	{
		std::string name;
		std::string fuzzyClass;
		int index;
		int numBands;
		int *bands;
	};
	
	struct DllExport FuzzyClassSet
	{
		std::string name;
		int index;
		int count;
	};
	
	class DllExport RSGISCalcFuzzyZonalStatsFromRasterPolygon : public rsgis::img::RSGISCalcImageSingleValue
		{
		public: 
			RSGISCalcFuzzyZonalStatsFromRasterPolygon(int numOutputValues, FuzzyAttributes **attributes, int numAttributes, float binsize, float hardThreshold);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB);
			void calcImageValue(float *bandValuesImage, int numBands, int band);
			void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent);
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt);
			double* getOutputValues()  ;
			void reset();
			void updateAttributes(FuzzyAttributes **attributes, int numAttributes, bool hard);
			~RSGISCalcFuzzyZonalStatsFromRasterPolygon();
		private:
			float calcHistogramCentre(int *histogram); 
			FuzzyAttributes **attributes;
			int numAttributes;
			float binsize;
			bool hard;
			int numPxls;
			int numBins;
			float *binRange;
			int **histograms;
			float hardThreshold;
		};
	
	
	class DllExport RSGISFuzzyZonalStats : public RSGISProcessOGRFeature
		{
		public:
			RSGISFuzzyZonalStats(GDALDataset *image, GDALDataset *rasterFeatures, FuzzyAttributes** attributes, int numAttributes, float binSize, float threshold, bool outPxlCount, std::string classattribute);
			virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid);
			virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid);
			virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn);
			virtual ~RSGISFuzzyZonalStats();
		protected:
			void setupFuzzyAttributes();
			GDALDataset **datasets;
			FuzzyAttributes** attributes;
			
            std::vector<FuzzyClassSet*> *classSets;
			FuzzyAttributes*** groupedAttributes;
			int *groupedIndexes;
			bool foundHard;
			int hardGroupIndex;
			
			int numAttributes;
			int dataSize;
			double *data;
			float binSize;
			float threshold;
			bool outPxlCount;
			std::string classattribute;
            rsgis::img::RSGISCalcImageSingle *calcImage;
			RSGISCalcFuzzyZonalStatsFromRasterPolygon *calcValue;
		};
	
	
	
}}


#endif

