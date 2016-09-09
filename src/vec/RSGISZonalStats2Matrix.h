/*
 *  RSGISZonalStats2Matrix.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/11/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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


#ifndef RSGISZonalStats2Matrix_H
#define RSGISZonalStats2Matrix_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include <boost/lexical_cast.hpp>

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorZonalException.h"
#include "vec/RSGISPolygonData.h"
#include "vec/RSGISVectorIO.h"

#include "math/RSGISMatrices.h"
#include "math/RSGISMathsUtils.h"

#include "vec/RSGISClassPolygon.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "img/RSGISPolygonPixelCount.h"
#include "img/RSGISCalcImageSingle.h"
#include "img/RSGISPopulateMatrix.h"
#include "img/RSGISImageBand2Matrix.h"
#include "vec/RSGISEmptyPolygon.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISPixelInPoly.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Coordinate.h"

namespace rsgis{namespace vec{
		
	struct DllExport ClassVariables
	{
		std::string name;
        rsgis::math::Matrix *matrix;
		int numPxls;
	};
	
	class DllExport RSGISZonalStats2Matrix
	{
	public:
		RSGISZonalStats2Matrix();
		ClassVariables** findPixelStats(GDALDataset **image, int numImgs, OGRLayer *shpfile, std::string classAttribute, int *numMatrices, rsgis::img::pixelInPolyOption method);
		rsgis::math::Matrix** findPixelsForImageBand(GDALDataset **image, int numImgs, OGRLayer *shpfile, int *numMatrices, int band);
		~RSGISZonalStats2Matrix();
	};
    
    class DllExport RSGISPixelVals22Txt : public RSGISProcessOGRFeature
    {
        /** Save pixel values to text file. Saves the values of all pixels within a text file to a text file.
         */
    public:
        RSGISPixelVals22Txt(GDALDataset *image, std::string outFileBase, std::string outNameHeading = "FID", math::outTXTform outType = math::csv, rsgis::img::pixelInPolyOption method = rsgis::img::pixelContainsPolyCenter,  unsigned int maxPrintout = 10);
        virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException){throw RSGISVectorException("Not implemented");};
        virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
        virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException){throw RSGISVectorException("Not implemented");};
        virtual ~RSGISPixelVals22Txt();
    protected:
        GDALDataset **datasets;
        unsigned int nImageBands;
        int dataSize;
        double *data;
        std::vector<double> **pixelValues;
        rsgis::img::RSGISCalcImageSingle *calcImage;
        rsgis::img::RSGISCalcImageSingleValue *calcValue;
        rsgis::img::pixelInPolyOption method;
        std::string outFileBase;
        std::string outNameHeading;
        math::outTXTform outType;
        std::string outStatusText;
        unsigned int nFeatures; // Count of number of features
        unsigned int maxPrintout; // Max features to print out
    };
    
    class DllExport RSGISCalcPixelValsFromPolygon : public rsgis::img::RSGISCalcImageSingleValue
    {
    public:
        RSGISCalcPixelValsFromPolygon(std::vector<double> **pixelValues, unsigned int numInBands);
        void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float *bandValuesImage, int numBands, int band) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img::RSGISImageCalcException);
        double* getOutputValues() throw(rsgis::img::RSGISImageCalcException);
        void reset();
        ~RSGISCalcPixelValsFromPolygon();
    protected:
        std::vector<double> **pixelValues;
        int totalPxl;
        int numAttributes;
        unsigned int numInBands;
    };
    
}}
#endif


