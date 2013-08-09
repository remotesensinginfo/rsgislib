/*
 *  RSGISCmdZonalStats.h
 *
 *
 *  Created by Dan Clewley on 08/08/2013.
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

#ifndef RSGISCmdZonalStats_H
#define RSGISCmdZonalStats_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {
    
    /** Function to extract pixel value for points and save as a Shape file */
    void executePointValue2SHP(std::string inputImage, std::string inputVecPolys, std::string outputVecPolys, bool force = false, bool useBandNames = true);
    /** Function to extract pixel value for points and save as a text file */
    void executePointValue2TXT(std::string inputImage, std::string inputVecPolys, std::string outputTextFile, bool useBandNames);
}}


#endif

