/*
 *  RSGISCmdSegmentation.cpp
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

#include "RSGISCmdSegmentation.h"
#include "RSGISCmdParent.h"

#include "common/RSGISImageException.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISCalcClumpStats.h"

namespace rsgis{ namespace cmds {
    
    void executePopulateStats(std::string clumpsImage, bool addColourTable2Img, bool calcImgPyramids)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            
            GDALDataset *clumpsDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_Update);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            rsgis::rastergis::RSGISPopulateWithImageStats popImageStats;
            popImageStats.populateImageWithRasterGISStats(clumpsDataset, addColourTable2Img, calcImgPyramids);            
            
            GDALClose(clumpsDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
}}

