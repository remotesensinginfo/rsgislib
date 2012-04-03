/*
 *  RSGISCreateImageFromAttributeTable.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/02/2012.
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

#ifndef RSGISCreateImageFromAttributeTable_H
#define RSGISCreateImageFromAttributeTable_H

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

using namespace std;
using namespace rsgis;
using namespace rsgis::utils;
using namespace rsgis::math;
using namespace rsgis::img;
using boost::lexical_cast;
using boost::bad_lexical_cast;

namespace rsgis{namespace rastergis{
    
    class RSGISCreateImageFromAttributeTable
    {
    public:
        RSGISCreateImageFromAttributeTable();
        void createImageFromAttTable(GDALDataset *clumpsDataset, string outputImage, RSGISAttributeTable *attTable, vector<pair<unsigned int, string> > *bands, string outImageFormat)throw(RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISCreateImageFromAttributeTable();
    };
    
    
    
    class RSGISCreateImageFromAttributeTableCalcImg : public RSGISCalcImageValue
    {
    public: 
        RSGISCreateImageFromAttributeTableCalcImg(int numberOutBands, RSGISAttributeTable *attTable, pair<RSGISAttributeDataType, unsigned int> *attBands);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCreateImageFromAttributeTableCalcImg();
    protected:
        RSGISAttributeTable *attTable;
        pair<RSGISAttributeDataType, unsigned int> *attBands;
    };
    
    class RSGISAttributeTableImageUtils
    {
    public:
        RSGISAttributeTableImageUtils(RSGISAttributeTable *attTable);
        void populateGDALDataset(GDALDataset *clumps, GDALDataset *output, RSGISAttributeDataType dataType, unsigned int attributeIdx)throw(RSGISImageException, RSGISAttributeTableException);
        ~RSGISAttributeTableImageUtils();
    protected:
        RSGISAttributeTable *attTable;
    };
    
    
}}

#endif




