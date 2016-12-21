/*
 *  RSGISEliminateSinglePixels.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/05/2012.
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

#ifndef RSGISEliminateSinglePixels_H
#define RSGISEliminateSinglePixels_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "img/RSGISImageUtils.h"
#include "img/RSGISImageCalcException.h"
#include "math/RSGISMathsUtils.h"

#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

namespace rsgis{namespace segment{
    
    class DllExport RSGISEliminateSinglePixels
    {
    public:
        RSGISEliminateSinglePixels();
        void eliminate(GDALDataset *inSpecData, GDALDataset *inClumpsData, GDALDataset *tmpData, std::string outputImage, float noDataVal, bool noDataValProvided, bool projFromImage, std::string proj, std::string format)throw(rsgis::img::RSGISImageCalcException);
        void eliminateBlocks(GDALDataset *inSpecData, GDALDataset *inClumpsData, GDALDataset *tmpData, std::string outputImage, float noDataVal, bool noDataValProvided, bool projFromImage, std::string proj, std::string format)throw(rsgis::img::RSGISImageCalcException);
        ~RSGISEliminateSinglePixels();
    private:
        unsigned long findSinglePixels(GDALDataset *inClumpsData, GDALDataset *tmpData, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException);
        bool eliminateSinglePixels(GDALDataset *inSpecData, GDALDataset *inClumpsData, GDALDataset *tmpData, GDALDataset *outDataset, float noDataVal, bool noDataValProvided) throw(rsgis::img::RSGISImageCalcException);
        inline float eucDistance(float **vals1, float **vals2, unsigned int numBands, unsigned int col1, unsigned int col2);
    };
    
    
    
    class DllExport RSGISFindSinglePixels : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISFindSinglePixels(float noDataVal, bool noDataValProvided);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void resetCount(){this->numSingles = 0;};
        unsigned long getCount(){return this->numSingles;};
        ~RSGISFindSinglePixels();
    protected:
        float noDataVal;
        bool noDataValProvided;
        unsigned long numSingles;
	};
    
    
    class DllExport RSGISElimSinglePixelsCalcImg : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISElimSinglePixelsCalcImg(float noDataVal, bool noDataValProvided);
		void calcImageValue(float *bandValues, int numBands, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, geos::geom::Envelope extent)throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, double *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, double *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void resetChangeOccured(){this->hasChangeOccured = false;};
        bool getChangeOccured(){return this->hasChangeOccured;};
        ~RSGISElimSinglePixelsCalcImg();
    protected:
        bool hasChangeOccured;
        float noDataVal;
        bool noDataValProvided;
        inline float eucDistance(float *vals1, float *vals2, unsigned int numBands);
	};
    
}}

#endif
