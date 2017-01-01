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

#include "common/RSGISCommons.h"
#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "cmds/RSGISCmdImageCalc.h"
#include "cmds/RSGISCmdException.h"
#include "cmds/RSGISCmdCommon.h"

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
#include "img/RSGISGenHistogram.h"

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

namespace rsgisexe{

    class DllExport RSGISExeImageCalculation : public rsgis::RSGISAlgorithmParameters
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
            consum1linearspecunmix,
            nnconsum1linearspecunmix,
            kmeanscentres,
            isodatacentres,
            allbandsequalto,
            histogram,
            bandpercentile,
            imgdist2geoms,
            imgcalcdist,
            mahalanobisdistwindow,
            mahalanobisdistimg2window,
            calcpxlcolstats,
            pxlcolregression,
            correlationWindow,
            imagebandstatsenv
		};
		
		
		RSGISExeImageCalculation();
		virtual rsgis::RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(rsgis::RSGISException);
		virtual void printParameters();
		virtual std::string getDescription();
		virtual std::string getXMLSchema();
        virtual void setInMin(double min);
        virtual void setInMax(double max);
		virtual void help();
		~RSGISExeImageCalculation();
	protected:
		options option;
		std::string inputImage;
		std::string *inputImages;
		std::string outputImage;
		std::string *outputImages;
		std::string inputImageA;
		std::string inputImageB;
		std::string inputMatrixA;
		std::string inputMatrixB;
		std::string outputMatrix;
		std::string eigenvectors;
		std::string meanvectorStr;
		std::string mathsExpression;
		std::string inMatrixfile;
        std::string inputVector;
        std::string outputFile;
        std::string endmembersFile;
        std::string imageMask;
        std::string bandValues;
		int numComponents;
		int numImages;
		int numVars;
		int inputBandA;
		int inputBandB;
		double inMin;
		double inMax;
		double outMin;
		double outMax;
        double binWidth;
		bool calcInMinMax;
		bool calcMean;
        rsgis::cmds::VariableStruct *variables;
		double value;
		double threshold;
		unsigned int *imageBands;
		float *imageTimes;
		float upper;
		float lower;
        float stepResolution;
		gsl_matrix *coeffMatrix;
		rsgis::math::RSGISMathTwoVariableFunction *twoVarFunction;
		rsgis::math::RSGISMathThreeVariableFunction *threeVarFunction;
		unsigned int polyOrderX;
		unsigned int polyOrderY;
		unsigned int polyOrderZ;
        float imgResolution;
        unsigned int maxNumIterations;
        float degreeOfChange;
        unsigned int subSample;
        unsigned int numClusters;
        rsgis::cmds::RSGISInitClustererMethods initClusterMethod;
        bool ignoreZeros;
        float minDistBetweenClusters;
        unsigned int minNumFeatures;
        float maxStdDev;
        unsigned int minNumClusters;
        unsigned int startIteration;
        unsigned int endIteration;
		std::string imageFormat;
        float imgValue;
        float outputTrueVal;
        float outputFalseVal;
        GDALDataType outDataType;
        rsgis::RSGISLibDataType rsgisOutDataType;
        float lsumWeight;
        float lsumGain;
        float lsumOffset;
        unsigned int imgBand;
        float percentile;
        float noDataValue;
        bool noDataValueSpecified;
        unsigned int windowSize;
        rsgis::cmds::RSGISCmdStatsSummary statsSummary;
        unsigned int corrBandA;
        unsigned int corrBandB;
        unsigned int imageBand;
        double latMin;
        double latMax;
        double longMin;
        double longMax;
	};
}
#endif

