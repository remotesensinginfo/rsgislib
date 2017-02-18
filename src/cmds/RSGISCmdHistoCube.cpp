/*
 *  RSGISCmdHistoCube.cpp
 *
 *
 *  Created by Pete Bunting on 17/02/2017.
 *  Copyright 2017 RSGISLib.
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

#include "RSGISCmdHistoCube.h"
#include "RSGISCmdParent.h"

#include "common/RSGISHistoCubeException.h"
#include "histocube/RSGISHistoCubeFileIO.h"

namespace rsgis{ namespace cmds {
    
    void executeCreateEmptyHistoCube(std::string histCubeFile)throw(RSGISCmdException)
    {
        try
        {
            rsgis::histocube::RSGISHistoCubeFile histoCubeFileObj = rsgis::histocube::RSGISHistoCubeFile();
            histoCubeFileObj.createNewFile(histCubeFile);
            histoCubeFileObj.closeFile();
        }
        catch(rsgis::RSGISHistoCubeException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
}}




