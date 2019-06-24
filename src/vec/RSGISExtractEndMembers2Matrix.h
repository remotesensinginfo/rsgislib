/*
 *  RSGISExtractEndMembers2Matrix.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/02/2012.
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


#ifndef RSGISExtractEndMembers2Matrix_H
#define RSGISExtractEndMembers2Matrix_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "vec/RSGISVectorZonalException.h"
#include "vec/RSGISVectorIO.h"

#include "vec/RSGISProcessOGRFeature.h"

#include "math/RSGISMatrices.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISPixelInPoly.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Coordinate.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
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
    
	class DllExport RSGISExtractEndMembers2Matrix
	{
	public:
		RSGISExtractEndMembers2Matrix();
		void extractColumnPerPolygon2Matrix(GDALDataset *dataset, OGRLayer *vecLayer, std::string outputMatrix, rsgis::img::pixelInPolyOption pixelPolyOption);
		~RSGISExtractEndMembers2Matrix();
	};
    
    
    class DllExport RSGISCalcSumValues : public rsgis::img::RSGISCalcImageValue
    {
    public: 
        RSGISCalcSumValues(float *sumVals, unsigned int *countVals, unsigned int numSumVals);
        void calcImageValue(float *bandValues, int numBands, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent);
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) {throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) {throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISCalcSumValues();
    protected:
        float *sumVals;
        unsigned int *countVals;
        unsigned int numSumVals;
    };
    
    
    class DllExport RSGISExtractSumPixelValues
    {
    public:
        RSGISExtractSumPixelValues(unsigned int numImageBands, RSGISCalcSumValues *valueCalc, GDALDataset **datasets, int numDS, rsgis::img::pixelInPolyOption pixelPolyOption);
        void processFeature(OGRFeature *feature, OGRPolygon *poly, geos::geom::Envelope *env, long fid);
        ~RSGISExtractSumPixelValues();
    protected:
        unsigned int numImageBands;
        RSGISCalcSumValues *valueCalc;
        GDALDataset **datasets;
        int numDS;
        rsgis::img::pixelInPolyOption pixelPolyOption;
    };
    
    

}}
#endif
