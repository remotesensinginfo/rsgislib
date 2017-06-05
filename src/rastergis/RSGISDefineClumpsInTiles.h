/*
 *  RSGISDefineClumpsInTiles.h
 *  RSGIS_LIB
 *
 *  Created by John Armston on 19/10/2014.
 *  Copyright 2014 RSGISLib.
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

#ifndef RSGISDefineClumpsInTiles_H
#define RSGISDefineClumpsInTiles_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <math.h>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "img/RSGISPopWithStats.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcEditImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImage.h"

#include "common/RSGISAttributeTableException.h"
#include "common/RSGISImageException.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_rastergis_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis{namespace rastergis{
    
    struct RSGISSegTilePos
    {
        bool boundary;
        bool overlap;
        bool body;
    };    
    
    
    class DllExport RSGISDefineClumpsInTiles
    {
    public:
        RSGISDefineClumpsInTiles();
        void defineSegmentTilePos(GDALDataset *clumpsDataset, GDALDataset *tileDataset, std::string outColName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody) throw(RSGISImageException, RSGISAttributeTableException);
        void defineBorderSegmentsUsingMask(GDALDataset *clumpsDataset, GDALDataset *maskDataset, std::string outColName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody) throw(RSGISImageException, RSGISAttributeTableException);
        void defineBorderSegments(GDALDataset *clumpsDataset, std::string outColName) throw(RSGISImageException, RSGISAttributeTableException);
        ~RSGISDefineClumpsInTiles();
    };
    
    
    class DllExport RSGISFindClumpPositionsInTile : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISFindClumpPositionsInTile(size_t numRows, RSGISSegTilePos *clumpPos, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		~RSGISFindClumpPositionsInTile();
    private:
        size_t numRows;
        RSGISSegTilePos *clumpPos;
        unsigned int tileOverlap;
        unsigned int tileBoundary;
        unsigned int tileBody;
	};
    
    
    class DllExport RSGISFindImageBorder : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISFindImageBorder(size_t numRows, int *borderMask);
        void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        ~RSGISFindImageBorder();
    private:
        size_t numRows;
        int *borderMask;
    };
        

}}

#endif
