/*
 *  RSGISCalcClumpShapeParameters.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/03/2013.
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

#ifndef RSGISCalcClumpShapeParameters_H
#define RSGISCalcClumpShapeParameters_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>
#include <math.h>

#include "rastergis/RSGISAttributeTable.h"
#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISClumpBorders.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "math/RSGISMathsUtils.h"
#include "math/RSGISPrincipalComponentAnalysis.h"
#include "math/RSGISMatrices.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

namespace rsgis{namespace rastergis{
    
    enum rsgisshapeindex
    {
        rsgis_shapena = 0,
        rsgis_shapearea = 1,
        rsgis_asymmetry = 2,
        rsgis_borderindex = 3,
        rsgis_borderlength = 4,
        rsgis_compactness = 5,
        rsgis_density = 6,
        rsgis_ellipticfit = 7,
        rsgis_length = 8,
        rsgis_lengthwidth = 9,
        rsgis_width = 10,
        rsgis_maindirection = 11,
        rsgis_radiuslargestenclosedellipse = 12,
        rsgis_radiussmallestenclosedellipse = 13,
        rsgis_rectangularfit = 14,
        rsgis_roundness = 15,
        rsgis_shapeindex = 16
    };
    
    struct RSGISShapeParam
    {
        rsgisshapeindex idx;
        std::string colName;
        unsigned int colIdx;
    };
    
    struct ShapePoint
    {
        double x;
        double y;
    };
    
    
    class RSGISCalcClumpShapeParameters
    {
    public:
        RSGISCalcClumpShapeParameters();
        void calcClumpShapeParams(GDALDataset *clumpImage, std::vector<rsgis::rastergis::RSGISShapeParam*> *shapeIndexes) throw(rsgis::img::RSGISImageCalcException);
        static rsgisshapeindex getRSGISShapeIndex(std::string indexName) throw(rsgis::RSGISException);
        static std::string getRSGISShapeIndexAsString(rsgisshapeindex index) throw(rsgis::RSGISException);
        ~RSGISCalcClumpShapeParameters();
        
    };
    
    
    class RSGISCalcShapePrimativesAreaPxlLocs : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCalcShapePrimativesAreaPxlLocs(size_t *pxlCount, std::vector<ShapePoint*> **pts, size_t numRows, bool needNumPxls, bool needPxlLocs);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCalcShapePrimativesAreaPxlLocs();
    protected:
        size_t *pxlCount;
        std::vector<ShapePoint*> **pts;
        size_t numRows;
        bool needNumPxls;
        bool needPxlLocs;
	};
    
    
}}

#endif
