/*
 *  RSGISExeImageRegistration.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/09/2010.
 *  Copyright 2010 RSGISLib.
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

#ifndef RSGISExeImageRegistration_H
#define RSGISExeImageRegistration_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISRegistrationException.h"

#include "registration/RSGISImageRegistration.h"
#include "registration/RSGISBasicImageRegistration.h"
#include "registration/RSGISImageSimilarityMetric.h"
#include "registration/RSGISStandardImageSimilarityMetrics.h"
#include "registration/RSGISSingleConnectLayerImageRegistration.h"
#include "registration/RSGISWarpImage.h"
#include "registration/RSGISBasicNNGCPImageWarp.h"
#include "registration/RSGISWarpImageInterpolator.h"
#include "registration/RSGISWarpImageUsingTriangulation.h"

#include "utils/RSGISFileUtils.h"
#include "utils/RSGISTextUtils.h"
#include "math/RSGISMathsUtils.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::reg;
using namespace rsgis::utils;
using namespace rsgis::math;

class RSGISExeImageRegistration : public RSGISAlgorithmParameters
{
public:
	enum options 
	{
		none,
		basic,
		singlelayer,
		triangularwarp,
		nnwarp
	};
	
	enum OutputType
	{
		undefinedOutput,
		envi_img2img,
		envi_img2map,
		rsgis_img2map
	};
	
	enum SimilarityMetric 
	{
		undefinedMetric,
		euclidean,
		sqdiff,
		manhatten,
		correlation
	};
	
	RSGISExeImageRegistration();
	virtual RSGISAlgorithmParameters* getInstance();
	virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
	virtual void runAlgorithm() throw(RSGISException);
	virtual void printParameters();
	virtual string getDescription();
	virtual string getXMLSchema();
	virtual void help();
	~RSGISExeImageRegistration();
protected:
	options option;
	string inputImage;
	string outputImage;
	string inputGCPs;
	string inputReferenceImage;
	string inputFloatingmage;
	string outputGCPFile;
	string projFile;
	OutputType outputType;
	SimilarityMetric metricType;
	unsigned int gcpGap;
	unsigned int windowSize;
	unsigned int searchArea;
	float metricThreshold;
	float stdDevRefThreshold;
	float stdDevFloatThreshold;
	unsigned int subPixelResolution;
	float distanceThreshold;
	unsigned int maxNumIterations;
	float moveChangeThreshold;
	float pSmoothness;
	float resolution;
};

#endif
