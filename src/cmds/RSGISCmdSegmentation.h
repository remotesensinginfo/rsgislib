/*
 *  RSGISCmdSegmentation.h
 *
 *
 *  Created by Pete Bunting on 03/05/2013.
 *  Copyright 2013 RSGISLib.
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

#ifndef RSGISCmdSegmentation_H
#define RSGISCmdSegmentation_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_cmds_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{ namespace cmds {
    
    struct DllExport FeatureShapeDescription
    {
        bool area;
        double areaLower;
        double areaUpper;
        bool lenWidth;
        double lenWidthLower;
        double lenWidthUpper;
    };
    
    struct DllExport VarImgBandPairs
    {
        std::string varName;
        unsigned int imgBand;
    };
    
    
    /** Function to run the label pixels from clusters centres command */
    DllExport void executeLabelPixelsFromClusterCentres(std::string inputImage, std::string outputImage, std::string clusterCentresFile, bool ignoreZeros, std::string imageFormat);
    
    /** Function to run the eliminate single pixels command */
    DllExport void executeEliminateSinglePixels(std::string inputImage, std::string clumpsImage, std::string outputImage, std::string tempImage, std::string imageFormat, bool processInMemory, bool ignoreZeros);
    
    /** Function to run the clump command */
    DllExport void executeClump(std::string inputImage, std::string outputImage, std::string imageFormat, bool processInMemory, bool noDataValProvided, float noDataVal, bool addRatPxlVals=true);

    /** Function to run the iterative stepwise elimination command */
    DllExport void executeRMSmallClumpsStepwise(std::string inputImage, std::string clumpsImage, std::string outputImage, std::string imageFormat, bool stretchStatsAvail, std::string stretchStatsFile, bool storeMean, bool processInMemory, unsigned int minClumpSize, float specThreshold);
    
    /** Function to run the relabel clumps command */
    DllExport void executeRelabelClumps(std::string inputImage, std::string outputImage, std::string imageFormat, bool processInMemory);
    
    /** Function to run generate mean image command */
    DllExport void executeMeanImage(std::string inputImage, std::string clumpsImage, std::string outputImage, std::string imageFormat, RSGISLibDataType outDataType, bool processInMemory);
    
    /** Function to run assign random colours to clumps commands */
    DllExport void executeRandomColourClumps(std::string inputImage, std::string outputImage, std::string imageFormat, bool processInMemory, std::string importLUTFile, bool importLUT, std::string exportLUTFile, bool exportLUT);
    
    /** Function to run union of segmentations command */
    DllExport void executeUnionOfClumps(std::vector<std::string> inputImagePaths, std::string outputImage, std::string imageFormat, bool noDataValProvided, float noDataVal, bool addRatPxlVals=true);
    
    /** Function to run merge segment tiles command */
    DllExport void executeMergeSegmentationTiles(std::string outputImage, std::string borderMaskImage, std::vector<std::string> inputImagePaths, unsigned int tileBoundary, unsigned int tileOverlap, unsigned int tileBody, std::string colsName);
    
    /** Function to run command to find the tile borders */
    DllExport void executeFindTileBordersMask(std::vector<std::string> inputImagePaths, std::string borderMaskImage, unsigned int tileBoundary, unsigned int tileOverlap, unsigned int tileBody, std::string colsName);

    /** Function to run command to merge clump image */
    DllExport void executeMergeClumpImages(std::vector<std::string> inputImagePaths, std::string outputImage, bool mergeRATs);
    
    /** Function to run command to merge clump image */
    DllExport void executeExtractBrightFeatures(std::string inputImage, std::string maskImage, std::string outputImage, std::string temp1Image, std::string temp2Image, std::string outputFormat, float initThres, float thresIncrement, float thresholdUpper, std::vector<rsgis::cmds::FeatureShapeDescription*> shapeFeatDescript);
    
    /** Function to remove small clumps */
    DllExport void executeRMSmallClumps(std::string clumpsImage, std::string outputImage, float threshold, std::string imgFormat);
    
    /** Function to produce a regular grid of clumps for a defined image area */
    DllExport void executeGenerateRegularGrid(std::string inputImage, std::string outputClumpImage, std::string imageFormat, unsigned int numXPxls, unsigned int numYPxls, bool offset);
    
    /** Function to include a clumped masked region into an existing segmebtation */
    DllExport void executeIncludeClumpedRegion(std::string inputClumps, std::string inputRegion, std::string outputClumpImage, std::string imageFormat);
    
    /** Function to merge selected clumps to neighbour with closest values from input image */
    DllExport void executeMergeSelectClumps2Neighbour(std::string inputImage, std::string clumpsImage, std::string outputImage, std::string imageFormat, std::string selectClumpsCol, std::string noDataClumpsCol);
    
    /** Function to drop selected clumps from the segmentation */
    DllExport void executeDropSelectedClumps(std::string clumpsImage, std::string outputImage, std::string imageFormat, std::string selectClumpsCol);
    
    /** Function merge clumps with same value */
    DllExport void executeMergeClumpsEquivalentVal(std::string clumpsImage, std::string outputImage, std::string imageFormat, std::vector<std::string> clumpsValCols);
    
    /** Function to grow regions until some termination criteria are met */
    DllExport void executePxlGrowRegions(std::string clumpsImage, std::string valsImage, std::string outputImage, std::string imageFormat, std::string muParseCriteria, std::vector<VarImgBandPairs> varNameBandPairs);
    
    
}}


#endif

