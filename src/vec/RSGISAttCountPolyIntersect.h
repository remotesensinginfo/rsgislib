/*
 *  RSGISAttCountPolyIntersect.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 08/03/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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


#ifndef RSGISAttCountPolyIntersect_H
#define RSGISAttCountPolyIntersect_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "vec/RSGISVectorIO.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISPixelInPoly.h"

#include "rastergis/RSGISAttributeTable.h"
#include "common/RSGISAttributeTableException.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/Polygon.h"

using namespace std;
using namespace rsgis;
using namespace rsgis::utils;
using namespace rsgis::rastergis;
using namespace rsgis::img;
using namespace geos::geom;
using namespace rsgis::math;

namespace rsgis{namespace vec{    
    
	class RSGISAttCountPolyIntersect
	{
	public:
		RSGISAttCountPolyIntersect();
        void countPolysIntersections(RSGISAttributeTable *attTable, GDALDataset *clumps, OGRLayer *polys, string field, pixelInPolyOption pixelPolyOption)throw(RSGISAttributeTableException);
        void countPolysIntersections(RSGISAttributeTable *attTable, GDALDataset *clumps, vector<OGRPolygon*> *polys, string field, pixelInPolyOption pixelPolyOption)throw(RSGISAttributeTableException);
		void countPolyIntersections(RSGISAttributeTable *attTable, GDALDataset *clumps, OGRPolygon *ogrPoly, unsigned int fieldIdx, pixelInPolyOption pixelPolyOption)throw(RSGISAttributeTableException);
        ~RSGISAttCountPolyIntersect();
	};
    
    
    class RSGISCalcValueCountInAttTable : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcValueCountInAttTable(RSGISAttributeTable *attTable, unsigned int fieldIdx);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcValueCountInAttTable();
    protected:
        RSGISAttributeTable *attTable;
        unsigned int fieldIdx;
    };
    
    class RSGISPopulateAttTableForPolygonIntersect
    {
    public:
        RSGISPopulateAttTableForPolygonIntersect(RSGISCalcValueCountInAttTable *valueCalc, GDALDataset **datasets, int numDS, pixelInPolyOption pixelPolyOption);
        void processFeature(OGRFeature *feature, OGRPolygon *poly, Envelope *env, long fid) throw(RSGISVectorException);
        ~RSGISPopulateAttTableForPolygonIntersect();
    protected:
        unsigned int numImageBands;
        RSGISCalcValueCountInAttTable *valueCalc;
        GDALDataset **datasets;
        int numDS;
        pixelInPolyOption pixelPolyOption;
    };
    

    
}}

#endif




