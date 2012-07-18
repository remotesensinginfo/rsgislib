/*
 *  RSGISImageStatistics.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/05/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISImageStatistics_H
#define RSGISImageStatistics_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageUtils.h"
#include "img/RSGISCalcImage.h"

#include "math/RSGISMathFunction.h"

namespace rsgis{namespace img{
	
	struct ImageStats
	{
		double mean;
		double max;
		double min;
		double stddev;
        double sum;
	};
	
	class RSGISImageStatistics
    {
    public: 
        RSGISImageStatistics();
        void calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats **stats, int numInputBands, bool stddev, bool ignoreZeros=false)throw(RSGISImageCalcException,RSGISImageBandException);
        void calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats **stats, int numInputBands, bool stddev, rsgis::math::RSGISMathFunction *func, bool ignoreZeros=false)throw(RSGISImageCalcException,RSGISImageBandException);
        void calcImageStatistics(GDALDataset **datasets, int numDS, ImageStats *stats, bool stddev, bool ignoreZeros=false)throw(RSGISImageCalcException,RSGISImageBandException);
    };
	
	
	class RSGISCalcImageStatistics : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcImageStatistics(int numberOutBands, int numInputBands, bool calcSD, rsgis::math::RSGISMathFunction *func, bool ignoreZeros);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void getImageStats(ImageStats** inStats, int numInputBands) throw(RSGISImageCalcException);
        void calcStdDev();
        ~RSGISCalcImageStatistics();
    protected:
        bool ignoreZeros;
        bool calcSD;
        int numInputBands;
        bool *firstMean;
        bool *firstSD;
        bool calcMean;
        unsigned long *n;
        double *mean;
        double *meanSum;
        double *min;
        double *max;
        double *sumDiffZ;
        double diffZ;
        rsgis::math::RSGISMathFunction *func;
        
    };
    
    class RSGISCalcImageStatisticsAllBands : public RSGISCalcImageValue
    {
    public: 
        RSGISCalcImageStatisticsAllBands(int numberOutBands, bool calcSD, rsgis::math::RSGISMathFunction *func, bool ignoreZeros);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void getImageStats(ImageStats *inStats) throw(RSGISImageCalcException);
        void calcStdDev();
        ~RSGISCalcImageStatisticsAllBands();
    protected:
        bool ignoreZeros;
        bool calcSD;
        bool firstMean;
        bool firstSD;
        bool calcMean;
        unsigned long n;
        double mean;
        double meanSum;
        double min;
        double max;
        double sumDiffZ;
        double diffZ;
        rsgis::math::RSGISMathFunction *func;
        
    };
	
}}
#endif


