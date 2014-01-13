/*
 *  RSGISMaxLikelihoodRATClassification.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/11/2012.
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

#ifndef RSGISMaxLikelihoodRATClassification_H
#define RSGISMaxLikelihoodRATClassification_H

#include <iostream>
#include <string>
#include <math.h>
#include <list>
#include <map>
#include <set>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "math/RSGISMathException.h"
#include "math/RSGISMathsUtils.h"
#include "math/RSGISMaximumLikelihood.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

namespace rsgis{namespace rastergis{
    
    enum rsgismlpriors
    {
        rsgis_samples, // as calculated by ML
        rsgis_area,    // priors set by the relative area
        rsgis_equal,   // priors all equal
        rsgis_userdefined, // priors passed in to function
        rsgis_weighted     // priors by area but with a weight applied
    };
    
    class RSGISMaxLikelihoodRATClassification
    {
    public:
        RSGISMaxLikelihoodRATClassification();
        void applyMLClassifier(GDALDataset *image, std::string inClassCol, std::string outClassCol, std::string trainingSelectCol, std::string classifySelectCol, 
                std::string areaCol, std::vector<std::string> inColumns, rsgismlpriors priorsMethod, std::vector<float> defPriors) throw(rsgis::RSGISAttributeTableException);
        void applyMLClassifierLocalPriors(GDALDataset *image, std::string inClassCol, std::string outClassCol, std::string trainingSelectCol, 
                std::string classifySelectCol, std::string areaCol, std::vector<std::string> inColumns, std::string eastingsCol, std::string northingsCol, float searchRadius, rsgismlpriors priorsMethod, float weightA, bool allowZeroPriors) throw(rsgis::RSGISAttributeTableException);
        ~RSGISMaxLikelihoodRATClassification();
    protected:
        inline double getEuclideanDistance(std::vector<double> *vals1, std::vector<double> *vals2)throw(rsgis::math::RSGISMathException);
        inline void getLocalPriors(rsgis::math::MaximumLikelihood *mlStruct, GDALRasterAttributeTable *attTable, size_t fid, int trainingSelectColIdx, int eastingsIdx, int northingsIdx, int classColIdx, std::map<int, int> &forwardMapping, int areaColIdx, float spatialRadius, bool allowZeroPriors, rsgismlpriors priorsMethod, float weightA)throw(rsgis::RSGISAttributeTableException);
    };
	
}}

#endif

