/*
 *  RSGISCreateImageTiles.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/03/2013.
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

#ifndef RSGISCreateImageTiles_H
#define RSGISCreateImageTiles_H

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "img/RSGISPopWithStats.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcEditImage.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"

#include "rastergis/RSGISRasterAttUtils.h"

#include "utils/RSGISTextUtils.h"

#include "common/RSGISImageException.h"

#include "geos/geom/Envelope.h"

namespace rsgis{namespace rastergis{

    class RSGISCreateImageTileMasks
    {
    public:
        RSGISCreateImageTileMasks();
        void createTileMasks(GDALDataset *image, std::string outputBase, std::string format, std::string ext, bool useOverlap, float overlap, bool growOverlap) throw(rsgis::RSGISImageException);
        ~RSGISCreateImageTileMasks();
    };
    
    
    class RSGISDefineTileMask : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISDefineTileMask();
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void setTileVal(size_t tileVal);
		~RSGISDefineTileMask();
    private:
        size_t tileVal;
	};
    
    
    class RSGISGrowOverlapRingRegion : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISGrowOverlapRingRegion(unsigned int maxNumGrowthPixels);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool changeOccurred();
        void resetChange();
        void incrementCounter();
        void reset();
		~RSGISGrowOverlapRingRegion();
    protected:
        float counter;
        bool change;
        unsigned int maxNumGrowthPixels;
	};
    
    
    
    class RSGISCutOutTile : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCutOutTile(float noDataVal, unsigned int numOfBands);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCutOutTile();
    private:
        float noDataVal;
	};
    
}}


#endif



