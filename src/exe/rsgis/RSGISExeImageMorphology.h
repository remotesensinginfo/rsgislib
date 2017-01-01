/*
 *  RSGISExeImageMorphology.h
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 01/03/2012
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISExeImageMorphology_H
#define RSGISExeImageMorphology_H

#include <string>
#include <iostream>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "img/RSGISCalcImage.h"
#include "filtering/RSGISMorphologyDilate.h"
#include "filtering/RSGISMorphologyErode.h"
#include "filtering/RSGISMorphologyGradient.h"
#include "filtering/RSGISMorphologyFindExtrema.h"
#include "filtering/RSGISMorphologyClosing.h"
#include "filtering/RSGISMorphologyOpening.h"
#include "filtering/RSGISMorphologyTopHat.h"
#include "img/RSGISImageUtils.h"

#include "utils/RSGISFileUtils.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISMatrices.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

namespace rsgisexe{

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::utils;
using namespace rsgis::math;
using namespace rsgis::filter;

class DllExport RSGISExeImageMorphology : public RSGISAlgorithmParameters
{
public:
    enum options 
    {
        none,
        dilate,
        erode,
        gradient,
        dilateall,
        erodeall,
        gradientall,
        localminima,
        localminimaall,
        opening,
        closing,
        blacktophat,
        whitetophat
    };
    RSGISExeImageMorphology();
    virtual RSGISAlgorithmParameters* getInstance();
    virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
    virtual void runAlgorithm() throw(RSGISException);
    virtual void printParameters();
    virtual string getDescription();
    virtual string getXMLSchema();
    virtual void help();
    ~RSGISExeImageMorphology();
protected:
    options option;
    string inputImage;
    string outputImage;
    string tempImage;
    Matrix *matrixOperator;
    bool allowEquals;
    RSGISImageMorphologyFindExtrema::RSGISMinimaOutputs minOutType;
    bool useMemory;
    unsigned int numIterations;
    GDALDataType outDataType;
    string imageFormat;
};
    
}

#endif
