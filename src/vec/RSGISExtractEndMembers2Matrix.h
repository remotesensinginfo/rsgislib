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

using namespace std;
using namespace rsgis::img;
using namespace geos::geom;
using namespace rsgis::math;

namespace rsgis{namespace vec{    
    
	class RSGISExtractEndMembers2Matrix
	{
	public:
		RSGISExtractEndMembers2Matrix();
		void extractColumnPerPolygon2Matrix(GDALDataset *dataset, OGRLayer *vecLayer, string outputMatrix, pixelInPolyOption pixelPolyOption)throw(RSGISVectorZonalException);
		~RSGISExtractEndMembers2Matrix();
	};
    
    
    class RSGISCalcSumValues : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcSumValues(float *sumVals, unsigned int *countVals, unsigned int numSumVals);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        ~RSGISCalcSumValues();
    protected:
        float *sumVals;
        unsigned int *countVals;
        unsigned int numSumVals;
    };
    
    
    class RSGISExtractSumPixelValues
    {
    public:
        RSGISExtractSumPixelValues(unsigned int numImageBands, RSGISCalcSumValues *valueCalc, GDALDataset **datasets, int numDS, pixelInPolyOption pixelPolyOption);
        void processFeature(OGRFeature *feature, OGRPolygon *poly, Envelope *env, long fid) throw(RSGISVectorException);
        ~RSGISExtractSumPixelValues();
    protected:
        unsigned int numImageBands;
        RSGISCalcSumValues *valueCalc;
        GDALDataset **datasets;
        int numDS;
        pixelInPolyOption pixelPolyOption;
    };
    
    

}}
#endif
