/*
 *  RSGISCumulativeAreaClassifier.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#ifndef RSGISCumulativeAreaClassifier_H
#define RSGISCumulativeAreaClassifier_H

#include <math.h>
#include <limits>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"

namespace rsgis { namespace classifier {
	
    using namespace rsgis::img;
    using namespace rsgis::math;
    
	/**
	 * A pair of classes which generate the a rule image (distance to sample) using
	 * the euclidean distance to measure the distance between two cumulative area
	 * plots.
	 */
	
	
	class RSGISCumulativeAreaClassifierGenRules : public RSGISCalcImageValue
	{
	public:
		RSGISCumulativeAreaClassifierGenRules(int numOutBands, Matrix *bandValuesWidths, Matrix *samples);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};															
		float* calculateCumulativeArea(float *dataValues, int numVals);
		float calcEuclideanDistance(Matrix *sample, int sampleNum, float *data);
		~RSGISCumulativeAreaClassifierGenRules();
	private:
		Matrix *bandValuesWidths;
		Matrix *samples;
	};
	
	/// Classify rule image produced by the cumulative area classifier
	class RSGISCumulativeAreaClassifierDecide : public RSGISCalcImageValue
	{
	public:
		RSGISCumulativeAreaClassifierDecide(int numOutBands, double threshold);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};															
		~RSGISCumulativeAreaClassifierDecide();
	private:
		double threshold;
	};
	
}}

#endif


