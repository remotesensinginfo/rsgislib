/*
 *  RSGISFindClumpCategoryMajority.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/03/2012.
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

#ifndef RSGISFindClumpCategoryMajority_H
#define RSGISFindClumpCategoryMajority_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "common/RSGISAttributeTableException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "rastergis/RSGISAttributeTable.h"

#include "math/RSGISMathsUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

namespace rsgis{namespace rastergis{
    
    class RSGISFindClumpCategoryMajority
    {
    public:
        RSGISFindClumpCategoryMajority();
        void findMajorityCategory(GDALDataset *categoriesDataset, GDALDataset *clumpsDataset, RSGISAttributeTable *attTable, std::string areaField, std::string majorityRatioField, std::string majorityCatField)throw(RSGISAttributeTableException);
        ~RSGISFindClumpCategoryMajority();
    };
        
    class RSGISFindClumpCategoryMajorityCalcImage : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISFindClumpCategoryMajorityCalcImage(int numberOutBands, std::vector<std::vector< std::pair<unsigned long, unsigned int> > > *attMajorityFields);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISFindClumpCategoryMajorityCalcImage();
    protected:
        std::vector<std::vector< std::pair<unsigned long, unsigned int> > > *attMajorityFields;
    };
    
    
}}

#endif
