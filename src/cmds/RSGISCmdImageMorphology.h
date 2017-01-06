/*
 *  RSGISCmdImageMorphology.h
 *
 *
 *  Created by Pete Bunting on 10/09/2016.
 *  Copyright 2016 RSGISLib.
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

#ifndef RSGISCmdImageMorphology_H
#define RSGISCmdImageMorphology_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

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
    
    /** A function to create a circular morphological operator */
    DllExport void executeCreateCircularOperator(std::string morphOperatorFile, unsigned int morphOpSize)throw(RSGISCmdException);
    /** A function to perform a morphological dilation on an image */
    DllExport void executeImageDilate(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to perform a morphological erosion on an image */
    DllExport void executeImageErode(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to calculate a morphological gradiance for an image */
    DllExport void executeImageGradiant(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to perform a morphological dilation on an image combining the results of the output bands into a single image band */
    DllExport void executeImageDilateCombinedOut(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to perform a morphological erosion on an image combining the results of the output bands into a single image band */
    DllExport void executeImageErodeCombinedOut(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to calculate a morphological gradiance for an image combining the results of the output bands into a single image band */
    DllExport void executeImageGradiantCombinedOut(std::string inImage, std::string outImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to perform a morphological operation to find local minima */
    DllExport void executeImageLocalMinima(std::string inImage, std::string outImage, bool outputSequencial, bool allowEquals, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to perform a morphological operation to find local minima combining the results of the output bands into a single image band */
    DllExport void executeImageLocalMinimaCombinedOut(std::string inImage, std::string outImage, bool outputSequencial, bool allowEquals, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to perform a morphological opening on an image */
    DllExport void executeImageOpening(std::string inImage, std::string outImage, std::string tmpImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, unsigned int numIterations, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to perform a morphological closing on an image */
    DllExport void executeImageClosing(std::string inImage, std::string outImage, std::string tmpImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, unsigned int numIterations, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to perform a morphological black top hat on an image */
    DllExport void executeImageBlackTopHat(std::string inImage, std::string outImage, std::string tmpImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    /** A function to perform a morphological white top hat on an image */
    DllExport void executeImageWhiteTopHat(std::string inImage, std::string outImage, std::string tmpImage, std::string morphOperatorFile, bool useOperatorFile, unsigned int morphOpSize, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    
    
    
    
}}


#endif


