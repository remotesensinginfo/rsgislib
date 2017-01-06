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

#include "common/RSGISAttributeTableException.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/Polygon.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_vec_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace vec{    
    /*
	class DllExport RSGISAttCountPolyIntersect
	{
	public:
		RSGISAttCountPolyIntersect();
        void countPolysIntersections(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, OGRLayer *polys, std::string field, rsgis::img::pixelInPolyOption pixelPolyOption)throw(RSGISAttributeTableException);
        void countPolysIntersections(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, std::vector<OGRPolygon*> *polys, std::string field, rsgis::img::pixelInPolyOption pixelPolyOption)throw(RSGISAttributeTableException);
		void countPolyIntersections(rsgis::rastergis::RSGISAttributeTable *attTable, GDALDataset *clumps, OGRPolygon *ogrPoly, unsigned int fieldIdx, rsgis::img::pixelInPolyOption pixelPolyOption)throw(RSGISAttributeTableException);
        ~RSGISAttCountPolyIntersect();
	};
    
    
    class DllExport RSGISCalcValueCountInAttTable : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISCalcValueCountInAttTable(rsgis::rastergis::RSGISAttributeTable *attTable, unsigned int fieldIdx);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISCalcValueCountInAttTable();
    protected:
        rsgis::rastergis::RSGISAttributeTable *attTable;
        unsigned int fieldIdx;
    };
    
    class DllExport RSGISPopulateAttTableForPolygonIntersect
    {
    public:
        RSGISPopulateAttTableForPolygonIntersect(RSGISCalcValueCountInAttTable *valueCalc, GDALDataset **datasets, int numDS, rsgis::img::pixelInPolyOption pixelPolyOption);
        void processFeature(OGRFeature *feature, OGRPolygon *poly, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
        ~RSGISPopulateAttTableForPolygonIntersect();
    protected:
        RSGISCalcValueCountInAttTable *valueCalc;
        GDALDataset **datasets;
        int numDS;
        rsgis::img::pixelInPolyOption pixelPolyOption;
    };
    
*/
    
}}

#endif




