 /*
 *  RSGISObjectBasedEstimation.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 25/11/2010.
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

#ifndef RSGISObjectBasedEstimation_H
#define RSGISObjectBasedEstimation_H

#include <iostream>
#include <string>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISVectorIO.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "math/RSGISRandomDistro.h"
#include "math/RSGISMathFunction.h"

#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISPixelInPoly.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/Coordinate.h"

#include "radar/RSGISEstimationAlgorithm.h"
#include "radar/RSGISEstimationOptimiser.h"
#include "radar/RSGISEstimationParameters.h"
#include "radar/RSGISEstimationConjugateGradient.h"

#include <boost/math/special_functions/fpclassify.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_radar_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace radar{

	class DllExport RSGISObjectBasedEstimation : public rsgis::vec::RSGISProcessOGRFeature
	{
	public:
		RSGISObjectBasedEstimation(GDALDataset *inputImage, GDALDataset *outputImage, GDALDataset *rasterFeatures, std::vector<gsl_vector*> *initialPar, std::vector<RSGISEstimationOptimiser*> *slowOptimiser, std::vector<RSGISEstimationOptimiser*> *fastOptimiser, estParameters parameters, double ***minMaxVals = NULL, std::string classHeading = "", bool useClass = false);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(rsgis::vec::RSGISVectorOutputException);
		virtual ~RSGISObjectBasedEstimation();
	protected:
		GDALDataset **datasetsIO;
		GDALDataset **datasetsInput;
		std::vector<float> **pixelVals;
		int numBands;
		unsigned int objectSamples;
		double objectSamplesPercent;
		unsigned int numOutputPar;
		unsigned int numOutputBands;
        rsgis::img::RSGISCalcImageSingleValue *getValues;
		rsgis::img::RSGISCalcImageSingle *calcImageSingle;
		std::vector<gsl_vector*> *initialPar;
		std::vector<RSGISEstimationOptimiser*> *slowOptimiser;
        std::vector<RSGISEstimationOptimiser*> *fastOptimiser;
		rsgis::radar::RSGISEstimationOptimiser *slowOptimiserSingle;
		rsgis::radar::RSGISEstimationOptimiser *fastOptimiserSingle;
		gsl_vector *initialParSingle;
		estParameters parameters;
		std::string classHeading;
		bool useClass; // Use multiple classes
        bool useRasPoly; // Use rasterised version of polygon
		bool useDefaultMinMax; // Pass in minimum and maximum values to estimation algorithm or use default
		double ***minMaxVals; // Minimum and maximum values for parameters

	};

    class DllExport RSGISObjectBasedEstimationObjectAP : public rsgis::vec::RSGISProcessOGRFeature
	{
	public:
		RSGISObjectBasedEstimationObjectAP(GDALDataset *inputImage, GDALDataset *outputImage, GDALDataset *rasterFeatures, std::vector<gsl_vector*> *initialPar, std::vector<RSGISEstimationOptimiser*> *slowOptimiser, std::vector<RSGISEstimationOptimiser*> *fastOptimiser, estParameters parameters, std::string *apParField, double ***minMaxVals = NULL, std::string classHeading = "", bool useClass = false);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid)throw(RSGISVectorException);
        virtual void processFeature(OGRFeature *feature, geos::geom::Envelope *env, long fid)throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(rsgis::vec::RSGISVectorOutputException);
		virtual ~RSGISObjectBasedEstimationObjectAP();
	protected:
		GDALDataset **datasetsIO;
		GDALDataset **datasetsInput;
		std::vector<float> **pixelVals;
		int numBands;
		unsigned int objectSamples;
		double objectSamplesPercent;
		unsigned int numOutputPar;
		unsigned int numOutputBands;
		rsgis::img::RSGISCalcImageSingleValue *getValues;
		rsgis::img::RSGISCalcImageSingle *calcImageSingle;
		rsgis::img::RSGISCalcImage *calcImage;
		std::vector<gsl_vector*> *initialPar;
		std::vector<RSGISEstimationOptimiser*> *slowOptimiser;
		std::vector<RSGISEstimationOptimiser*> *fastOptimiser;
		rsgis::radar::RSGISEstimationOptimiser *slowOptimiserSingle;
		rsgis::radar::RSGISEstimationOptimiser *fastOptimiserSingle;
		gsl_vector *initialParSingle;
		estParameters parameters;
		std::string classHeading;
		bool useClass; // Use multiple classes
        bool useRasPoly; // Use rasterised version of polygon
		bool useDefaultMinMax; // Pass in minimum and maximum values to estimation algorithm or use default
		double ***minMaxVals; // Minimum and maximum values for parameters
		std::string *apParField;
        gsl_matrix *covMatrixP;
        gsl_matrix *invCovMatrixD;
        rsgis::math::RSGISMathTwoVariableFunction *functionA;
        rsgis::math::RSGISMathTwoVariableFunction *functionB;
	};

	class DllExport RSGISObjectBasedEstimationGetObjVals : public rsgis::img::RSGISCalcImageSingleValue
	{
		/// Loops though bands and loads pixel values to vectors
	public:
		RSGISObjectBasedEstimationGetObjVals(std::vector<float> **pixelVals, int numBands);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValuesImage, int numBands, geos::geom::Envelope *extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, geos::geom::Polygon *poly, geos::geom::Point *pt) throw(rsgis::img::RSGISImageCalcException);
		double* getOutputValues() throw(rsgis::img::RSGISImageCalcException);
		void reset();
		~RSGISObjectBasedEstimationGetObjVals();
	protected:
		std::vector<float> **pixelVals;
		int numInBands;
	};


	class DllExport RSGISEstimationAssignAP : public RSGISEstimationOptimiser
	{
		/// Assign output vector to initial parameters
		/** - Intended to assign object classification to pixels within object, 
              so estimation is run on an object only basis */
	public:
		RSGISEstimationAssignAP(){};
		virtual int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return rsgis::radar::assignAP;};
		virtual void printOptimiser(){std::cout << "Assign" << std::endl;};
		~RSGISEstimationAssignAP(){};
	private:
	};
    
    class DllExport RSGISEstimationNoEstimation : public RSGISEstimationOptimiser
    {
        /// Class to skip estimation for a class.
        /** Intended to be used within object based estimation as the slow optimiser, 
            so the estimation is run on a pixel only basis */
    public:
        RSGISEstimationNoEstimation(){};
        int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError){return -999;};
        virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
        virtual estOptimizerType getOptimiserType(){return rsgis::radar::noOptimiser;};
        virtual void printOptimiser(){std::cout << "No Optimiser" << std::endl;};
        double calcLeastSquares(std::vector<double> *values);
        ~RSGISEstimationNoEstimation(){};
    };

}}

#endif
