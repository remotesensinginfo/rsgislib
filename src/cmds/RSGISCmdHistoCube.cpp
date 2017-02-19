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
    
    void executeCreateEmptyHistoCube(std::string histCubeFile, unsigned long numFeats)throw(RSGISCmdException)
    {
        try
        {
            rsgis::histocube::RSGISHistoCubeFile histoCubeFileObj = rsgis::histocube::RSGISHistoCubeFile();
            histoCubeFileObj.createNewFile(histCubeFile, numFeats);
            histoCubeFileObj.closeFile();
            /*
            
            
            
            std::vector<int> bins = std::vector<int>();
            for(int i = 0; i <= 100; ++i)
            {
                bins.push_back(i);
            }
        
            float scale = 1;
            float offset = 0;
            bool hasTime = true;
            bool hasDate = true;
            boost::posix_time::ptime *layerDateTime = new boost::posix_time::ptime(boost::posix_time::time_from_string("2002-01-20 23:59:59.000"));
            histoCubeFileObj.createDataset("LS_RED", bins, scale, offset, hasTime, hasDate, layerDateTime);
            histoCubeFileObj.createDataset("LS_GREEN", bins, scale, offset, false, false, NULL);
            histoCubeFileObj.createDataset("LS_BLUE", bins, scale, offset, false, false, NULL);
            histoCubeFileObj.closeFile();
            */
            
            /*
            rsgis::histocube::RSGISHistoCubeFile histoCubeFileObj2 = rsgis::histocube::RSGISHistoCubeFile();
            histoCubeFileObj2.openFile(histCubeFile, true);
            
            unsigned int *data = new unsigned int[101];
            
            histoCubeFileObj2.getHistoRow("LS_RED", 4, data, 101);
            
            data[1] = 1;
            data[4] = 4;
            data[8] = 8;
            
            histoCubeFileObj2.setHistoRow("LS_RED", 4, data, 101);
            
            histoCubeFileObj2.closeFile();
            */
        }
        catch(rsgis::RSGISHistoCubeException &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
}}




