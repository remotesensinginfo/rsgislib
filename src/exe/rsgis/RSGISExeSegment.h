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

#include "common/RSGISException.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISImageException.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISProjectionStrings.h"
#include "img/RSGISImageUtils.h"

#include "segmentation/RSGISSpecGroupSegmentation.h"
#include "segmentation/RSGISGenMeanSegImage.h"
#include "segmentation/RSGISEliminateSmallClumps.h"
#include "segmentation/RSGISClumpPxls.h"
#include "segmentation/RSGISRandomColourClumps.h"
#include "segmentation/RSGISRegionGrowingFromClumps.h"
#include "segmentation/RSGISMergeSmallClumps.h"
#include "segmentation/RSGISLabelPixelsUsingClusters.h"
#include "segmentation/RSGISRegionGrowSegmentsPixels.h"
#include "segmentation/RSGISDefineSpectralDivision.h"
#include "segmentation/RSGISEliminateSinglePixels.h"

#include "rastergis/RSGISAttributeTable.h"
#include "rastergis/RSGISCreateNewAttributeTable.h"

#include "utils/RSGISFileUtils.h"
#include "math/RSGISMathsUtils.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "gdal_priv.h"

using namespace std;
using namespace xercesc;
using namespace rsgis;
using namespace rsgis::img;
using namespace rsgis::utils;
using namespace rsgis::math;
using namespace rsgis::segment;

class RSGISExeSegment : public RSGISAlgorithmParameters
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
            growregionspixelsauto,
            growregionspixels,
            spectraldiv,
            stepwiseelimination,
            elimsinglepxls
		};
		RSGISExeSegment();
		virtual RSGISAlgorithmParameters* getInstance();
		virtual void retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException);
		virtual void runAlgorithm() throw(RSGISException);
		virtual void printParameters();
		virtual string getDescription();
		virtual string getXMLSchema();
		virtual void help();
		~RSGISExeSegment();
	protected:
		options option;
		string inputImage;
        string clumpsImage;
        string largeClumpsImage;
		string outputImage;
        string imageFormat;
        string outputTextFile;
        string seedsTextFile;
        string clustersMatrix;
        string tempTable;
        string tempImage;
        string proj;
        unsigned int minClumpSize;
        float specThreshold;
        float initRegionGrowthreshold;
        float thresholdRegionGrowincrements;
        float maxRegionGrowthreshold;
        unsigned int maxRegionGrowiterations;
        bool processInMemory;
        bool projFromImage;
        vector<ImgSeeds> *seedPxls;
        vector<BandThreshold> *bandThresholds;
        unsigned int noDataVal;
        bool noDataValProvided;
        RSGISSelectClumps::ClumpSelection selectionMethod;
        bool ignoreZeros;
        vector<RSGISRegionGrowPxlSeeds> *regionGrowingPxlSeeds;
        unsigned int subDivision;
        unsigned int levels;
        bool outputWithConsecutiveFIDs;
        unsigned int cacheSize;
        string importLUTFile;
        bool importLUT;
        string exportLUTFile;
        bool exportLUT;
	};

#endif
