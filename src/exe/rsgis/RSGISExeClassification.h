/*
 *  RSGISExeClassification.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/12/2008.
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

#ifndef RSGISExeClassification_H
#define RSGISExeClassification_H

#include <string>
#include <limits>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <gsl/gsl_matrix.h>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"
#include "common/RSGISFileException.h"

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISColour.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"

#include "classifier/RSGISClassifier.h"
#include "classifier/RSGISNearestNeighbourClassifier.h"
#include "classifier/RSGISSpectralAngleMapper.h"
#include "classifier/RSGISSpectralCorrelationMapper.h"
#include "classifier/RSGISCumulativeAreaClassifier.h"
#include "classifier/RSGISKMeanImageClassifier.h"
#include "classifier/RSGISISODATAImageClassifier.h"
#include "classifier/RSGISClassificationUtils.h"
#include "classifier/RSGISRATClassificationUtils.h"

#include "cmds/RSGISCmdClassification.h"
#include "cmds/RSGISCmdException.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISImageUtils.h"

#include "vec/RSGISVectorSQLClassification.h"
#include "vec/RSGISVectorUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace rsgisexe{

class DllExport RSGISExeClassification : public rsgis::RSGISAlgorithmParameters
	{
	public:
		
		enum options 
		{
			none,
			nn,
			vectorsql,
			samRule,
			samClassify,
			scmRule,
			scmClassify,
			cumulativeAreaRule,
			cumulativeAreaClassify,
			kmeans,
			isodata,
			createspeclib,
            addcolourtable,
            elimsinglepxls,
            collapseclasses,
            colour3bands
		};
		
		enum UnsupervisedInit
		{
			undefinedInit,
			randomInit,
			kppInit
		};
		
		RSGISExeClassification();
		virtual rsgis::RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(rsgis::RSGISException);
		virtual void printParameters();
		virtual std::string getDescription();
		virtual std::string getXMLSchema();
		virtual void help();
		~RSGISExeClassification();
	protected:
		options option;
		std::string inputImage;
		std::string outputImage;
		std::string outputFile;
		std::string vector;
		std::string classAttribute;
		std::string ruleImage;
        std::string imageFormat;
        std::string classNameCol;
		rsgis::classifier::ClassData **trainingData;
        rsgis::vec::sqlclass **rules;
		int numClasses;
		gsl_matrix *specLib;
		double threshold;
		std::string inMatrixfile;
		std::string inMatrixSpecLibStr;
		UnsupervisedInit initAlgor;
		unsigned int numClusters;
		unsigned int maxNumIterations;
		float clusterMoveThreshold;
		unsigned int minNumVals;
		double minDistanceBetweenCentres;
		double stddevThres;
		float propOverAvgDist;
		bool printinfo;
		bool groupSamples;
		std::vector<std::string> *valueAttributes;
		bool savekmeansCentres;
		std::string outkmeansCentresFileName;
        std::vector<std::pair<int, rsgis::utils::RSGISColourInt> > classColourPairs;
        unsigned int imageBand;
        rsgis::img::RSGISRasterConnectivity filterConnectivity;
        bool ignoreZeros;
	};
}
#endif


