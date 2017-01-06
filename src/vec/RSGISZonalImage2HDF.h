/*
 *  RSGISZonalImage2HDF.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/09/2013.
 *  Copyright 2013 RSGISLib. All rights reserved.
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


#ifndef RSGISZonalImage2HDF_H
#define RSGISZonalImage2HDF_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "vec/RSGISVectorZonalException.h"
#include "vec/RSGISVectorIO.h"

#include "vec/RSGISProcessOGRFeature.h"

#include "math/RSGISMatrices.h"

#include "utils/RSGISExportData2HDF.h"

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISPixelInPoly.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Coordinate.h"

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
    
	class DllExport RSGISZonalImage2HDF
	{
	public:
		RSGISZonalImage2HDF();
		void extractBandsToColumns(GDALDataset *dataset, OGRLayer *vecLayer, std::string outputFile, rsgis::img::pixelInPolyOption pixelPolyOption)throw(RSGISVectorZonalException);
		~RSGISZonalImage2HDF();
	};
    
    
    class DllExport RSGISExtractPixelValues : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISExtractPixelValues(std::vector<float*> *pxlVals);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISExtractPixelValues();
    protected:
        std::vector<float*> *pxlVals;
    };
    
    
    class DllExport RSGISExtractZonalPixelValues
    {
    public:
        RSGISExtractZonalPixelValues(RSGISExtractPixelValues *valueCalc, GDALDataset **datasets, int numDS, rsgis::img::pixelInPolyOption pixelPolyOption);
        void processFeature(OGRFeature *feature, OGRPolygon *poly, geos::geom::Envelope *env, long fid) throw(RSGISVectorException);
        ~RSGISExtractZonalPixelValues();
    protected:
        std::vector<float*> *pxlVals;
        RSGISExtractPixelValues *valueCalc;
        GDALDataset **datasets;
        int numDS;
        rsgis::img::pixelInPolyOption pixelPolyOption;
    };
    
    
    
}}
#endif
