/*
 *  RSGISFindInfoBetweenLayers.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 30/09/2012.
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

#ifndef RSGISFindInfoBetweenLayers_H
#define RSGISFindInfoBetweenLayers_H

#include <iostream>
#include <string>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "gdal_rat.h"

#include "common/RSGISAttributeTableException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

namespace rsgis{namespace rastergis{
    
    class RSGISFindInfoBetweenLayers
    {
    public:
        RSGISFindInfoBetweenLayers();
        void findClassMajority(GDALDataset *baseSegmentsDS, GDALDataset *infoSegmentsDS, std::string baseClassCol, std::string infoClassCol) throw(RSGISAttributeTableException);
        ~RSGISFindInfoBetweenLayers();
    };
    
    
    class RSGISCalcClumpClassMajorities : public rsgis::img::RSGISCalcImageValue
	{
	public:
		RSGISCalcClumpClassMajorities(GDALRasterAttributeTable *baseATable, GDALRasterAttributeTable *infoATable, unsigned int infoClassIdx, std::vector<std::string> classes, unsigned int **clumpCounter);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
		~RSGISCalcClumpClassMajorities();
    protected:
        GDALRasterAttributeTable *baseATable;
        GDALRasterAttributeTable *infoATable;
        unsigned int infoClassIdx;
        std::vector<std::string> classes;
        unsigned int **clumpCounter;
        
	};
	
}}

#endif

