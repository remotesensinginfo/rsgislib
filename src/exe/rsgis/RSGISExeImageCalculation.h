/*
 *  RSGISExeImageCalculation.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/12/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISExeImageCalculation_H
#define RSGISExeImageCalculation_H

#include <string>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISMathFunction.h"
#include "math/RSGISFunctions.h"
#include "math/RSGISClustering.h"

#include "utils/RSGISFileUtils.h"

#include "geom/RSGISGeometry.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISApplyEigenvectors.h"
#include "img/RSGISMeanVector.h"
#include "img/RSGISCalcImageMatrix.h"
#include "img/RSGISCalcCovariance.h"
#include "img/RSGISCalcCorrelationCoefficient.h"
#include "img/RSGISImageNormalisation.h"
#include "img/RSGISStandardiseImage.h"
#include "img/RSGISBandMath.h"
#include "img/RSGISReplaceValuesLessThanGivenValue.h"
#include "img/RSGISConvertSpectralToUnitArea.h"
#include "img/RSGISImageMaths.h"
#include "img/RSGISCalculateImageMovementSpeed.h"
#include "img/RSGISCountValsAboveThresInCol.h"
#include "img/RSGISCalcRMSE.h"
#include "img/RSGISApplyFunction.h"
#include "img/RSGISCalcEditImage.h"
#include "img/RSGISCalcDist2Geom.h"
#include "img/RSGISImageStatistics.h"
#include "img/RSGISLinearSpectralUnmixing.h"
#include "img/RSGISImageClustering.h"

#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISGetOGRGeometries.h"
#include "vec/RSGISProcessVector.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/PrecisionModel.h"

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "muParser.h"

using namespace std;
using namespace xercesc;
using namespace mu;
using namespace boost;
using namespace rsgis;
using namespace rsgis::math;
using namespace rsgis::img;
using namespace rsgis::utils;
using namespace rsgis::vec;
using namespace rsgis::geom;
using namespace geos::geom;

class RSGISExeImageCalculation : public RSGISAlgorithmParameters
	{
	public:
		
		enum options 
		{
			none,
			normalise,
			correlation,
			covariance,
			meanvector,
			pca,
			standardise,
			bandmaths,
			replacevalueslessthan,
			unitarea,
			imagemaths,
			movementspeed,
			countvalsincol,
			calcRMSE,
			apply2VarFunction,
			apply3VarFunction,
            dist2geoms,
            imagebandstats,
            imagestats,
            unconlinearspecunmix,
            exhconlinearspecunmix,
            kmeanscentres,
            isodatacentres
		};
		
		struct VariableStruct
		{
			string image;
			string name;
			int bandNum;
		};
		
		RSGISExeImageCalculation();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeImageCalculation();
	protected:
		options option;
		string inputImage;
		string *inputImages;
		string outputImage;
		string *outputImages;
		string inputImageA;
		string inputImageB;
		string inputMatrixA;
		string inputMatrixB;
		string outputMatrix;
		string eigenvectors;
		string meanvectorStr;
		string mathsExpression;
		string inMatrixfile;
        string inputVector;
        string outputFile;
        string endmembersFile;
		int numComponents;
		int numImages;
		int numVars;
		int inputBandA;
		int inputBandB;
		double inMin;
		double inMax;
		double outMin;
		double outMax;
		bool calcInMinMax;
		bool calcMean;
		VariableStruct *variables;
		double value;
		double threshold;
		unsigned int *imageBands;
		float *imageTimes;
		float upper;
		float lower;
        float stepResolution;
		gsl_matrix *coeffMatrix;
		RSGISMathTwoVariableFunction *twoVarFunction;
		RSGISMathThreeVariableFunction *threeVarFunction;
		unsigned int polyOrderX;
		unsigned int polyOrderY;
		unsigned int polyOrderZ;
        float imgResolution;
        unsigned int maxNumIterations;
        float degreeOfChange;
        unsigned int subSample;
        unsigned int numClusters;
        rsgis::math::InitClustererMethods initClusterMethod;
        bool ignoreZeros;
        float minDistBetweenClusters;
        unsigned int minNumFeatures;
        float maxStdDev;
        unsigned int minNumClusters;
        unsigned int startIteration;
        unsigned int endIteration;
		string imageFormat;
	};

#endif

