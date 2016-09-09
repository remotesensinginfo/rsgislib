/*
 *  RSGISCmdImageRegistrations.h
 *
 *
 *  Created by Dan Clewley on 08/09/2013.
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

#ifndef RSGISCmdImageRegistrations_H
#define RSGISCmdImageRegistrations_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {

    /** Basic image registration */
    DllExport void excecuteBasicRegistration(std::string inputReferenceImage, std::string inputFloatingmage, int gcpGap,
                                   float metricThreshold, int windowSize, int searchArea, float stdDevRefThreshold,
                                   float stdDevFloatThreshold, int subPixelResolution, unsigned int metricTypeInt,
                                   unsigned int outputType, std::string outputGCPFile)throw(RSGISCmdException);
    
    /** Single connected layer image registration */
    DllExport void excecuteSingleLayerConnectedRegistration(std::string inputReferenceImage, std::string inputFloatingmage, int gcpGap,
                                                  float metricThreshold, int windowSize, int searchArea, float stdDevRefThreshold,
                                                  float stdDevFloatThreshold, int subPixelResolution, int distanceThreshold,
                                                  int maxNumIterations, float moveChangeThreshold, float pSmoothness, unsigned int metricTypeInt,
                                                  unsigned int outputType, std::string outputGCPFile)throw(RSGISCmdException);

    /** Warp image using triangulation interpolation */
    DllExport void excecuteTriangularWarp(std::string inputImage, std::string outputImage, std::string projFile, std::string inputGCPs,
                        float resolution, std::string imageFormat = "KEA", bool genTransformImage = false)throw(RSGISCmdException);
    
    /** Warp image using NN interpolation */
    DllExport void excecuteNNWarp(std::string inputImage, std::string outputImage, std::string projFile, std::string inputGCPs,
                        float resolution, std::string imageFormat = "KEA", bool genTransformImage = false)throw(RSGISCmdException);
    
    /** Warp image using polynominal interpolation */
    DllExport void excecutePolyWarp(std::string inputImage, std::string outputImage, std::string projFile, std::string inputGCPs,
                        float resolution, int polyOrder = 3, std::string imageFormat = "KEA", bool genTransformImage = false)throw(RSGISCmdException);
    
    /** Add tie points to GCP */
    DllExport void excecuteAddGCPsGDAL(std::string inputImage, std::string inputGCPs, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType)throw(RSGISCmdException);
    
    /** Apply offset to image file */
    DllExport void executeApplyOffset2Image(std::string inputImage, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, double xOff, double yOff)throw(RSGISCmdException);
}}


#endif

