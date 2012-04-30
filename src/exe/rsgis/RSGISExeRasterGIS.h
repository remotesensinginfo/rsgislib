/*
 *  RSGISExeRasterGIS.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/02/2012.
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

#ifndef RSGISExeRasterGIS_H
#define RSGISExeRasterGIS_H

#include <string>
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <utility>

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISFileUtils.h"

#include "math/RSGISMathsUtils.h"

#include "img/RSGISPixelInPoly.h"

#include "segmentation/RSGISRegionGrowAttributeTable.h"

#include "rastergis/RSGISAttributeTable.h"
#include "rastergis/RSGISAttributeTableMem.h"
#include "rastergis/RSGISAttributeTableNeighbours.h"
#include "rastergis/RSGISPopulateAttributeTable.h"
#include "rastergis/RSGISCreateImageFromAttributeTable.h"
#include "rastergis/RSGISFindClumpCategoryMajority.h"
#include "rastergis/RSGISCreateNewAttributeTable.h"
#include "rastergis/RSGISFindClumpNeighbours.h"
#include "rastergis/RSGISFindMeanDist2Neighbours.h"

#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISAttCountPolyIntersect.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::utils;
using namespace rsgis::math;
using namespace rsgis::img;
using namespace rsgis::vec;
using namespace rsgis::rastergis;
using namespace rsgis::segment;

class RSGISExeRasterGIS : public RSGISAlgorithmParameters
{
public:
    
    enum options 
    {
        none,
        createattributetable,
        popattributemean,
        exporttable2img,
        boolclasstable,
        addfields,
        regiongrowthres,
        findmajority,
        countpolyintersects,
        calcattributes,
        popattributestatsinmem,
        export2gdal,
        popattributestatsallbands,
        summarisefield,
        findneighbours,
        meaneucdist2neighbours,
        calcintrapxleucdist,
        meanlitbandspopattributestats,
        meanlitpopattributestats,
        meanlitpopattributestatsinmem,
        exportfield2ascii,
        popattributestats,
        export2hdf,
        export2ascii,
        popmeansumattributes,
        printattsummary
    };
    
    RSGISExeRasterGIS();
    virtual RSGISAlgorithmParameters* getInstance();
    virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
    virtual void runAlgorithm() throw(RSGISException);
    virtual void printParameters();
    virtual string getDescription();
    virtual string getXMLSchema();
    virtual void help();
    ~RSGISExeRasterGIS();
protected:
    options option;
    string inputImage;
    string clumpsImage;
    string catagoriesImage;
    string outputFile;
    string inputVector;
    string mathsExpression;
    string attprefix;
    string attTableFile;
    string outAttTableFile;
    string neighboursFile;
    string imageFormat;
    string majorityRatioField;
    string majorityCatField;
    string attField;
    string attMeanField;
    string attMinField;
    string attMaxField;
    RSGISAttributeDataType attFieldDT;
    string meanlitField;
    string meanlitImage;
    unsigned int meanLitBand;
    bool meanLitUseUpper;
    string areaField;
    string classAttributeName;
    int classAttributeVal;
    pixelInPolyOption pixelPolyOption;
    vector<pair<unsigned int, string> > *bands;
    vector<RSGISAttribute*> *attributes;
    vector<RSGISIfStatement*> *statements;
    vector<RSGISMathAttVariable*> *variables;
    vector<RSGISBandAttStats*> *bandStats;
    vector<RSGISBandAttStatsMeanLit*> *bandStatsMeanLit;
    vector<string> *attributeNames;
    RSGISBandAttStats *imageStats;
    RSGISStatsSummary *statsSummary;
    vector<RSGISBandAttName*> *bandAttNames;
    bool inoutTable;
    bool neighboursProvided;
    bool attInMemory;
    unsigned int cacheSize;
};

#endif



