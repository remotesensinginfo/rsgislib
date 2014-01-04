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
#include "rastergis/RSGISAttributeTableMem.h"
#include "rastergis/RSGISAttributeTableHDF.h"

#include "math/RSGISMathsUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

namespace rsgis{namespace rastergis{
    
    class RSGISCreateImageFromAttributeTable
    {
    public:
        RSGISCreateImageFromAttributeTable();
        void createImageFromAttTable(GDALDataset *clumpsDataset, std::string outputImage, RSGISAttributeTable *attTable, std::vector<std::pair<unsigned int, std::string> > *bands, std::string outImageFormat)throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        ~RSGISCreateImageFromAttributeTable();
    };    
    
    class RSGISCreateImageFromAttributeTableCalcImg : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISCreateImageFromAttributeTableCalcImg(int numberOutBands, RSGISAttributeTable *attTable, std::pair<RSGISAttributeDataType, unsigned int> *attBands);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISCreateImageFromAttributeTableCalcImg();
    protected:
        RSGISAttributeTable *attTable;
        std::pair<RSGISAttributeDataType, unsigned int> *attBands;
    };
    
    class RSGISAttributeTableImageUtils
    {
    public:
        RSGISAttributeTableImageUtils(RSGISAttributeTable *attTable);
        void populateGDALDataset(GDALDataset *clumps, GDALDataset *output, RSGISAttributeDataType dataType, unsigned int attributeIdx)throw(rsgis::RSGISImageException, RSGISAttributeTableException);
        ~RSGISAttributeTableImageUtils();
    protected:
        RSGISAttributeTable *attTable;
    };
    
    
}}

#endif




