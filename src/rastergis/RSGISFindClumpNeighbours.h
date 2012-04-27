/*
 *  RSGISFindClumpNeighbours.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 27/03/2012.
 *  Copyright 2012 RSGISLib.
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

#ifndef RSGISFindClumpNeighbours_H
#define RSGISFindClumpNeighbours_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>

#include "img/RSGISImageCalcException.h"
#include "rastergis/RSGISAttributeTable.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

using namespace std;
using namespace rsgis;
using namespace rsgis::img;

namespace rsgis{namespace rastergis{
    
    class RSGISFindClumpNeighbours
    {
    public:
        RSGISFindClumpNeighbours();
        vector<list<size_t>* >* findNeighbours(GDALDataset *clumpImage) throw(RSGISImageCalcException);
        void findNeighbours(GDALDataset *clumpImage, RSGISAttributeTable *attTable) throw(RSGISImageCalcException);
        void findNeighboursInBlocks(GDALDataset *clumpImage, RSGISAttributeTable *attTable) throw(RSGISImageCalcException);
        ~RSGISFindClumpNeighbours();
    protected:
        inline void addNeighbourToFeature(RSGISFeature *feat, unsigned long long neighbourID)
        {
            bool foundID = false;
            for(vector<size_t>::iterator iterNeigh = feat->neighbours->begin(); iterNeigh != feat->neighbours->end(); ++iterNeigh)
            {
                if((*iterNeigh) == neighbourID)
                {
                    foundID = true;
                }
            }
            if(!foundID)
            {
                feat->neighbours->push_back(neighbourID);
            }
        };
    };
    
}}

#endif
