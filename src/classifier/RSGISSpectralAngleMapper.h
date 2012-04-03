 /*
 *  RSGISSpectralAngleMapper.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 08/01/2010.
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

#ifndef RSGISSpectralAngleMapper_H
#define RSGISSpectralAngleMapper_H

#include <math.h>
#include <gsl/gsl_matrix.h>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

using namespace rsgis::img;

namespace rsgis { namespace classifier {

	class RSGISSpectralAngleMapperRule : public RSGISCalcImageValue
	{
	public:
		RSGISSpectralAngleMapperRule(int numOutBands, gsl_matrix *refSpectra);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};												
		~RSGISSpectralAngleMapperRule();
	private:
		int numOutBands;
		double *imageSpecArray;
		gsl_matrix *refSpectra;
	};
	
	class RSGISSpectralAngleMapperED : public RSGISCalcImageValue
	{
	public:
		RSGISSpectralAngleMapperED(int numOutBands, gsl_matrix *refSpectra);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};															
		~RSGISSpectralAngleMapperED();
	private:
		int numOutBands;
		gsl_matrix *refSpectra;
	};
	
	/// Classify rule image produced by SAM
	class RSGISSpectralAngleMapperClassifier : public RSGISCalcImageValue
	{
	public:
		RSGISSpectralAngleMapperClassifier(int numOutBands, double threashold);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};															
		~RSGISSpectralAngleMapperClassifier();
	private:
		int numOutBands;
		double threashold;
	};
	
}}

#endif
