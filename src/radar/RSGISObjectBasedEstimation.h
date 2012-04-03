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

using namespace std;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::math;
using namespace rsgis::vec;
using namespace geos::geom;

namespace rsgis{namespace radar{
	
	class RSGISObjectBasedEstimation : public RSGISProcessOGRFeature
	{
	public:
		RSGISObjectBasedEstimation(GDALDataset *inputImage, GDALDataset *outputImage, vector <gsl_vector*> *initialPar, vector <RSGISEstimationOptimiser*> *slowOptimiser, vector <RSGISEstimationOptimiser*> *fastOptimiser, estParameters parameters, double ***minMaxVals = NULL, string classHeading = "", bool useClass = false);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid)throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, Envelope *env, long fid)throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		virtual ~RSGISObjectBasedEstimation();
	protected:
		GDALDataset **datasetsIO;
		GDALDataset **datasetsInput;
		vector<float> **pixelVals;
		int numBands;
		unsigned int objectSamples;
		double objectSamplesPercent;
		unsigned int numOutputPar;
		unsigned int numOutputBands;
		RSGISCalcImageSingleValue *getValues;
		RSGISCalcImageSingle *calcImageSingle;
		RSGISCalcImage *calcImage;
		vector <gsl_vector*> *initialPar;
		vector <RSGISEstimationOptimiser*> *slowOptimiser;
		vector <RSGISEstimationOptimiser*> *fastOptimiser;
		RSGISEstimationOptimiser *slowOptimiserSingle;
		RSGISEstimationOptimiser *fastOptimiserSingle;
		gsl_vector *initialParSingle;
		estParameters parameters;
		string classHeading;
		bool useClass; // Use multiple classes
		bool useDefaultMinMax; // Pass in minimum and maximum values to estimation algorithm or use default
		double ***minMaxVals; // Minimum and maximum values for parameters
		
	};
    
    class RSGISObjectBasedEstimationObjectAP : public RSGISProcessOGRFeature
	{
	public:
		RSGISObjectBasedEstimationObjectAP(GDALDataset *inputImage, GDALDataset *outputImage, vector <gsl_vector*> *initialPar, vector <RSGISEstimationOptimiser*> *slowOptimiser, vector <RSGISEstimationOptimiser*> *fastOptimiser, estParameters parameters, string *apParField, double ***minMaxVals = NULL, string classHeading = "", bool useClass = false);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid)throw(RSGISVectorException);
        virtual void processFeature(OGRFeature *feature, Envelope *env, long fid)throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		virtual ~RSGISObjectBasedEstimationObjectAP();
	protected:
		GDALDataset **datasetsIO;
		GDALDataset **datasetsInput;
		vector<float> **pixelVals;
		int numBands;
		unsigned int objectSamples;
		double objectSamplesPercent;
		unsigned int numOutputPar;
		unsigned int numOutputBands;
		RSGISCalcImageSingleValue *getValues;
		RSGISCalcImageSingle *calcImageSingle;
		RSGISCalcImage *calcImage;
		vector <gsl_vector*> *initialPar;
		vector <RSGISEstimationOptimiser*> *slowOptimiser;
		vector <RSGISEstimationOptimiser*> *fastOptimiser;
		RSGISEstimationOptimiser *slowOptimiserSingle;
		RSGISEstimationOptimiser *fastOptimiserSingle;
		gsl_vector *initialParSingle;
		estParameters parameters;
		string classHeading;
		bool useClass; // Use multiple classes
		bool useDefaultMinMax; // Pass in minimum and maximum values to estimation algorithm or use default
		double ***minMaxVals; // Minimum and maximum values for parameters
		string *apParField;
        gsl_matrix *covMatrixP;
        gsl_matrix *invCovMatrixD;
        RSGISMathTwoVariableFunction *functionA;
        RSGISMathTwoVariableFunction *functionB;
	};
	
	class RSGISObjectBasedEstimationRasterPolygon : public RSGISProcessOGRFeature
	{
	public:
		RSGISObjectBasedEstimationRasterPolygon(GDALDataset *inputImage, GDALDataset *outputImage,  GDALDataset *rasterFeatures, vector <gsl_vector*> *initialPar, vector <RSGISEstimationOptimiser*> *slowOptimiser, vector <RSGISEstimationOptimiser*> *fastOptimiser, estParameters parameters, double ***minMaxVals = NULL, string classHeading = "", bool useClass = false);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, Envelope *env, long fid)throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		virtual ~RSGISObjectBasedEstimationRasterPolygon();
	protected:
		GDALDataset **datasetsIO;
		GDALDataset **datasetsInput;
		vector<float> **pixelVals;
		int numBands;
		unsigned int objectSamples;
		double objectSamplesPercent;
		unsigned int numOutputPar;
		unsigned int numOutputBands;
		RSGISCalcImageSingleValue *getValues;
		RSGISCalcImageSingle *calcImageSingle;
		RSGISCalcImageValue *invValues;
		RSGISCalcImage *calcImage;
		vector <gsl_vector*> *initialPar;
		vector <RSGISEstimationOptimiser*> *slowOptimiser;
		vector <RSGISEstimationOptimiser*> *fastOptimiser;
		RSGISEstimationOptimiser *slowOptimiserSingle;
		RSGISEstimationOptimiser *fastOptimiserSingle;
		gsl_vector *initialParSingle;
		estParameters parameters;
		string classHeading;
		bool useClass; // Use multiple classes
		bool useDefaultMinMax; // Pass in minimum and maximum values to estimation algorithm or use default
		double ***minMaxVals; // Minimum and maximum values for parameters
	};
	
	class RSGISObjectBasedEstimationGetObjVals : public RSGISCalcImageSingleValue
	{
		/// Loops though bands and loads pixel values to vectors
	public: 
		RSGISObjectBasedEstimationGetObjVals(vector<float> **pixelVals, int numBands);
		void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, int numBands, int band) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValuesImage, int numBands, Envelope *extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException);
		double* getOutputValues() throw(RSGISImageCalcException);
		void reset();
		~RSGISObjectBasedEstimationGetObjVals();
	protected:
		vector<float> **pixelVals;
		int numBands;
	};
	
	
	class RSGISEstimationAssignAP : public RSGISEstimationOptimiser
	{
		/// Assign output vector to initial parameters
		/** - Primarily to assign object classification to pixels within object.*/
	public:
		RSGISEstimationAssignAP(){};
		virtual int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return assignAP;}; 
		virtual void printOptimiser(){cout << "Assign" << endl;};
		~RSGISEstimationAssignAP(){};
	private:
	};
	
}}

#endif
