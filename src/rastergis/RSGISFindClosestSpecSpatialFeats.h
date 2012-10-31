/*
 *  RSGISFindClosestSpecSpatialFeats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 30/10/2012.
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

#ifndef RSGISFindClosestSpecSpatialFeats_H
#define RSGISFindClosestSpecSpatialFeats_H

#include <iostream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathException.h"

#include "rastergis/RSGISFindTopNWithinDist.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

namespace rsgis{namespace rastergis{
    
    class RSGISFindClosestSpecSpatialFeats
    {
    public:
        RSGISFindClosestSpecSpatialFeats();
        void calcFeatsWithinSpatSpecThresholds(GDALDataset *dataset, std::string spatDistCol, std::string distCol, std::string outColumn, float specDistThreshold, float spatDistThreshold)throw(rsgis::RSGISAttributeTableException);
        void applyMajorityClassifier(GDALDataset *image, std::string inClassCol, std::string outClassCol, std::string trainingSelectCol, std::string eastingsCol, std::string northingsCol, std::string areaCol, std::string majWeightCol, std::vector<std::string> inColumns, float spatDistThreshold, float specDistThreshold) throw(rsgis::RSGISAttributeTableException);
        ~RSGISFindClosestSpecSpatialFeats();
    protected:
        inline double getEuclideanDistance(std::vector<double> *vals1, std::vector<double> *vals2)throw(rsgis::math::RSGISMathException);
        inline std::pair<int, double> findMajorityClassStandard(GDALRasterAttributeTable *attTable, size_t fid, int classIdx, int trainingSelectColIdx, int eastingsIdx, int northingsIdx, int *infoColIdxs, size_t numCols, float spatDistThreshold, float specDistThreshold) throw(rsgis::RSGISAttributeTableException);
    };
	
}}

#endif

