/*
 *  RSGISCmdFilterImages.h
 *
 *
 *  Created by Dan Clewley on 09/11/2013.
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

#ifndef RSGISCmdFilterImages_H
#define RSGISCmdFilterImages_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {

    struct RSGISFilterParameters
    {
        std::string type = "";
        std::string fileEnding = "";
        std::string option = "";
        unsigned int size = 3;
        unsigned int nLooks = 3;
        float stddev = 0;
        float stddevX = 0;
        float stddevY = 0;
        float angle = 0;
    };

    /** Function to apply filters to an image */
    void executeFilter(std::string inputImage, std::vector <rsgis::cmds::RSGISFilterParameters*> *filterParameters, std::string outputImageBase, std::string imageFormat, std::string imageExt, RSGISLibDataType outDataType) throw(RSGISCmdException);

}}


#endif

