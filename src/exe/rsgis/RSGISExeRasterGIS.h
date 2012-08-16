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
            gencolourtab
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
        std::vector<std::string> fields;
        size_t fid;
        unsigned int nFeatures;
        float distThreshold;
        std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats;
        std::vector<rsgis::rastergis::RSGISBandAttPercentiles*> *bandPercentiles;
        bool copyClassNames;
        rsgis::rastergis::ClassMajorityMethod majMethod;
        float weightA;
        std::map<size_t, size_t> classPairs;
        std::map<size_t, rsgis::utils::RSGISColourInt> classColourPairs;
        unsigned int redBand;
        unsigned int greenBand;
        unsigned int blueBand;
    };
    
}

#endif



