/*
 *  RSGISExeRasterGIS.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/07/2012.
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

#include "cmds/RSGISCmdRasterGIS.h"
#include "cmds/RSGISCmdSegmentation.h"
#include "cmds/RSGISCmdException.h"

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISFileUtils.h"
#include "utils/RSGISColour.h"

#include "rastergis/RSGISCalcClusterLocation.h"
#include "rastergis/RSGISCalcEucDistanceInAttTable.h"
#include "rastergis/RSGISFindTopNWithinDist.h"
#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISCalcClumpStats.h"
#include "rastergis/RSGISFindClumpCatagoryStats.h"
#include "rastergis/RSGISKNNATTMajorityClassifier.h"
#include "rastergis/RSGISExportColumns2Image.h"
#include "rastergis/RSGISFindInfoBetweenLayers.h"
#include "rastergis/RSGISFindClosestSpecSpatialFeats.h"
#include "rastergis/RSGISMaxLikelihoodRATClassification.h"
#include "rastergis/RSGISClassMask.h"
#include "rastergis/RSGISFindClumpNeighbours.h"
#include "rastergis/RSGISClumpBorders.h"
#include "rastergis/RSGISCalcClumpShapeParameters.h"
#include "rastergis/RSGISDefineImageTiles.h"
#include "rastergis/RSGISFindChangeClumps.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace rsgisexe{    
    
    class RSGISExeRasterGIS : public rsgis::RSGISAlgorithmParameters
    {
    public:
        
        enum options 
        {
            none,
            copyGDALATT,
            spatiallocation,
            eucdistfromfeat,
            findtopn,
            copyGDALATTColumns,
            popattributestats,
            popcategoryproportions,
            copycatcolours,
            knnmajorityclassifier,
            popattributepercentile,
            export2ascii,
            classtranslate,
            colourclasses,
            colourstrclasses,
            gencolourtab,
            exportcols2raster,
            strclassmajority,
            findspecclose,
            specdistmajorityclassifier,
            maxlikelihoodclassifier,
            maxlikelihoodclassifierlocalpriors,
            classmask,
            findneighbours,
            findboundarypixels,
            calcborderlength,
            calcrelborder,
            calcshapeindices,
            defineclumptileposition,
            defineborderclumps,
            populatestats,
            findchangeclumpsfromstddev,
            rmsmallclumps,
            selectclumpsongrid
        };
        
        RSGISExeRasterGIS();
        virtual rsgis::RSGISAlgorithmParameters* getInstance();
        virtual void retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
        virtual void runAlgorithm() throw(rsgis::RSGISException);
        virtual void printParameters();
        virtual std::string getDescription();
        virtual std::string getXMLSchema();
        virtual void help();
        ~RSGISExeRasterGIS();
    protected:
        options option;
        std::string inputImage;
        std::string clumpsImage;
        std::string outputFile;
        std::string categoriesImage;
        std::string outputField;
        std::string northingsField;
        std::string eastingsField;
        std::string spatialDistField;
        std::string distanceField;
        std::string outColsName;
        std::string majorityColName;
        std::string majClassNameField;
        std::string classNameField;
        std::string inClassNameField;
        std::string outClassNameField;
        std::string areaField;
        std::string classField;
        std::string classInField;
        std::string classOutField;
        std::string majWeightField;
        std::string imageFormat;
        std::vector<std::string> fields;
        std::string baseSegment;
        std::string infoSegment;
        std::string baseClassCol;
        std::string infoClassCol;
        std::string trainingSelectCol;
        std::string className;
        std::string tileImage;
        std::string maskImage;
        std::string changeField;
        std::string inSelectField;
        std::string outSelectField;
        std::string metricCol;
        std::string methodStr;
        size_t fid;
        unsigned int nFeatures;
        float specDistThreshold;
        float distThreshold;
        float areaThreshold;
        std::vector<rsgis::cmds::RSGISBandAttStatsCmds*> *bandStats;
        std::vector<rsgis::cmds::RSGISBandAttPercentilesCmds*> *bandPercentiles;
        bool copyClassNames;
        rsgis::rastergis::ClassMajorityMethod majMethod;
        float weightA;
        std::map<size_t, size_t> classPairs;
        std::map<size_t, rsgis::utils::RSGISColourInt> classColourPairs;
        std::map<std::string, rsgis::utils::RSGISColourInt> classStrColourPairs;
        unsigned int redBand;
        unsigned int greenBand;
        unsigned int blueBand;
        GDALDataType outDataType;
        rsgis::RSGISLibDataType rsgisOutDataType;
        rsgis::rastergis::SpectralDistanceMethod distThresMethod;
        float specThresOriginDist;
        rsgis::rastergis::rsgismlpriors priorsMethod;
        std::vector<std::string> priorStrs;
        bool allowZeroPriors;
        bool ignoreZeroEdges;
        std::vector<rsgis::rastergis::RSGISShapeParam*> *shapeIndexes;
        unsigned int tileBoundary;
        unsigned int tileOverlap;
        unsigned int tileBody;
        bool calcImgPyramids;
        bool addColourTable2Img;
        std::vector<rsgis::rastergis::RSGISClassChangeFields*> *classChangeField;
        std::vector<std::string> *attFields;
        unsigned int numRows;
        unsigned int numCols;
    };
    
}

#endif



