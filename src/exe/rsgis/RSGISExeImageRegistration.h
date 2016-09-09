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
#include "registration/RSGISPolynomialImageWarp.h"
#include "registration/RSGISImagePixelRegistration.h"
#include "registration/RSGISAddGCPsGDAL.h"

#include "utils/RSGISFileUtils.h"
#include "utils/RSGISTextUtils.h"
#include "math/RSGISMathsUtils.h"

#include "cmds/RSGISCmdParent.h"
#include "cmds/RSGISCmdImageRegistration.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

namespace rsgisexe{

    class DllExport RSGISExeImageRegistration : public rsgis::RSGISAlgorithmParameters
    {
    public:
        enum options 
        {
            none,
            basic,
            singlelayer,
            triangularwarp,
            nnwarp,
            polywarp,
            pxlshift,
            gcp2gdal
        };
        
        enum OutputType
        {
            undefinedOutput,
            envi_img2img,
            envi_img2map,
            rsgis_img2map,
            rsgis_mapoffs
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
        virtual rsgis::RSGISAlgorithmParameters* getInstance();
        virtual void retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
        virtual void runAlgorithm() throw(rsgis::RSGISException);
        virtual void printParameters();
        virtual std::string getDescription();
        virtual std::string getXMLSchema();
        virtual void help();
        ~RSGISExeImageRegistration();
    protected:
        options option;
        std::string inputImage;
        std::string outputImage;
        std::string inputGCPs;
        std::string inputReferenceImage;
        std::string inputFloatingmage;
        std::string outputGCPFile;
        std::string projFile;
        std::string imageFormat;
        GDALDataType outDataType;
        rsgis::RSGISLibDataType rsgisOutDataType;
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
        int polyOrder;
        bool genTransformImage;
        
    };
}

#endif
