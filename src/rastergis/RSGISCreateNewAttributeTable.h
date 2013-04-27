/*
 *  RSGISCreateNewAttributeTable.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/03/2012.
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

#ifndef RSGISCreateNewAttributeTable_H
#define RSGISCreateNewAttributeTable_H

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
    
    class RSGISCreateNewAttributeTable
    {
    public:
        RSGISCreateNewAttributeTable();
        RSGISAttributeTable* createAndPopPixelCount(GDALDataset *clumpsDataset, bool useMemory=true, std::string outFilePath="", unsigned long cacheSize=10000)throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        RSGISAttributeTable* createAndPopPixelCountOffLine(GDALDataset *clumpsDataset, bool useMemory=true, std::string outFilePath="", unsigned long cacheSize=10000)throw(rsgis::img::RSGISImageCalcException, RSGISAttributeTableException);
        size_t calcMaxValue(GDALDataset *dataset)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISCreateNewAttributeTable();
    };
    
    
    
    class RSGISPopAttributeTablePxlCountCalcImg : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISPopAttributeTablePxlCountCalcImg(int numberOutBands, RSGISAttributeTable *attTable, size_t pxlCountIdx);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISPopAttributeTablePxlCountCalcImg();
    protected:
        RSGISAttributeTable *attTable;
        size_t pxlCountIdx;
    };
    
    class RSGISPopAttributeTablePxlCountArrCalcImg : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISPopAttributeTablePxlCountArrCalcImg(int numberOutBands, size_t *pxlCounts, size_t len);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISPopAttributeTablePxlCountArrCalcImg();
    protected:
        size_t *pxlCounts;
        unsigned int len;
    };
    
    
}}

#endif
