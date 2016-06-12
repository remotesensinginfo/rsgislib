/*
 *  RSGISCmdClassification.h
 *
 *
 *  Created by Pete Bunting on 18/11/2013.
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

#ifndef RSGISCmdClassification_H
#define RSGISCmdClassification_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {

    /** A function to collapse a segmentation RAT to a classification (i.e., 1 row per class) */
    DllExport void executeCollapseRAT2Class(std::string clumpsImage, std::string outputImage, std::string outImageFormat, std::string classColumn, std::string classIntCol="", bool useIntCol=false)throw(RSGISCmdException);
    
    /** A function to generate a 3 band colour image from the colour table */
    DllExport void executeGenerate3BandFromColourTable(std::string clumpsImage, std::string outputImage, std::string outImageFormat)throw(RSGISCmdException);
    
    /** A function to generate random points which can be used to assess the accuracy of a map */
    DllExport void executeGenerateRandomAccuracyPts(std::string classImage, std::string outputShp, std::string classImgCol, std::string classImgVecCol, std::string classRefVecCol, unsigned int numPts, unsigned int seed, bool force)throw(RSGISCmdException);
    
    /** A function to generate stratified random points which can be used to assess the accuracy of a map */
    DllExport void executeGenerateStratifiedRandomAccuracyPts(std::string classImage, std::string outputShp, std::string classImgCol, std::string classImgVecCol, std::string classRefVecCol, unsigned int numPtsPerClass, unsigned int seed, bool force)throw(RSGISCmdException);
    
    /** A function to populate a set of points with the class information to assess the accuracy of a map */
    DllExport void executePopClassInfoAccuracyPts(std::string classImage, std::string shpFile, std::string classImgCol, std::string classImgVecCol, std::string classRefVecCol="", bool addRefCol=false)throw(RSGISCmdException);

    
}}
#endif


