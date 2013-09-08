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
    void excecuteBasicRegistration(std::string inputReferenceImage, std::string inputFloatingmage, int gcpGap,
                                   float metricThreshold, int windowSize, int searchArea, float stdDevRefThreshold,
                                   float stdDevFloatThreshold, int subPixelResolution, unsigned int metricTypeInt,
                                   unsigned int outputType, std::string outputGCPFile);
    
    /** Single connected layer image registration */
    void excecuteSingleLayerConnectedRegistration(std::string inputReferenceImage, std::string inputFloatingmage, int gcpGap,
                                                  float metricThreshold, int windowSize, int searchArea, float stdDevRefThreshold,
                                                  float stdDevFloatThreshold, int subPixelResolution, float distanceThreshold,
                                                  int maxNumIterations, float moveChangeThreshold, int pSmoothness, unsigned int metricTypeInt,
                                                  unsigned int outputType, std::string outputGCPFile);
}}


#endif

