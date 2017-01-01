/*
 *  RSGISExeSegment.h
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 13/01/2012
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

#ifndef RSGISExeSegment_H
#define RSGISExeSegment_H

#include <string>
#include <iostream>

#include "common/RSGISCommons.h"
#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "cmds/RSGISCmdSegmentation.h"
#include "cmds/RSGISCmdException.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISProjectionStrings.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISStretchImage.h"

#include "segmentation/RSGISSpecGroupSegmentation.h"
#include "segmentation/RSGISGenMeanSegImage.h"
#include "segmentation/RSGISEliminateSmallClumps.h"
#include "segmentation/RSGISClumpPxls.h"
#include "segmentation/RSGISRandomColourClumps.h"
#include "segmentation/RSGISRegionGrowingFromClumps.h"
#include "segmentation/RSGISMergeSmallClumps.h"
#include "segmentation/RSGISLabelPixelsUsingClusters.h"
#include "segmentation/RSGISLabelPixelsUsingPixels.h"
#include "segmentation/RSGISRegionGrowSegmentsPixels.h"
#include "segmentation/RSGISDefineSpectralDivision.h"
#include "segmentation/RSGISEliminateSinglePixels.h"
#include "segmentation/RSGISMergeSegmentationTiles.h"

//#include "rastergis/RSGISAttributeTable.h"
//#include "rastergis/RSGISCreateNewAttributeTable.h"

#include "utils/RSGISFileUtils.h"
#include "math/RSGISMathsUtils.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

namespace rsgisexe{

    class DllExport RSGISExeSegment : public rsgis::RSGISAlgorithmParameters
	{
	public:
		enum options 
		{
			none,
            specgrp,
            meanimg,
            rmsmallclumps,
            rmsmallclumpsstepwise,
            clump,
            randomcolourclumps,
            regiongrowingseedclumpids,
            growregionsusingclumps,
            histogramseeds,
            histogramseedstxt,
            selectclumps,
            selectclumpstxt,
            mergesmallclumps,
            relabelclumps,
            specgrpweighted,
            labelsfromclusters,
            labelsfrompixels,
            growregionspixelsauto,
            growregionspixels,
            spectraldiv,
            stepwiseelimination,
            elimsinglepxls,
            unionsegments,
            mergeclumptiles,
            findtilebordersmask,
            mergeclumpimages,
            extractbrightfeatures
		};
		RSGISExeSegment();
		virtual rsgis::RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(xercesc::DOMElement *argElement) throw(rsgis::RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(rsgis::RSGISException);
		virtual void printParameters();
		virtual std::string getDescription();
		virtual std::string getXMLSchema();
		virtual void help();
		~RSGISExeSegment();
	protected:
		options option;
		std::string inputImage;
        std::string clumpsImage;
        std::string largeClumpsImage;
		std::string outputImage;
        std::string borderMaskImage;
        std::string imageFormat;
        std::string outputTextFile;
        std::string seedsTextFile;
        std::string clustersMatrix;
        std::string tempTable;
        std::string tempImage;
        std::string temp1Image;
        std::string temp2Image;
        std::string maskImage;
        std::string proj;
        std::string stretchStatsFile;
        std::vector<std::string> inputImagePaths;
        bool stretchStatsAvail;
        unsigned int minClumpSize;
        float specThreshold;
        float initRegionGrowthreshold;
        float thresholdRegionGrowincrements;
        float maxRegionGrowthreshold;
        unsigned int maxRegionGrowiterations;
        bool processInMemory;
        bool projFromImage;
        std::vector<rsgis::segment::ImgSeeds> *seedPxls;
        std::vector<rsgis::segment::BandThreshold> *bandThresholds;
        unsigned int noDataVal;
        bool noDataValProvided;
        rsgis::segment::RSGISSelectClumps::ClumpSelection selectionMethod;
        bool ignoreZeros;
        //std::vector<rsgis::segment::RSGISRegionGrowPxlSeeds> *regionGrowingPxlSeeds;
        unsigned int subDivision;
        unsigned int levels;
        bool outputWithConsecutiveFIDs;
        unsigned int cacheSize;
        std::string importLUTFile;
        bool importLUT;
        std::string exportLUTFile;
        bool exportLUT;
        bool storeMean;
        unsigned int tileBoundary;
        unsigned int tileOverlap;
        unsigned int tileBody;
        std::string colsName;
        rsgis::RSGISLibDataType rsgisOutDataType;
        float initThres;
        float thresIncrement;
        float thresholdUpper;
        std::vector<rsgis::cmds::FeatureShapeDescription*> shapeFeatDescript;
	};
}
#endif
