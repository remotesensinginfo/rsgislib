/*
 *  RSGISFindTopNWithinDist.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 28/07/2012.
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

#ifndef RSGISFindTopNWithinDist_H
#define RSGISFindTopNWithinDist_H

#include <iostream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathException.h"
#include "img/RSGISImageUtils.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

namespace rsgis{namespace rastergis{
	
    
    struct DistItem
    {
        DistItem(size_t fid, double dist)
        {
            this->fid = fid;
            this->dist = dist;
        }
        size_t fid;
        double dist;
    };
    
    class RSGISFindTopNWithinDist
    {
    public:
        RSGISFindTopNWithinDist();
        void calcMinDistTopN(GDALDataset *dataset, std::string spatDistCol, std::string distCol, std::string outColumn, unsigned int n, float distThreshold)throw(rsgis::RSGISAttributeTableException);
        ~RSGISFindTopNWithinDist();
    };
	
}}

#endif

